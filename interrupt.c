#include "S3C2440A.h"
#include "interrupt.h"

irq_func irq_array[32];


/* SRCPND 用来显示哪个中断产生了，需要清除对应位
 * bit0-eint0
 * bit2-eint2
 * bit5-eint8-23
 */

/* INTMSK 用来屏蔽中断，不给置位的中断源提供中断服务 1-masked
 * bit0-eint0
 * bit2-eint2
 * bit5-eint8-23
 */

/* INTPND 用来显示当前优先级最高的、正在发生的中断，需要清除对应位
 * bit0-eint0
 * bit2-eint2
 * bit5-eint8-23
 */

/* INTOFFSET ：用来显示INTPND中哪一位被设置为1
 */


/* 初始化中断控制器 
void init_interrupt(void)	
{
	INTMSK &= ~((1<<0) | (1<<2) | (1<<5));
	INTMSK &= ~(1<<10); /* enable timer0 interrupt
}
*/



/* 初始化按键，设为中断源 */
void init_key_eint(void)	
{
	/*set GPFCON 0[1:0], 2[5:4]; S2 S3 function to EINT*/
	GPFCON &= ~((3<<0) | (3<<4));
	GPFCON |=  ((2<<0) | (2<<4));
	GPFUP  &= ~((1<<0) | (1<<2));
	
	/*set GPGCON0 3[7:6], 11[23:22]; S4 S5 function to EINT*/
	GPGCON &= ~((3<<6) | (3<<22));
	GPGCON |=  ((2<<6) | (2<<22));
	GPGUP  &= ~((1<<3) | (1<<11));

	/* seting interrupt signaling method: both edge triggered */
	EXTINT0 |= ((7<<0) | (7<<8));	/* S2 S3 */
	EXTINT1 |= (7<<12);				/* S4 */
	EXTINT2 |= (7<<12);				/* S5 */

	/* seting EINTMASK enable interrupt */
	EINTMASK &= ~((1<<11) | (1<<19));  /* EINT11, 19 */

	register_irq(0, key_eint_irq);
	register_irq(2, key_eint_irq);
	register_irq(5, key_eint_irq);
}



/* 读EINTPEND分辨哪个EINT产生（eint4~23） 
 * 清除中断时，写EINTPEND的相应位
 */ 
void key_eint_irq(int irq)
{
	unsigned int val = EINTPEND;
	unsigned int val1 = GPFDAT;
	unsigned int val2 = GPGDAT;

	if (irq == 0) 	   /* eint0 ：S2 控制 D12*/
	{
		if (val1 & (1<<0)) /* S2 --> gpf6 */
		{
			/* 松开 */
			GPFDAT |= (1<<6);
		}
		else
		{
			/* 按下 */
			GPFDAT &= ~(1<<6);
		}
	}
	
	else if (irq == 2) /* eint2 ：S3 控制 D11 */
	{
		if (val1 & (1<<2)) /* S3 --> gpf5 */
		{
			/* 松开 */
			GPFDAT |= (1<<5);
		}
		else
		{
			/* 按下 */
			GPFDAT &= ~(1<<5);
		}
	}
	
	else if (irq == 5) /* eint8_23, eint11--S4 控制 D10, eint19--S5 控制所有LED*/
	{
		if (val & (1<<11)) /* eint11 */
		{
			if (val2 & (1<<3)) /* S4 --> gpf4 */
			{
				/* 松开 */
				GPFDAT |= (1<<4);
			}
			else
			{
				/* 按下 */
				GPFDAT &= ~(1<<4);
			}
		}
		
		else if (val & (1<<19)) /* eint19 */
		{
			if (val2 & (1<<11))
			{
				/* 松开 */
				/* 熄灭所有的LEDs */
				GPFDAT |= (7<<4);
			}
			else
			{
				/* 按下：点亮所有LED */
				GPFDAT &= ~(7<<4);
			}
		}
	}

	EINTPEND = val;
}

void handle_irq_c(void)
{
	/* 分辨中断源 */
	int bit = INTOFFSET;

	/* 调用对应的处理函数 */
	irq_array[bit](bit);
	
	/* 请中断 ：从源头开始清 */
	SRCPND |= (1<<bit);
	INTPND |= (1<<bit);
	
}


void register_irq(int irq, irq_func fp)
{
	irq_array[irq] = fp;

	/* ISR注册时，同时初始化中断控制器的MASK */
	INTMSK &= ~(1<<irq);

	/* 初始化优先级PRIORITY */
	/* 没必要，需要时设置 */
}







