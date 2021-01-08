#include "my_printf.h"
#include "string_utils.h"

#define NOR_FLASH_BASE 0 /* jz2440, nor-->cs0(bank0), base addr = 0 */


/* ���磺	55H 98
 * ���ܣ���(0 + (0x55)<<1)д��0x98
 */
void nor_write_word(unsigned int base, unsigned int offset, unsigned int val)
{
	volatile unsigned short *p = (volatile unsigned short *)(base + (offset<<1));

	*p = val;
}


/* ���ܣ���offset�����ַ����cmd������� 
 * ������offset�ǻ���NOR�ĽǶȿ�����(cpu-A1 == nor-A0)
 */
void nor_cmd(unsigned int offset, unsigned int cmd)
{
	nor_write_word(NOR_FLASH_BASE, offset, cmd);
}


/* ���ܣ���base+offset�ж�ȡһ��word�����������word */
unsigned short nor_read_word(unsigned int base, unsigned int offset)
{
	volatile unsigned short *p = (volatile unsigned short *)(base + (offset<<1));

	return *p;
}


/* Ч������װ�ײ�������� */
unsigned short nor_dat(unsigned int offset)
{
	return nor_read_word(NOR_FLASH_BASE, offset);
}

/* �ȴ���д���� */
void nor_wait_ready(unsigned int addr)
{
	unsigned int val;
	unsigned int pre;

	pre = (unsigned int)nor_dat(addr>>1);
	val = (unsigned int)nor_dat(addr>>1);	

	while ( (val & (1<<6)) != (pre & (1<<6)) )
	{
		pre = val;
		val = (unsigned int)nor_dat(addr>>1);	
	}
}

/* ����NOR FLASH��CFIģʽ
 * ��ȡ������Ϣ
 */
void do_scan_nor_flash(void)
{
	char str[4];
	unsigned int size;
	int i, j, count;
	int regions, region_info_base;
	unsigned int blocks, block_size, block_addr;

	int manufacture, device;
	
	/* ��ӡ����ID���豸ID */		
	nor_cmd(0x555, 0xaa);	/* ���� */
	nor_cmd(0x2aa, 0x55);	
	nor_cmd(0x555, 0x90);	/* read id command */
	manufacture = nor_dat(0);
	device = nor_dat(1);
	nor_cmd(0, 0xf0);		/* reset */
	printf("Manufacture ID: 0x%x, Device ID: 0x%x, ", manufacture, device);
	
	nor_cmd(0x55, 0x98);  /* ����CFIģʽ */ 

	str[0] = (char)nor_dat(0x10);
	str[1] = (char)nor_dat(0x11);
	str[2] = (char)nor_dat(0x12);
	str[3] = '\0';
	printf("str = %s\n\r", str);
	
	/* ��ӡ���� */
	size = 1<<(nor_dat(0x27));
	printf("nor size = 0x%x, %dM\n\r", size, size/(1024*1024));

	/* ��ӡ������������ʼ��ַ */
	/* Erase block region information 
	 * ��2�ֽڵ�ֵ+1����ʾ��region�ж��ٸ�block
     * ��2�ֽڵ�ֵ*256����ʾblock�Ĵ�С
	*/
	regions = nor_dat(0x2c);
	region_info_base = 0x2d;
	block_addr = NOR_FLASH_BASE;
	printf("Block/Sector start Address:\n\r");
	count = 0;
	for (i=0; i<regions; i++)
	{
		blocks = 1 + nor_dat(region_info_base) + (nor_dat(region_info_base+1)<<8);
		block_size = 256 * (nor_dat(region_info_base+2) + (nor_dat(region_info_base+3)<<8));
		region_info_base += 4;

		//printf("\n\rregion %d, blocks = %d, block_size = 0x%x, block_addr = 0x%x\n\r", i, blocks, block_size, block_addr);
			
		for (j=0; j<blocks; j++)
		{
			/* ��ӡÿ��block����ʼ��ַ */
			printf("0x%08x  ", block_addr);
			count++;
			block_addr += block_size;
			if (count%5 == 0)
				printf("\n\r");
		}
	} 
	printf("\n\r");

	/* �˳�CIFģʽ */
	nor_cmd(0, 0xf0);
}


void do_erase_nor_flash(void)
{
	unsigned int addr;

	/* ��õ�ַ */
	printf("Enter the address of sector to erase: ");
	addr = get_uint();

	printf("erasing ...\n\r");	
	nor_cmd(0x555, 0xaa);	/* ���� */
	nor_cmd(0x2aa, 0x55);
	nor_cmd(0x555, 0x80);   /* erase sector command */	
	nor_cmd(0x555, 0xaa);	/* �ٴν��� */
	nor_cmd(0x2aa, 0x55);	
	nor_cmd(addr>>1, 0x30);   /* ����sector addr */	
	nor_wait_ready(addr);
}


void do_write_nor_flash(void)
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

	/* str[0],str[1]==>16bit
	 * str[2],str[3]==>16bit 
	 */
	i = 0;
	j = 1;
	while (str[i] && str[j])
	{
		val = str[i] + ((unsigned int)str[j]<<8);

		/* ��д */		
		nor_cmd(0x555, 0xaa);	/* ���� */
		nor_cmd(0x2aa, 0x55);
		nor_cmd(0x555, 0xA0);	/* write sector command */	
		nor_cmd(addr>>1, val);  /* ��д���� */
		/* �ȴ���д��ɣ������ݣ�Q6�ޱ仯ʱ��ʾ���� */
		nor_wait_ready(addr);

		i += 2;
		j += 2;
		addr += 2;
	}

	val = (unsigned int)str[i];
	/* ��д */		
	nor_cmd(0x555, 0xaa);	/* ���� */
	nor_cmd(0x2aa, 0x55);
	nor_cmd(0x555, 0xA0);	/* write sector command */	
	nor_cmd(addr>>1, val);	/* ��д���� */
	/* �ȴ���д��ɣ������ݣ�Q6�ޱ仯ʱ��ʾ���� */
	nor_wait_ready(addr);
}


void do_read_nor_flash(void)
{
	unsigned int addr;
	volatile unsigned char *p;
	int i, j;
	unsigned char c;
	unsigned char str[16];

	/* ��õ�ַ */
	printf("Enter the address to read: ");
	addr = get_uint();

	p = (volatile unsigned char *)addr;

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


void nor_flash_test(void)
{
	char c;

	while (1)
	{
		/* ��ӡ�˵���������ѡ��������� */
		printf("[s] Scan nor flash\n\r");
		printf("[e] Erase nor flash\n\r");
		printf("[w] Write nor flash\n\r");
		printf("[r] Read nor flash\n\r");	
		printf("[q] quit\n\r");	
		printf("Enter selection: ");	

		c = getchar();
		printf("%c\n\r", c);

		/* �������ݣ�
		 * 1. ʶ��nor flash
		 * 2. ����nor flashĳ������
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
				do_scan_nor_flash();
				break;

			case 'e':
			case 'E':
				do_erase_nor_flash();
				break;

			case 'w':
			case 'W':
				do_write_nor_flash();
				break;
			
			case 'r':
			case 'R':
				do_read_nor_flash();
				break;

			default:
				break;
		}
		
	} 
}

