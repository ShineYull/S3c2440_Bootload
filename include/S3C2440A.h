/* 制  作：虞灵乐
 * 工程师：虞灵乐
 * 当前版本：version 1.0
 * 
 * 文件内容：寄存器的宏定义
 */

#ifndef _S3C2440A_H_
#define _S3C2440A_H_

#define _REG(x)				(*(volatile unsigned int *)(x))
#define _REG_BYTE(x)		(*(volatile unsigned char *)(x))

/*GPIO*/
#define GPBCON				_REG(0x56000010)
#define GPBDAT				_REG(0x56000014)
#define GPBUP				_REG(0x56000018)
#define GPCCON				_REG(0x56000020)
#define GPCDAT				_REG(0x56000024)
#define GPCUP				_REG(0x56000028)
#define GPDCON				_REG(0x56000030)
#define GPDDAT				_REG(0x56000034)
#define GPDUP				_REG(0x56000038)
#define GPFCON				_REG(0x56000050)
#define GPFDAT				_REG(0x56000054)
#define GPFUP				_REG(0x56000058)
#define GPGCON				_REG(0x56000060)
#define GPGDAT				_REG(0x56000064)
#define GPGUP				_REG(0x56000068)
#define GPHCON				_REG(0x56000070)
#define GPHDAT				_REG(0x56000074)
#define GPHUP				_REG(0x56000078)


/*UART*/
#define ULCON0				_REG(0x50000000)
#define UCON0				_REG(0x50000004)
#define UTRSTAT0			_REG(0x50000010)
#define UTXH0				_REG_BYTE(0x50000020)
#define URXH0				_REG_BYTE(0x50000024)
#define UBRDIV0				_REG(0x50000028)		//baud rate
#define ULCON1				_REG(0x50004000)
#define UCON1				_REG(0x50004004)
#define UTRSTAT1			_REG(0x50004010)
#define UTXH1				_REG_BYTE(0x50004020)
#define URXH1				_REG_BYTE(0x50004024)
#define UBRDIV1				_REG(0x50004028)		//baud rate


/*Memory Controller*/
#define BWSCON				_REG(0x48000000)
#define BANKCON6			_REG(0x4800001c)
#define BANKCON7			_REG(0x48000020)
#define REGRESH				_REG(0x48000024)
#define BANKSIZE			_REG(0x48000028)
#define MRSRB6				_REG(0x4800002c)
#define MRSRB7				_REG(0x48000030)

/* EXTINT control */
#define EXTINT0				_REG(0x56000088)
#define EXTINT1				_REG(0x5600008c)
#define EXTINT2				_REG(0x56000090)
#define EINTMASK			_REG(0x560000a4)
#define EINTPEND			_REG(0x560000a8)

/* interrupt controller */
#define SRCPND				_REG(0x4a000000)
#define INTMSK				_REG(0x4a000008)
#define PRIORITY			_REG(0x4a00000c)
#define INTPND				_REG(0x4a000010)
#define INTOFFSET			_REG(0x4a000014)
#define SUBSRCPND			_REG(0x4a000018)
#define INTSUBMSK			_REG(0x4a00001c)

/* PWM TIMER */
#define TCFG0				_REG(0x51000000)
#define TCFG1				_REG(0x51000004)
#define TCON				_REG(0x51000008)
#define TCNTB0				_REG(0x5100000c)
#define TCMPB0				_REG(0x51000010)
#define TCNTO0				_REG(0x51000014)

/* Nand Flash Controller */
#define NFCONF				_REG(0x4e000000)
#define NFCONT				_REG(0x4e000004)
#define NFCMMD				_REG_BYTE(0x4e000008)
#define NFADDR				_REG_BYTE(0x4e00000c)
#define NFDATA				_REG_BYTE(0x4e000010)
#define NFSTAT				_REG_BYTE(0x4e000020)

/* LCD CONTROLLER */
#define LCDCON1				_REG(0x4d000000)
#define LCDCON2				_REG(0x4d000004)
#define LCDCON3				_REG(0x4d000008)
#define LCDCON4				_REG(0x4d00000c)
#define LCDCON5				_REG(0x4d000010)
#define LCDSADDR1			_REG(0x4d000014)
#define LCDSADDR2			_REG(0x4d000018)
#define LCDSADDR3			_REG(0x4d00001c)
#define TPAL				_REG(0x4d000050)
//#define PALETTE_BASE		_REG(0x4d000400)	

/* A/D Converte */
#define ADCCON 				_REG(0x58000000)
#define ADCTSC				_REG(0x58000004)
#define ADCDLY				_REG(0x58000008)
#define ADCDAT0				_REG(0x5800000c)
#define ADCDAT1				_REG(0x58000010)
#define ADCUPDN				_REG(0x58000014)



#endif
