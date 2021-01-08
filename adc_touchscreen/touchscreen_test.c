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
	/* 初始化TS */
	touchscreen_init();

	/* 获得LCD的参数：fb_base, xres, yres, bpp */
	get_lcd_params(&fb_base, &xres, &yres, &bpp);
	fb_get_lcd_params();
	font_init();

	/* 清屏 */
	clear_screen(0x0);
	/*************************************************************/

	
	/*************************************************************/

	/* 显示文字提示校准 */
	fb_print_string(70, 70, "Touch cross to calibrate Touchscreen!", 0x00, 0xffffff);
	ts_calibrate();

	/* 显示文字提示绘画 */
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
	/* 初始化TS */
	touchscreen_init();

	/* 获取lcd供清屏使用 */
	fb_get_lcd_params();

	/* 清屏 */
	clear_screen(0x0);

	/* 获得LCD的参数：fb_base, xres, yres, bpp */
	get_lcd_params(&fb_base, &xres, &yres, &bpp);
	font_init();

	/* 显示文字提示校准 */
	fb_print_string(70, 70, "Touch cross to calibrate Touchscreen!", 0x00, 0xffffff);
	ts_calibrate();

	/* 显示文字提示校准完成 */
	fb_print_string(110, yres-70, "The calibration completed!", 0x00, 0xffffff);

	
}
