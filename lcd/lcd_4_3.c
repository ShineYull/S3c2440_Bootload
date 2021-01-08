#include "lcd.h"

/* framebuffer 的栈位置 */
#define LCD_FB_BASE 0x33c00000


lcd_params lcd_4_3_params = {

	.name = "lcd_4.3",
		
	.pins_pol = {
		.de    = NORMAL,	/* normal: 高电平时可以传输数据 */
		.pwren = NORMAL,	/* normal: 高电平有效 */
		.vclk  = NORMAL,	/* normal：hight pluse, 在下降沿获取数据 */
		.rgb   = NORMAL,	/* normal：高电平表示1 */
		.hsync = INVERT,	/* normal：hight pluse */
		.vsync = INVERT,	/* normal：hight pluse */
	},
	.time_seq = {
		/* 垂直(vertical)方向 */
		.tvp = 10,	/* Vsync pluse width */
		.tvb = 2, 	/* Vsync pluse back porch */
		.tvf = 2, 	/* Vsync pluse front porch */

		/* 水平(horizontal)方向 */
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

