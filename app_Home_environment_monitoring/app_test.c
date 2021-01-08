
/*  工程师：虞灵乐
 *  版  本：version 1.0
 */

#include "lcd.h"
#include "app_test.h"
#include "touchscreen_test.h"
#include "display_sensor_value_to_lcd.h"




void hardware_init(void)
{
	/*****************UART initialization***************/
	uart0_init();
	uart1_init();
	/***************************************************/

	/*****************TIMER initialization***************/
	init_timer();
	/***************************************************/

	/*****************LCD initialization****************/
	/* 初始化LCD */
	lcd_init();

	/* 使能LCD */
	lcd_enable();
	/***************************************************/

	
	
	/************register app_touch_handle**************/
	register_timer("app_touch_handle", touch_handle);
	
	/***************************************************/
	

	/*********TS initialization & calibrate*************/
	touchscreen_on();
	/***************************************************/
}


void app_test(void)
{

	/* hardware initialization */
	hardware_init();
	
	/* into sensor desktop */
	sensor_desktop.into_desktop();
	

}

