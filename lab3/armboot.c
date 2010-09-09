

void uart_init()
{
    unsigned int reg,i;
    unsigned volatile int *ULCON,*UCON,*UFCON,*UMCON,*UBRDIV, *GPIO;

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

void NOR_check_id()
{
    unsigned short m_id, d_id;
    volatile unsigned short *flash_base;
    flash_base = 0x0;
    *(flash_base + 0x5555) = 0xAA;
    *(flash_base + 0x2AAA) = 0x55;
    *(flash_base + 0x5555) = 0x90;
    m_id = *(flash_base+0x0);
    d_id = *(flash_base+0x1);
    
    if(m_id == 0x00BF)
        putstr("ManId:0x00BF\r\n");
    else
        putstr("Unknown ManId\r\n");
    if(d_id == 0x234B)
        putstr("DevId:0x234B\r\n");
    else
        putstr("Unknown DevId\r\n");
}
void NOR_erase_block(char blockno)
{
    volatile unsigned short *flash_base;
    volatile unsigned short *dst; 
    
    flash_base = 0x0;
    dst = flash_base + (0x8000*blockno);
    putstr("Erasing...\r\n");

    *(flash_base + 0x5555) = 0xAA;
    *(flash_base + 0x2AAA) = 0x55;
    *(flash_base + 0x5555) = 0x80;
    *(flash_base + 0x5555) = 0xAA;
    *(flash_base + 0x2AAA) = 0x55;
    *(dst) = 0x50;

    while(*(dst) != 0xFFFF) ;
    *(flash_base)=0xF0;

    putstr("Erase OK\r\n");
}

void print_menu()
{
    putstr("'1': Read NOR Flash id\r\n");
    putstr("'2': Erase NOR Flash block\r\n");
}

void start_armboot()
{
    led_flash();
    uart_init();
    putstr("Lab 3:\r\n");
    putstr("Hello World!\r\n");
    

    while(1)
    {
        char ch;
        print_menu();
        ch = getchar();
        switch(ch)
        {
            case '1':
                NOR_check_id();
                break;
            case '2':
                putstr("Which block do you want to erase?(0..9)?\r\n");
                ch = getchar();
		if(ch == '0')
		    putstr("Do you want to erase me? y/n\r\n");
		    ch = getchar();
		    if(ch == 'y'||ch == 'Y')
			NOR_erase_block(0);
		else
		    NOR_erase_block(ch - '0');
                break;
        }
    }
}
