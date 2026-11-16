#include "view.h"
static int bound(int x,int max) { return x<0?0:x>max?max:x; }
void mac_view_clamp(struct mac_view *v) {
    v->vx=bound(v->vx,512-v->width);v->vy=bound(v->vy,342-v->height);
}
static void fit(const struct mac_view *v,int *w,int *h) {
    *w=v->width;*h=*w*342/512;
    if(*h>v->height) { *h=v->height;*w=*h*512/342; }
}
int mac_view_point(const struct mac_view *v,int x,int y,int clamp,int *mx,int *my) {
    int w=v->width,h=v->height;
    if(!v->zoom)fit(v,&w,&h);
    x-=(v->width-w)/2;y-=(v->height-h)/2;
    if(!clamp && (x<0 || y<0 || x>=w || y>=h))return 0;
    x=bound(x,w-1);y=bound(y,h-1);
    *mx=v->zoom?x+v->vx:x*512/w;
    *my=v->zoom?y+v->vy:y*342/h;
    return 1;
}
void mac_view_render(uint16_t *out,int stride,const uint8_t *fb,struct mac_view *v) {
    mac_view_clamp(v);
    int w=v->width,h=v->height,xmap[320];
    if(!v->zoom)fit(v,&w,&h);
    int left=(v->width-w)/2,top=(v->height-h)/2;
    for(int x=0;x<w;x++)xmap[x]=v->zoom?x+v->vx:x*512/w;
    for(int y=0;y<v->height;y++) {
        uint16_t *row=out+y*stride;
        for(int x=0;x<v->width;x++)row[x]=0x8410;
        if(y<top || y>=top+h)continue;
        int sy=v->zoom?y+v->vy:(y-top)*342/h;
        for(int x=0;x<w;x++) {
            int sx=xmap[x];row[left+x]=(fb[sy*64+sx/8]&(128>>(sx&7)))?0:65535;
        }
    }
}
