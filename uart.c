#include "S3C2440A.h"


/* 115200,8n1 */
void uart0_init()
{
	/* 设置引脚用于串口 */
	/* GPH2,3用于TxD0, RxD0 */
	GPHCON &= ~((3<<4) | (3<<6));
	GPHCON |= ((2<<4) | (2<<6));

	GPHUP &= ~((1<<2) | (1<<3));  /* 使能内部上拉 */
	

	/* 设置波特率 */
	/* UBRDIVn = (int)( UART clock / ( buad rate x 16) ) –1
	 *  UART clock = 50M
	 *  UBRDIVn = (int)( 50000000 / ( 115200 x 16) ) –1 = 26
	 */
	UCON0 = 0x00000005; /* PCLK,中断/查询模式 */
	UBRDIV0 = 26;
	//UBRDIV0 = 80;	/* buad rate: 38400 */

	/* 设置数据格式 */
	ULCON0 = 0x00000003; /* 8n1: 8个数据位, 无较验位, 1个停止位 */

	/*  */

}



/**********************UART_channel_1***********************/
void uart1_init()
{
	/* 设置引脚用于串口 */
	/* GPH4,5用于TxD1, RxD1 */
	GPHCON &= ~((3<<8) | (3<<10));
	GPHCON |= ((2<<8) | (2<<10));

	GPHUP &= ~((1<<4) | (1<<5));  /* 使能内部上拉 */
	

	/* 设置波特率 */
	/* UBRDIVn = (int)( UART clock / ( buad rate x 16) ) –1
	 *  UART clock = 50M
	 *  UBRDIVn = (int)( 50000000 / ( 115200 x 16) ) –1 = 26
	 */
	UCON1 = 0x00000005; /* PCLK,中断/查询模式 */
	//UBRDIV0 = 26;
	UBRDIV1 = 80;	/* buad rate: 38400 */

	/* 设置数据格式 */
	ULCON1 = 0x00000003; /* 8n1: 8个数据位, 无较验位, 1个停止位 */

	/*  */

}

int getchar_uart1(void)
{	
	while (!(UTRSTAT1 & (1<<0)));
	return URXH1;
}

int putchar_uart1(int c)
{
	/* UTRSTAT1 */
	/* UTXH1 */

	while (!(UTRSTAT1 & (1<<2)));
	UTXH1 = (unsigned char)c;
	return 0;
}

int puts_uart1(const char *s)
{
	while (*s)
	{
		putchar_uart1(*s);
		s++;
	}
	return 0;
}

void uart1_isr(void)
{
	
}

/******************************************************/



int putchar(int c)
{
	/* UTRSTAT0 */
	/* UTXH0 */

	while (!(UTRSTAT0 & (1<<2)));
	UTXH0 = (unsigned char)c;
	return 0;
}

int getchar(void)
{	
	while (!(UTRSTAT0 & (1<<0)));
	return URXH0;
}

int puts(const char *s)
{
	while (*s)
	{
		putchar(*s);
		s++;
	}
	return 0;
}

/* 0xABCDEF12 */
void printHex(unsigned int val)
{
	int i;
	unsigned int arr[8];

	/* 先取出每一位的值 */
	for (i = 0; i < 8; i++)
	{
		arr[i] = val & 0xf;
		val >>= 4;   /* arr[0] = 2, arr[1] = 1, arr[2] = 0xF */
	}

	/* 打印 */
	puts("0x");
	for (i = 7; i >=0; i--)
	{
		if (arr[i] >= 0 && arr[i] <= 9)
			putchar(arr[i] + '0');
		else if(arr[i] >= 0xA && arr[i] <= 0xF)
			putchar(arr[i] - 0xA + 'A');
	}
}

/***调试用例***/
void print1(void)
{
	puts("abc\n\r");
}

void print2(void)
{
	puts("123\n\r");
}
/***************/
