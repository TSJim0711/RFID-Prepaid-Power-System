#include "lvgl.h"
#include "ui/ui_add_value.h"
#include "lv_port_indev.h"

// 全局变量，用于控制弹窗显示/隐藏
lv_obj_t * popup_add_value;

// 内部回调函数声明
static void btn_rtn_cb(lv_event_t * e);
static void btn_amount_cb(lv_event_t * e); // 增加金额按钮的回调

extern uint32_t SLOT_1_IN_USE_SIG;

lv_obj_t * label_txt_row_1;
lv_obj_t * label_txt_row_2;
lv_obj_t * amount_cont;

void ui_preload_add_value(lv_obj_t * scr) 
{
    // === 1. 创建全屏半透明蒙版层 ===
    popup_add_value = lv_obj_create(scr);
    lv_obj_set_size(popup_add_value, lv_pct(100), lv_pct(100)); // 全屏
    lv_obj_center(popup_add_value);
    // 设置灰色、半透明背景
    lv_obj_set_style_bg_color(popup_add_value, lv_palette_main(LV_PALETTE_GREY), 0);
    lv_obj_set_style_bg_opa(popup_add_value, LV_OPA_50, 0); 
    lv_obj_set_style_border_width(popup_add_value, 0, 0);
    lv_obj_set_style_radius(popup_add_value, 0, 0);
	lv_obj_clear_flag(popup_add_value, LV_OBJ_FLAG_SCROLLABLE);//fix the window
    
    // 关键：设为 Flex 垂直居中布局，确保内容框居中
    lv_obj_set_layout(popup_add_value, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(popup_add_value, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(popup_add_value, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);


    // === 2. 创建弹窗实体 (白色的框) ===
    lv_obj_t * content_box = lv_obj_create(popup_add_value);
    lv_obj_set_size(content_box, 320, LV_SIZE_CONTENT); // 宽度固定，高度自适应
    lv_obj_set_style_bg_color(content_box, lv_color_white(), 0);
    lv_obj_set_style_radius(content_box, 10, 0); // 圆角
    lv_obj_set_style_shadow_width(content_box, 20, 0); // 添加阴影
    lv_obj_set_style_shadow_opa(content_box, LV_OPA_30, 0);
    
    // 实体内部设为 Flex 布局，垂直排列
    lv_obj_set_layout(content_box, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(content_box, LV_FLEX_FLOW_COLUMN);
    // 子元素水平居中，垂直靠顶
    lv_obj_set_flex_align(content_box, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_all(content_box, 20, 0);
    lv_obj_set_style_pad_gap(content_box, 10, 0); // 内部元素间距加大一点


    // === 3. 添加标题标签 ===
	label_txt_row_1 = lv_label_create(content_box);
    lv_label_set_text(label_txt_row_1, "Top Up.");
	label_txt_row_2 = lv_label_create(content_box);
    lv_label_set_text(label_txt_row_2, "Please TAP.");
    lv_obj_set_style_text_font(label_txt_row_2, &lv_font_montserrat_28, 0); // 字体稍大
	lv_obj_add_flag(label_txt_row_2, LV_OBJ_FLAG_HIDDEN);
	

    // === 4. 添加金额选择容器 (核心部分) ===
    // 创建一个容器，用于水平均匀排列金额按钮卡片
    amount_cont = lv_obj_create(content_box);
    lv_obj_set_size(amount_cont, lv_pct(100), LV_SIZE_CONTENT);//auto adapt height
    lv_obj_set_style_bg_opa(amount_cont, LV_OPA_0, 0); // 透明背景
    lv_obj_set_style_border_width(amount_cont, 0, 0);
    lv_obj_set_style_pad_all(amount_cont, 0, 0);

    // 设为 Flex 水平布局，并使子元素均匀分布
    lv_obj_set_layout(amount_cont, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(amount_cont, LV_FLEX_FLOW_ROW);
    // 关键：LV_FLEX_ALIGN_SPACE_EVENLY 让按钮均匀分布，两端也有间距
    lv_obj_set_flex_align(amount_cont, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);


    // === 5. 创建金额按钮 (卡片样式) ===
    const int amounts[] = {5,10,20,50,100};
    for(int i = 0; i < 5; i++) {
        // 创建一个看起来像卡片的按钮
        lv_obj_t * btn_amt = lv_btn_create(amount_cont);
        lv_obj_set_size(btn_amt, 45, 35); 
        lv_obj_set_style_radius(btn_amt, 8, 0); // 按钮圆角
        lv_obj_set_style_bg_color(btn_amt, lv_color_white(), 0); // 默认白色
        lv_obj_set_style_border_width(btn_amt, 2, 0); // 添加边框
        lv_obj_set_style_border_color(btn_amt, lv_palette_main(LV_PALETTE_BLUE), 0); // 蓝色边框
        lv_obj_set_style_shadow_width(btn_amt, 0, 0); // 移除按钮阴影，使其看起来更扁平

        // 按钮文本颜色
        lv_obj_t * label_amt = lv_label_create(btn_amt);
        lv_label_set_text_fmt(label_amt, "$%d", amounts[i]);
        lv_obj_set_style_text_font(label_amt, &lv_font_montserrat_14, 0); // 字体稍大
        lv_obj_set_style_text_color(label_amt, lv_palette_main(LV_PALETTE_BLUE), 0); // 蓝色文字
        lv_obj_center(label_amt);

        // 添加点击事件
        lv_obj_add_event_cb(btn_amt, btn_amount_cb, LV_EVENT_CLICKED, (void *)(uintptr_t)amounts[i]);
    }


    // === 6. 添加 "Return" 按钮 ===
    lv_obj_t * btn_rtn = lv_btn_create(content_box);
    lv_obj_set_size(btn_rtn, 70, 30);
    lv_obj_set_style_bg_color(btn_rtn, lv_color_hex(0x3498DB), 0); // 蓝色
    lv_obj_add_event_cb(btn_rtn, btn_rtn_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t * label_btn_rtn = lv_label_create(btn_rtn);
    lv_label_set_text(label_btn_rtn, "Return");
    lv_obj_center(label_btn_rtn);	
	
    // 默认隐藏弹窗
    lv_obj_add_flag(popup_add_value, LV_OBJ_FLAG_HIDDEN);
}

// === 显示弹窗 ===
void ui_show_add_value(lv_event_t * e)
{
	lv_obj_remove_flag(amount_cont, LV_OBJ_FLAG_HIDDEN);
	lv_obj_add_flag(label_txt_row_2, LV_OBJ_FLAG_HIDDEN);
	lv_obj_remove_flag(popup_add_value, LV_OBJ_FLAG_HIDDEN);
}

// === "Return" 按钮回调 ===
static void btn_rtn_cb(lv_event_t * e) {
    lv_obj_add_flag(popup_add_value, LV_OBJ_FLAG_HIDDEN);
	lv_label_set_text(label_txt_row_1, "Top Up.");
	rfid_reader_task = NORMAL;
	lv_obj_send_event(lv_layer_sys(), SLOT_1_IN_USE_SIG, (void *)0);
}

// === 金额按钮点击回调 (示例) ===
static void btn_amount_cb(lv_event_t * e) {
	int topn_amount = (int)(uintptr_t)lv_event_get_user_data(e);
    lv_label_set_text_fmt(label_txt_row_1, "Topping Up $%d",topn_amount);
	lv_obj_remove_flag(label_txt_row_2, LV_OBJ_FLAG_HIDDEN);
	rfid_reader_task = topn_amount;
	lv_obj_send_event(lv_layer_sys(), SLOT_1_IN_USE_SIG, (void *)3);//send 3 msg, to change disp_1 content
}