


.global _start

_start:
    ldr r0, =0x56000050 /*GPFCON*/
    mov r1, #0x5500     /*GPF6:outpur,GPF7:output*/
    str r1, [r0]
#if 0
    ldr r0, =0x56000054 /*GPFDAT*/

1:
    mov r1, #0xC0 /*11000000,light GPF4,GPF5*/
    str r1, [r0]  /*output 0 for light*/
    b 1b
#endif
    bl ledloop
