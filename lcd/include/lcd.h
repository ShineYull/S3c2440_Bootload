#ifndef _LCD_H_
#define _LCD_H_


enum {
	NORMAL = 0,
	INVERT = 1 
};


/* NORMAL : 正常极性
 * INVERT : 反转极性
 */
typedef struct pins_polarity {
	int de;		/* normal: 高电平时可以传输数据 */
	int pwren;	/* normal: 高电平有效 */
	int vclk;	/* normal：hight pluse, 在下降沿获取数据 */
	int rgb;	/* normal：高电平表示1 */
	int hsync;	/* normal：hight pluse */
	int vsync;	/* normal：hight pluse */
}pins_polarity, *p_pins_polarity;


typedef struct time_sequence {
	/* 垂直(vertical)方向 */
	int tvp;	/* Vsync pluse width */
	int tvb; 	/* Vsync pluse back porch */
	int tvf; 	/* Vsync pluse front porch */

	/* 水平(horizontal)方向 */
	int thp;	/* Hsync pluse width */
	int thb;	/* Hsync pluse back porch */
	int thf;	/* Hsync pluse front porch */

	int vclk;
}time_sequence, *p_time_sequence;


typedef struct lcd_params {
	char *name;
	
	/* 引脚极性 */
	pins_polarity pins_pol;
	
	/* 时序 */
	time_sequence time_seq;

	/* 分辨率(resolution) */
	int xres;
	int yres;

	/* bpp(bit per pixel) */
	int bpp;

	/* FRAME format of 16 BPP */
	int FRM565;
	
	/* framebuffer的addr */
	unsigned int fb_base;
	
}lcd_params, *p_lcd_params;


void get_lcd_params(unsigned int *fb_base, int *xres, int *yres, int *bpp);
int lcd_init(void);
void lcd_disable(void);
void lcd_enable(void);
int register_lcd(p_lcd_params plcd);


#endif /* _LCD_H_ */


