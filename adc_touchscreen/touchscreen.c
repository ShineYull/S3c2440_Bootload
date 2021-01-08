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
	/* "�ȴ��ж�ģʽ"����, �����ô��ʰ���ʱ�����ж� */
	ADCTSC = (WAIT_PEN_DOWN | PULLUP_ENABLE | YM_ENABLE | YP_DISABLE |\
			  XP_DISABLE | XM_DISABLE | WAIT_INT_MODE);
}

void enter_wait_pen_up_mode(void)
{
	/* "�ȴ��ж�ģʽ"����, �����ô����ɿ�ʱ�����ж� */
	ADCTSC = (WAIT_PEN_UP | PULLUP_ENABLE | YM_ENABLE | YP_DISABLE |\
			  XP_DISABLE | XM_DISABLE | WAIT_INT_MODE);
}

void enter_auto_measure_mode(void)
{
	/* �����Զ���������ģʽ */
	ADCTSC = (AUTO_PST | NO_OPERATION_MODE);
}


int is_in_auto_mode(void)
{	
	return (ADCTSC & AUTO_PST);
}


void Isr_Tc(void)
{
	//printf("ADCUPDN = 0x%x, SUBSRCPND = 0x%x, SRCPND = 0x%x, INTPND = 0x%x\n\r", ADCUPDN, SUBSRCPND, SRCPND, INTPND);
	
	if (ADCDAT0 & (1<<15))	/* �ɿ� */
	{
		//printf("pen up\n\r");			
		enter_wait_pen_down_mode();				
		report_ts_xy(0, 0, 0);		

		
	}
	else	/* ���� */
	{
		//printf("pen down\n\r");

		/* ����auto (X-Y)squential convert mode */
		enter_auto_measure_mode();

		/* ����ADC */
		ADCCON |= (1<<0);
	}
}



/* ��������������ts_timer's flag for on/off  */
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


/* ÿ10ms, �ú���������һ�� 
 */
void touchscreen_timer_irq(void)
{
	/* �ж���ڱ�־�Ƿ��� */
	if (!get_status_of_ts_timer())
		return;

	if (is_in_auto_mode())
		return;

	/* ֻ����"�ȴ��ж�ģʽ"�²ſ���ʹ��ADCDAT0'BIT 15���ж�stylus status
	 * �������Զ�����ģʽ�£�ADCDAT0'BIT 15�ǲ���¼stylus status��
	 */
	
	if ( !(ADCDAT0 & (1<<15)) )	/* ��Ȼ���� */
	{
		/* ����auto (X-Y)squential convert mode */
		enter_auto_measure_mode();

		/* ����ADC */
		ADCCON |= (1<<0);
	}
	else	/* �ɿ� */
	{
		printf("timer set pen down\n\r");
	
		/* �ر���ڱ�־ */
		ts_timer_disable();

		/* 2019/12/14, Ʈ��Ʈ��~~~~, ��ʱ�����������鷳�� [YLL] */
		/* �ٴ��ж��Ƿ��£�
		 * ���Ż����⡿������ͻȻֹͣ���������Ϣ��2019/12/6.
		 *  bugԴͷ��wait stylus down���������û���������û�����Ϣ��
		 *  �������˴�����Ӧ�Լ��ر���ڣ���, 
		 *		  ��adc�ж������Ϣ���ж��Ƿ��º����ڣ��ȴ�pen up
		 *		  10ms�����˺��������ٴ��ж��Ƿ���ʱ���û������֣������ٰ���
		 *		  ���ڴ˺��timer_interrupt�У��˺������ᱻ�������ã�
		 *		  ���ԣ��粻���йر���ڣ������˷�����ʱ�䣬�ظ���ת
		 */
		enter_wait_pen_down_mode();
		report_ts_xy(0, 0, 0);
	}
	
}


void report_ts_xy(int x, int y, int pressure)
{	
	/* ��ӡ���� */
	//printf("x = 0x%08x, y = 0x%08x\n\r", x, y);

	/* �ж������Ƿ���Ч, ����Ч�򲻸������� */
	if (g_ts_data_valid == 0)
	{
		g_ts_x = x;
		g_ts_y = y;
		g_ts_pressure = pressure;
		g_ts_data_valid = 1;  /* ������֮������������Чflag */
	}
}

void ts_read_raw(int *px, int *py, int *ppressure)
{
	/* ��������Чʱѭ���ȴ� */
	while ( !g_ts_data_valid );
	*px = g_ts_x;
	*py = g_ts_y;
	*ppressure = g_ts_pressure;
	g_ts_data_valid = 0;
}

int ts_read_raw_for_sensor_button(int *px, int *py, int *ppressure)
{
	static int temp_x, temp_y;

	/* ��������Чʱ����0 */
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

	/* ����ADC�ж�ʱ��TS����"�Զ�����ģʽ" */

	/* ֻ����"�ȴ��ж�ģʽ"�²ſ���ʹ��ADCDAT0'BIT 15���ж�stylus status
	 * �������Զ�����ģʽ�£�ADCDAT0'BIT 15�ǲ���¼stylus status��
	 */
	enter_wait_pen_up_mode();


	if ( !(x & (1<<15)) ) /* ��Ȼ���� */
	{

		/* 2019/12/14, �����Ժ󻹵ø�, ����û��~~~~  [YLL] */

#if 0		
		/* ����������ȻΪ����ʱ���ϱ�TS raw value */		
		report_ts_xy( (x & 0x3ff), (y & 0x3ff), 1);
		printf("value: adc_x = %08d, adc_x = %08d.\n\r", (x & 0x3ff), (y & 0x3ff));

		/* ������TS����"�ȴ��ж�ģʽ" */
		enter_wait_pen_up_mode();

		/* ������ʱ�����ٴζ�ȡ���� */
		ts_timer_enable();
#endif


		/* ��1������ADC��
		 * a. Ҫ��������N�Σ����N�����ݣ���ƽ��ֵ���ϱ�
		 * b. �õ�N�����ݺ�������TIMER
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
			
			/* ������ʱ�����ٴζ�ȡ���� */
			/* ������TS����"�ȴ��ж�ģʽ" */
			enter_wait_pen_up_mode();
			ts_timer_enable();
		}
		else
		{
			/* �ٴ�����ADC*/
			/* ����auto (X-Y)squential convert mode */
			enter_auto_measure_mode();
			
			/* ����ADC */
			ADCCON |= (1<<0);
		}

	}
	else	/* �ɿ� */
	{
		adc_count = 0;
		adc_x = 0;
		adc_y = 0;

		/* �رն�ʱ�� */
		ts_timer_disable();

		/* �ٴ��ж��Ƿ��£�
		 * ���Ż����⡿������ͻȻֹͣ���������Ϣ��2019/12/6.
		 *  bugԴͷ��wait stylus down���������û���������û�����Ϣ��
		 *  ��������adc's ISR ����touchscreen_timer_irq��ڱ�־��
		 *		  �ʣ������һ��adc�жϲ���ʱ���û����֣��ڴ˹ر���timer���
		 *		  ֮���enter_wait_pen_up_mode����������Wait Stylus Up,
		 * 		  ���ԣ�touchscreen_timer_irq��ڱ��رգ����ɲɼ����ݣ�
		 *  	  Ӧ���ٴ�ʹwait stylus down�򿪣������Զ�����ת����
		 *		  ����adc�жϣ�����touchscreen_timer_irq��ڴ򿪡�  
		 */
		enter_wait_pen_down_mode();
		report_ts_xy(0, 0, 0);
	}

	/* �ȴ�Stylus Up */
	//enter_wait_pen_up_mode();
}


void AdcTsIntHandle(int irq)
{
	if (SUBSRCPND & (1<<TC_INT_SUBBIT))		/* ����Ǵ������ж� */
	{
		Isr_Tc();

		/* ����ж�Դ */
		SUBSRCPND |= (1<<TC_INT_SUBBIT);
	}
	if (SUBSRCPND & (1<<ADC_INT_SUBBIT))	/* �����ADC�ж� */
	{
		Isr_Adc();
		
		/* ����ж�Դ */
		SUBSRCPND |= (1<<ADC_INT_SUBBIT);
	}
	
}

void adc_ts_int_init(void)
{
	/* 2019/12/14, ���������������Ϊ�˸�����..... [YLL] */
	/* ����жϱ�־λ */
	SUBSRCPND |= (1<<TC_INT_SUBBIT) | (1<<ADC_INT_SUBBIT);

	/* ע���жϴ����� */
	register_irq(31, AdcTsIntHandle);

	/* ʹ���ж� */
	INTSUBMSK &= ~((1<<ADC_INT_SUBBIT) | (1<<TC_INT_SUBBIT));

	/* ע��ʱ��ע�ắ���Ὺ�����жϺŵ����ж�����λ */
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

	/* ���´���������ʱһ����ٷ���TC�ж�
	 * ��ʱʱ�� = ADCDLY * �������� = ADCDLY * 1/12000000 = 5ms
	 */
	ADCDLY = 60000;
}


void touchscreen_init(void)
{
	/* ��ʼ��adc�ӿ� */
	adc_ts_reg_init();
	//printf("ADCUPDN = 0x%x, SUBSRCPND = 0x%x, SRCPND = 0x%x, INTPND = 0x%x", ADCUPDN, SUBSRCPND, SRCPND, INTPND);

	/* ע�ᶨʱ�������� */
	register_timer("touchscreen", touchscreen_timer_irq);
	
	/* �����ж� */
	adc_ts_int_init();

	/* �ô���������������"�ȴ��ж�ģʽ"(wait for interrupt) */
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
	/* ��������Чʱѭ���ȴ� */
	while ( !g_ts_data_valid );	
	*px = g_ts_x;
	*py = g_ts_y;
	*ppressure = g_ts_pressure;
	print_test_array();
	g_ts_data_valid = 0;

}

