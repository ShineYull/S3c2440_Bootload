//#include "../include/S3C2440A.h"
#include "S3C2440A.h"
#include "lcd.h"
#include "lcd_controller.h"

#define HCLK 100


void jz2440_lcd_pin_init(void)
{
	/* ��ʼ������ : �������� */
	GPBCON &= ~(3<<0);
	GPBCON |=  (1<<0);

	/* LCDר������ */
	GPCCON = 0xaaaaaaaa;
	GPDCON = 0xaaaaaaaa;

	/* pwren */
	GPGCON |= (3<<8);
}

/* ���ݴ����LCD��������LCD������ */
void s3c2440a_lcd_controller_init(p_lcd_params plcdparams)
{
	int pixelplace;
	unsigned int addr;

	jz2440_lcd_pin_init();
	
	/* clkval[17:8], vclk = HCLK / [(CLKVAL+1)*2]
	 * 			 9M	= 100M / [(CLKVAL+1)*2]
	 *	     CLKVAL = 5
	 * 		   vclk = 100M / [(5+1)*2] = 8.3M
	 * ʹ��p_lcd_params�е�vclk��Ա��ֵ���м��㣺
	 * 		 CLKVAL = 100M/vclk/2-1
	 * PNRMODE[6:5], 0b11, tft lcd.
	 * BPPMODE[4:1], plcdparams->bpp.
	 * ENVID  [0],	 LCD video output and the logic enable/disable.
	 */
	/* ʹ�����������㷨 */
	int clkval  = (double)HCLK/plcdparams->time_seq.vclk/2-1+0.5;
	//int clkval  = 5;
	int bppmode = plcdparams->bpp==8  ? 0xb:\
				  plcdparams->bpp==16 ? 0xc:\
				  0xd;	/* 0xd : 24bpp */
	LCDCON1 &= ~(0x3ffff);
	LCDCON1 |=  (clkval<<8) | (3<<5) | (bppmode<<1);

	/* VBPD	  [31:24],  VBPD    = tvb-1;
	 * LINEVAL[23:14],  LINEVAL = tvd(yres)-1;
	 * VFPD	  [13:6],	VFPD	= tvf-1;
	 * VSPW   [5:0],	VSPW	= tvp-1;
	 */
	LCDCON2 =  ((plcdparams->time_seq.tvb-1) << 24) | \
			   ((plcdparams->yres-1) 		 << 14) | \
			   ((plcdparams->time_seq.tvf-1) << 6 ) | \
			   ((plcdparams->time_seq.tvp-1) << 0 );

	/* HBPD	  [25:19],  HBPD    = thb-1;
	 * HOZVAL [18:8],   HOZVAL  = thd(xres)-1;
	 * HFPW	  [7:0],	HFPW	= thf-1;
	 */
	LCDCON3 =  ((plcdparams->time_seq.thb-1) << 19) | \
			   ((plcdparams->xres-1) 		 << 8 ) | \
			   ((plcdparams->time_seq.thf-1) << 0 );

	/* HSPW   [7:0],	HSPW	= thp-1 */
	LCDCON4 &= ~(0xff<<0);
	LCDCON4 |= ((plcdparams->time_seq.thp-1) << 0);

	/* Setting pins polarity������16bpp���ڴ�(framebuffer)�����ش�ŵ�format
	 * BPP24BL   [12],
	 * FRM565    [11], 1 = 5-6-5 format;
	 * INVVCLK   [10], 0 = The video data is fetched at VCLK falling edge
	 *				   1 = The video data is fetched at VCLK rising edge
	 * INVVLINE  [9],  HSYNC�Ƿ�ת
	 * INVVFRAME [8],  VSYNC�Ƿ�ת
	 * INVVD	 [7],  RGB data bus�Ƿ�ת
	 * INVVDEN   [6],  
	 * INVPWREN	 [5],  This bit indicates the PWREN signal polarity
	 * INVLEND   [4],
	 * PWREN	 [3],  LCD_PWREN output signal enable/disable
	 * ENLEND	 [2],
	 * BSWP		 [1],
	 * HWSWP	 [0],
	 */

	pixelplace = plcdparams->bpp==24 ? 0:\
				 plcdparams->bpp==16 ? 1:\
				 (1<<1); /* 8bpp */
	
	LCDCON5 =  (plcdparams->pins_pol.vclk<<10) |\
			   (plcdparams->pins_pol.hsync<<9) |\
			   (plcdparams->pins_pol.vsync<<8) |\
			   (plcdparams->pins_pol.rgb<<7)   |\
   			   (plcdparams->pins_pol.de<<6)    |\
			   (plcdparams->pins_pol.pwren<<5) |\
			   (1<<11) | pixelplace;

	/*framebuffer address*/
	/* 
	 * LCDBANK[29:21], framebuffer start address[30:22]
	 * LCBASEU[20:0], framebuffer start address[21:1]
	 */
	addr = plcdparams->fb_base & (~(1<<31));
	LCDSADDR1 = (addr >> 1);

	/*
	 * LCDBASEL[20:0], framebuffer end address[21:1]
	 */
	addr = plcdparams->fb_base + (plcdparams->xres * plcdparams->yres *\
							     ((plcdparams->bpp==24 ? 32:plcdparams->bpp) / 8));
	addr >>= 1;
	addr &= 0x1fffff;
	LCDSADDR2 = addr;
}


void s3c2440a_lcd_controller_enable(void)
{
	/* �������ţ�GPB0(KEYBOARD) */
	GPBDAT |= (1<<0);
	
	/* pwren����LCD�ṩAVDD */
	LCDCON5 |= (1<<3);

	/* LCDCON1'BIT0 : ����LCD�������Ƿ�����ź� */
	LCDCON1 |= (1<<0);
}


void s3c2440a_lcd_controller_disble(void)
{
	/* �رձ��� */
	GPBDAT &= ~(1<<0);
	
	/* pwren����LCD�ṩAVDD */
	LCDCON5 &= ~(1<<3);
	
	/* LCDCON1'BIT0 : ����LCD�������Ƿ�����ź� */
	LCDCON1 &= ~(1<<0);
}

/* ���ܣ���ʼ����ɫ���ڵ���ɫ
 * ע�⣺��ֹ��LCDSADDR5��HSTATUS��VSTATUS
 *       ��λ״̬ΪACTIVE(0b10)ʱ��palette���в�����
 */
void s3c2440a_lcd_controller_disble_init_palette(void)
{
	volatile unsigned int *palette_base = (volatile unsigned int *)0x4d000400;
	int i, bit;

	bit = LCDCON1 & (1<<0);	
	/* LCDCON1'BIT0 : ����LCD�������Ƿ�����ź� */	
	if (bit)
		LCDCON1 &= ~(1<<0);	/* disable */

	for (i=0; i<256; i++)
	{
		/* ��16λ��rgb565 */
		*palette_base++ = i;
	}

	if (bit)
		LCDCON1 |= (1<<0);
}

lcd_controller s3c2440a_lcd_controller = {
	.name 	 = "s3c2440a", 
	.init 	 = s3c2440a_lcd_controller_init,
	.enable  = s3c2440a_lcd_controller_enable,
	.disable = s3c2440a_lcd_controller_disble,
	.init_palette = s3c2440a_lcd_controller_disble_init_palette,
};

void s3c2440a_lcd_controller_add(void)
{
	register_lcd_controller(&s3c2440a_lcd_controller);
}


