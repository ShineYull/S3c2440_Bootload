#include "font_8x16.h"
#include "framebuffer.h"


/* 获得LCD参数 */
static unsigned int fb_base;
static int xres, yres, bpp;

/* 功能：初始化LCD参数值
 */
void font_init(void)
{	
	get_lcd_params(&fb_base, &xres, &yres, &bpp);
}


/* 功能：根据字母的点阵在LCD上描画文字
 */
void fb_print_char(int x, int y, char c, unsigned int bgcolor, unsigned int color)
{
	int col, row;

	
	/* 根据c的ascii码在fontdata_8x16中得到点阵数据 */
	const unsigned char *dots = &fontdata_8x16[c*16];

	unsigned char data;
	int bit;

	/* 根据点阵来设置对应像素的颜色 */
	for (row=y; row<y+16; row++)
	{
		data = *dots++;
		bit = 7;
		for (col=x; col<x+8; col++)
		{
			/* 根据点阵中的per bit决定是否描绘颜色 */
			if (data & (1<<bit))
				fb_put_pixel(col, row, color);
			else
				fb_put_pixel(col, row, bgcolor);
			bit--;
		}
	}
}

/* 功能：描绘字符串
 */
void fb_print_string(int x, int y, char *string, unsigned int bgcolor, unsigned int color)
{
	int i=0;

	while (string[i])
	{
		/* 判断字符是否为换行/回车符 */
		if (string[i]=='\n')
		{
			y += 16;
		}
		else if (string[i]=='\r')
		{
			x = 0;
		}
		else
		{
			/* 绘制字符 */
			fb_print_char(x, y, string[i], bgcolor, color);
			x += 8;

			/* 超出LCD右边边界 */
			if (x >= xres)	
			{
				x = 0;
				y += 16;
			}
		}

		i++;
	}
}
