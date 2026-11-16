/* Optional ROM-based core runner. Inputs are read-only; guest writes use RAM. */
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "umac.h"
#include "rom.h"
int main(int argc,char **argv) {
    if(argc!=4)return 2;
    uint8_t *ram=calloc(1,RAM_SIZE),*rom=malloc(131072),*disk=malloc(819200);
    FILE *f=fopen(argv[1],"rb");if(!f || fread(rom,1,131072,f)!=131072)return 2;fclose(f);
    if(rom_patch(rom))return 2;
    f=fopen(argv[2],"rb");if(!f)return 2;unsigned size=fread(disk,1,819200,f);fclose(f);
    disc_descr_t discs[DISC_NUM_DRIVES]={0};discs[0].base=disk;discs[0].size=size;
    for(int session=0;session<2;session++) {
    memset(ram,0,RAM_SIZE);umac_init(ram,rom,discs);
    for(int i=0;i<12000;i++) { if(umac_loop())return 1;if(i%3==0)umac_vsync_event();if(i%200==0)umac_1hz_event(); }
    /* Absolute jumps bypass acceleration; the guest VBL must move its cursor. */
    const int points[][2]={{420,250},{8,8},{511,341},{100,80}};
    for(unsigned p=0;p<sizeof points/sizeof points[0];p++) {
        umac_mouse(30,30,0); /* stale relative motion must not cause drift */
        umac_mouse_absolute(points[p][0],points[p][1],0);
        for(int i=0;i<120;i++) { assert(!umac_loop());if(i%3==0)umac_vsync_event(); }
        int y=(ram[0x830]<<8)|ram[0x831],x=(ram[0x832]<<8)|ram[0x833];
        assert(x==points[p][0] && y==points[p][1]);
    }
    }
    f=fopen(argv[3],"wb");if(!f)return 2;fprintf(f,"P5\n512 342\n255\n");
    for(int y=0;y<342;y++)for(int x=0;x<512;x++)fputc(ram[umac_get_fb_offset()+y*64+x/8]&(128>>(x&7))?0:255,f);
    fclose(f);free(ram);free(rom);free(disk);puts("Macintosh core completed two 60-second sessions");return 0;
}
