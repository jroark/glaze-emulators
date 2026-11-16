#ifndef MAC_SETJMP_H
#define MAC_SETJMP_H
typedef void *jmp_buf[5];
#define setjmp(b) __builtin_setjmp(b)
#define longjmp(b,v) __builtin_longjmp(b,1)
#endif
