#include "S3C2440A.h"
#include "timer.h"
#include "interrupt.h"

#define NULL ((void *) 0)
#define TIMER_NUM 32


/* 定时器服务队列 */
timer_desc timer_array[TIMER_NUM];


/* 定时器服务队列的注册 */
int register_timer(char *name, timer_func fp)
{
	int i;

	for (i=0; i<TIMER_NUM; i++)
	{
		if (!timer_array[i].fp)
		{
			timer_array[i].name = name;
			timer_array[i].fp   = fp;
			return 0;
		}
	}
	return -1;
}

/* 定时器服务队列的注销 */
int unregister_timer(char *name)
{
	int i;

	for (i=0; i<TIMER_NUM; i++)
	{
		if (!strcmp(timer_array[i].name, name))
		{
			timer_array[i].name = NULL;
			timer_array[i].fp   = NULL;
			return 0;
		}
	}
	return -1;
}


/* 定时器服务队列的注册 */
void timer_irq(void)
{
	int i;

	for (i=0; i<TIMER_NUM; i++)
	{
		if (timer_array[i].name)
		{
			timer_array[i].fp();
		}
	}
}


void init_timer(void)
{
	/* 设置TIMER0的时钟 */
	/* Timer clk = PCLK / (prescaler value+1) / (divider value) 
	 *           = 50000000/(49+1)/16
	 *			 = 62500hz
	*/
	TCFG0 = 49;	   /* prescaler 0 = 49, 用于timer0, 1 */
	TCFG1 &= ~0xf;
	TCFG1 |= 3;	   /* MUX0 : 1/16 */

	
	/* 设置TIMER0的初值 */
	TCNTB0 = 625; /* 625, 10ms中断一次 */

	/* 加载初值 */
	TCON |= (1<<1);	/* update from TCNTB0 & TCMPB0 */

	/* 设置为自动加载并启动 */
	TCON &= ~(1<<1);
	TCON |= ((1<<0) | (1<<3)); /* bit0: start, bit3: auto reload */

	/* 注册ISR */
	register_irq(10, (irq_func)timer_irq);
}


