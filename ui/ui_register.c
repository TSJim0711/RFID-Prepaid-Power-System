#include "lvgl.h"
#include "ui/ui_register.h"
#include "lv_port_indev.h"

lv_obj_t * popup_register;
static void btn_rtn_cb(lv_event_t * e);
extern uint32_t SLOT_1_IN_USE_SIG;

void ui_preload_register(lv_obj_t * scr) 
{
    // 1. 创建一个覆盖全屏的透明蒙版层（点击蒙版可以关闭弹窗）
    popup_register = lv_obj_create(scr);
    lv_obj_set_size(popup_register, lv_pct(100), lv_pct(100)); // 全屏
    lv_obj_center(popup_register);
    lv_obj_set_style_bg_color(popup_register, lv_palette_main(LV_PALETTE_GREY), 0);
    lv_obj_set_style_bg_opa(popup_register, LV_OPA_50, 0); // 半透明背景
    lv_obj_set_style_border_width(popup_register, 0, 0);
    lv_obj_set_style_radius(popup_register, 0, 0);
    
    // 关键：设为 Flex 居中布局，里面的内容自动居中
    lv_obj_set_layout(popup_register, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(popup_register, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(popup_register, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_gap(popup_register, 15, 0); // 设置子对象之间的间距


    // 2. 创建弹窗实体 (白色的框)
    lv_obj_t * content_box = lv_obj_create(popup_register);
    lv_obj_set_size(content_box, 320, LV_SIZE_CONTENT); // 宽度固定，高度自适应
    lv_obj_set_style_bg_color(content_box, lv_color_white(), 0);
    lv_obj_set_style_radius(content_box, 10, 0);
    lv_obj_set_style_shadow_width(content_box, 20, 0); // 添加阴影
    lv_obj_set_style_shadow_opa(content_box, LV_OPA_30, 0);
    
    // 实体内部设为 Flex 布局，垂直排列
    lv_obj_set_layout(content_box, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(content_box, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(content_box, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_all(content_box, 20, 0);
    lv_obj_set_style_pad_gap(content_box, 10, 0); // 内部元素间距


    // 3. 添加 "Your Name" 标签
    lv_obj_t * label_txt_row_1 = lv_label_create(content_box);
    lv_label_set_text(label_txt_row_1, "Card registration.");
	lv_obj_t * label_txt_row_2 = lv_label_create(content_box);
    lv_label_set_text(label_txt_row_2, "Please TAP.");
    lv_obj_set_style_text_font(label_txt_row_2, &lv_font_montserrat_28, 0); // 字体稍大
	

    // 5. 添加 "return" 按钮
    lv_obj_t * btn_rtn = lv_btn_create(content_box);
    lv_obj_set_size(btn_rtn, 70, 30);
    lv_obj_align(btn_rtn, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_color(btn_rtn, lv_color_hex(0x3498DB), 0);
    lv_obj_add_event_cb(btn_rtn, btn_rtn_cb, LV_EVENT_CLICKED, NULL);
	
	

    lv_obj_t * label_btn_rtn = lv_label_create(btn_rtn);
    lv_label_set_text(label_btn_rtn, "Return");
    lv_obj_center(label_btn_rtn);	
	
	lv_obj_add_flag(popup_register, LV_OBJ_FLAG_HIDDEN);
}

void ui_show_register(lv_event_t * e)
{
	lv_obj_remove_flag(popup_register, LV_OBJ_FLAG_HIDDEN);
	rfid_reader_task = REGISTRATION;
	lv_obj_send_event(lv_layer_sys(), SLOT_1_IN_USE_SIG, (void *)3);//send 3 msg, to change disp_1 content
}


static void btn_rtn_cb(lv_event_t * e) {
    lv_obj_add_flag(popup_register, LV_OBJ_FLAG_HIDDEN);
	lv_obj_send_event(lv_layer_sys(), SLOT_1_IN_USE_SIG, (void *)0);
	rfid_reader_task = NORMAL;
}