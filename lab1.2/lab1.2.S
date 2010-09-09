


.global _start
_start:
   ldr pc, reset_ptr
   ldr pc, undef_ptr
   ldr pc, swi_ptr
   ldr pc, pabt_ptr
   ldr pc, dabt_ptr
   b .
   ldr pc, irq_ptr
   ldr pc, fiq_ptr
   
reset_ptr:
    .word _reset
undef_ptr:
    .word .
swi_ptr:
    .word .
pabt_ptr:
    .word .
dabt_ptr:
    .word .
irq_ptr:
    .word .
fiq_ptr:
    .word . 

_reset:
led_init:
    ldr r0, =0x56000050
    mov r1, #0x5500
    str r1, [r0]

    bl ledloop
