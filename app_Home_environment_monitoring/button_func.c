#include "framebuffer.h"
#include "button_func.h"
#include "string_utils.h"
#include "picture.h"
#include "app_test.h"
#include "my_printf.h"

#define MAX_QUE	32


/* button's queue */
static pbutton buttons[MAX_QUE];


button button_gas = {	
	.name = "button_gas",
	.status = 0,
	.button_func = button_handle,
	.display = display_button,
};


button button_light = {
	.name = "button_light",
	.status = 0,
	.button_func = button_handle,
	.display = display_button,
};

button button_auto = {	
	.name = "button_auto",
	.status = 0,
	.button_func = button_handle,
	.display = display_button,
};

button button_go_back = {
	.name = "button_go_back",
	.status = 0,
	.button_func = button_handle,
	.display = display_button,
};


/* 2019/12/21, 以后再优化, 现在手动添加.....[YLL] */
#if 0

/* 选择按钮 */

/* 按钮注册 */
void register_button(pbutton button)
{
	int i;

	for (i=0; i<MAX_QUE; i++)
	{
		/* 检查并注册 */
		if (!buttons[i])
		{
			buttons[i] = button;
		}
	}
}



/* 查找按钮 */
int check_button_in_queue(char *name)
{
	int i;

	for (i=0; i<MAX_QUE; i++)
	{
		/* 如果不为空 */
		if (buttons[i])
		{
			return ( !strcmp(buttons[i]->name, name) );
		}
	}
}

#endif

/* button on/off of light */
void display_button(int start_lcd_x, int start_lcd_y, int width, int higth, int bpp, char *name)
{
	int which;

	if (strcmp(button_light.name, name) == 0)
	{
		/* 2019/12/15, 困了~不封装选择按钮先了，就这样吧。。。。[YLL] */
		if (button_light.status)
		{
			/* 开启时按钮在lcd上的展现 */
			scan_picture(start_lcd_x, start_lcd_y, width, higth, (unsigned char*)gImage_button_on, bpp);
			//printf("display button on OK!\n\r");
		}
		else
		{	
			/* 关闭时按钮在lcd上的展现 */
			scan_picture(start_lcd_x, start_lcd_y, width, higth, (unsigned char*)gImage_button_off, bpp);
		}
	}		
	else if (strcmp(button_auto.name, name) == 0)
	{
		if (button_auto.status)
		{
			/* 开启时按钮在lcd上的展现 */
			scan_picture(start_lcd_x, start_lcd_y, width, higth, (unsigned char*)gImage_button_on, bpp);
		}
		else
		{	
			/* 关闭时按钮在lcd上的展现 */
			scan_picture(start_lcd_x, start_lcd_y, width, higth, (unsigned char*)gImage_button_off, bpp);
		}
	}
	else if (strcmp(button_go_back.name, name) == 0)
	{
		if (button_go_back.status)
		{
			/* 按下时按钮在lcd上的展现 */
			scan_picture(start_lcd_x, start_lcd_y, width, higth, (unsigned char*)gImage_button_down, bpp);
		}
		else
		{	
			/* 松开时按钮在lcd上的展现 */
			scan_picture(start_lcd_x, start_lcd_y, width, higth, (unsigned char*)gImage_button_up, bpp);
		}
	}
	else if (strcmp(button_gas.name, name) == 0)
	{
		if (button_gas.status)
		{
			/* 开启时按钮在lcd上的展现 */
			scan_picture(start_lcd_x, start_lcd_y, width, higth, (unsigned char*)gImage_button_on, bpp);
		}
		else
		{	
			/* 关闭时按钮在lcd上的展现 */
			scan_picture(start_lcd_x, start_lcd_y, width, higth, (unsigned char*)gImage_button_off, bpp);
		}
	}		
	else
	{
		return;
	}
}





void set_button_statu(char *name, int status)
{
	if (strcmp(button_gas.name, name) ==0 )
	{
		button_gas.status = status;
	}
	else if (strcmp(button_light.name, name) == 0 )
	{
		button_light.status = status;
		printf("status = %d.\n\r", status);
	}
}



/* 发送通知给从机 */
void send_control_signal(char *str)
{
	/* 通知从机，开始动作 */
	puts_uart1(str);
}



/* function of button */
void button_handle(char *name)
{
	/* 这一大串，看的我都烦，有时间一定优化.......[YLL] */
	/* check button_gas status to send signal */
	if (strcmp(button_gas.name, name) ==0 )
	{
		printf("%s\n\r", name);
		if (button_gas.status == 1)
		{
			send_control_signal("21");
			printf("21 signal send finish.\n\r");
		}
		else if (button_gas.status == 0)
		{
			send_control_signal("20");
			printf("20 signal send finish.\n\r");
		}
	}
	/* check button_light status to send signal */
	else if (strcmp(button_light.name, name) == 0 )
	{
		printf("%s\n\r", name);
		
		if (button_light.status == 1)
			send_control_signal("11");
		else if (button_light.status == 0)
			send_control_signal("10");
	}
}

