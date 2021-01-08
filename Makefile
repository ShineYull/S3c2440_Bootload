objs = start.o sdram.o mmu.o nand_flash.o interrupt.o main.o uart.o my_printf.o led.o exception.o timer.o nor_flash.o string_utils.o

#LCD
objs += lcd/font.o
objs += lcd/font_8x16.o
objs += lcd/framebuffer.o
objs += lcd/geometry.o
objs += lcd/lcd.o
objs += lcd/lcd_4_3.o
objs += lcd/lcd_controller.o
objs += lcd/lcd_test.o
objs += lcd/s3c2440a_lcd_controller.o

#模数转换
objs += adc_touchscreen/adc_test.o
objs += adc_touchscreen/adc.o

#触屏
objs += adc_touchscreen/touchscreen.o
objs += adc_touchscreen/touchscreen_test.o
objs += adc_touchscreen/ts_calibrate.o

#Myapp
objs += app_Home_environment_monitoring/display_sensor_value_to_lcd.o
objs += app_Home_environment_monitoring/display_login_desktop.o
objs += app_Home_environment_monitoring/display_user_desktop.o
#objs += app_Home_environment_monitoring/login_desktop_picture.o
#objs += app_Home_environment_monitoring/user_desktop_picture.o
objs += app_Home_environment_monitoring/alarm_window_picture.o
objs += app_Home_environment_monitoring/app_test.o
objs += app_Home_environment_monitoring/button_picture.o
objs += app_Home_environment_monitoring/button_func.o

#除法库
objs += lib/libgcc.a

INCDIR = -I ./include/ -I ./lcd/include/ -I ./adc_touchscreen/include/ -I ./app_Home_environment_monitoring/include/
CFLAGS = -Werror $(INCDIR) -march=armv4 
#THUMB = -I ./include/

all: $(objs)
	arm-linux-ld -T sdram.lds $^ -o main.elf
	arm-linux-objcopy -O binary -S main.elf main.bin
	arm-linux-objdump -D main.elf > main.dis


%.o : %.c
	arm-linux-gcc $(CFLAGS) -c $< -o $@
	
%.o : %.S
	arm-linux-gcc -march=armv4 -c $< -o $@

clean:
	rm -fr *.bin *.o *.elf *.dis
	rm -fr ./lcd/*.bin ./lcd/*.o ./lcd/*.elf ./lcd/*.dis

	rm -fr ./adc_touchscreen/*.bin ./adc_touchscreen/*.o ./adc_touchscreen/*.elf ./adc_touchscreen/*.dis
	
	rm -fr ./app_Home_environment_monitoring/*.bin ./app_Home_environment_monitoring/*.o ./app_Home_environment_monitoring/*.elf ./app_Home_environment_monitoring/*.dis

.PHONY: clean
