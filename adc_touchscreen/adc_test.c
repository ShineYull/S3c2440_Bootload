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
	int m;	/* �������� */
	int n;	/* С������ */
	
	adc_init();
	
	/* ��ʼ��LCD */
	lcd_init();
	
	/* ʹ��LCD */
	lcd_enable();
	
	/* ���LCD�Ĳ�����fb_base, xres, yres, bpp */
	fb_get_lcd_params();
	font_init();
	
	/* ѡ�񱳾���ɫ */
	lcd_clear(0xff);

	while (1)
	{
		val = adc_read_ain0();
		vol = (double)val/1023*3.3;	/* value: 1023 == 3.3v */

		/* printfδʵ�ָ����ӡ */
		m = (int)(vol);	/* ���� */
		vol = vol - m;	/* С������ */
		n = (int)(vol*1000); /* ֻ����С�������λ */
		
		/* �ڴ����ϴ�ӡ */
		printf("vol : %d.%03dv\r", m, n); 

		/* ת����ֵΪ�ַ��� */
		vol_str[6] = m+48;	/* �������� */
		vol_str[7] = '.';
		vol_str[8] = (int)(n/100)+48;	/* С������ */
		vol_str[9] = (int)(n%100/10)+48;
		vol_str[10] = (int)(n%10)+48;
		vol_str[11] = 'v';
		vol_str[12] = '\0';


		/* ��LCD�ϴ�ӡ */		
		/* ��ӡ�ַ��� */
		fb_print_string(10, 10, vol_str, 0xff, 0xff0000);
	}
}

