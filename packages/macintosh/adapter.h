#ifndef MAC_ADAPTER_H
#define MAC_ADAPTER_H
#include <stdint.h>
struct mac_disk { int fd; unsigned size; int failed; };
int mac_read(void *, uint8_t *, unsigned, unsigned);
int mac_write(void *, uint8_t *, unsigned, unsigned);
int mac_key(unsigned c, int *shift);
void mac_render(uint16_t *out,int stride,const uint8_t *fb,int height,int zoom,int *vx,int *vy);
#endif
