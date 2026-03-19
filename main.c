#include <stdio.h>

#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"

#include "usart/usart.h"
#include "delay/delay.h"
#include "i2c/i2c.h"
#include "led/led.h"
#include "btn/button.h"
#include "oled_small/oled.h"
#include "tim/tim.h"

#include "ui/ui_register.h"
#include "ui/ui_add_value.h"

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
    lv_label_set_text(title, "Party Room Rental");
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
    lv_obj_t * footer = lv_obj_create(scr);
    lv_obj_set_size(footer, lv_pct(100), 50); // 宽度100%，高度略大于按钮
    lv_obj_align(footer, LV_ALIGN_BOTTOM_MID, 0, -5);
    lv_obj_set_style_border_width(footer, 0, 0);
    lv_obj_set_style_pad_all(footer, 0, 0);
    lv_obj_set_style_pad_column(footer, 10, 0); // 设置两个按钮之间的间距
	//flex layout in obj
    lv_obj_set_layout(footer, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(footer, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(footer, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    //reg btn
    lv_obj_t * btn_reg = lv_btn_create(footer);
    lv_obj_set_size(btn_reg, 140, 35);
    lv_obj_set_style_bg_color(btn_reg, lv_color_hex(0x3498DB), 0);
	lv_obj_add_event_cb(btn_reg, ui_show_register, LV_EVENT_CLICKED, NULL);
	ui_preload_register(scr);
	
    lv_obj_t * reg_label = lv_label_create(btn_reg);
    lv_label_set_text(reg_label, "REGISTER");
    lv_obj_center(reg_label);

    //add val btn
    lv_obj_t * btn_add = lv_btn_create(footer);
    lv_obj_set_size(btn_add, 140, 35);
    lv_obj_set_style_bg_color(btn_add, lv_color_hex(0x3498DB), 0);
	lv_obj_add_event_cb(btn_add, ui_show_add_value, LV_EVENT_CLICKED, NULL);
	ui_preload_add_value(scr);
	
    lv_obj_t * add_label = lv_label_create(btn_add);
    lv_label_set_text(add_label, "Top UP");
    lv_obj_center(add_label);

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
	
	//txt
    label_text = lv_label_create(screen);
    lv_label_set_text(label_text, "Insert\nYour\nCARD"); 
    lv_obj_set_style_text_line_space(label_text, 2, 0);//2px away each col
    lv_obj_align(label_text, LV_ALIGN_LEFT_MID, 30, 0);
    lv_obj_set_style_text_color(label_text, lv_color_black(), 0);
}


int is_using=0;
task cur_task=NORMAL;

void using_mode_switch(lv_event_t * e)
{
	uintptr_t value = (uintptr_t)lv_event_get_param(e);//data store as addr
	if(value==1)//so just read it
	{
		if(card_balance>0)
		{
			//disp_main
			lv_obj_set_style_bg_color(card[0], lv_palette_main(LV_PALETTE_BLUE), 0);
			lv_label_set_text(stat[0], "In\nUSE");
			//disp_1
			lv_obj_set_style_bg_color(blue_rect, lv_palette_main(LV_PALETTE_BLUE), 0);
			lv_label_set_text(label_arrow, "");//hide the arrow
			lv_label_set_text_fmt(label_text, "ID:%d\n$%d.%d",card_id, card_balance/100,card_balance-card_balance/100*100); 
			is_using=1;
		}
		else//no balance
		{
			//disp_main
			lv_obj_set_style_bg_color(card[0], lv_palette_main(LV_PALETTE_ORANGE), 0);
			lv_label_set_text(stat[0], "Low\nBalance");
			//disp_1
			lv_obj_set_style_bg_color(blue_rect, lv_palette_main(LV_PALETTE_ORANGE), 0);
			lv_label_set_text(label_arrow, "$?");//where is your money?
			lv_label_set_text_fmt(label_text, "ID:%d\n$%s%d.%d",card_id,(card_balance<0)?"-":"", abs(card_balance/100),abs(card_balance-card_balance/100*100)); 
			is_using=1;
		}
	}
	else if(value==0)
	{
		//disp_main
		lv_obj_set_style_bg_color(card[0], lv_palette_main(LV_PALETTE_GREEN),0);
		lv_label_set_text(stat[0],"READY");
		//disp_1
		lv_obj_set_style_bg_color(blue_rect, lv_palette_main(LV_PALETTE_GREEN), 0);
		lv_label_set_text(label_text, "Insert\nYour\nCARD"); 
		lv_label_set_text(label_arrow, LV_SYMBOL_LEFT);//show the arrow
		is_using=0;
	}
	else if(value==3)//disp_1 tells user to tap for reg and add value
	{
		lv_obj_set_style_bg_color(blue_rect, lv_color_white(), 0);//invisible, label_text decleared later than blue_rect, won't block
		lv_label_set_text(label_text, "TAP your CARD"); 
		lv_label_set_text(label_arrow, LV_SYMBOL_NEW_LINE);//show cross
	}
}