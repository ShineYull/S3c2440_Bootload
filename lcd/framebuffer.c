#include "lcd.h"




/* 获得LCD参数 */
static unsigned int fb_base;
static int xres, yres, bpp;

void fb_get_lcd_params(void)
{	
	get_lcd_params(&fb_base, &xres, &yres, &bpp);
}

/* parameter rgb : 0x00RRGGBB */
unsigned short convert32bppto16bpp(unsigned int rgb)
{
	int r = (rgb>>16)&0xff;
	int g = (rgb>>8)&0xff;
	int b = (rgb>>0)&0xff;

	/* convert to rgb565 format */
	r = r>>3;
	g = g>>2;
	b = b>>3;
	
	return ((r<<11) | (g<<5) | (b));
}

/* 实现画点 */
/* color : 32bit, 0x00RRGGBB, 24bpp
 *
 */
void fb_put_pixel(int x, int y, unsigned int color)
{
	unsigned char  *pc; /* 8bpp */	
	unsigned short *pw; /* 16bpp */
	unsigned int   *pdw; /* 24bpp, 32bit */

	unsigned int pixel_base = fb_base + (xres * ((bpp==24 ? 32:bpp)/8) * y) \
									  + ( x   * ((bpp==24 ? 32:bpp)/8));

	switch (bpp)
	{
		case 8:
			pc = (unsigned char *)pixel_base;
			*pc = (unsigned char)color;
			break;
			
		case 16:
			pw = (unsigned short *)pixel_base;
			*pw = convert32bppto16bpp(color);
			break;
			
		case 24:
			pdw = (unsigned int *)pixel_base;
			*pdw = color;
			break;
	}
}


void clear_screen(unsigned int color)
{	
	int x, y;
	unsigned short *p;
	unsigned int *p1;
	unsigned char *p2;

	if (bpp == 8)
	{
		/* bpp: palette[color] */
		p2 = (unsigned char *)fb_base;
		for (y=0; y<yres; y++)
			for (x=0; x<xres; x++)
				*p2++ = color & 0xff;		
	}
	else if (bpp == 16)
	{
		/* rgb565: color */
		p = (unsigned short *)fb_base;
		for (x=0; x<xres; x++)
			for (y=0; y<yres; y++)
				*p++ = convert32bppto16bpp(color);
	}
	else if (bpp == 24)
	{
		/* rgb888: color */
		p1 = (unsigned int *)fb_base;
		for (y=0; y<yres; y++)
			for (x=0; x<xres; x++)
				*p1++ = color & 0xffffff;
	}
}

void fill_color(int start_x, int start_y, int end_x, int end_y, unsigned int color)
{	
	int x, y;
	unsigned short *p;
	unsigned int *p1;
	unsigned char *p2;


	if (bpp == 8)
	{
		/* bpp: palette[color] */
		p2 = ((unsigned char *)fb_base) + start_x;
		for (y=start_y; y<end_y; y++)
		{
			for (x=start_x; x<end_x; x++)
				*p2++ = color & 0xff;		
			p2 += (end_x + start_x);
		}
	}
	else if (bpp == 16)
	{
		/* rgb565: color */
		p = ((unsigned short *)fb_base) + start_x;
		for (y=start_y; y<end_y; y++)
		{
			for (x=start_x; x<end_x; x++)
				*p++ = convert32bppto16bpp(color);
			p += (end_x + start_x);
		}
	}
	else if (bpp == 24)
	{
		/* rgb888: color */
		p1 = ((unsigned int *)fb_base) + start_x;
		for (y=start_y; y<end_y; y++)
		{
			for (x=start_x; x<end_x; x++)
				*p1++ = color & 0xffffff;
			p1 += (end_x + start_x);
		}
	}
}


/* 图片扫描 
 * 提供给图片取模软件的专用函数
 */
void scan_picture(int start_lcd_x, int start_lcd_y, int width, int higth, const unsigned char *picture, int bpp)
{
	int row, col;
	volatile unsigned int color;
	volatile int i=0;

	/* 判断bpp */
	if (bpp == 8)
	{
		for (row=start_lcd_y; row<start_lcd_y+higth; row++)
		{
			for (col=start_lcd_x; col<start_lcd_x+width; col++)
				{
					color = (picture[i++]);
					//printf("%x %x %x, ", gImage_button_up[i-3],gImage_button_up[i-2], gImage_button_up[i-1]);
					//printf("%d \n\r", color);
					fb_put_pixel(col, row, color);
				}		
		}
	}
	else if (bpp == 16)
	{
		
		for (row=start_lcd_y; row<start_lcd_y+higth; row++)
		{
			for (col=start_lcd_x; col<start_lcd_x+width; col++)
				{
					color += (picture[i++] << 8);
					color += picture[i++];
					//printf("%x %x %x, ", gImage_button_up[i-3],gImage_button_up[i-2], gImage_button_up[i-1]);
					//printf("%d \n\r", color);
					fb_put_pixel(col, row, color);
					color = 0;
				}		
		}
	}
	else if (bpp == 24)
	{
		/* 松开时按钮在lcd上的展现 */
		for (row=start_lcd_y; row<start_lcd_y+higth; row++)
		{
			for (col=start_lcd_x; col<start_lcd_x+width; col++)
				{
					/* 【问题优化】
					 * 此处出现经典的"++"混乱, 需要分开表达式, 分别自加。2019/12/14
					 */
					color += (picture[i++] <<16);
					color += (picture[i++] <<8);
					color += (picture[i++]);
					//printf("%x %x %x, ", gImage_button_up[i-3],gImage_button_up[i-2], gImage_button_up[i-1]);
					//printf("%d \n\r", color);
					fb_put_pixel(col, row, color);
					color = 0;	
				}		
		}
	}
}
