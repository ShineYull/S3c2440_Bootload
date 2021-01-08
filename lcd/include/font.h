#ifndef _FONT_H_
#define _FONT_H_

void font_init(void);
void fb_print_char(int x, int y, char c, unsigned int bgcolor, unsigned int color);
void fb_print_string(int x, int y, char *string, unsigned int bgcolor, unsigned int color);

#endif /* _FONT_H_ */
