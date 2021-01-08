#include "S3C2440A.h"
#include "led.h"

void delay(unsigned int time)
{
	while (time--);
}

/* 每10ms, 该函数被调用一次 
 * 每500ms操作一次LED实现计数
 */
void led_timer_irq(void)
{
	/* 点灯计数 */
	static int timer_num = 0;
	static int count = 0;
	int tmp;

	timer_num++;
	if (timer_num < 50)
		return;
	timer_num = 0;
		

	count++;
	
	tmp = ~count;
	tmp &= 7;
	GPFDAT &= ~(7<<4);
	GPFDAT |=  (tmp<<4);
}


void init_led(void)
{
	GPFCON &= ~((3<<8) | (3<<10) | (3<<12));
	GPFCON |=  ((1<<8) | (1<<10) | (1<<12));
	GPFDAT |=  (7<<4);
	
	register_timer("led", led_timer_irq);
}


void led_test(void)
{
	int val = 1;

	while (1)
	{
		GPFDAT |= (7<<4);
		GPFDAT &= ~(val<<4);
		delay(100000);
		val++;

		if (val == 8)
			val = 1;
	}
}

