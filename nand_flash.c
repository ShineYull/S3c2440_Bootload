#include "S3C2440A.h"
#include "my_printf.h"


#define TACLS	0
#define TWRPH0	1
#define TWRPH1	0

void nand_init(void)
{
	/* ����NAND FLASH��ʱ�� */
	NFCONF = ((TACLS<<12) | (TWRPH0<<8) | (TWRPH1<<4));

	/* ʹ��NAND FLASH����������ʼ��ECC����ֹƬѡ */
	NFCONT = ((1<<0) | (1<<1) | (1<<4));
}


void nand_select(void)
{
	/* ʹ��Ƭѡ�ź� */
	NFCONT &= ~(1<<1);
}


void nand_deselect(void)
{
	/* ��ֹƬѡ */
	NFCONT |= (1<<1);
}
 

void nand_cmd(unsigned char cmd)
{
	volatile int i;

	NFCMMD = cmd;
	for (i=0; i<10; i++); /* hold signal */
}

void nand_addr_byte(unsigned char addr)
{
	volatile int i;

	NFADDR = addr;
	for (i=0; i<10; i++); /* hold signal */
}


unsigned char nand_r_data(void)
{
	return NFDATA;
}


void nand_w_data(unsigned char val)
{
	NFDATA = val;
}


void nand_wait_ready(void)
{
	while (!(NFSTAT & 1));
}

void nand_page(unsigned int page)
{
	volatile int i;
	
	NFADDR  = page & 0xff;
	for (i = 0; i < 10; i++);
	NFADDR  = (page >> 8) & 0xff;
	for (i = 0; i < 10; i++);
	NFADDR  = (page >> 16) & 0xff;
	for (i = 0; i < 10; i++);	
}

void nand_col(unsigned int col)
{
	volatile int i;

	NFADDR = col & 0xff;
	for (i = 0; i < 10; i++);
	NFADDR = (col >> 8) & 0xff;
	for (i = 0; i < 10; i++);
}


void nand_send_five_addrs(unsigned int addr)
{
	unsigned int col = addr & (2048 - 1);
	unsigned int page = addr / 2048;
	volatile int i;

	NFADDR = col & 0xff;
	for (i = 0; i < 10; i++);
	NFADDR = (col >> 8) & 0xff;
	for (i = 0; i < 10; i++);
	
	NFADDR  = page & 0xff;
	for (i = 0; i < 10; i++);
	NFADDR  = (page >> 8) & 0xff;
	for (i = 0; i < 10; i++);
	NFADDR  = (page >> 16) & 0xff;
	for (i = 0; i < 10; i++);	
}


int nand_bad(unsigned char addr)
{
	unsigned int col = 2048;
	unsigned int page = (addr/2048);
	unsigned char val;


	/* ʹ��Ƭѡ */
	nand_select();

	/* ����0x00���� */
	nand_cmd(0x00);
	
	/* ����addr five cycle */
	nand_col(col);
	nand_page(page);
	
	/* ����0x30COMMAND */
	nand_cmd(0x30);
	
	/* ready to nRE low */
	nand_wait_ready();

	/* �������� */
	val = NFDATA;

	/* ��ֹƬѡ */
	nand_deselect();


	/* �ж��Ƿ�ΪBAD BLOCK */
	if (val != 0xFF)
		return 1;
	else
		return 0;
}


void nand_read(unsigned int addr, unsigned char *buf, unsigned int len)
{
	int i=0;
	int col = addr & (2048 - 1);


	while (i < len)
	{		
		if (!(addr & 0x1FFFF) && nand_bad(addr))
		{
			addr += (128*1024);  /* ������ǰblock */
			continue;
		}
		
		
		/* CE signal enable */
		nand_select();
	
		/* ����0x00���� */
		nand_cmd(0x00);

		/* ����addr five cycle */
		nand_send_five_addrs(addr);
		
		/* ����0x30COMMAND */
		nand_cmd(0x30);
		
		/* ready to nRE low */
		nand_wait_ready();
		
		/* ������ */
		for (; (col<2048) && (i<len); col++)
		{
			buf[i++] = nand_r_data();
			addr++;
		}

		col = 0;
		
		/* CE signal deselect */
		nand_deselect();
	}

}


int nand_erase(unsigned int addr, unsigned int len)
{	
	int page = addr/2048;

	if (addr & (0x1ffff))
	{
		printf("nand_erase err, addr is not block align\n\r");
		return -1;	
	}
	if (len & (0x1ffff))
	{
		printf("nand_erase err, len is not block align\n\r");
		printf("");
		return -1;
	}

	/* CE signal enable */
	nand_select();

	while (1)
	{	
		/* ������һ��block�ĵ�ַ */
		page = addr/2048;
	
		/* write COMMAND */
		nand_cmd(0x60);

		/* row/page addr */
		nand_addr_byte(page & 0xff);
		nand_addr_byte((page>>8) & 0xff);
		nand_addr_byte((page>>16) & 0xff); 
		
		/* write COMMAND */
		nand_cmd(0xD0);

		/* wait ready */
		nand_wait_ready();

		len -= (128*1024);
		if (len <= 0)
			break;
		addr += (128*1024);
	}
	
	/* CE signal deselect */
	nand_deselect();

	return 0;
}


void nand_write(unsigned int addr, unsigned char *buf, unsigned int len)
{	
	int page = addr/2048;
	int col  = addr & (2048 - 1);
	int i=0;
	char str[10];

	if (nand_bad(addr))
	{
		printf("There's a bad block!!! Jump to the next block.\n\r");
		addr += (128*1024);  /* ������ǰblock */
		page = addr/2048;
		col  = addr & (2048 - 1);
	}

	/* CE signal enable */
	nand_select();

	while (i < len)
	{
		/* send COMMAND */
		nand_cmd(0x80);

		/* ����addr five cycle */
		/* col addr */
		nand_addr_byte(col & 0xff);
		nand_addr_byte((col>>8) & 0xff);

		/* row addr */
		nand_addr_byte(page & 0xff);
		nand_addr_byte((page>>8) & 0xff);
		nand_addr_byte((page>>16) & 0xff); 

		/* send data */
		for (; (col<2048) && (i<len); col++)
		{
			nand_w_data(buf[i++]);
		}		
		/* send COMMAND initiate internal programming process */
		nand_cmd(0x10);
		nand_wait_ready();

		/* ��ʼwrite next page */
		col = 0;
		page++;
	}
	
	/* CE signal deselect */
	nand_deselect();
}


void nand_chip_id(void)
{
	unsigned char buf[5]={0};
	int i;

	/* ѡ��оƬ */
	nand_select();

	/* �����INFO����ȡINFO ID*/
	nand_cmd(0x90);
	nand_addr_byte(0x00);

	for (i=0; i<5; i++)	/* ��ȡ5�� */
	{
		buf[i] = nand_r_data();
	}

	/* ȡ��оƬѡ�� */
	nand_deselect();

	/* ��ӡBUFFER�е����� */
	printf("Maker  ID  = 0x%x.\n\r", buf[0]);
	printf("Device ID  = 0x%x.\n\r", buf[1]);
	printf("3rd byte   = 0x%x.\n\r", buf[2]);
	/* ��ʾҳ��С���㷨��Ŀ! */
	printf("4th byte   = 0x%x.\n\r", buf[3]);
	printf("Page Size  = %d KB.\n\r", (1<<(buf[3]&0x03)) );
	printf("Block Size = %d KB.\n\r", (64<<(buf[3]&0x30>>4)) );
	
	printf("5th byte   = 0x%x.\n\r\n\r", buf[4]);
}


void do_read_nand_flash(void)
{
	int i, j;
	unsigned int addr;
	unsigned char c;
	unsigned char str[16];
	unsigned char buf[64];
	volatile unsigned char *p;

	/* ��õ�ַ */
	printf("Enter the address to read: ");
	addr = get_uint();

	nand_read(addr, buf, 64);
	p = (volatile unsigned char *)buf;

	printf("Data : \n\r");
	/* ���ȹ̶�Ϊ64�ֽ� */
	for (i=0; i<4; i++)
	{
		/* ÿ�д�ӡ16������ */
		for (j=0; j<16; j++)
		{
			/* �ȴ�ӡ��ֵ */
			c = *p++;
			str[j] = c;
			printf("%02x ", c);
		}

		printf("   ; ");
		
		for (j=0; j<16; j++)
		{
			/* ���ӡ�ַ� */
			
			/* ���ɼ��ַ� 
			 * ASCII����95���ɴ�ӡ�ַ�(0x20-0x7E)��
			 * 33�������ַ����
			 */
			if (str[j]<0x20 || str[j]>0x7e)	
				putchar('.');
			else
				putchar(str[j]);
		}
		printf("\n\r");
	}
}

void do_erase_nand_flash(void)
{
	unsigned int addr;

	/* ��õ�ַ */
	printf("Enter the address of sector to erase: ");
	addr = get_uint();

	printf("erasing ...\n\r");	
	nand_erase(addr, (128*1024));
}


void do_write_nand_flash(void)
{
	unsigned int addr;
	unsigned char str[100];
	int i, j;
	unsigned int val;
	
	/* ��õ�ַ */
	printf("Enter the address of sector to write: ");
	addr = get_uint();

	/* ���Ҫ������ִ�(data) */
	printf("Enter the string to write: ");
	gets(str);

	printf("writing ...\n\r");	
	nand_write(addr, str, (strlen(str)+1));
}


void nand_flash_test(void)
{
	char c;

	while (1)
	{
		/* ��ӡ�˵���������ѡ��������� */
		printf("[s] Scan nand flash\n\r");
		printf("[e] Erase nand flash\n\r");
		printf("[w] Write nand flash\n\r");
		printf("[r] Read nand flash\n\r");	
		printf("[q] quit\n\r");	
		printf("Enter selection: ");	

		c = getchar();
		printf("%c\n\r", c);

		/* �������ݣ�
		 * 1. ʶ��nand flash
		 * 2. ����nand flashĳ������
		 * 3. ��дĳ����ַ
		 * 4. ��ĳ����ַ
		 */
		switch (c)
		{
			case 'q':
			case 'Q':
				return;
				break;

			case 's':
			case 'S':
				nand_chip_id();
				break;

			case 'e':
			case 'E':
				do_erase_nand_flash();
				break;

			case 'w':
			case 'W':
				do_write_nand_flash();
				break;
			
			case 'r':
			case 'R':
				do_read_nand_flash();
				break;

			default:
				break;
		}
		
	} 
}


