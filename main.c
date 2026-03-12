#include <stdio.h>

#include "lvgl.h"
#include "lv_port_disp.h"

#include "usart/usart.h"
#include "delay/delay.h"
#include "i2c/i2c.h"
#include "led/led.h"
#include "btn/button.h"
#include "oled_small/oled.h"
#include "tim/tim.h"

int main()
{
	// led初始化
	led_init();	
	
	//button初始化
	key_init();
	
	// 串口1初始化波特率为115200bps
	usart1_init(115200);

	OLED_Init();
	
	tim3_init();
	// 串口延迟一会，确保芯片内部完成全部初始化,printf无乱码输出
	delay_ms(500);

	printf("\r\n LVGL Loading\r\n");

	lv_init();
	lv_display_t* disp_main=lv_port_disp_main_init();
	lv_display_t* disp_1=lv_port_disp_sub_init();
	lv_port_indev_init(disp_main);
	
	lv_display_set_default(disp_main);
	lv_obj_t * swc = lv_switch_create(lv_scr_act());
	lv_obj_align(swc, LV_ALIGN_CENTER, 0, 0);

	lv_obj_t * label = lv_label_create(lv_scr_act());
	lv_label_set_text(label, "Main screen1");
	lv_obj_align_to(label, swc, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);

	lv_obj_t * label1 = lv_label_create(lv_scr_act());
	lv_label_set_text(label1, "0");
	lv_obj_align_to(label1, swc, LV_ALIGN_OUT_BOTTOM_MID, 0, 50);
	
	lv_display_set_default(disp_1);
	lv_obj_t * btn_sub = lv_btn_create(lv_scr_act());
	lv_obj_align(btn_sub, LV_ALIGN_CENTER, 0, 0);

	lv_obj_t * label_sub = lv_label_create(btn_sub);
	lv_label_set_text(label_sub, "Screen 1");
	lv_obj_center(label_sub);

	OLED_Clear();
	
	int i=0;
	while(1)
	{
		if(i%100==0)
		{
			lv_label_set_text_fmt(label1, "Cnt:%d", i);
			printf("log:%d\r\n",i);
		}
		i++;
		lv_timer_handler();
		delay_ms(5);
	}
}