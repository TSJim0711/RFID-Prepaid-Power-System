#include <stdio.h>

#include "lvgl.h"
#include "lv_port_disp.h"


#include "usart/usart.h"
#include "delay/delay.h"
#include "i2c/i2c.h"
#include "led/led.h"
#include "btn/button.h"
#include "oled_small/oled.h"

int main()
{
	// led初始化
	led_init();	
	
	//button初始化
	key_init();
	
	// 串口1初始化波特率为115200bps
	usart1_init(115200);

	OLED_Init();
	// 串口延迟一会，确保芯片内部完成全部初始化,printf无乱码输出
	delay_ms(500);

	printf("\r\n LVGL Loading\r\n");

	lv_init();
	lv_display_t* disp_main=lv_port_disp_main_init();
	lv_display_t* disp_1=lv_port_disp_sub_init();
	
	lv_display_set_default(disp_main);
	lv_obj_t * btn = lv_btn_create(lv_scr_act());
	lv_obj_align(btn, LV_ALIGN_CENTER, 0, 0);    

	lv_obj_t * label = lv_label_create(btn);
	lv_label_set_text(label, "Main screen");
	lv_obj_center(label);
	
	lv_display_set_default(disp_1);
	lv_obj_t * btn_sub = lv_btn_create(lv_scr_act());
	lv_obj_align(btn_sub, LV_ALIGN_CENTER, 0, 0);

	lv_obj_t * label_sub = lv_label_create(btn_sub);
	lv_label_set_text(label_sub, "Screen 1");
	lv_obj_center(label_sub);

	OLED_Clear();
	
	while(1)
	{
		lv_timer_handler();
		delay_ms(1);
	}
}