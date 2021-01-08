#include "adc.h"
#include "lcd.h"
#include "lcd_test.h"
#include "my_printf.h"
#include "font.h"
#include "framebuffer.h"

static char vol_str[13] = {"vol : "};

void adc_test(void)
{
	int val;
	double vol;
	int m;	/* 整数部分 */
	int n;	/* 小数部分 */
	
	adc_init();
	
	/* 初始化LCD */
	lcd_init();
	
	/* 使能LCD */
	lcd_enable();
	
	/* 获得LCD的参数：fb_base, xres, yres, bpp */
	fb_get_lcd_params();
	font_init();
	
	/* 选择背景颜色 */
	lcd_clear(0xff);

	while (1)
	{
		val = adc_read_ain0();
		vol = (double)val/1023*3.3;	/* value: 1023 == 3.3v */

		/* printf未实现浮点打印 */
		m = (int)(vol);	/* 整数 */
		vol = vol - m;	/* 小数部分 */
		n = (int)(vol*1000); /* 只保留小数点后三位 */
		
		/* 在串口上打印 */
		printf("vol : %d.%03dv\r", m, n); 

		/* 转换数值为字符串 */
		vol_str[6] = m+48;	/* 整数部分 */
		vol_str[7] = '.';
		vol_str[8] = (int)(n/100)+48;	/* 小数部分 */
		vol_str[9] = (int)(n%100/10)+48;
		vol_str[10] = (int)(n%10)+48;
		vol_str[11] = 'v';
		vol_str[12] = '\0';


		/* 在LCD上打印 */		
		/* 打印字符串 */
		fb_print_string(10, 10, vol_str, 0xff, 0xff0000);
	}
}

