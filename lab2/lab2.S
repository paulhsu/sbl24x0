.global _start
_start:
   ldr pc, reset_ptr
   ldr pc, undef_ptr
   ldr pc, swi_ptr
   ldr pc, dbt_ptr
   b .
   ldr pc, irq_ptr
   ldr pc, fiq_ptr
   
reset_ptr:
    .word _reset
undef_ptr:
    .word .
swi_ptr:
    .word .
dbt_ptr:
    .word .
irq_ptr:
    .word .
fiq_ptr:
    .word . 

_reset:
    mrs r0, cpsr
    bic r0, r0, #0x1F
    orr r0, r0, #0xD3
    msr cpsr, r0

turn_off_wdt:
    ldr r0, =0x53000000
    ldr r1, =0x0
    str r1, [r0]

turn_off_irq_control:
    ldr r0, =0x4A000008
    ldr r1, =0xFFFFFFFF
    str r1, [r0]

    ldr r0, =0x4A00001C
    ldr r1, =0x3FF
    str r1, [r0]

clock_init:
    ldr r0, =0x4C000004 /*MPLLCON*/
    ldr r1, =0xA1031
    str r1, [r0]
    nop
    nop
    nop
    nop
    nop
    nop
    nop

    ldr r0, =0x4C000014 /*CLKDIV*/
    ldr r1, =0x3
    str r1, [r0]

memory_init:
    ldr r0, =0x48000000 /*BWSCON*/
    ldr r1, =0x2000000  /*BANK6:32bit*/
    str r1, [r0]

    ldr r0, =0x4800001C
    ldr r1, =0x18005
    str r1, [r0]

    ldr r0, =0x48000024
    ldr r1, =0x8E04E8
    str r1, [r0]

    ldr r0, =0x48000028
    ldr r1, =0x31
    str r1, [r0]

    ldr r0, =0x4800002C
    ldr r1, =0x30
    str r1, [r0]

setGPFCON:
    ldr r0, =0x56000050
    ldr r1, =0x5500
    str r1, [r0]


setup_stack:
    ldr sp, =0x33FF0000

relinquish:
    ldr pc, _start_armboot

_start_armboot:
    .word start_armboot
