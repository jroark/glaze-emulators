#include <glaze/ui.h>
#include <glaze/sys.h>
#include <glaze/fs.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "umac.h"
#include "rom.h"
#include "adapter.h"
#include "view.h"
static char rompath[128],diskpath[128],notice[96];
static uint8_t *ram,*rom;
static int zoom,pan,vx,vy,keyboard,paused,done,drag,lx,ly,mods;
static struct ui_keyboard kb;
static struct mac_disk disk;
static unsigned char held[128];
struct stroke { unsigned char key,down; };
static struct stroke queue[256];
static unsigned qhead,qtail;
static void enqueue(int key,int down) { if(qhead-qtail<256) { queue[qhead++%256]=(struct stroke){(unsigned char)key,(unsigned char)down}; } }
static void release(void) {
    for(int i=0;i<128;i++) if(held[i]) umac_kbd_event((i<<1)|1,0);
    memset(held,0,sizeof held); qhead=qtail=0; mods=drag=0; umac_mouse(0,0,0);
}
static void character(unsigned c) {
    int shift,key=mac_key(c,&shift); if(key<0 || qhead-qtail>248)return;
    if(shift && !(mods&1))enqueue(0x38,1);
    enqueue(key,1);enqueue(key,0);if(shift && !(mods&1))enqueue(0x38,0);
}
static int viewtop(void) { return ui_landscape()?24:58; }
static int keyboard_y(void) { return ui_h()-(ui_landscape()?UI_KB_LAND_H:UI_KB_H); }
static int modifiers_y(void) { return keyboard_y()-24; }
static struct mac_view view(void) {
    struct mac_view v={ui_w(),(keyboard?modifiers_y():ui_h())-viewtop(),zoom,vx,vy};
    mac_view_clamp(&v);vx=v.vx;vy=v.vy;return v;
}
static void draw(void) {
    gfx_fill(&ui.surf,gfx_r(0,0,ui.surf.w,ui.surf.h),0xe8ecf0);
    if(ui_landscape()) {
        const char *labels[]={"Setup",zoom?"Fit":"Zoom",pan?"Mouse":"Pan","Keys",paused?"Play":"Pause","Reset"};
        for(int i=0;i<6;i++) {
            int x=i*ui_w()/6,w=(i+1)*ui_w()/6-x;
            ui_draw_button(&ui.surf,gfx_r(x,0,w,24),labels[i],false);
        }
    } else {
        ui_draw_navbar(&ui.surf,"Macintosh","Setup",NULL,0);
        const char *labels[]={zoom?"Fit":"Zoom",pan?"Mouse":"Pan","Keys",paused?"Play":"Pause","Reset"};
        for(int i=0;i<5;i++)ui_draw_button(&ui.surf,gfx_r(i*48,30,48,26),labels[i],false);
    }
    struct mac_view v=view();
    mac_view_render(ui.surf.px+viewtop()*ui.surf.stride,ui.surf.stride,ram+umac_get_fb_offset(),&v);
    if(keyboard) {
        int my=modifiers_y();
        ui_draw_keyboard(&ui.surf,&kb,keyboard_y());
        const char *names[]={"Shift","Cmd","Opt","Ctrl","Esc"};
        for(int i=0;i<5;i++) {
            int x=i*ui_w()/5,w=(i+1)*ui_w()/5-x;
            ui_draw_button(&ui.surf,gfx_r(x,my,w,24),names[i],i<4 && (mods&(1<<i)));
        }
    }
    if(paused)gfx_text_center(&ui.surf,0,viewtop()+7,ui.surf.w,ui_font(FONT_B17),"Paused",0xff0000,255);
    ui_invalidate_all();ui_present();
}
static void point_mouse(int x,int y,int button,int clamp) {
    struct mac_view v=view();int mx,my;
    if(mac_view_point(&v,x,y-viewtop(),clamp,&mx,&my))umac_mouse_absolute(mx,my,button);
}
static void toolbar(int action) {
    release();
    switch(action) {
    case 0:done=1;break;case 1:zoom=!zoom;break;case 2:pan=!pan;break;
    case 3:keyboard=!keyboard;break;case 4:paused=!paused;break;case 5:done=2;break;
    }
    draw();
}
static void event(const struct gz_input_event *e) {
    /* SpringBoard already maps touches into the current app surface. */
    if(e->type==GZ_EV_APP_DEACTIVATE || e->type==GZ_EV_TOUCH_CANCEL) { release();return; }
    if(e->type==GZ_EV_APP_RESIZE) {
        release();kb.pressed=-1;printf("macintosh: resized %dx%d\n",ui_w(),ui_h());draw();return;
    }
    if(e->type==GZ_EV_APP_ACTIVATE) { draw();return; }
    if(e->type==GZ_EV_TEXT && !paused)character(e->code);
    if(e->type==GZ_EV_KEY_DOWN && !paused) {
        switch(e->code) {
        case GZ_KEY_ENTER:character('\r');break;case GZ_KEY_TAB:character('\t');break;
        case GZ_KEY_BACKSPACE:case GZ_KEY_DELETE:character('\b');break;case GZ_KEY_ESC:character(27);break;
        case GZ_KEY_LEFT:character(28);break;case GZ_KEY_RIGHT:character(29);break;
        case GZ_KEY_UP:character(30);break;case GZ_KEY_DOWN:character(31);break;
        }
    }
    if(drag && (e->type==GZ_EV_TOUCH_MOVE || e->type==GZ_EV_TOUCH_UP)) {
        if(pan && zoom) { vx-=e->x-lx;vy-=e->y-ly;draw(); }
        else if(!pan)point_mouse(e->x,e->y,e->type!=GZ_EV_TOUCH_UP,1);
        lx=e->x;ly=e->y;
        if(e->type==GZ_EV_TOUCH_UP) { drag=0;umac_mouse(0,0,0); }
        return;
    }
    if(e->type==GZ_EV_TOUCH_UP) {
        if(ui_landscape() && e->y<24) { toolbar(e->x*6/ui_w());return; }
        if(!ui_landscape()) {
            if(e->y<30 && e->x<65) { toolbar(0);return; }
            if(e->y>=30 && e->y<56) { toolbar(1+e->x/48);return; }
        }
        int my=modifiers_y();
        if(keyboard && e->y>=my && e->y<my+24 && !paused) {
            int i=e->x*5/ui_w();static const int codes[]={0x38,0x37,0x3a,0x3b};
            if(i<4) { mods^=1<<i;enqueue(codes[i],!!(mods&(1<<i))); } else character(27);
            draw();return;
        }
    }
    if(keyboard && e->y>=keyboard_y() && !paused) {
        bool changed=false;
        int c=ui_keyboard_event(&kb,e,keyboard_y(),&changed);if(c)character(c);if(changed)draw();return;
    }
    struct mac_view v=view();int mx,my;
    if(e->type==GZ_EV_TOUCH_DOWN && !paused &&
       mac_view_point(&v,e->x,e->y-viewtop(),0,&mx,&my)) {
        drag=1;lx=e->x;ly=e->y;if(!pan)umac_mouse_absolute(mx,my,1);
    }
}
static void play(void) {
    struct fs_stat st;
    if(fs_stat(rompath,&st)<0 || st.size!=131072) { strcpy(notice,"Need a 128 KB Mac Plus V3 ROM");return; }
    rom=malloc(131072);ram=calloc(1,512*1024);
    if(!rom || !ram) { strcpy(notice,"Out of memory");goto cleanup; }
    int fd=fs_open(rompath,FS_O_READ);unsigned n=0;
    while(fd>=0 && n<131072) { int r=fs_read(fd,rom+n,4096);if(r<=0)break;n+=r; }
    if(fd>=0)fs_close(fd);
    if(n!=131072 || rom[0]!=0x4d || rom[1]!=0x1f || rom[2]!=0x81 || rom[3]!=0x72 || rom_patch(rom)) { strcpy(notice,"Unsupported or unreadable ROM");goto cleanup; }
    if(fs_stat(diskpath,&st)<0 || st.type!=FS_FILE || !st.size || st.size%512 || st.size>0x7fffffff) { strcpy(notice,"Need a raw sector-aligned disk image");goto cleanup; }
    disk=(struct mac_disk){fs_open(diskpath,FS_O_READ|FS_O_WRITE),st.size,0};
    if(disk.fd<0) { strcpy(notice,"Cannot open disk for writing");goto cleanup; }
    disc_descr_t discs[DISC_NUM_DRIVES]={0};discs[0].size=disk.size;discs[0].op_ctx=&disk;discs[0].op_read=mac_read;discs[0].op_write=mac_write;
    do {
        memset(ram,0,512*1024);umac_init(ram,rom,discs);done=paused=0;release();draw();
        uint64_t tick=gz_time_us(),second=tick,frame=tick,keytime=tick;
        while(!done) {
            struct gz_input_event e;
            if(ui_next_event(&e,paused?50:0)) { event(&e); if(e.type==GZ_EV_APP_ACTIVATE || paused) tick=second=frame=gz_time_us(); }
            if(done || paused)continue;
            if(umac_loop()) { done=1;break; }
            uint64_t now=gz_time_us();
            if(now-keytime>=20000 && qhead!=qtail) { struct stroke s=queue[qtail++%256];held[s.key]=s.down;umac_kbd_event((s.key<<1)|1,s.down);keytime=now; }
            if(now-tick>=16667) { umac_vsync_event();tick+=16667;if(now-tick>100000)tick=now; }
            if(now-second>=1000000) { umac_1hz_event();second=now; }
            if(now-frame>=33333) { draw();frame=now; }
            if(disk.failed) { strcpy(notice,"Disk I/O failed; session stopped");done=1; }
        }
        release();
    } while(done==2);
    fs_close(disk.fd);
cleanup: free(rom);free(ram);rom=ram=NULL;
}
/* Paginated browser for ROMs and disks; directories are always visible. */
static void choose(char *out,int isrom) {
    char dir[128]="/Card";int skip=0;
    for(;;) {
        int rows=(ui_h()-74)/32,bottom=ui_h()-32;
        struct fs_dirent entries[8];int n=fs_list(dir,skip,entries,rows);
        gfx_fill(&ui.surf,gfx_r(0,0,ui_w(),ui_h()),0xe8ecf0);
        ui_draw_navbar(&ui.surf,isrom?"Select ROM":"Select disk","Back",NULL,0);
        for(int i=0;i<n;i++) { entries[i].name[63]=0;ui_draw_button(&ui.surf,gfx_r(0,34+i*32,ui_w(),30),entries[i].name,false); }
        const char *buttons[]={"Root","Prev","Next"};
        for(int i=0;i<3;i++) { int x=i*ui_w()/3,w=(i+1)*ui_w()/3-x;ui_draw_button(&ui.surf,gfx_r(x,bottom,w,28),buttons[i],false); }
        ui_invalidate_all();ui_present();
        struct gz_input_event e={0};
        do { if(!ui_next_event(&e,100)) e.type=0; }
        while(e.type!=GZ_EV_TOUCH_UP && e.type!=GZ_EV_APP_RESIZE && e.type!=GZ_EV_APP_ACTIVATE);
        if(e.type!=GZ_EV_TOUCH_UP)continue;
        if(e.y<30) { char *p=strrchr(dir,'/');if(p && p!=dir)*p=0;else return;skip=0; }
        else if(e.y>=bottom && e.y<bottom+28) {
            int action=e.x*3/ui_w();
            if(action==0) { strcpy(dir,"/");skip=0; }
            else if(action==1)skip=skip>=rows?skip-rows:0;
            else if(n==rows)skip+=rows;
        }
        else if(e.y>=34 && e.y<34+rows*32) {
            int i=(e.y-34)/32;if(i>=n)continue;
            if(!strcmp(entries[i].name,".") || !strcmp(entries[i].name,".."))continue;
            char path[128];if(snprintf(path,sizeof path,"%s%s%s",dir,strcmp(dir,"/")?"/":"",entries[i].name)>=128)continue;
            if(entries[i].type==FS_DIR) { strcpy(dir,path);skip=0; }
            else { const char *dot=strrchr(path,'.');if(!isrom && (!dot || (strcasecmp(dot,".img") && strcasecmp(dot,".dsk"))))continue;strcpy(out,path);return; }
        }
    }
}
int main(void) {
    ui_init(SB_STATUS_BLACK);
    ui_set_orientations(SB_ORIENT_PORTRAIT | SB_ORIENT_LANDSCAPE);
    ui_keyboard_init(&kb);puts("macintosh: ready");
    for(;;) {
        gfx_fill(&ui.surf,gfx_r(0,0,ui_w(),ui_h()),0xe8ecf0);ui_draw_navbar(&ui.surf,"Macintosh",NULL,NULL,0);
        ui_draw_button(&ui.surf,gfx_r(10,44,ui_w()-20,34),*rompath?"Change ROM":"Select ROM",false);
        ui_draw_button(&ui.surf,gfx_r(10,88,ui_w()-20,34),*diskpath?"Change disk":"Select disk",false);
        ui_draw_button(&ui.surf,gfx_r(10,132,ui_w()-20,34),"Start Macintosh",false);
        gfx_text_center(&ui.surf,0,ui_h()-44,ui_w(),ui_font(FONT_R10),"512 KB  -  disk writes persist",UI_TEXT_DETAIL,255);
        gfx_text_center(&ui.surf,0,ui_h()-24,ui_w(),ui_font(FONT_R10),notice,UI_TEXT_DETAIL,255);
        ui_invalidate_all();ui_present();struct gz_input_event e;
        if(ui_next_event(&e,100) && e.type==GZ_EV_TOUCH_UP) {
            if(e.y>=44 && e.y<78)choose(rompath,1);else if(e.y>=88 && e.y<122)choose(diskpath,0);else if(e.y>=132 && e.y<166) { notice[0]=0;play(); }
        }
    }
}
