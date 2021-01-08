#ifndef _TIMER_H_
#define _TIMER_H_


typedef void(*timer_func)(void);

typedef struct timer_desc{
	char *name;
	timer_func fp;	
}timer_desc, *p_timer_desc;


int register_timer(char *name, timer_func fp);
int unregister_timer(char *name);
void timer_irq(void);
void init_timer(void);

#endif
