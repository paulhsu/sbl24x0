

void uart_init()
{
//#define MDIV 0xA1
//#define PDIV 0x3
//#define SDIV 1
//    unsigned volatile int *MPLLCON;
//    unsigned int MPLL;
    unsigned int reg,i;
    unsigned volatile int *ULCON,*UCON,*UFCON,*UMCON,*UBRDIV, *GPIO;


//    MPLLCON = (unsigned int *)0x4c000004;
//    *MPLLCON = (MDIV<<12)+(PDIV<<4)+(SDIV);
//    MPLL=12*1024*1024;
//    MPLL=(MPLL)*(MDIV+8)/(PDIV+2)/(2*SDIV); /*setup PLL */

    reg = (202800000/4) / (16* 115200) -1;
    UFCON=(unsigned int *)0x50000008;
    *(UFCON)=7; /*enble FIFO*/

    UMCON=(unsigned int *)0x50000010;
    *(UMCON)=0;

    ULCON=(unsigned int*)0x50000000;
    *(ULCON)=(( 0<<3)|(0)<<2|3); /* 8-n-1 */

    UCON=(unsigned int*)0x50000004;
    *(UCON)=( 1 | 1<<2 | 1<<6 ); /* Enable both TX & RX |  Gerenate status if receive err */

    UBRDIV=(unsigned int *)0x50000028;
    *(UBRDIV)=reg;

    /* gpio UART0 init */
    GPIO=(unsigned int *)0x56000070;
    *GPIO=0xaa;

    for(i=0;i<100;i++);
}

int getchar(void)
{
    unsigned volatile long *UTRSTAT;
    unsigned volatile char *URXH;
    UTRSTAT=(unsigned long*)0x50000010;
    URXH=(unsigned char*)0x50000024;
    while (!(*UTRSTAT & 0x1));

    return (*URXH) & 0xff;
}

void putc(char ch)
{
    while( !( (*(volatile unsigned int *)0x50000010) & 0x2 ) ) ;
    
    (*(volatile unsigned int *)0x50000020) = ch;
}

void putstr(char *str)
{
    while(*str!='\0')
    {
        putc(*str);
        str++;
    }
}

void led_flash()
{
    volatile register unsigned int i asm("r8");
    *((volatile unsigned int *)0x56000054) = 0x10;
    for(i=0;i<(8000*2);i++);
    *((volatile unsigned int *)0x56000054) = 0x20;
    for(i=0;i<(8000*2);i++);
    *((volatile unsigned int *)0x56000054) = 0x40;
    for(i=0;i<(8000*2);i++);
    *((volatile unsigned int *)0x56000054) = 0x80;
    for(i=0;i<(8000*2);i++);
    *((volatile unsigned int *)0x56000054) = 0xF0;
}

/* See also ARM920T    Technical reference Manual */
#define C1_MMU		(1<<0)		/* mmu off/on */
#define C1_ALIGN	(1<<1)		/* alignment faults off/on */
#define C1_DC		(1<<2)		/* dcache off/on */

#define C1_BIG_ENDIAN	(1<<7)		/* big endian off/on */
#define C1_SYS_PROT	(1<<8)		/* system protection */
#define C1_ROM_PROT	(1<<9)		/* ROM protection */
#define C1_IC		(1<<12)		/* icache off/on */
#define C1_HIGH_VECTORS	(1<<13)		/* location of vectors: low/high addresses */
int clean_cache(void)
{
    /*
     * this function is called just before we call linux
     * it prepares the processor for linux
     *
     * we turn off caches etc ...
     */

    unsigned int i;


    /* turn off I/D-cache */
    asm ("mrc p15, 0, %0, c1, c0, 0":"=r" (i));
    i &= ~(C1_DC | C1_IC);
    asm ("mcr p15, 0, %0, c1, c0, 0": :"r" (i));

    /* flush I/D-cache */
    i = 0;
    asm ("mcr p15, 0, %0, c7, c7, 0": :"r" (i));

    return (0);
}

void load_OS()
{
    unsigned int *src = 0x30000;     //flash
    unsigned int *dst = 0x32030000;  //sdram
    int (*OS_Entry)(void *);

    while(src < 0x200000)
    {
        *dst = *src;
        dst++;
        src++;
    }

    clean_cache();

    putstr("OS load OK!\n");
    OS_Entry = (int (*)(void *))0x32030000;
    OS_Entry(1);
}
void start_armboot()
{
    led_flash();
    uart_init();
    putstr("Hello World!\r\n");
    putstr("Press Any key to continue\r\n");
    getchar();
    putstr("Loading!!\r\n");
    load_OS();

    while(1)
        ;
}
