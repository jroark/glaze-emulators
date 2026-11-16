#include "adapter.h"
#include "view.h"
#include <glaze/fs.h>
#include <string.h>
static int transfer(struct mac_disk *d,uint8_t *p,unsigned off,unsigned len,int write)
{
    if (off>d->size || len>d->size-off || fs_seek(d->fd,(int)off,FS_SEEK_SET)!=(int)off) goto fail;
    unsigned done=0;
    while (done<len) {
        unsigned count=len-done; if(count>FS_IO_MAX) count=FS_IO_MAX;
        int n=write ? fs_write(d->fd,p+done,count) : fs_read(d->fd,p+done,count);
        if(n<=0 || (unsigned)n>count) goto fail;
        done+=(unsigned)n;
    }
    return (int)len;
fail: d->failed=1; return -1;
}
int mac_read(void *d,uint8_t *p,unsigned o,unsigned n) { return transfer(d,p,o,n,0); }
int mac_write(void *d,uint8_t *p,unsigned o,unsigned n) { return transfer(d,p,o,n,1); }
int mac_key(unsigned c,int *shift)
{
    static const char plain[]="asdfhgzxcv\0bqwerty123465=97-80]ou[ip\rlj'k;\\,/nm.\t \x60\b";
    static const char upper[]="ASDFHGZXCV\0BQWERTY!@#$^%+(&_*)}OU{IP\rLJ\"K:|<?NM>\t ~\b";
    *shift=0;
    for(unsigned i=0;i<sizeof plain-1;i++) if(plain[i] && c==(unsigned char)plain[i]) return (int)i;
    for(unsigned i=0;i<sizeof upper-1;i++) if(upper[i] && c==(unsigned char)upper[i]) { *shift=1; return (int)i; }
    if(c>=28 && c<=31) { static const int arrows[]={0x7b,0x7c,0x7e,0x7d}; return arrows[c-28]; }
    if(c==27) return 0x35;
    return -1;
}
void mac_render(uint16_t *out,int stride,const uint8_t *fb,int height,int zoom,int *vx,int *vy)
{
    struct mac_view v={240,height,zoom,*vx,*vy};
    mac_view_render(out,stride,fb,&v);*vx=v.vx;*vy=v.vy;
}
