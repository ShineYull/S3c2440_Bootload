#include "lcd.h"
#include "geometry.h"
#include "framebuffer.h"
#include "led.h"
#include "font.h"

static unsigned int fb_base;
static int xres, yres, bpp;

void lcd_test(void)
{
	int x, y;
	unsigned short *p;
	unsigned int *p1;
	unsigned char *p2;

	/* 初始化LCD */
	lcd_init();

	/* 使能LCD */
	lcd_enable();

	/* 获得LCD的参数：fb_base, xres, yres, bpp */
	get_lcd_params(&fb_base, &xres, &yres, &bpp);
	fb_get_lcd_params();
	font_init();

	/* 往framebuffer中写数据 */
	
	if (bpp == 8)
	{
		/* bpp: palette[208] */
		p2 = (unsigned char *)fb_base;
		for (y=0; y<yres; y++)
			for (x=0; x<xres; x++)
				*p2++ = 208;
			
		/* bpp: palette[86] */
		p2 = (unsigned char *)fb_base;
		for (y=0; y<yres; y++)
			for (x=0; x<xres; x++)
				*p2++ = 86;
			
		/* bpp: palette[102] */
		p2 = (unsigned char *)fb_base;
		for (y=0; y<yres; y++)
			for (x=0; x<xres; x++)
				*p2++ = 102;

		
		/* bpp: palette[8] */
		p2 = (unsigned char *)fb_base;
		for (y=0; y<yres; y++)
			for (x=0; x<xres; x++)
				*p2++ = 8;
		
	}
	else if (bpp == 16)
	{
		/* 让LCD输出整屏的红色 */
		/* rgb565: 0xf800 */
		p = (unsigned short *)fb_base;
		for (x=0; x<xres; x++)
			for (y=0; y<yres; y++)
				*p++ = 0xf800;
			
		/* 让LCD输出整屏的绿色 */	
		/* rgb565: 0x7e0 */
		p = (unsigned short *)fb_base;
		for (x=0; x<xres; x++)
			for (y=0; y<yres; y++)
				*p++ = 0x7e0;
			
		/* 让LCD输出整屏的蓝色 */	
		/* rgb565: 0x1f */
		p = (unsigned short *)fb_base;
		for (x=0; x<xres; x++)
			for (y=0; y<yres; y++)
				*p++ = 0x1f;
	}
	else if (bpp == 24)
	{
		/* 让LCD输出整屏的红色 */
		/* rgb888: 0xff0000 */
		p1 = (unsigned int *)fb_base;
		for (y=0; y<yres; y++)
			for (x=0; x<xres; x++)
				*p1++ = 0xff0000;
			
		/* 让LCD输出整屏的绿色 */	
		/* rgb888: 0x00ff00 */
		p1 = (unsigned int *)fb_base;
		for (y=0; y<yres; y++)
			for (x=0; x<xres; x++)
				*p1++ = 0x00ff00;
			
		/* 让LCD输出整屏的蓝色 */	
		/* rgb888: 0x0000ff */
		p1 = (unsigned int *)fb_base;
		for (y=0; y<yres; y++)
			for (x=0; x<xres; x++)
				*p1++ = 0x0000ff;

		
		/* 让LCD输出整屏的选定颜色 	*/
		/* rgb888: 0x000000黑 */
		p1 = (unsigned int *)fb_base;
		for (y=0; y<yres; y++)
			for (x=0; x<xres; x++)
				*p1++ = 0x000000;
		
	}

	delay(1000000);
	
	/* 画线 */
	draw_line(0, 0, xres-1, 0, 0xff0000);
	draw_line(xres-1, 0, xres-1, yres-1, 0xffff00);
	draw_line(0, yres-1, xres-1, yres-1, 0xff00aa);
	draw_line(0, 0, 0, yres-1, 0xff00ef);
	draw_line(0, 0, xres- 1, yres-1, 0xff4500);
	draw_line(xres-1, 0, 0, yres-1, 0xff0780);

	delay(1000000);

	/* 画圆 */
	draw_circle(xres/2, yres/2, yres/4, 0xff00);

	/* 输出英文字符 */
	fb_print_string(10, 10, "Test LCD print string function\n\rAuthor YLL"\
							"\n\rversion: 1.0", 0x000000, 0xff);
}

/* 测试用例：
 * 此函数提供给24bpp mode 使用
 */
void lcd_clear(unsigned int color)
{		
	unsigned int *p1;
	int x, y;
	
	/* 获得LCD的参数：fb_base, xres, yres, bpp */
	get_lcd_params(&fb_base, &xres, &yres, &bpp);

	/* 让LCD输出整屏的选定颜色	*/
	/* rgb888: 0x000000 */
	p1 = (unsigned int *)fb_base;
	for (y=0; y<yres; y++)
		for (x=0; x<xres; x++)
			*p1++ = color & 0xffffff;
}


