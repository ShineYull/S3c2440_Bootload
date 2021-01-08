#include "uart.h"


void printException (unsigned int cpsr, char *str)
{
	puts("Exception! cpsr = ");
	printHex(cpsr);
	putchar(' ');
	puts(str);
	puts("\n\r");
}

void printSWIVal(unsigned int *pSWI)
{
	puts("SWI val = ");
	printHex(*pSWI & ~0xff000000);
	puts("\n\r");
}


