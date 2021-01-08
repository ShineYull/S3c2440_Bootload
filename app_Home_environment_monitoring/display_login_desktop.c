#include "app_test.h"
#include "framebuffer.h"
#include "led.h"
#include "picture.h"


#define NULL ((void *) 0)




/* <Framework> */

/* display login desktop */

void display_login_desktop(void)
{
	/* initialization framebuffer */
	fb_get_lcd_params();
	
	/* display login desktop picture */
	//scan_picture(0, 0, 480, 272, (unsigned char*)gImage_login_desktop, 24);
	delay(1000000);

	/* 仅展现，还未实现 */
}


/* wait user touchscreen */

/* distinguish touch case :
 * a. username input
 * b. password input
 */

/* display keyboard */

/* check the input data in nand flash */

/* get check result */


desktop login_desktop = {
	.name "login_desktop",
	.into_desktop = display_login_desktop,
};

