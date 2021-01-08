//#include "../include/S3C2440A.h"
#include "S3C2440A.h"

void adc_init(void)
{
	/* ECFLG  [15], 1 = End of A/D conversion
	 * PRSCEN [14], 1 = A/D converter prescaler enable
	 * PRSCVL [13:6], adc clk = PCLK / (PRSCVL+1)
	 * SEL_MUX [5:3], 000 = AIN0
	 * STDBM   [2], 1 = Standby mode
	 * ENABLE_START [0], 1 = A/D conversion starts and this bit is cleared after the startup
	 */
	ADCCON = ((1<<14) | (49<<6) | (0<<3) | (0<<2));

	/* 取默认值 */
	ADCDLY = (0xff);
}

int adc_read_ain0(void)
{
	/* 启动ADC */
	ADCCON |= (1<<0);

	/* 查看ADC converter status: 1==busy */
	while (!(ADCCON & (1<<15)));

	return (ADCDAT0 & 0x3ff);
}
