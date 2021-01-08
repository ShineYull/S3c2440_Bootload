
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

	/* 2. ����va,pa����ҳ����Ŀ */
	index = va / 0x100000;  /* ҳ���е���һ�� */

	if (io)
		ttb[index] = (pa & 0xfff00000) | MMU_SECTION_DESCRIPTOR_FOR_IO;
	else
		ttb[index] = (pa & 0xfff00000) | MMU_SECTION_DESCRIPTOR_FOR_MEMORY;
	
}



/* ����һ��ҳ�� 
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
 *	 0x48000000   0x48000000   00		���ڼĴ�����Ҫ�õ����µ�ֵ, ���Բ�Ӧ��ʹ��cache��write buffer 
 *   .......
 * 	 0x5b00001c   0x5b00001c   00
 *   
 *   Framebuffer: 0x33c00000		��ΪҪ������ʾ��Ӳ��LCD�ϣ����Բ�ʹ��CB, ��Ϊ֮ǰ������CBΪ11��������Ҫ��������Ϊ00
 *   0x33c00000   0x33c00000   00	����0x33c00000��ʼ֮���1M�ռ�ӳ�䵽�����ַ0x33c00000
 *
 *	 link address:
 *   0xB0000000	  0x30000000   11   ����cup�����Ķ�д��ַӳ�䵽0x30000000
 *									��������ʹ�����ض�λ��0x30000000����
 */


/* ��ν����ҳ�����ǰ������ַVA����Ӧ����Ŀ����
 * 				 ��Ŀ����MMU�е���Ŀ/��������
 */
void create_page_table(void)
{
	/* 1. ҳ�����ģ�0x32000000(ռ��16KB) */
	/* ttb: translation table base */
	unsigned int *ttb = (unsigned int *)0x32000000;

	unsigned int va, pa;
	int index;


	/* ���´����Ѿ���װ��create_section_descriptor()���� */
#if 0

	va = 0;
	pa = 0;
	index = va / 0x100000;  /* ҳ���е���һ�� */
	ttb[index] = (pa & 0xfff00000) | MMU_SECTION_DESCRIPTOR_FOR_MEMORY;

#endif	


	/* 2. ����va,pa����ҳ����Ŀ */

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
	for (; va < 0x5b000000; )  /* ��Ϊӳ����1M�ռ�, ����ֻ�辫ȷ��M���� */
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


