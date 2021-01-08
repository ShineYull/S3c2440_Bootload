#ifndef _BUTTON_FUNC_H_
#define _BUTTON_FUNC_H_

typedef struct button{
	char *name;
	int status;
	void (*display)(int start_lcd_x, int start_lcd_y, int width, int higth, int bpp, char *name);
	void (*button_func)(char *name);
}button, *pbutton;

/* ∞¥≈•∂‘œÛ */
extern button button_light;
extern button button_auto;
extern button button_go_back;
extern button button_gas;


extern void display_button(int start_lcd_x, int start_lcd_y, int width, int higth, int bpp, char *name);
extern void auto_button(int start_lcd_x, int start_lcd_y, int width, int higth, int bpp);
extern void go_back_button(int start_lcd_x, int start_lcd_y, int width, int higth, int bpp);
extern void button_handle(char *name);
extern void set_button_statu(char *name, int status);
extern void send_control_signal(char *str);



#endif  /* _BUTTON_FUNC_H_ */
