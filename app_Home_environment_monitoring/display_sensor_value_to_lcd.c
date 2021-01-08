#include "font.h"
#include "string_utils.h"
#include "geometry.h"
#include "framebuffer.h"
#include "picture.h"
#include "button_func.h"
#include "app_test.h"
#include "display_sensor_value_to_lcd.h"
#include "ts_calibrate.h"
#include "timer.h"
#include "my_printf.h"
#include "led.h"



/* zigbee����������ǰ׺ */
#define TEMH 	"ws"
#define GAS 	"qt"
#define LIGHT 	"gm"

/* �жϱ�ǩ */
#define TEMH_NUM 	1
#define GAS_NUM 	2
#define LIGHT_NUM 	3

/* ������������ʾ���� */
#define TEM_X		242
#define TEM_Y	 	99
#define HUM_X		242
#define HUM_Y		141
#define GAS_X		242
#define GAS_Y		183
#define LIGHT_X		242
#define LIGHT_Y		223

/* �Ƿ��д��� */
#define NO_TOUCH	2
#define TOUCH_DOWN	1
#define TOUCH_UP	0

/* ���鳤�� */
#define DATA_MAX	128
#define BUFFER_MAX	128

/* ���� */
int distinguish_sensor(char *str);
void display_data(int lcd_x, int lcd_y, char *str);
void get_display_place(int *lcd_x, int *lcd_y, int result);




desktop sensor_desktop = {
	.name = "sensor_desktop",
	.go_back_to = user_desktop_enum,
	.into_desktop = into_sensor_desktop,
};


/* ���������괦����ڱ�־ */
static int touch_handle_status = 0;


/* �Ƿ�Ҫ�˳���sensor���� */
static int go_back_to_sensor_desktop = 0;

static char data[DATA_MAX];
static char buffer[BUFFER_MAX];

/* lcd ���� */
static unsigned int fb_base;
static int xres, yres, bpp;





/* 
	display desktop Figure:

	[button]					[button]
	_____________________________________
	| Sensor|	Current Value	     	 |
	|_______|____________________________|
	| Tem   |	0.0		     			 |
	|_______|____________________________|
	| Hum   |	0.0		     			 |
	|_______|____________________________|
	| Gas   |	0.00V		     		 |
	|_______|____________________________|
	| Light |	0.00V		    [button] |
	|_______|____________________________|
*/

/* ��ʾSensor���� */
void display_sensor_desktop(void)
{
	/* ��ʼ�� */
	font_init();
	fb_get_lcd_params();
	
	/* ���LCD�Ĳ�����fb_base, xres, yres, bpp */
	get_lcd_params(&fb_base, &xres, &yres, &bpp);

	/* ��lcd�ײ���ɫ */
	clear_screen(0xf6f6f6);
	
	/* ������ */
	draw_line(30, 30, xres-30, 30, 0xff);
	draw_line(30, yres-30, xres-30, yres-30, 0xff);
	draw_line(30, 30, 30, yres-30, 0xff);
	draw_line(xres-30, 30, xres-30, yres-30, 0xff);

	draw_line(90, 30, 90, yres-30, 0xff);

	draw_line(30, 74,  xres-30, 74, 0xff);
	draw_line(30, 116, xres-30, 116, 0xff);
	draw_line(30, 158, xres-30, 158, 0xff);
	draw_line(30, 200, xres-30, 200, 0xff);

	/* �����ɫ */
	//fill_color(31, 31, xres-31, yres-31, 0xffff);

	/* �ַ�����ʾ */
	fb_print_string(36,  57, "Sensor", 		  0xf6f6f6, 0x0000);
	fb_print_string(218, 57, "Current Value", 0xf6f6f6, 0x0000);
	fb_print_string(48,  99, "Tem", 		  0xf6f6f6, 0x0000);
	fb_print_string(48, 141, "Hum", 		  0xf6f6f6, 0x0000);
	fb_print_string(48, 183, "Gas", 		  0xf6f6f6, 0x0000);
	fb_print_string(40, 223, "Light", 		  0xf6f6f6, 0x0000);

	/* go back button */
	//button_go_back.display(0, 0, 35, 25, bpp, button_go_back.name);

	/* button on/off of light */
	button_light.display(398, 215, 50, 19, bpp, button_light.name);

	/* button on/off of fan */
	button_gas.display(398, 173, 50, 19, bpp, button_gas.name);

	/* �Զ����ƿ���, ���Ƶƣ������� */
	//button_auto.display(427, 3, 50, 19, bpp, button_auto.name);
}



/* ͨ�����ڻ�ȡZigBeeЭ�������������� */
void get_sensor_data(void)
{
	/* �Ӵ��ڻ�ȡ���ݣ��ַ����� */
	gets_no_echo(buffer);
	//printf(" after gets_no_echo function \n\r ");
}


/* ������ʾ��LCD�� */
void display_data_to_lcd(char *buffer)
{
	int which;
	int lcd_x, lcd_y;
	int i;

	for (i=0; i<DATA_MAX; i++)
	{
		data[i] = buffer[i];
		
		if (!buffer[i])
			break;
	}

	
	//printf(" before into distinguish_sensor function \n\r ");
	
	/* �ȷֱ�Ӵ��ڽ��յ�������������һ��sensor�ڵ㷢���ģ��������ǰ׺ */
	which = distinguish_sensor(data);

	/* ������������, ��Ϊ�������ж�����
	 * ����ʱ���ò����ϺõĽ���, ����
	 * ���ִ������ݡ�
	 * 2019/12/23.....................[YLL]
	 */
	if (which == -1)
		return;
	
	//printf(" before get_display_place function \n\r ");


	/* �õ�lcd��Ӧ��ʾ������ */
	get_display_place(&lcd_x, &lcd_y, which);


	//printf(" before into display_data function \n\r ");


	display_data(lcd_x, lcd_y, data);
}


/* �ֱ�����һ��sensor, "ws:" or "qt:" or "gm:", �������ǰ׺: "ws:" or "qt:" or "gm:" */
int distinguish_sensor(char *str)
{
	int i;
	char distinguish[32];

	/* ȥ��ǰ׺, ע�⣺ǰ׺�Ǳ����޸�zigbee������ϵġ� */
	for (i=0; (str[i] != ':') && (i < 32); i++)
	{
		distinguish[i] = str[i];
		str[i] = ' ';
	}

	if (str[i] == ':')
	{
		str[i] = ' ';
		distinguish[i] = '\0';
	}
	else
		return -1;	/* ǰ׺����������-1������� */


	
	/* ���ڴ�����������, 
	 * ��Ч�ַ���������������������Ч�ַ���, 
	 * �����ｫ��������
	 * 2019/12/23.........[YLL]
	 */

	/* ����ǰ׺���ض�Ӧ�ĺ� */
	if (!strcmp(distinguish, TEMH))
	{
		/* ��ʪ���ִ���: "ws:%d.%d %d.%d\n\r" */
		/* %d.%d ȡ��λ���� */
		str[7] = '\0';
		str[12] = '\0';
			
		return TEMH_NUM;
	}
	else if (!strcmp(distinguish, GAS))
	{
		/* �����ִ���: "qt:%d.%02dV\n\r" */
		str[8] = '\0';
		
		return GAS_NUM;
	}
	else if (!strcmp(distinguish, LIGHT))
	{
		/* �����ִ���: "gm:%d.%02dV\n\r" */
		str[8] = '\0';

		return LIGHT_NUM;
	}
	else 
		return -1;	/* �Ҳ�����Ӧsensor������-1������� */
}


/* ���ݷֱ���Ľ�����õ���ʾ������ */
void get_display_place(int *lcd_x, int *lcd_y, int result)
{
	if (result == TEMH_NUM)
	{
		*lcd_x = TEM_X;
		*lcd_y = TEM_Y;
	}
	else if (result == GAS_NUM)
	{
		*lcd_x = GAS_X;
		*lcd_y = GAS_Y;
	}
	else if (result == LIGHT_NUM)
	{
		*lcd_x = LIGHT_X;
		*lcd_y = LIGHT_Y;
	}
}



int is_safety(char *str, char *compare)
{
	if (strcmp(str, compare) >= 0)
		return 0;
	else 
		return 1;
}


int check_alarm(char *str)
{
	int select;
	int lcd_x, lcd_y;
	int i;

	/* �ȷֱ�Ӵ��ڽ��յ�������������һ��sensor�ڵ㷢���ģ��������ǰ׺ */
	select = distinguish_sensor(data);

	/* ������������, ��Ϊ�������ж�����
	 * ����ʱ���ò����ϺõĽ���, ����
	 * ���ִ������ݡ�
	 * 2019/12/23.....................[YLL]
	 */
	if (select == -1)
		return;

	/* �����ַ���ǰ�հ� */
	for (i=0; str[i] == ' '; i++);


	/* ��������Ƿ񳬳����� */
	switch (select)
	{
		case TEMH_NUM :
			/* ����ʪ������ */
			for ( ; str[i++] != '\0'; );
			
			if (is_safety(&str[i], "50"))
				return 0;
			break;

		case GAS_NUM :
			if (is_safety(&str[i], "0.8"))
			{
				/* �����Զ���ť, ���ڻ�ûʵ��.....[YLL] */
				/* ֪ͨ�ӻ����趯�� */
				//send_control_signal("20");
				//set_button_statu(button_gas.name, 0);
				return 0;
			}
			break;

		case LIGHT_NUM :
			if (is_safety(&str[i], "0.3"))
			{
				/* �����Զ���ť, ���ڻ�ûʵ��.....[YLL] */
				/* ֪ͨ�ӻ����趯�� */
				//send_control_signal("10");
				//set_button_statu(button_light.name, 0);
				return 0;
			}
			break;
	}

	while (select)
	{
		/* ����Խ�߾���
		 * a. ��ʾ���洰��
		 * b. ��ӡ����
		 */
		if (select == TEMH_NUM || select == GAS_NUM)
		{
			/* ��ʾ���� */
			scan_picture(60, 60, 353, 153, (unsigned char*)gImage_alarm_window, 24);

			/* ��ӡ���� */
			if (select == TEMH_NUM)
			{
				fb_print_string(185, 105, "Tem: ", 0xf6f6f6, 0xff00);
			}
			else if (select == GAS_NUM)
			{
				fb_print_string(185, 105, "Gas: ", 0xf6f6f6, 0xff00);
				/* ������ */
				send_control_signal("21");
				set_button_statu(button_gas.name, 1);
			}

			fb_print_string(225, 105, str, 0xf6f6f6, 0xff00);
		}
		else if (select == LIGHT_NUM)
		{
			/* ���� */
			send_control_signal("11");
			set_button_statu(button_light.name, 1);
		}
	}
}



/* ��ʾ����Ӧ���� */
void display_data(int lcd_x, int lcd_y, char *str)
{	 
	/* ���ݵõ���which����������ʾ��lcd�϶�Ӧ���� */

	int i, j;

	font_init();

	for (i=0; str[i] == ' '; i++);

	
	/* ���Ż����⡿ 
	 *	��ʪ�Ƚڵ㷢�����ַ��������¶���ʪ�ȵ�����, 
	 *  ��ֿ���ӡ
	 *	2019/12/14
	 */
	if ((lcd_x == TEM_X) && (lcd_y == TEM_Y))
	{
		/* �˴���ӡǰ�账��, �ֿ���ʪ���ַ��� */
	
		/* �滻ʪ���ִ���Ŀո�Ϊ������'\0' */
		for (j=i; str[j++] != '\0'; );
				
		/* �ȴ�ӡʪ�� */ 
		fb_print_string(HUM_X, HUM_Y, &str[i], 0xf6f6f6, 0xff00);
		
		/* ���ӡ�¶� */
		/* �ж������Ƿ�Խ�� */
		if ( is_safety(&str[j], "50") )
		{
			fb_print_string(lcd_x-56, lcd_y, "       " , 0xf6f6f6, 0xff00);			
			fb_print_string(lcd_x, lcd_y, &str[j], 0xf6f6f6, 0xff00);
		}
		else		
		{
			fb_print_string(lcd_x-56, lcd_y, "Alarm! ", 0xf6f6f6, 0xff0000);
			fb_print_string(lcd_x, lcd_y, &str[j], 0xf6f6f6, 0xff00);
		}
	}
	else if ( (lcd_x == GAS_X) && (lcd_y == GAS_Y) )
	{	
		//printf(" in display_data function:  ");
		//printf(" lcd_x = %d , lcd_y = %d. \n\r", lcd_x, lcd_y);
		
		/* ������ӡ */
		//fb_print_string(lcd_x, lcd_y, &str[i], 0xf6f6f6, 0xff00);

		
		/* �ж������Ƿ�Խ�� */
		if ( is_safety(&str[i], "0.8") )
		{		
			fb_print_string(lcd_x-56, lcd_y, "       ", 0xf6f6f6, 0xff00);
			fb_print_string(lcd_x, lcd_y, &str[i], 0xf6f6f6, 0xff00);
		}
		else
		{	
			fb_print_string(lcd_x-56, lcd_y, "Alarm! ", 0xf6f6f6, 0xff0000);
			fb_print_string(lcd_x, lcd_y, &str[i], 0xf6f6f6, 0xff00);
		}
	}	
	else if ( (lcd_x == LIGHT_X) && (lcd_y == LIGHT_Y) )
	{	
		//printf(" in display_data function:  ");
		//printf(" lcd_x = %d , lcd_y = %d. \n\r", lcd_x, lcd_y);
		
		/* ������ӡ */
		//fb_print_string(lcd_x, lcd_y, &str[i], 0xf6f6f6, 0xff00);

		
		/* �ж������Ƿ�Խ�� */
		if ( is_safety(&str[i], "0.3") )
		{			
			fb_print_string(lcd_x-88, lcd_y, "open light ", 0xf6f6f6, 0xff0000);
			fb_print_string(lcd_x, lcd_y, &str[i], 0xf6f6f6, 0xff00);
		}
		else		
		{						
			fb_print_string(lcd_x-88, lcd_y, "           ", 0xf6f6f6, 0xff00);
			fb_print_string(lcd_x, lcd_y, &str[i], 0xf6f6f6, 0xff00);
		}
	}
}


/* touch handle func */
void touch_handle(void)
{
	if (!touch_handle_status)
		return;

	int lcd_x, lcd_y;
	static int pressure = NO_TOUCH;

	if (ts_read_for_sensor_desktop(&lcd_x, &lcd_y, &pressure) == 0 )
	{	
		if (pressure != NO_TOUCH && pressure != TOUCH_DOWN)
		{
			printf("User touch, lcd_x = %d, lcd_y = %d, pressure = %d.\n\r", lcd_x, lcd_y, pressure);
			if (lcd_x >= 398 && lcd_x <= 448 && lcd_y >= 215 && lcd_y <= 234)
			{
				/* light button */
				/* set button status */
				if (button_light.status == 0)
					set_button_statu(button_light.name, 1);
				else
					set_button_statu(button_light.name, 0);

				printf("button_light.status = %d.\n\r", button_light.status);
				
				pressure = NO_TOUCH;
			}
			else if (lcd_x >= 398 && lcd_x <= 448 && lcd_y >= 173 && lcd_y <= 192)
			{
				/* fan button */	
				/* set button status */
				if (button_gas.status== 0)
					set_button_statu(button_gas.name, 1);
				else
					set_button_statu(button_gas.name, 0);

				printf("button_gas.status = %d.\n\r", button_gas.status);
				
				pressure = NO_TOUCH;
			}
		}
	}
}


void check_button_status(void)
{
	/* refresh button display */
	button_light.display(398, 215, 50, 19, bpp, button_light.name);
	button_gas.display(398, 173, 50, 19, bpp, button_gas.name);

	/* start button function */
	button_gas.button_func(button_gas.name);
	/* ���ڶ�ʱ���жϵ�ԭ��
	 * �ܳ���᲻�ϵĽ���IRQģʽ
	 * Ҳ���ǲ�ͣ�Ĵ�ϴ��ڹ���
	 * ��Ӷ�ʱ����������Ǹ�����İܱ�
	 * ����Ϊ�����ô�����ɷ��͹���
	 * �������������ʱ......[YLL]...2019/12/24
	 */
	delay(50);
	button_light.button_func(button_light.name);
}



/* Sensor desktop��� */ 
void into_sensor_desktop(void)
{	
	/* Open touch_handle function entrance */
	touch_handle_status = 1;
	
	while (1)
	{
		/* ��ʾ���� */
		display_sensor_desktop();
		

		while (1)
		{
			//printf(" before into get_sensor_data function \n\r ");
			
			/* get sensor data to buffer array */
			get_sensor_data();

			//printf(" before into display_data_to_lcd function \n\r ");
			
			/* handle and refresh data to LCD */
			display_data_to_lcd(buffer);

			//printf(" after display_data_to_lcd function \n\r ");

			/* check touch and set button status */
			//touch_handle();  /* Ǩȥ��ʱ���жϷ�����.....[YLL] 2019/12/23 */

			/* check touch down for button, send data to slave */
			check_button_status();

			/* check data out */
			//check_alarm(buffer);	/* ����ʱ��ʵ����......[YLL] */
		}
	}
}
