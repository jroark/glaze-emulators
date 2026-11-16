#ifndef MAC_VIEW_H
#define MAC_VIEW_H
#include <stdint.h>
struct mac_view { int width,height,zoom,vx,vy; };
void mac_view_clamp(struct mac_view *v);
/* Coordinates relative to the viewport. Returns zero for letterboxing. */
int mac_view_point(const struct mac_view *v,int x,int y,int clamp,int *mx,int *my);
void mac_view_render(uint16_t *out,int stride,const uint8_t *fb,struct mac_view *v);
#endif
