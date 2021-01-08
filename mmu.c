
#define MMU_SECTION_DESCRIPTOR_AP		(3<<10)
#define MMU_SECTION_DESCRIPTOR_DOMAIN	(0<<5)
#define MMU_SECTION_DESCRIPTOR_NCNB		(0<<2)
#define MMU_SECTION_DESCRIPTOR_WB		(3<<2)
#define MMU_SECTION_DESCRIPTOR_TYPE		((1<<4) | (1<<1))

#define MMU_SECTION_DESCRIPTOR_FOR_IO   (MMU_SECTION_DESCRIPTOR_AP 	   | \
										 MMU_SECTION_DESCRIPTOR_DOMAIN | \
										 MMU_SECTION_DESCRIPTOR_NCNB   | \
										 MMU_SECTION_DESCRIPTOR_TYPE)

#define MMU_SECTION_DESCRIPTOR_FOR_MEMORY  (MMU_SECTION_DESCRIPTOR_AP 	  | \
										 	MMU_SECTION_DESCRIPTOR_DOMAIN | \
										 	MMU_SECTION_DESCRIPTOR_WB     | \
										 	MMU_SECTION_DESCRIPTOR_TYPE)

#define IO  	1
#define MEMORY  0

void create_section_descriptor(unsigned int *ttb, unsigned int va, unsigned int pa, int io)
{
	int index;

	/* 2. 根据va,pa设置页表条目 */
	index = va / 0x100000;  /* 页表中的哪一项 */

	if (io)
		ttb[index] = (pa & 0xfff00000) | MMU_SECTION_DESCRIPTOR_FOR_IO;
	else
		ttb[index] = (pa & 0xfff00000) | MMU_SECTION_DESCRIPTOR_FOR_MEMORY;
	
}



/* 创建一级页表 
 * 	 VA		   	  PA 	       CB
 *	 0         	  0            11
 *	 0x40000000   0x40000000   11
 *
 *	 64M sdram:
 *	 0x30000000   0x30000000   11
 *	 .......
 *	 0x34000000   0x34000000   11
 *   
 *   register: 0x48000000~0x5b00001c
 *	 0x48000000   0x48000000   00		对于寄存器，要得到最新的值, 所以不应该使用cache和write buffer 
 *   .......
 * 	 0x5b00001c   0x5b00001c   00
 *   
 *   Framebuffer: 0x33c00000		因为要马上显示到硬件LCD上，所以不使用CB, 因为之前设置了CB为11，这里需要重新设置为00
 *   0x33c00000   0x33c00000   00	设置0x33c00000开始之后的1M空间映射到物理地址0x33c00000
 *
 *	 link address:
 *   0xB0000000	  0x30000000   11   设置cup发出的读写地址映射到0x30000000
 *									这样就能使程序重定位到0x30000000中了
 */


/* 所谓创建页表：就是把虚拟地址VA所对应的条目填充好
 * 				 条目就是MMU中的条目/描述符。
 */
void create_page_table(void)
{
	/* 1. 页表在哪？0x32000000(占据16KB) */
	/* ttb: translation table base */
	unsigned int *ttb = (unsigned int *)0x32000000;

	unsigned int va, pa;
	int index;


	/* 以下代码已经封装到create_section_descriptor()中了 */
#if 0

	va = 0;
	pa = 0;
	index = va / 0x100000;  /* 页表中的哪一项 */
	ttb[index] = (pa & 0xfff00000) | MMU_SECTION_DESCRIPTOR_FOR_MEMORY;

#endif	


	/* 2. 根据va,pa设置页表条目 */

	/* 2.1 for sram/nor flash */
	create_section_descriptor(ttb, 0, 0, IO);

	/* 2.2 for sram when nor boot */
	create_section_descriptor(ttb, 0x40000000, 0x40000000, MEMORY);

	/* 2.3 for 64M sdram */
	va = 0x30000000;
	pa = 0x30000000;
	for (; va < 0x34000000; )
	{
		create_section_descriptor(ttb, va, pa, MEMORY);
		va += 0x100000;
		pa += 0x100000;
	}

	/* 2.4 for register: 0x48000000~0x5b00001c */	
	va = 0x48000000;
	pa = 0x48000000;
	for (; va < 0x5b000000; )  /* 因为映射是1M空间, 所以只需精确到M即可 */
	{
		create_section_descriptor(ttb, va, pa, IO);
		va += 0x100000;
		pa += 0x100000;
	}

	/* 2.5 for Framebuffer: 0x33c00000	 */
	create_section_descriptor(ttb, 0x33c00000, 0x33c00000, IO);

	/* 2.6 for link address: va,0xB0000000 pa,0x30000000 */

	va = 0xB0000000;
	pa = 0x30000000;
	for (; va < 0xB0500000; )
	{
		create_section_descriptor(ttb, va, pa, MEMORY);
		va += 0x100000;
		pa += 0x100000;
	}

	
	//create_section_descriptor(ttb, 0xB0000000, 0x30000000, MEMORY);

}


