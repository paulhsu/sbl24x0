

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
int Getc(int timeout) {
    unsigned volatile long *UTRSTAT;
    unsigned volatile char *URXH;
    int i;
    UTRSTAT=(unsigned long*)0x50000010;
    URXH=(unsigned char*)0x50000024;
    while (!(*UTRSTAT & 0x1)) {
        for(i=0;i<100;i++); 
        timeout--;
        if (timeout==0) return -1;
    }
    return (*URXH) & 0xff;
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
#define TACLS 0
#define TWRPH0 4
#define TWRPH1 2	
#define RESET 0xFF
#define	READ_1_1	0x00
#define NAND_SECTOR_SIZE 512

#define NF_CMD(cmd)     { *NFCMD=cmd;}
#define NF_ADDR(addr)   { *NFADDR=addr;} 
#define NF_nFCE_L()     { *NFCONF&=~(1<<11);}
#define NF_nFCE_H()     { *NFCONF|=(1<<11);}
#define NF_RSTECC()     { *NFCONF|=(1<<12);}
#define NF_RDDATA()     (*NFDATA)
#define NF_WRDATA(data) { *NFDATA=data;}
#define NF_WAITRB()     { while(!(*NFSTAT&(1<<0))); } 

void enable_nand() {
    unsigned volatile short *NFCONF;
    unsigned volatile short *NFCMD;
    unsigned volatile short *NFSTAT;
    unsigned int i;

    NFCONF=(unsigned short *)0x4e000000;
    NFCMD=(unsigned short *)0x4e000004;
    NFSTAT=(unsigned short *)0x4e000010;

    //*NFCONF=(1<<15|1<<14|1<<13|1<<12|TACLS<<8|TWRPH0<<4|TWRPH1);  /*Setup NFCONF */
    *NFCONF=0xf842;
    NF_nFCE_L(); /*enable nand chip */
    *NFCMD=(unsigned char )RESET;
    NF_WAITRB();
    NF_nFCE_H(); /*disable nand chip */
}

static int NAND_ReadPage(unsigned long block,unsigned long page,unsigned char *buffer)        //?Flash
{
    int i;
    unsigned int blockPage;    
    unsigned volatile short *NFCONF;
    unsigned volatile short *NFCMD;
    unsigned volatile short *NFSTAT;
    unsigned volatile short *NFADDR;
    unsigned volatile short *NFDATA;

    NFCONF=(unsigned short *)0x4e000000;
    NFCMD=(unsigned short *)0x4e000004;
    NFSTAT=(unsigned short *)0x4e000010;
    NFDATA=(unsigned short *)0x4e00000C;
    NFADDR=(unsigned short *)0x4e000008;

    page=page&0x1f;
    blockPage=(block<<5)+page;                      //1Bolck¥]§t32page
    //NF_RSTECC();                                    // Initialize ECC    
    NF_nFCE_L();    
    NF_CMD(0x00);                                   // Read command
    NF_ADDR(0);                                     // Column = 0
    NF_ADDR(blockPage&0xff);                        //
    NF_ADDR((blockPage>>8)&0xff);                   // Block & Page num.
    NF_ADDR((blockPage>>16)&0xff);                  //

    for(i=0;i<10;i++);                              //wait tWB(100ns)    
    NF_WAITRB();                                    // Wait tR(max 12us)
    for(i=0;i<512;i++)
    {
        *buffer++=NF_RDDATA();                       // Read one page
    }
    NF_nFCE_H();   
}
static int NAND_WritePage(unsigned long block,unsigned long page,unsigned char *buffer)  //?Flash 
{
    int i;
    unsigned long blockPage=(block<<5)+page;
    //NF_RSTECC();                              // Initialize ECC    
    unsigned volatile short *NFCONF;
    unsigned volatile short *NFCMD;
    unsigned volatile short *NFSTAT;
    unsigned volatile short *NFADDR;
    unsigned volatile short *NFDATA;

    NFCONF=(unsigned short *)0x4e000000;
    NFCMD=(unsigned short *)0x4e000004;
    NFSTAT=(unsigned short *)0x4e000010;
    NFDATA=(unsigned short *)0x4e00000C;
    NFADDR=(unsigned short *)0x4e000008;

    NF_nFCE_L(); 
    NF_CMD(0x0);                            //?????\\Read Mode 1
    NF_CMD(0x80);                               // Write 1st command,?Õu?¤J
    NF_ADDR(0);                                 // Column 0
    NF_ADDR(blockPage&0xff);        
    NF_ADDR((blockPage>>8)&0xff);               // Block & page num.
    NF_ADDR((blockPage>>16)&0xff);  

    for(i=0;i<512;i++)
    {
        NF_WRDATA(*buffer++);                    // Write one page to NFM from buffer
    }  
#if 0
    seBuf[0]=rNFECC0;
    seBuf[1]=rNFECC1;
    seBuf[2]=rNFECC2;
    seBuf[5]=0xff;                          // Marking good block  

    for(i=0;i<16;i++)
        NF_WRDATA(seBuf[i]);                    // Write spare array(ECC and Mark)
#endif  

    NF_CMD(0x10);                           // Write 2nd command
    for(i=0;i<10;i++);                      //tWB = 100ns. ////??????

    NF_WAITRB();                            //wait tPROG 200~500us; 
    NF_CMD(0x70);                           // Read status command   

    for(i=0;i<3;i++);                       //twhr=60ns   
    if (NF_RDDATA()&0x1) 
    {
        NF_nFCE_H();
       	return 0;
    }//error }
    else 
    {
        NF_nFCE_H();
       	return 1;
    }

}
static int NAND_EraseBlock(unsigned long block)
{
    unsigned long blockPage=(block<<5);
    int i;
    unsigned volatile short *NFCONF;
    unsigned volatile short *NFCMD;
    unsigned volatile short *NFSTAT;
    unsigned volatile short *NFADDR;
    unsigned volatile short *NFDATA;

    NFCONF=(unsigned short *)0x4e000000;
    NFCMD=(unsigned short *)0x4e000004;
    NFSTAT=(unsigned short *)0x4e000010;
    NFDATA=(unsigned short *)0x4e00000C;
    NFADDR=(unsigned short *)0x4e000008;
    NF_nFCE_L();    
    NF_CMD(0x60);                            // Erase one block 1st command 

    NF_ADDR(blockPage&0xff);                 // Page number="0"
    NF_ADDR((blockPage>>8)&0xff);   
    NF_ADDR((blockPage>>16)&0xff);

    NF_CMD(0xd0 );                           // Erase one blcok 2nd command

    for(i=0;i<10;i++);                       //wait tWB(100ns)//??????
    NF_WAITRB();                            // Wait tBERS max 3ms.
    NF_CMD(0x70);                           // Read status command
    if (NF_RDDATA()&0x1)                    // Erase error
    {   
        NF_nFCE_H();
        printf("[ERASE_ERROR:block#=%d]\n",block);
        return 0;
    }
    else 
    {
        NF_nFCE_H();
        return 1;
    }

}
void nand_write() {
    unsigned long offset;
    int blockno;
    int blockstart;
    int blockend;
    int page;
    enable_nand();
    blockstart=0x20000/0x4000;
    blockend=blockstart+(0x170000/0x4000);
    offset=0x31000000;
    for(blockno=blockstart;blockno<(blockend+1);blockno++) {
        NAND_EraseBlock(blockno);
        for(page=0;page<32;page++) {
            NAND_WritePage(blockno,page,offset);
            offset = offset + 512;
        }
    }	
}

void nand_demo() {
    int (*OS_Entry)(void *);
    unsigned int offset;
    int blockno;
    int blockstart;
    int blockend;
    int page;
    enable_nand();
    blockstart=0x20000/0x4000;
    blockend=blockstart+(0x170000/0x4000);
    offset=0x31000000;
    for(blockno=blockstart;blockno<(blockend+1);blockno++) {
        for(page=0;page<32;page++) {
            NAND_ReadPage(blockno,page,offset);
            offset = offset + 512;
        }
    }
    clean_cache();
    putstr("image ready, any key continue\n");
    getchar();
    OS_Entry=(int *)0x31000000;
    OS_Entry(1);
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

    putstr("OS load from NOR Flash, OK!\n");
    OS_Entry = (int (*)(void *))0x32030000;
    OS_Entry(1);
}
void print_menu()
{
    putstr("'1': Read NOR Flash id\r\n");
    putstr("'2': Load OS from  NOR Flash\r\n");
    putstr("'3': Erase NOR Flash block\r\n");
    putstr("'4': Write OS image to NAND Flash\r\n");
    putstr("'5': Load OS from  NAND Flash\r\n");
    putstr("'6': Download from XMODEM\r\n");
    putstr("'7': Execute OS after downloaded\r\n");

}

void start_armboot()
{
    int (*X_OS_Entry)(void *);
    char *dest;
    led_flash();
    uart_init();
    putstr("Lab 5:\r\n");

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
                load_OS();
                break;
            case '3':
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
            case '4':
                printf("nand write\r\n");
                nand_write();
                break;
	    case '5':
		putstr("Here is a nand demo\r\n");
                nand_demo();
		break;
            case '6':
                dest=(char *)0x31000000;
                printf("enter xmodem receiving...\r\n");
                xmodem_rx(dest);
                putstr("Download OS image from XMODEM successfully!\r\n");
                break;
            case '7':
                clean_cache();
                X_OS_Entry=(int *)0x31000000;
		X_OS_Entry(1);
                break;
        }
    }
}
