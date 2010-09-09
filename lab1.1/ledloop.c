
void ledloop()
{
    while(1)
    {
        volatile register unsigned int i asm("r8");
        *((volatile unsigned int *)0x56000054) = 0x10;
        for(i=0;i<8000;i++);
        *((volatile unsigned int *)0x56000054) = 0x20;
        for(i=0;i<8000;i++);
        *((volatile unsigned int *)0x56000054) = 0x40;
        for(i=0;i<8000;i++);
        *((volatile unsigned int *)0x56000054) = 0x80;
        for(i=0;i<8000;i++);
    }
}
