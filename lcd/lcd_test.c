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

	/* ��ʼ��LCD */
	lcd_init();

	/* ʹ��LCD */
	lcd_enable();

	/* ���LCD�Ĳ�����fb_base, xres, yres, bpp */
	get_lcd_params(&fb_base, &xres, &yres, &bpp);
	fb_get_lcd_params();
	font_init();

	/* ��framebuffer��д���� */
	
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
		/* ��LCD��������ĺ�ɫ */
		/* rgb565: 0xf800 */
		p = (unsigned short *)fb_base;
		for (x=0; x<xres; x++)
			for (y=0; y<yres; y++)
				*p++ = 0xf800;
			
		/* ��LCD�����������ɫ */	
		/* rgb565: 0x7e0 */
		p = (unsigned short *)fb_base;
		for (x=0; x<xres; x++)
			for (y=0; y<yres; y++)
				*p++ = 0x7e0;
			
		/* ��LCD�����������ɫ */	
		/* rgb565: 0x1f */
		p = (unsigned short *)fb_base;
		for (x=0; x<xres; x++)
			for (y=0; y<yres; y++)
				*p++ = 0x1f;
	}
	else if (bpp == 24)
	{
		/* ��LCD��������ĺ�ɫ */
		/* rgb888: 0xff0000 */
		p1 = (unsigned int *)fb_base;
		for (y=0; y<yres; y++)
			for (x=0; x<xres; x++)
				*p1++ = 0xff0000;
			
		/* ��LCD�����������ɫ */	
		/* rgb888: 0x00ff00 */
		p1 = (unsigned int *)fb_base;
		for (y=0; y<yres; y++)
			for (x=0; x<xres; x++)
				*p1++ = 0x00ff00;
			
		/* ��LCD�����������ɫ */	
		/* rgb888: 0x0000ff */
		p1 = (unsigned int *)fb_base;
		for (y=0; y<yres; y++)
			for (x=0; x<xres; x++)
				*p1++ = 0x0000ff;

		
		/* ��LCD���������ѡ����ɫ 	*/
		/* rgb888: 0x000000�� */
		p1 = (unsigned int *)fb_base;
		for (y=0; y<yres; y++)
			for (x=0; x<xres; x++)
				*p1++ = 0x000000;
		
	}

	delay(1000000);
	
	/* ���� */
	draw_line(0, 0, xres-1, 0, 0xff0000);
	draw_line(xres-1, 0, xres-1, yres-1, 0xffff00);
	draw_line(0, yres-1, xres-1, yres-1, 0xff00aa);
	draw_line(0, 0, 0, yres-1, 0xff00ef);
	draw_line(0, 0, xres- 1, yres-1, 0xff4500);
	draw_line(xres-1, 0, 0, yres-1, 0xff0780);

	delay(1000000);

	/* ��Բ */
	draw_circle(xres/2, yres/2, yres/4, 0xff00);

	/* ���Ӣ���ַ� */
	fb_print_string(10, 10, "Test LCD print string function\n\rAuthor YLL"\
							"\n\rversion: 1.0", 0x000000, 0xff);
}

/* ����������
 * �˺����ṩ��24bpp mode ʹ��
 */
void lcd_clear(unsigned int color)
{		
	unsigned int *p1;
	int x, y;
	
	/* ���LCD�Ĳ�����fb_base, xres, yres, bpp */
	get_lcd_params(&fb_base, &xres, &yres, &bpp);

	/* ��LCD���������ѡ����ɫ	*/
	/* rgb888: 0x000000 */
	p1 = (unsigned int *)fb_base;
	for (y=0; y<yres; y++)
		for (x=0; x<xres; x++)
			*p1++ = color & 0xffffff;
}


