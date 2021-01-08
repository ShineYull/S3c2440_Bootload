#include "S3C2440A.h"
#include "led.h"
#include "uart.h"
#include "sdram.h"
#include "timer.h"
#include "nor_flash.h"
#include "nand_flash.h"
#include "lcd_test.h"
#include "adc_test.h"
#include "touchscreen_test.h"
#include "display_sensor_value_to_lcd.h"
#include "app_test.h"
#include "my_printf.h"
#include "string_utils.h"


int main(void)
{	
	/* ½øÈësensor desktop */
	app_test();
	
	while (1)
	{
	}
	
	
	return 0;
}
