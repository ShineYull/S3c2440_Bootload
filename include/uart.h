

#ifndef _UART_H
#define _UART_H

void uart0_init();
void uart1_init();
int putchar_uart1(int c);
int getchar_uart1(void);
int putchar(int c);
int getchar(void);
int puts(const char *s);
void printHex(unsigned int val);
int puts_uart1(const char *s);


#endif

