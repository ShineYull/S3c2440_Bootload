
/* ��У׼����ʹ�ô��������У��ԭ��
 * ���ҵ�csdn��������ժ�������½��⡣
 */

#include "lcd.h"
#include "geometry.h"
#include "framebuffer.h"
#include "touchscreen.h"


static double g_kx;
static double g_ky;

static int g_ts_xc, g_ts_yc;
static int g_lcd_xc, g_lcd_yc;
static int g_ts_xy_swap;

static unsigned int fb_base;
static int xres, yres, bpp;


/* �� Touchscreen �Ĺ�ʽ���з�װ */
static inline int get_lcd_x_from_ts_x(int ts_x)
{
	return (g_kx * (ts_x - g_ts_xc) + g_lcd_xc); /* ����������תLCD���깫ʽ */
}

static inline int get_lcd_y_from_ts_y(int ts_y)
{
	return (g_ky * (ts_y - g_ts_yc) + g_lcd_yc);
}


void get_calibrate_point_data(int lcd_x, int lcd_y, int *px, int *py)
{
	int x = 0, y = 0;
	int temp_x, temp_y;
	int pressure;
	int sum_x = 0, sum_y = 0;
	int count = 0;;

	/* ��LCD�ϻ���У׼�� */
	draw_cross(lcd_x, lcd_y, 0xffffff);

	/* ͨ��ѹ��ֵ�����˶ദ��Чֵ���ϱ� */
	do
	{
		ts_read_raw(&x, &y, &pressure);
		//ts_read_raw_test(&x, &y, &pressure);
	} while (pressure == 0);	

	/* �ȴ�����������津��ĵ�ѹֵ */
	do
	{
		/* �ٴ��󴥵��ѹֵ��average */
		if (count < 128)
		{
			sum_x += x;
			sum_y += y;
			count++;
		}
		
		ts_read_raw(&x, &y, &pressure);
		//ts_read_raw_test(&x, &y, &pressure);

		printf("get raw data: x = %08d, y = %08d, count = %08d.\n\r", x, y, count);
	} while (pressure);	

	*px = sum_x / count;
	*py = sum_y / count;
	printf("return raw data: x = %08d, y = %08d\n\r", *px, *py);

	/* ����"+" */
	//clear_screen(0x00);
	draw_cross(lcd_x, lcd_y, 0x00);
}


int is_ts_xy_swap(int a_ts_x, int a_ts_y, int b_ts_x, int b_ts_y)
{
	int dx = b_ts_x - a_ts_x;
	int dy = b_ts_y - a_ts_y;

	if (dx < 0)
		dx = 0 - dx;
	if (dy < 0)
		dy = 0 - dy;

	if (dx < dy)	/* xy�ᷴת */
		return 1;
	else			/* xy��û�з�ת */
		return 0;
}

void swap_xy(int *px, int *py)
{
	int temp = *px;
	*px = *py;
	*py = temp;
}


/*
----------------------------
|                          |
|  +(A)              (B)+  |
|                          |
|                          |
|                          |
|            +(E)          |
|                          |
|                          |
|                          |
|  +(D)              (C)+  |
|                          |
----------------------------
*/

/* ��ȡУ׼��ʽ�������
*/
void ts_calibrate(void)
{	
	int a_ts_x, a_ts_y;
	int b_ts_x, b_ts_y;
	int c_ts_x, c_ts_y;
	int d_ts_x, d_ts_y;
	int e_ts_x, e_ts_y;

	/* X�᷽�� */
	int ts_s1, ts_s2;
	int lcd_s;
	
	/* Y�᷽�� */
	int ts_d1, ts_d2;
	int lcd_d;
	
	/* ���LCD�Ĳ�����fb_base, xres, yres, bpp */
	get_lcd_params(&fb_base, &xres, &yres, &bpp);

	/* ����A,B,C,D,E��ѭ������
	 * ʹ�����"+", ��ȡ�����津��ĵĵ�ѹֵ
	 */

	/* A(50, 50)*/
	get_calibrate_point_data(50, 50, &a_ts_x, &a_ts_y);

	/* B(xres-50, 50) */
	get_calibrate_point_data(xres-50, 50, &b_ts_x, &b_ts_y);

	/* C(xres-50, yres-50) */
	get_calibrate_point_data(xres-50, yres-50, &c_ts_x, &c_ts_y);

	/* D(50, yres-50) */
	get_calibrate_point_data(50, yres-50, &d_ts_x, &d_ts_y);

	/* E(xres/2, yres/2) */
	get_calibrate_point_data(xres/2, yres/2, &e_ts_x, &e_ts_y);

	/* ȷ������������XY�Ƿ�ת */
	g_ts_xy_swap = is_ts_xy_swap(a_ts_x, a_ts_y, b_ts_x, b_ts_y);

	if (g_ts_xy_swap)
	{
		/* �Ե����д����XY���� */
		swap_xy(&a_ts_x, &a_ts_y);
		swap_xy(&b_ts_x, &b_ts_y);
		swap_xy(&c_ts_x, &c_ts_y);
		swap_xy(&d_ts_x, &d_ts_y);
		swap_xy(&e_ts_x, &e_ts_y);		
	}

	/* ȷ��У׼��ʽ�еĲ��������� */
	ts_s1 = b_ts_x - a_ts_x;
	ts_s2 = c_ts_x - d_ts_x;
	lcd_s = (xres-50) - 50;

	ts_d1 = d_ts_y - a_ts_y;
	ts_d2 = c_ts_y - b_ts_y;
	lcd_d = (yres-50) - 50;

	g_kx = ((double)(2*lcd_s)) / (ts_s1+ts_s2);
	g_ky = ((double)(2*lcd_d)) / (ts_d1+ts_d2);

	g_ts_xc = e_ts_x;
	g_ts_yc = e_ts_y;

	g_lcd_xc = xres/2;
	g_lcd_yc = yres/2;

	/* �������� */
	printf("A lcd_x = %08d, lcd_y = %08d\n\r", get_lcd_x_from_ts_x(a_ts_x), get_lcd_y_from_ts_y(a_ts_y));
	printf("B lcd_x = %08d, lcd_y = %08d\n\r", get_lcd_x_from_ts_x(b_ts_x), get_lcd_y_from_ts_y(b_ts_y));
	printf("C lcd_x = %08d, lcd_y = %08d\n\r", get_lcd_x_from_ts_x(c_ts_x), get_lcd_y_from_ts_y(c_ts_y));
	printf("D lcd_x = %08d, lcd_y = %08d\n\r", get_lcd_x_from_ts_x(d_ts_x), get_lcd_y_from_ts_y(d_ts_y));
	printf("E lcd_x = %08d, lcd_y = %08d\n\r", get_lcd_x_from_ts_x(e_ts_x), get_lcd_y_from_ts_y(e_ts_y));
}


/* ʹ�û�ȡ���Ĺ�ʽ����ֵ���������ӦLCD������ֵ */

/* ���ܣ���TS�������꣬��ת��Ϊ��lcd���꡿
 * ʹ�ã�ʹ�øú�����ȡ�����Ӧlcd�ϵ�����
 */
int ts_read(int *lcd_x, int *lcd_y, int *lcd_pressure)
{
	int ts_x, ts_y;
	int ts_pressure;
	int temp_x, temp_y;

	/* ��ȡ�����ѹֵ��ѹ��ֵ */
	ts_read_raw(&ts_x, &ts_y, &ts_pressure);
	
	/* �ж�TS����ʱ���з�ת */
	if (g_ts_xy_swap)
		swap_xy(&ts_x, &ts_y);

	/* ʹ�ù�ʽ����TS_xy��ѹֵ�������Ӧ��LCD_xy */
	temp_x = get_lcd_x_from_ts_x(ts_x);
	temp_y = get_lcd_y_from_ts_y(ts_y);

	if (temp_x<0 || temp_x>=xres || temp_y<0 || temp_y>=yres)
		return -1;

	*lcd_x = temp_x;
	*lcd_y = temp_y;
	*lcd_pressure = ts_pressure;
	return 0;
}


int ts_read_for_sensor_desktop(int *lcd_x, int *lcd_y, int *lcd_pressure)
{
	int ts_x, ts_y;
	int ts_pressure;
	int temp_x, temp_y;

	/* ��ȡ�����ѹֵ��ѹ��ֵ */
	if (ts_read_raw_for_sensor_button(&ts_x, &ts_y, &ts_pressure) == 0)
		return -1;
	
	/* �ж�TS����ʱ���з�ת */
	if (g_ts_xy_swap)
		swap_xy(&ts_x, &ts_y);

	/* ʹ�ù�ʽ����TS_xy��ѹֵ�������Ӧ��LCD_xy */
	temp_x = get_lcd_x_from_ts_x(ts_x);
	temp_y = get_lcd_y_from_ts_y(ts_y);
	printf("*temp_x = %d, *temp_y = %d.\n\r", temp_x, temp_y);

	if (temp_x<0 || temp_x>=xres || temp_y<0 || temp_y>=yres)
		return -1;

	*lcd_x = temp_x;
	*lcd_y = temp_y;
	*lcd_pressure = ts_pressure;
	
	printf("*lcd_x = %d, *lcd_y = %d, *lcd_pressure = %d.\n\r", *lcd_x, *lcd_y, *lcd_pressure);
	return 0;
}


