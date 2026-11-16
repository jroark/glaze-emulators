#ifndef MAC_STDIO_H
#define MAC_STDIO_H
#include_next <stdio.h>
#define stderr 0
#define stdout 0
#define fprintf(stream,...) printf(__VA_ARGS__)
#define fflush(stream) ((void)0)
#define vfprintf(stream,fmt,args) mac_vprint(fmt,args)
static inline int mac_vprint(const char *fmt,va_list args) { char b[256]; int n=vsnprintf(b,sizeof b,fmt,args); puts(b); return n; }

#endif
