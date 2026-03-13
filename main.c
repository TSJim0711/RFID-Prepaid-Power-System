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

lv_display_t* disp_1;
lv_obj_t * card[4];
lv_obj_t * num[4];
lv_obj_t * stat[4];

void display_sub_init();
int main()
{
	// led初始化
	led_init();	
	
	// 串口1初始化波特率为115200bps
	usart1_init(115200);

	OLED_Init();
	
	tim3_init();
	// 串口延迟一会，确保芯片内部完成全部初始化,printf无乱码输出
	delay_ms(500);

	printf("\r\n LVGL Loading...\r\n");

	lv_init();
	lv_display_t* disp_main=lv_port_disp_main_init();
	disp_1=lv_port_disp_sub_init();
	lv_port_indev_init(disp_main, disp_1);
	
	LV_FONT_DECLARE(lv_font_montserrat_28);
	LV_FONT_DECLARE(lv_font_montserrat_10);

	//main screen
	lv_display_set_default(disp_main);
    lv_obj_t * scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_hex(0xF4F7F6), 0);

    //header
    lv_obj_t * header = lv_obj_create(scr);
    lv_obj_set_size(header, 320, 40);
    lv_obj_set_style_bg_color(header, lv_color_hex(0x2C3E50), 0);
    lv_obj_set_style_radius(header, 0, 0);
    lv_obj_set_style_border_width(header, 0, 0);
	lv_obj_set_style_pad_all(header, 0, 0);
    lv_obj_align(header, LV_ALIGN_TOP_MID, 0, 0);

    lv_obj_t * title = lv_label_create(header);
    lv_label_set_text(title, "XYZ Network Cafe");
    lv_obj_set_style_text_color(title, lv_color_white(), 0);
    lv_obj_align(title, LV_ALIGN_LEFT_MID, 10, 0);

    // Settings Button, use * symb for now
    lv_obj_t * btn_set = lv_btn_create(header);
    lv_obj_set_size(btn_set, 30, 30);
    lv_obj_align(btn_set, LV_ALIGN_RIGHT_MID, -5, 0);
    lv_obj_set_style_bg_color(btn_set, lv_color_hex(0x3498DB), 0);
    
    lv_obj_t * set_label = lv_label_create(btn_set);
    lv_label_set_text(set_label, "*");
    lv_obj_center(set_label);

    //device list
    lv_obj_t * cont = lv_obj_create(scr);
    lv_obj_set_size(cont, 320, 160);
    lv_obj_align(cont, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_bg_opa(cont, 0, 0);//transp
    lv_obj_set_style_border_width(cont, 0, 0);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    for(int i = 0; i < 4; i++) {
        card[i] = lv_obj_create(cont);
        lv_obj_set_size(card[i] , 65, 120);
        lv_obj_set_style_radius(card[i] , 4, 0);
        lv_obj_set_style_border_width(card[i] , 0, 0);
        lv_obj_set_style_bg_color(card[i] , i==1-1?lv_palette_main(LV_PALETTE_GREEN):lv_palette_main(LV_PALETTE_GREY), 0);

        num[i] = lv_label_create(card[i]);
        lv_label_set_text_fmt(num[i], "%d", i+1);//i start from 0, here start from 1, so +1
        lv_obj_set_style_text_font(num[i], &lv_font_montserrat_28, 0);
        lv_obj_set_style_text_color(num[i], lv_color_white(), 0);
        lv_obj_align(num[i], LV_ALIGN_TOP_MID, 0, 10);

        stat[i] = lv_label_create(card[i]);
        lv_label_set_text(stat[i], (i == 1-1) ? "READY" : "NOT\nFOUND");
        lv_obj_set_style_text_font(stat[i], &lv_font_montserrat_10, 0);
        lv_obj_set_style_text_color(stat[i], lv_color_white(), 0);
        lv_obj_align(stat[i], LV_ALIGN_BOTTOM_MID, 0, -5);
    }

    //footer
    lv_obj_t * btn_reg = lv_btn_create(scr);
    lv_obj_set_size(btn_reg, 300, 35);
    lv_obj_align(btn_reg, LV_ALIGN_BOTTOM_MID, 0, -5);
    lv_obj_set_style_bg_color(btn_reg, lv_color_hex(0x3498DB), 0);

    lv_obj_t * reg_label = lv_label_create(btn_reg);
    lv_label_set_text(reg_label, "REGISTER");
    lv_obj_center(reg_label);

	//small screen
	display_sub_init();

	OLED_Clear();
	
	int i=0;
	while(1)
	{
		lv_timer_handler();
		delay_ms(3);
	}
}

lv_obj_t * blue_rect;
lv_obj_t * swc_mode_switcher;
lv_obj_t * label_text;
lv_obj_t * label_arrow;
uint32_t SLOT_1_IN_USE_SIG;//DISP1's seat in use singal
static void using_mode_switch(lv_event_t * e);
void display_sub_init()
{
	lv_display_set_default(disp_1);
    lv_obj_t * screen = lv_scr_act();
    lv_obj_set_style_bg_color(screen, lv_color_white(), 0);

    SLOT_1_IN_USE_SIG=lv_event_register_id();
    lv_obj_add_event_cb(lv_layer_sys(), using_mode_switch, (lv_event_code_t)SLOT_1_IN_USE_SIG, NULL);//subscribe to slot in use msg

    //arrow
    label_arrow = lv_label_create(screen);
    lv_label_set_text(label_arrow, LV_SYMBOL_LEFT);
    lv_obj_align(label_arrow, LV_ALIGN_LEFT_MID, 10, 0); 
    lv_obj_set_style_text_color(label_arrow, lv_color_black(), 0);

    //txt
    label_text = lv_label_create(screen);
    lv_label_set_text(label_text, "Insert\nYour\nCARD"); 
    lv_obj_set_style_text_line_space(label_text, 2, 0);//2px away each col
    lv_obj_align(label_text, LV_ALIGN_LEFT_MID, 30, 0);
    lv_obj_set_style_text_color(label_text, lv_color_black(), 0);

	//righter
	blue_rect=lv_obj_create(screen);
    lv_obj_set_size(blue_rect, 64, 80);//160 * 40% = 64
    lv_obj_align(blue_rect, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_set_style_bg_color(blue_rect, lv_palette_main(LV_PALETTE_GREEN), 0);
    lv_obj_set_style_bg_opa(blue_rect, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(blue_rect, 0, 0);
    lv_obj_set_style_border_width(blue_rect, 0, 0);
	lv_obj_set_style_pad_all(blue_rect, 0, 0);

    //text in righter
    lv_obj_t * label_num = lv_label_create(blue_rect);
    lv_label_set_text(label_num, "01");
	lv_obj_set_style_text_font(label_num, &lv_font_montserrat_28, 0);
    lv_obj_set_style_text_color(label_num, lv_color_white(), 0);
    lv_obj_center(label_num);
}

static void using_mode_switch(lv_event_t * e)
{
	uintptr_t value = (uintptr_t)lv_event_get_param(e);//data store as addr
    printf("Slot status :%d\r\n",value);
	if(value==1)//so just read it
	{
        printf("Slot status changed: in use\r\n");
		//disp_main
		lv_obj_set_style_bg_color(card[0], lv_palette_main(LV_PALETTE_BLUE), 0);
		lv_label_set_text(stat[0], "In\nUSE");
		//disp_1
		lv_obj_set_style_bg_color(blue_rect, lv_palette_main(LV_PALETTE_BLUE), 0);
		lv_obj_set_style_opa(label_arrow, 0, 0);//hide the arrow
		lv_label_set_text(label_text, "Enjoy!"); 
	}
	else if(value==0)
	{
        printf("Slot status changed: ready\r\n");
		//disp_main
		lv_obj_set_style_bg_color(card[0], lv_palette_main(LV_PALETTE_GREEN),0);
		lv_label_set_text(stat[0],"READY");
		//disp_1
		lv_obj_set_style_bg_color(blue_rect, lv_palette_main(LV_PALETTE_GREEN), 0);
		lv_label_set_text(label_text, "Insert\nYour\nCARD"); 
		lv_obj_set_style_opa(label_arrow, 255, 0);//show the arrow
	}else
        printf("Unexpect msg value.\r\n");
}