#include "lcd.h"

/* framebuffer ��ջλ�� */
#define LCD_FB_BASE 0x33c00000


lcd_params lcd_4_3_params = {

	.name = "lcd_4.3",
		
	.pins_pol = {
		.de    = NORMAL,	/* normal: �ߵ�ƽʱ���Դ������� */
		.pwren = NORMAL,	/* normal: �ߵ�ƽ��Ч */
		.vclk  = NORMAL,	/* normal��hight pluse, ���½��ػ�ȡ���� */
		.rgb   = NORMAL,	/* normal���ߵ�ƽ��ʾ1 */
		.hsync = INVERT,	/* normal��hight pluse */
		.vsync = INVERT,	/* normal��hight pluse */
	},
	.time_seq = {
		/* ��ֱ(vertical)���� */
		.tvp = 10,	/* Vsync pluse width */
		.tvb = 2, 	/* Vsync pluse back porch */
		.tvf = 2, 	/* Vsync pluse front porch */

		/* ˮƽ(horizontal)���� */
		.thp = 41,	/* Hsync pluse width */
		.thb = 2,	/* Hsync pluse back porch */
		.thf = 2,	/* Hsync pluse front porch */

		.vclk = 9,	/* MHz */
	},
	.xres = 480,
	.yres = 272,
	.bpp  = 24, /* 24 == 32bit */
	.fb_base = LCD_FB_BASE,
};

void led_4_3_add(void)	
{
	register_lcd(&lcd_4_3_params);
}

