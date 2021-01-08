#include "S3C2440A.h"
#include "interrupt.h"

irq_func irq_array[32];


/* SRCPND ������ʾ�ĸ��жϲ����ˣ���Ҫ�����Ӧλ
 * bit0-eint0
 * bit2-eint2
 * bit5-eint8-23
 */

/* INTMSK ���������жϣ�������λ���ж�Դ�ṩ�жϷ��� 1-masked
 * bit0-eint0
 * bit2-eint2
 * bit5-eint8-23
 */

/* INTPND ������ʾ��ǰ���ȼ���ߵġ����ڷ������жϣ���Ҫ�����Ӧλ
 * bit0-eint0
 * bit2-eint2
 * bit5-eint8-23
 */

/* INTOFFSET ��������ʾINTPND����һλ������Ϊ1
 */


/* ��ʼ���жϿ����� 
void init_interrupt(void)	
{
	INTMSK &= ~((1<<0) | (1<<2) | (1<<5));
	INTMSK &= ~(1<<10); /* enable timer0 interrupt
}
*/



/* ��ʼ����������Ϊ�ж�Դ */
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



/* ��EINTPEND�ֱ��ĸ�EINT������eint4~23�� 
 * ����ж�ʱ��дEINTPEND����Ӧλ
 */ 
void key_eint_irq(int irq)
{
	unsigned int val = EINTPEND;
	unsigned int val1 = GPFDAT;
	unsigned int val2 = GPGDAT;

	if (irq == 0) 	   /* eint0 ��S2 ���� D12*/
	{
		if (val1 & (1<<0)) /* S2 --> gpf6 */
		{
			/* �ɿ� */
			GPFDAT |= (1<<6);
		}
		else
		{
			/* ���� */
			GPFDAT &= ~(1<<6);
		}
	}
	
	else if (irq == 2) /* eint2 ��S3 ���� D11 */
	{
		if (val1 & (1<<2)) /* S3 --> gpf5 */
		{
			/* �ɿ� */
			GPFDAT |= (1<<5);
		}
		else
		{
			/* ���� */
			GPFDAT &= ~(1<<5);
		}
	}
	
	else if (irq == 5) /* eint8_23, eint11--S4 ���� D10, eint19--S5 ��������LED*/
	{
		if (val & (1<<11)) /* eint11 */
		{
			if (val2 & (1<<3)) /* S4 --> gpf4 */
			{
				/* �ɿ� */
				GPFDAT |= (1<<4);
			}
			else
			{
				/* ���� */
				GPFDAT &= ~(1<<4);
			}
		}
		
		else if (val & (1<<19)) /* eint19 */
		{
			if (val2 & (1<<11))
			{
				/* �ɿ� */
				/* Ϩ�����е�LEDs */
				GPFDAT |= (7<<4);
			}
			else
			{
				/* ���£���������LED */
				GPFDAT &= ~(7<<4);
			}
		}
	}

	EINTPEND = val;
}

void handle_irq_c(void)
{
	/* �ֱ��ж�Դ */
	int bit = INTOFFSET;

	/* ���ö�Ӧ�Ĵ����� */
	irq_array[bit](bit);
	
	/* ���ж� ����Դͷ��ʼ�� */
	SRCPND |= (1<<bit);
	INTPND |= (1<<bit);
	
}


void register_irq(int irq, irq_func fp)
{
	irq_array[irq] = fp;

	/* ISRע��ʱ��ͬʱ��ʼ���жϿ�������MASK */
	INTMSK &= ~(1<<irq);

	/* ��ʼ�����ȼ�PRIORITY */
	/* û��Ҫ����Ҫʱ���� */
}







