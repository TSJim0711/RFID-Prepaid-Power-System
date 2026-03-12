/**
 * @file lv_port_disp_template.c
 *
 */

/*Copy this file as "lv_port_disp.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_disp.h"
#include <stdbool.h>
#include "lcd/tft.h"
#include "lcd_small/tft_small.h"

/*********************
 *      DEFINES
 *********************/

#define BYTE_PER_PIXEL (LV_COLOR_FORMAT_GET_SIZE(LV_COLOR_FORMAT_RGB565)) /*will be 2 for RGB565 */

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void disp_init(void);

static void disp_flush_main(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map);
static void disp_flush_small(lv_display_t * disp_drv, const lv_area_t * area, uint8_t * px_map);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_display_t* lv_port_disp_main_init(void)
{
	LCD_Init();
    //Create a display and set a flush_cb
	static uint8_t buf_1[240 * 320 * BYTE_PER_PIXEL / 10];           /*A buffer for 10 rows*/
	lv_display_t * disp_main = lv_display_create(240, 320);
	lv_display_set_flush_cb(disp_main, disp_flush_main);
    lv_display_set_buffers(disp_main, buf_1, NULL, sizeof(buf_1), LV_DISPLAY_RENDER_MODE_PARTIAL);
	return disp_main;
}


lv_display_t* lv_port_disp_sub_init(void)
{
    lcd_init();
    //Create a display and set a flush_cb
    static uint8_t buf_1_small[80 * 160 * BYTE_PER_PIXEL / 10];           /*A buffer for 10 rows*/
    lv_display_t * disp_sub = lv_display_create(80, 160);
	lv_display_set_flush_cb(disp_sub, disp_flush_small);
    lv_display_set_buffers(disp_sub, buf_1_small, NULL, sizeof(buf_1_small), LV_DISPLAY_RENDER_MODE_PARTIAL);
    return disp_sub;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
volatile bool disp_flush_enabled = true;

/* Enable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_enable_update(void)
{
    disp_flush_enabled = true;
}

/* Disable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_disable_update(void)
{
    disp_flush_enabled = false;
}

/*Flush the content of the internal buffer the specific area on the display.
 *`px_map` contains the rendered image as raw pixel map and it should be copied to `area` on the display.
 *You can use DMA or any hardware acceleration to do this operation in the background but
 *'lv_display_flush_ready()' has to be called when it's finished.*/

static void disp_flush_main(lv_display_t * disp_drv, const lv_area_t * area, uint8_t * px_map)
{
    if(disp_flush_enabled) {
        /*The most simple case (but also the slowest) to put all pixels to the screen one-by-one*/
        uint16_t * color_p = (uint16_t *)px_map;//from u8 to u16 pointer 2 byte per ++
        int32_t x;
        int32_t y;
        for(y = area->y1; y <= area->y2; y++) {
            for(x = area->x1; x <= area->x2; x++) {
                /*Put a pixel to the display. For example:*/
                /*put_px(x, y, *px_map)*/
				LCD_Fill(x, y, x, y, *color_p);
                color_p++;
            }
        }
    }
    lv_display_flush_ready(disp_drv);
}

static void disp_flush_small(lv_display_t * disp_drv, const lv_area_t * area, uint8_t * px_map)
{
    if(disp_flush_enabled) {
        /*The most simple case (but also the slowest) to put all pixels to the screen one-by-one*/
        uint16_t * color_p = (uint16_t *)px_map;//from u8 to u16 pointer 2 byte per ++
        int32_t x;
        int32_t y;
        for(y = area->y1; y <= area->y2; y++) {
            for(x = area->x1; x <= area->x2; x++) {
                /*Put a pixel to the display. For example:*/
                /*put_px(x, y, *px_map)*/
				lcd_draw_point(x, y, *color_p);
                color_p++;
            }
        }
    }
    lv_display_flush_ready(disp_drv);
}

#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
