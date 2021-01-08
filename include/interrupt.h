#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_

typedef void(*irq_func)(int irq);

void init_interrupt(void);
void init_key_eint(void);
void key_eint_irq(int irq);
void handle_irq_c(void);
void register_irq(int irq, irq_func fp);

#endif
