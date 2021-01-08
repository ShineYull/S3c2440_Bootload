#ifndef _NAND_FLASH_H_
#define _NAND_FLASH_H_

extern void nand_init(void);
extern void nand_flash_test(void);
void nand_read(unsigned int addr, unsigned char *buf, unsigned int len);

#endif

