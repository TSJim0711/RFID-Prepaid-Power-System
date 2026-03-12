 /*��    ��:��Ƕ.�¹�*/
#ifndef __TFT_H__
#define __TFT_H__

#include "sys.h"

#define LCD_WIDTH  80
#define LCD_HEIGHT 160

#define LCD_SOFT_SPI_ENABLE  1

#define RED 	0XF800   	// ��ɫ
#define GREEN 	0X07E0 		// ��ɫ
#define BLUE 	0X001F  	// ��ɫ
#define WHITE 	0XFFFF 		// ��ɫ
#define BLACK 	0X0000 		// ��ɫ

#if LCD_SOFT_SPI_ENABLE
#define SPI_SCK_0 PCout(6) = 0
#define SPI_SCK_1 PCout(6) = 1

#define SPI_SDA_0 PCout(8) = 0
#define SPI_SDA_1 PCout(8) = 1

#define LCD_RST_0 PCout(11) = 0
#define LCD_RST_1 PCout(11) = 1

#define LCD_DC_0 PEout(5) = 0
#define LCD_DC_1 PEout(5) = 1

#define LCD_CS_0  PAout(6) = 0
#define LCD_CS_1  PAout(6) = 1

#define LCD_BLK_0  1 //BLK is connected to 3v3
#define LCD_BLK_1  1

#else

#define SPI_SCK_0 PBout(15) = 0
#define SPI_SCK_1 PBout(15) = 1

#define SPI_SDA_0 PBout(3) = 0
#define SPI_SDA_1 PBout(3) = 1

#define LCD_RST_0 PGout(8) = 0
#define LCD_RST_1 PGout(8) = 1

#define LCD_DC_0 PGout(7) = 0
#define LCD_DC_1 PGout(7) = 1

#define LCD_BLK_0 PBout(4) = 0
#define LCD_BLK_1 PBout(4) = 1

#endif

extern void lcd_init(void);
extern void lcd_addr_set(uint32_t x_s, uint32_t y_s, uint32_t x_e, uint32_t y_e);
extern void lcd_clear(uint32_t color);
extern void lcd_send_cmd(uint8_t cmd);
extern void lcd_send_data(uint8_t dat);
extern void lcd_draw_point(uint32_t x, uint32_t y, uint32_t color);
extern void lcd_draw_picture(uint32_t x_s, uint32_t y_s, uint32_t width, uint32_t height, const uint8_t *pic);
extern void lcd_show_chn(uint32_t x, uint32_t y,uint8_t no, uint32_t fc, uint32_t bc,uint32_t font_size);
extern void lcd_draw_rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint16_t color);
extern void lcd_draw_circle(uint16_t x0,uint16_t y0,uint8_t r,uint16_t color);
extern void lcd_draw_line(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t color);
#endif
