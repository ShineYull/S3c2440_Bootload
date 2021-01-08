#include "font_8x16.h"
#include "framebuffer.h"


/* ���LCD���� */
static unsigned int fb_base;
static int xres, yres, bpp;

/* ���ܣ���ʼ��LCD����ֵ
 */
void font_init(void)
{	
	get_lcd_params(&fb_base, &xres, &yres, &bpp);
}


/* ���ܣ�������ĸ�ĵ�����LCD���軭����
 */
void fb_print_char(int x, int y, char c, unsigned int bgcolor, unsigned int color)
{
	int col, row;

	
	/* ����c��ascii����fontdata_8x16�еõ��������� */
	const unsigned char *dots = &fontdata_8x16[c*16];

	unsigned char data;
	int bit;

	/* ���ݵ��������ö�Ӧ���ص���ɫ */
	for (row=y; row<y+16; row++)
	{
		data = *dots++;
		bit = 7;
		for (col=x; col<x+8; col++)
		{
			/* ���ݵ����е�per bit�����Ƿ������ɫ */
			if (data & (1<<bit))
				fb_put_pixel(col, row, color);
			else
				fb_put_pixel(col, row, bgcolor);
			bit--;
		}
	}
}

/* ���ܣ�����ַ���
 */
void fb_print_string(int x, int y, char *string, unsigned int bgcolor, unsigned int color)
{
	int i=0;

	while (string[i])
	{
		/* �ж��ַ��Ƿ�Ϊ����/�س��� */
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
			/* �����ַ� */
			fb_print_char(x, y, string[i], bgcolor, color);
			x += 8;

			/* ����LCD�ұ߽߱� */
			if (x >= xres)	
			{
				x = 0;
				y += 16;
			}
		}

		i++;
	}
}
