#ifndef _LCD_H_
#define _LCD_H_


enum {
	NORMAL = 0,
	INVERT = 1 
};


/* NORMAL : ��������
 * INVERT : ��ת����
 */
typedef struct pins_polarity {
	int de;		/* normal: �ߵ�ƽʱ���Դ������� */
	int pwren;	/* normal: �ߵ�ƽ��Ч */
	int vclk;	/* normal��hight pluse, ���½��ػ�ȡ���� */
	int rgb;	/* normal���ߵ�ƽ��ʾ1 */
	int hsync;	/* normal��hight pluse */
	int vsync;	/* normal��hight pluse */
}pins_polarity, *p_pins_polarity;


typedef struct time_sequence {
	/* ��ֱ(vertical)���� */
	int tvp;	/* Vsync pluse width */
	int tvb; 	/* Vsync pluse back porch */
	int tvf; 	/* Vsync pluse front porch */

	/* ˮƽ(horizontal)���� */
	int thp;	/* Hsync pluse width */
	int thb;	/* Hsync pluse back porch */
	int thf;	/* Hsync pluse front porch */

	int vclk;
}time_sequence, *p_time_sequence;


typedef struct lcd_params {
	char *name;
	
	/* ���ż��� */
	pins_polarity pins_pol;
	
	/* ʱ�� */
	time_sequence time_seq;

	/* �ֱ���(resolution) */
	int xres;
	int yres;

	/* bpp(bit per pixel) */
	int bpp;

	/* FRAME format of 16 BPP */
	int FRM565;
	
	/* framebuffer��addr */
	unsigned int fb_base;
	
}lcd_params, *p_lcd_params;


void get_lcd_params(unsigned int *fb_base, int *xres, int *yres, int *bpp);
int lcd_init(void);
void lcd_disable(void);
void lcd_enable(void);
int register_lcd(p_lcd_params plcd);


#endif /* _LCD_H_ */


