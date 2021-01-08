#include "touchscreen.h"
#include "framebuffer.h"
#include "ts_calibrate.h"
#include "geometry.h"
#include "my_printf.h"
#include "font.h"

unsigned int fb_base;
int xres, yres, bpp;

void touchscreen_test(void)
{
	int lcd_x, lcd_y, pressure;

	/*************************************************************/
	/* ��ʼ��TS */
	touchscreen_init();

	/* ���LCD�Ĳ�����fb_base, xres, yres, bpp */
	get_lcd_params(&fb_base, &xres, &yres, &bpp);
	fb_get_lcd_params();
	font_init();

	/* ���� */
	clear_screen(0x0);
	/*************************************************************/

	
	/*************************************************************/

	/* ��ʾ������ʾУ׼ */
	fb_print_string(70, 70, "Touch cross to calibrate Touchscreen!", 0x00, 0xffffff);
	ts_calibrate();

	/* ��ʾ������ʾ�滭 */
	fb_print_string(90, yres-70, "OK! To draw!", 0x00, 0xffffff);
	/*************************************************************/


	while (1)
	{
		if (ts_read(&lcd_x, &lcd_y, &pressure) == 0)
		{
			printf("x = %d, y = %d.\n\r", lcd_x, lcd_y);

			if (pressure)
			{
				fb_put_pixel(lcd_x, lcd_y, 0xff00);
			}
		}
	}
}

void touchscreen_on(void)
{
	/* ��ʼ��TS */
	touchscreen_init();

	/* ��ȡlcd������ʹ�� */
	fb_get_lcd_params();

	/* ���� */
	clear_screen(0x0);

	/* ���LCD�Ĳ�����fb_base, xres, yres, bpp */
	get_lcd_params(&fb_base, &xres, &yres, &bpp);
	font_init();

	/* ��ʾ������ʾУ׼ */
	fb_print_string(70, 70, "Touch cross to calibrate Touchscreen!", 0x00, 0xffffff);
	ts_calibrate();

	/* ��ʾ������ʾУ׼��� */
	fb_print_string(110, yres-70, "The calibration completed!", 0x00, 0xffffff);

	
}
