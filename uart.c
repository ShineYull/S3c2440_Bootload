#include "S3C2440A.h"


/* 115200,8n1 */
void uart0_init()
{
	/* �����������ڴ��� */
	/* GPH2,3����TxD0, RxD0 */
	GPHCON &= ~((3<<4) | (3<<6));
	GPHCON |= ((2<<4) | (2<<6));

	GPHUP &= ~((1<<2) | (1<<3));  /* ʹ���ڲ����� */
	

	/* ���ò����� */
	/* UBRDIVn = (int)( UART clock / ( buad rate x 16) ) �C1
	 *  UART clock = 50M
	 *  UBRDIVn = (int)( 50000000 / ( 115200 x 16) ) �C1 = 26
	 */
	UCON0 = 0x00000005; /* PCLK,�ж�/��ѯģʽ */
	UBRDIV0 = 26;
	//UBRDIV0 = 80;	/* buad rate: 38400 */

	/* �������ݸ�ʽ */
	ULCON0 = 0x00000003; /* 8n1: 8������λ, �޽���λ, 1��ֹͣλ */

	/*  */

}



/**********************UART_channel_1***********************/
void uart1_init()
{
	/* �����������ڴ��� */
	/* GPH4,5����TxD1, RxD1 */
	GPHCON &= ~((3<<8) | (3<<10));
	GPHCON |= ((2<<8) | (2<<10));

	GPHUP &= ~((1<<4) | (1<<5));  /* ʹ���ڲ����� */
	

	/* ���ò����� */
	/* UBRDIVn = (int)( UART clock / ( buad rate x 16) ) �C1
	 *  UART clock = 50M
	 *  UBRDIVn = (int)( 50000000 / ( 115200 x 16) ) �C1 = 26
	 */
	UCON1 = 0x00000005; /* PCLK,�ж�/��ѯģʽ */
	//UBRDIV0 = 26;
	UBRDIV1 = 80;	/* buad rate: 38400 */

	/* �������ݸ�ʽ */
	ULCON1 = 0x00000003; /* 8n1: 8������λ, �޽���λ, 1��ֹͣλ */

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

	/* ��ȡ��ÿһλ��ֵ */
	for (i = 0; i < 8; i++)
	{
		arr[i] = val & 0xf;
		val >>= 4;   /* arr[0] = 2, arr[1] = 1, arr[2] = 0xF */
	}

	/* ��ӡ */
	puts("0x");
	for (i = 7; i >=0; i--)
	{
		if (arr[i] >= 0 && arr[i] <= 9)
			putchar(arr[i] + '0');
		else if(arr[i] >= 0xA && arr[i] <= 0xF)
			putchar(arr[i] - 0xA + 'A');
	}
}

/***��������***/
void print1(void)
{
	puts("abc\n\r");
}

void print2(void)
{
	puts("123\n\r");
}
/***************/
