#include "S3C2440A.h"
#include "interrupt.h"
#include "my_printf.h"
#include "timer.h"


#define ADC_INT_SUBBIT (10)
#define TC_INT_SUBBIT  (9)
#define INT_ADC_TC	(31)

/* ADCTSC's bits */
#define WAIT_PEN_DOWN  (0<<8)
#define WAIT_PEN_UP	   (1<<8)

#define XP_ENABLE      (0<<4)
#define XP_DISABLE     (1<<4)

#define XM_ENABLE      (1<<5)
#define XM_DISABLE     (0<<5)

#define YP_ENABLE      (0<<6)
#define YP_DISABLE 	   (1<<6)

#define YM_ENABLE  	   (1<<7)
#define YM_DISABLE 	   (0<<7)

#define PULLUP_ENABLE  (0<<3)
#define PULLUP_DISABLE (1<<3)

#define AUTO_PST 	   (1<<2)

#define WAIT_INT_MODE  (3<<0)
#define NO_OPERATION_MODE  (0<<0)

static volatile char g_ts_timer_enable;
static volatile char g_ts_data_valid;
static int g_ts_x;
static int g_ts_y;
static int g_ts_pressure = 0;

static int test_x_array[16];
static int test_y_array[16];

void report_ts_xy(int x, int y, int pressure);
static inline void ts_timer_disable(void);


void enter_wait_pen_down_mode(void)
{
	/* "等待中断模式"设置, 并设置触笔按下时产生中断 */
	ADCTSC = (WAIT_PEN_DOWN | PULLUP_ENABLE | YM_ENABLE | YP_DISABLE |\
			  XP_DISABLE | XM_DISABLE | WAIT_INT_MODE);
}

void enter_wait_pen_up_mode(void)
{
	/* "等待中断模式"设置, 并设置触笔松开时产生中断 */
	ADCTSC = (WAIT_PEN_UP | PULLUP_ENABLE | YM_ENABLE | YP_DISABLE |\
			  XP_DISABLE | XM_DISABLE | WAIT_INT_MODE);
}

void enter_auto_measure_mode(void)
{
	/* 进入自动连续测量模式 */
	ADCTSC = (AUTO_PST | NO_OPERATION_MODE);
}


int is_in_auto_mode(void)
{	
	return (ADCTSC & AUTO_PST);
}


void Isr_Tc(void)
{
	//printf("ADCUPDN = 0x%x, SUBSRCPND = 0x%x, SRCPND = 0x%x, INTPND = 0x%x\n\r", ADCUPDN, SUBSRCPND, SRCPND, INTPND);
	
	if (ADCDAT0 & (1<<15))	/* 松开 */
	{
		//printf("pen up\n\r");			
		enter_wait_pen_down_mode();				
		report_ts_xy(0, 0, 0);		

		
	}
	else	/* 按下 */
	{
		//printf("pen down\n\r");

		/* 进入auto (X-Y)squential convert mode */
		enter_auto_measure_mode();

		/* 启动ADC */
		ADCCON |= (1<<0);
	}
}



/* 以下三个函数：ts_timer's flag for on/off  */
static inline void ts_timer_enable(void)
{
	g_ts_timer_enable = 1;
}


static inline void ts_timer_disable(void)
{
	g_ts_timer_enable = 0;
}

static inline int get_status_of_ts_timer(void)
{
	return g_ts_timer_enable;
}


/* 每10ms, 该函数被调用一次 
 */
void touchscreen_timer_irq(void)
{
	/* 判断入口标志是否开启 */
	if (!get_status_of_ts_timer())
		return;

	if (is_in_auto_mode())
		return;

	/* 只有在"等待中断模式"下才可以使用ADCDAT0'BIT 15来判断stylus status
	 * 即：在自动测量模式下，ADCDAT0'BIT 15是不记录stylus status的
	 */
	
	if ( !(ADCDAT0 & (1<<15)) )	/* 任然按下 */
	{
		/* 进入auto (X-Y)squential convert mode */
		enter_auto_measure_mode();

		/* 启动ADC */
		ADCCON |= (1<<0);
	}
	else	/* 松开 */
	{
		printf("timer set pen down\n\r");
	
		/* 关闭入口标志 */
		ts_timer_disable();

		/* 2019/12/14, 飘过飘过~~~~, 当时调试这里真麻烦。 [YLL] */
		/* 再次判断是否按下，
		 * 【优化问题】：长按突然停止输出坐标信息，2019/12/6.
		 *  bug源头：wait stylus down不触发，用户持续按下没输出信息。
		 *  分析：此处本不应自己关闭入口，但, 
		 *		  当adc中断输出信息，判断是否按下后打开入口，等待pen up
		 *		  10ms后进入此函数，当再次判断是否按下时，用户已松手，并不再按下
		 *		  则，在此后的timer_interrupt中，此函数都会被反复调用，
		 *		  所以，如不自行关闭入口，则是浪费运行时间，重复空转
		 */
		enter_wait_pen_down_mode();
		report_ts_xy(0, 0, 0);
	}
	
}


void report_ts_xy(int x, int y, int pressure)
{	
	/* 打印坐标 */
	//printf("x = 0x%08x, y = 0x%08x\n\r", x, y);

	/* 判断数据是否有效, 若有效则不更改数据 */
	if (g_ts_data_valid == 0)
	{
		g_ts_x = x;
		g_ts_y = y;
		g_ts_pressure = pressure;
		g_ts_data_valid = 1;  /* 更改完之后设置数据有效flag */
	}
}

void ts_read_raw(int *px, int *py, int *ppressure)
{
	/* 当数据无效时循环等待 */
	while ( !g_ts_data_valid );
	*px = g_ts_x;
	*py = g_ts_y;
	*ppressure = g_ts_pressure;
	g_ts_data_valid = 0;
}

int ts_read_raw_for_sensor_button(int *px, int *py, int *ppressure)
{
	static int temp_x, temp_y;

	/* 当数据无效时返回0 */
	if ( g_ts_data_valid )
	{
		if (g_ts_x != 0 && g_ts_y != 0)
		{
			*px = g_ts_x;
			*py = g_ts_y;
			*ppressure = g_ts_pressure;
		}
		else
		{
			*px = temp_x;
			*py = temp_y;
			*ppressure = 0;
		}
		g_ts_data_valid = 0;

		temp_x = *px;
		temp_y = *py;
		printf("*px = %d, *py = %d, ppressure = %d.\n\r", *px, *py, *ppressure);
		return 1;
	}
	else
		return 0;
}


void Isr_Adc(void)
{
	int x = ADCDAT0;
	int y = ADCDAT1;

	static int adc_count = 0;
	static int adc_x = 0;
	static int adc_y = 0;
	static int q_adc_x = 0;
	static int q_adc_y = 0;

	/* 进入ADC中断时，TS处于"自动测量模式" */

	/* 只有在"等待中断模式"下才可以使用ADCDAT0'BIT 15来判断stylus status
	 * 即：在自动测量模式下，ADCDAT0'BIT 15是不记录stylus status的
	 */
	enter_wait_pen_up_mode();


	if ( !(x & (1<<15)) ) /* 任然按下 */
	{

		/* 2019/12/14, 这里以后还得改, 现在没空~~~~  [YLL] */

#if 0		
		/* 当触摸屏任然为按下时，上报TS raw value */		
		report_ts_xy( (x & 0x3ff), (y & 0x3ff), 1);
		printf("value: adc_x = %08d, adc_x = %08d.\n\r", (x & 0x3ff), (y & 0x3ff));

		/* 先设置TS进入"等待中断模式" */
		enter_wait_pen_up_mode();

		/* 启动定时器以再次读取数据 */
		ts_timer_enable();
#endif


		/* 第1次启动ADC后：
		 * a. 要连续启动N次，获得N个数据，求平均值并上报
		 * b. 得到N次数据后，再启动TIMER
		 */
		adc_x += (x & 0x3ff);
		adc_y += (y & 0x3ff);

		test_x_array[adc_count] = (x & 0x3ff);
		test_y_array[adc_count] = (y & 0x3ff);
		
		adc_count++;

		if (adc_count == 5)
		{
			adc_x >>= 4;
			adc_y >>= 4;
	
			printf("average value: adc_x = %08d, adc_x = %08d.\n\r", q_adc_x, q_adc_y);
			report_ts_xy(q_adc_x, q_adc_y, 1);
			q_adc_x = adc_x;
			q_adc_y = adc_y;
			
			adc_count = 0;
			adc_x = 0;
			adc_y = 0;
			
			/* 启动定时器以再次读取数据 */
			/* 先设置TS进入"等待中断模式" */
			enter_wait_pen_up_mode();
			ts_timer_enable();
		}
		else
		{
			/* 再次启动ADC*/
			/* 进入auto (X-Y)squential convert mode */
			enter_auto_measure_mode();
			
			/* 启动ADC */
			ADCCON |= (1<<0);
		}

	}
	else	/* 松开 */
	{
		adc_count = 0;
		adc_x = 0;
		adc_y = 0;

		/* 关闭定时器 */
		ts_timer_disable();

		/* 再次判断是否按下，
		 * 【优化问题】：长按突然停止输出坐标信息，2019/12/6.
		 *  bug源头：wait stylus down不触发，用户持续按下没输出信息。
		 *  分析：由adc's ISR 控制touchscreen_timer_irq入口标志，
		 *		  故，当最后一次adc中断产生时，用户松手，在此关闭了timer入口
		 *		  之后的enter_wait_pen_up_mode函数将设置Wait Stylus Up,
		 * 		  所以，touchscreen_timer_irq入口被关闭，不可采集数据；
		 *  	  应该再次使wait stylus down打开，触发自动连续转换，
		 *		  进入adc中断，设置touchscreen_timer_irq入口打开。  
		 */
		enter_wait_pen_down_mode();
		report_ts_xy(0, 0, 0);
	}

	/* 等待Stylus Up */
	//enter_wait_pen_up_mode();
}


void AdcTsIntHandle(int irq)
{
	if (SUBSRCPND & (1<<TC_INT_SUBBIT))		/* 如果是触摸屏中断 */
	{
		Isr_Tc();

		/* 清除中断源 */
		SUBSRCPND |= (1<<TC_INT_SUBBIT);
	}
	if (SUBSRCPND & (1<<ADC_INT_SUBBIT))	/* 如果是ADC中断 */
	{
		Isr_Adc();
		
		/* 清除中断源 */
		SUBSRCPND |= (1<<ADC_INT_SUBBIT);
	}
	
}

void adc_ts_int_init(void)
{
	/* 2019/12/14, 忘记了这里清除是为了干嘛了..... [YLL] */
	/* 清除中断标志位 */
	SUBSRCPND |= (1<<TC_INT_SUBBIT) | (1<<ADC_INT_SUBBIT);

	/* 注册中断处理函数 */
	register_irq(31, AdcTsIntHandle);

	/* 使能中断 */
	INTSUBMSK &= ~((1<<ADC_INT_SUBBIT) | (1<<TC_INT_SUBBIT));

	/* 注册时，注册函数会开启该中断号的总中断屏蔽位 */
	//INTMSK    &= ~(1<<INT_ADC_TC);
}


void adc_ts_reg_init(void)
{
	/* ECFLG  [15], 1 = End of A/D conversion
	 * PRSCEN [14], 1 = A/D converter prescaler enable
	 * PRSCVL [13:6], adc clk = PCLK / (PRSCVL+1)
	 * SEL_MUX [5:3], 000 = AIN0
	 * STDBM   [2], 1 = Standby mode
	 * ENABLE_START [0], 1 = A/D conversion starts and this bit is cleared after the startup
	 */
	ADCCON = ((1<<14) | (49<<6) | (0<<3) | (0<<2));

	/* 按下触摸屏，延时一会儿再发出TC中断
	 * 延时时间 = ADCDLY * 晶振周期 = ADCDLY * 1/12000000 = 5ms
	 */
	ADCDLY = 60000;
}


void touchscreen_init(void)
{
	/* 初始化adc接口 */
	adc_ts_reg_init();
	//printf("ADCUPDN = 0x%x, SUBSRCPND = 0x%x, SRCPND = 0x%x, INTPND = 0x%x", ADCUPDN, SUBSRCPND, SRCPND, INTPND);

	/* 注册定时器处理函数 */
	register_timer("touchscreen", touchscreen_timer_irq);
	
	/* 设置中断 */
	adc_ts_int_init();

	/* 让触摸屏控制器进入"等待中断模式"(wait for interrupt) */
	enter_wait_pen_down_mode();
}

void print_test_array(void)
{
	int i;

	printf("test array x : ");
	for (i=0; i<16; i++)
	{
		printf("%08d  ", test_x_array[i]);
	}
	printf("\n\r");
	
	printf("test array y : ");
	for (i=0; i<16; i++)
	{
		printf("%08d  ", test_y_array[i]);
	}
	printf("\n\r");
}

int ts_read_raw_test(int *px, int *py, int *ppressure)
{
	/* 当数据无效时循环等待 */
	while ( !g_ts_data_valid );	
	*px = g_ts_x;
	*py = g_ts_y;
	*ppressure = g_ts_pressure;
	print_test_array();
	g_ts_data_valid = 0;

}

