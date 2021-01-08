#ifndef _FRAMEBUFFER_H_
#define _FRAMEBUFFER_H_

void fb_put_pixel(int x, int y, unsigned int color);
void fb_get_lcd_params(void);
void clear_screen(unsigned int color);
void fill_color(int start_x, int start_y, int end_x, int end_y, unsigned int color);
void scan_picture(int start_lcd_x, int start_lcd_y, int end_lcd_x, int end_lcd_y, unsigned char *picture, int bpp);



#endif /* _FRAMEBUFFER_H_ */
