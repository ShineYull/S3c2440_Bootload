#include "S3C2440A.h"
#include "sdram.h"
#include "nand_flash.h"

void init_sdram(void)
{
	/*set BWSCON Bank6,7*/
	BWSCON &= ~((3<<24) | (3<<28));
	BWSCON |=  ((1<<25) | (3<<29));

	/*set BANKCON6,7
	 *tHCLK = 10ns; clock = 2;
	 */
	BANKCON6 = 0x18001;
	BANKCON7 = 0x18001;		

	/*set REGRESH*/
	REGRESH = 0x8404F5;

	/*set BANKSIZE*/
	BANKSIZE  = 0xB1;

	/*set MRSRB6,7*/
	MRSRB6 = 0x20;
	MRSRB7 = 0x20;
	
}

int sdram_test(void)
{
	volatile unsigned char *p = (volatile unsigned char *)0x30000000;
	int i;

	// write sdram
	for (i = 0; i < 1000; i++)
		p[i] = 0x55;

	// read sdram
	for (i = 0; i < 1000; i++)
		if (p[i] != 0x55)
			return -1;

	return 0;
}


int isBootFromNor(void)
{
	volatile unsigned int *p = (volatile unsigned int *)0;
	unsigned int val = *p;

	*p = 0x12345678;
	if (*p == 0x12345678)
	{
		/* 写成功，程序在片内4KRAM中，是nand启动 */
		*p = val;
		return 0;
	}
	else
	{
		return 1;
	}
}


void copysdram(void)
{
	extern int _code_start, _bss_start;

	volatile unsigned int *dest = (volatile unsigned int *)&_code_start;
	volatile unsigned int *src  = (volatile unsigned int *)0;
	volatile unsigned int *end  = (volatile unsigned int *)&_bss_start;
	int len;

	len = ((int)&_bss_start) - ((int)&_code_start);

	if (isBootFromNor())
	{
		while (dest < end)
		{
			*dest++ = *src++;
		}
	}
	else
	{
		nand_init();
		nand_read((unsigned int)src, (unsigned char *)dest, len);
	}
}

void cleanbss(void)
{
	extern int 	_bss_start, _bss_end;

	volatile unsigned int *start = (volatile unsigned int *)&_bss_start;
	volatile unsigned int *end   = (volatile unsigned int *)&_bss_end;

	while (start < end)
	{
		*start++ = 0;
	}

	
}

