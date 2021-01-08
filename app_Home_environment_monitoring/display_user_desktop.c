#include "app_test.h"
#include "framebuffer.h"
#include "led.h"
#include "picture.h"

/* 显示用户桌面 */
void display_user_desktop(void)
{
	/* get lcd params: xres, yres, bpp...... */	
	fb_get_lcd_params();

	//scan_picture(0, 0, 480, 272, (unsigned char*)gImage_user_desktop, 24);
	delay(1000000);
}
/* 等待用户点击, 返回触点的LCD对应坐标 */
void get_touchdot_xy(int *lcd_x, int lcd_y)
{
	
}

desktop user_desktop = {
	.name  = "user_desktop",
	.go_where = sensor_enum,	/* 先初始化到这个桌面，观看效果 */
	.into_desktop = display_user_desktop,	
};

