#include "../../packages/macintosh/adapter.h"
#include <glaze/fs.h>
#include "../../packages/macintosh/view.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static FILE *backing;static int mode;
int fs_seek(int fd,int32_t offset,int whence) { (void)fd;(void)whence;if(mode==3)return -1;if(fseek(backing,offset,SEEK_SET))return -1;return offset; }
int fs_read(int fd,void *p,uint32_t n) { (void)fd;if(mode==1)return -1;if(mode==2)return 0;if(n>17)n=17;return (int)fread(p,1,n,backing); }
int fs_write(int fd,const void *p,uint32_t n) { (void)fd;if(mode==1)return -1;if(mode==2)return 0;if(n>13)n=13;return (int)fwrite(p,1,n,backing); }
int main(void) {
    const char *path="../out/host/macintosh-disk-test.img";
    backing=fopen(path,"w+b");assert(backing);
    struct mac_disk d={1,8192,0};uint8_t data[512],readback[512];
    for(int i=0;i<512;i++)data[i]=(unsigned char)i;
    assert(mac_write(&d,data,512,512)==512);assert(fclose(backing)==0);backing=fopen(path,"r+b");assert(backing);
    assert(mac_read(&d,readback,512,512)==512);assert(!memcmp(data,readback,512));
    assert(mac_write(&d,data,8192,1)==-1);assert(mac_read(&d,data,0xffffffff,512)==-1);
    for(mode=1;mode<=3;mode++) { d.failed=0;assert(mac_read(&d,data,0,512)==-1 && d.failed);assert(mac_write(&d,data,0,512)==-1); }
    uint8_t fb[512*342/8]={0};uint16_t pixels[240*222];int x=-9,y=999;
    mac_render(pixels,240,fb,222,1,&x,&y);assert(x==0 && y==120 && pixels[0]==65535);
    fb[120*64]=128;mac_render(pixels,240,fb,222,1,&x,&y);assert(pixels[0]==0 && pixels[1]==65535);
    x=999;y=-8;mac_render(pixels,240,fb,132,1,&x,&y);assert(x==272 && y==0);
    x=y=0;mac_render(pixels,240,fb,222,0,&x,&y);assert(pixels[0]==0x8410 && pixels[31*240]==65535);
    /* Input and rendering use exactly the same transform, including bars. */
    const struct mac_view cases[]={
        {240,222,0,0,0},{240,126,0,0,0},{320,200,0,0,0},{320,56,0,0,0},{240,246,0,0,0},{240,72,0,0,0},
        {240,222,1,272,120},{320,200,1,192,142},{320,56,1,192,286}
    };
    for(unsigned i=0;i<sizeof cases/sizeof cases[0];i++) {
        struct mac_view v=cases[i];uint16_t image[320*246];
        mac_view_render(image,v.width,fb,&v);
        for(int py=0;py<v.height;py++)for(int px=0;px<v.width;px++) {
            int mx,my;
            if(mac_view_point(&v,px,py,0,&mx,&my)) {
                assert(mx>=0 && mx<512 && my>=0 && my<342);
                assert(image[py*v.width+px]==((fb[my*64+mx/8]&(128>>(mx&7)))?0:65535));
                if(v.zoom)assert(mx==px+v.vx && my==py+v.vy);
            } else assert(image[py*v.width+px]==0x8410);
        }
        int mx,my;assert(!mac_view_point(&v,-1,-1,0,&mx,&my));
        assert(mac_view_point(&v,-999,999,1,&mx,&my));assert(mx>=0 && my<342);
    }
    int shift;assert(mac_key('a',&shift)==0 && !shift);assert(mac_key('A',&shift)==0 && shift);
    assert(mac_key('!',&shift)==0x12 && shift);assert(mac_key('\r',&shift)==0x24);assert(mac_key('\b',&shift)==0x33);
    assert(mac_key(28,&shift)==0x7b);assert(mac_key(255,&shift)==-1);
    fclose(backing);
    puts("Macintosh adapters PASS: partial I/O, errors, bounds, display, keys");return 0;
}
