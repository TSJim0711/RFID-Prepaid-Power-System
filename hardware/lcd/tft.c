#include "stm32f4xx.h"
#include "lcd/tft.h"
#include "lcd/lcd_font.h"
#include "sys.h"
void spi1_send_byte(u8 byte)
{
#if LCD_SOFT_SPI_ENABLE
  unsigned char counter;

  for (counter = 0; counter < 8; counter++)
  {
    SPI_SCK_0;
    if ((byte & 0x80) == 0)
    {
      SPI_SDA_0;
    }
    else
      SPI_SDA_1;
    byte = byte << 1;
    SPI_SCK_1;
  }
  SPI_SCK_0;	
	
#else
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
		;
	SPI_I2S_SendData(SPI1, byte);

	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
		;
	SPI_I2S_ReceiveData(SPI1);	
	
#endif

}

void lcd_send_cmd(u8 cmd)
{
	LCD_DC_0;
	spi1_send_byte(cmd);
}

// 向液晶屏写一个8位数据
void lcd_send_data(u8 dat)
{
	LCD_DC_1;
	spi1_send_byte(dat);
}

void lcd_addr_set(u32 x_s, u32 y_s, u32 x_e, u32 y_e)
{
    // 如果发现显示偏了，尝试修改这里的 +26 和 +1
    x_s += 26;
    x_e += 26;
    y_s += 1;
    y_e += 1;

    lcd_send_cmd(0x2a);         // 列地址设置
    lcd_send_data(x_s >> 8);
    lcd_send_data(x_s);
    lcd_send_data(x_e >> 8);
    lcd_send_data(x_e);

    lcd_send_cmd(0x2b);         // 行地址设置
    lcd_send_data(y_s >> 8);
    lcd_send_data(y_s);
    lcd_send_data(y_e >> 8);
    lcd_send_data(y_e);
    
    lcd_send_cmd(0x2C);         // 写显存
}

void lcd_fill(u32 x_s, u32 y_s, u32 x_len, u32 y_len,u32 color)
{
	u32 x, y;
	
	lcd_addr_set(x_s,y_s,x_s+x_len-1,y_s+y_len - 1);

	for (y = y_s; y < y_s+y_len; y++) 
	{
		for (x = x_s; x < x_s+x_len; x++) 
		{

			lcd_send_data(color >> 8);
			lcd_send_data(color);
		}
	}
}

void lcd_clear(u32 color)
{
	lcd_fill(0,0,LCD_WIDTH,LCD_HEIGHT,color);
}

void lcd_draw_picture(u32 x_s, u32 y_s, u32 width, u32 height, const u8 *pic)
{

	const u8 *p = pic;
	u32 i = 0;
	
	lcd_addr_set(x_s, y_s, x_s+width-1, y_s+height-1);

	for (i = 0; i <width*height*2; i += 2) 
	{
		lcd_send_data(p[i]);	
		lcd_send_data(p[i + 1]); 
	}
} 

void lcd_show_char(u32 x, u32 y,char* ch_ptr,u32 fc,u32 bc,u32 font_size,u32 mode)
{
	char ch = (uint8_t)(ch_ptr)-'0';//得到偏移后的值

	u8 temp,sizex,t,m=0;
	u16 i,TypefaceNum;//一个字符所占字节大小
	
	u16 x0=x;
	
	sizex=font_size/2;
	
	TypefaceNum=(sizex/8+((sizex%8)?1:0))*font_size;						
	
	lcd_addr_set(x,y,x+sizex-1,y+font_size-1);  //设置光标位置 
	
	for(i=0;i<TypefaceNum;i++)
	{ 
		if(font_size==12)temp=ascii_1206[ch][i];		      //调用6x12字体
		else if(font_size==16)temp=ascii_1608[ch][i];		 //调用8x16字体
		else if(font_size==24)temp=ascii_2412[ch][i];		 //调用12x24字体
		else if(font_size==32)temp=ascii_3216[ch][i];		 //调用16x32字体
		else return;
		for(t=0;t<8;t++)
		{
			if(!mode)//非叠加模式
			{
				if(temp&(0x01<<t))
				{
					lcd_send_data(fc>>8);
					lcd_send_data(fc);
				}
				else 
				{
					lcd_send_data(bc>>8);
					lcd_send_data(bc);
				}
				m++;
				if(m%sizex==0)
				{
					m=0;
					break;
				}
			}
			else//叠加模式
			{
				if(temp&(0x01<<t))lcd_draw_point(x,y,fc);//画一个点
				x++;
				if((x-x0)==sizex)
				{
					x=x0;
					y++;
					break;
				}
			}
		}
	}  
}

/******************************************************************************
*函数说明：显示数字
*入口数据：m底数，n指数
*返回值：  无
******************************************************************************/
u32 mypow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;
	return result;
}


void lcd_init(void)
{
#if LCD_SOFT_SPI_ENABLE
    // 开启时钟，确保包含 GPIOE
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
    // ... GPIOD 初始化保持不变 ...
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // --- 修改部分：GPIOE 初始化 ---
    // 增加 GPIO_Pin_10 (BLK) 和 GPIO_Pin_12 (CS), GPIO_Pin_14 (DC)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_12 | GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    // --- 修改结束 ---

    // ... GPIOB 初始化保持不变 ...
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_Init(GPIOB, &GPIO_InitStructure); // 这里省略了重复的配置代码，实际应保留完整配置
	
#else
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

	// SCK=PB3,  MOSI=PB5
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	 //复用功能模式
	GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed; //引脚高速工作，收到指令立即工作；缺点：功耗高
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 	 //增加输出电流的能力
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; //不需要上下拉电阻
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; 	 //输出模式
	GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed; //引脚高速工作，收到指令立即工作；缺点：功耗高
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 	 //增加输出电流的能力
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; //不需要上下拉电阻
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; 	 //输出模式
	GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed; //引脚高速工作，收到指令立即工作；缺点：功耗高
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 	 //增加输出电流的能力
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; //不需要上下拉电阻
	GPIO_Init(GPIOG, &GPIO_InitStructure);

	// PB3 PB5连接到SPI1硬件
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_SPI1);

	//关闭SPI1
	SPI_Cmd(SPI1, DISABLE);

	//设置SPI
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; //全双工收发
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master; 	   //设为主机
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b; 	   // 8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High; 	   //空闲时时钟为低
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge; 	   //第1个时钟沿捕获数据
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft; 	   // CS由SSI位控制（自控）
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2; //波特率为2分频
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; 	   //高位先传送
	SPI_InitStructure.SPI_CRCPolynomial = 0; 	   //不使用CRC
	SPI_Init(SPI1, &SPI_InitStructure); 	   //初始化SPI1

	//启动SPI1
	SPI_Cmd(SPI1, ENABLE);
#endif

	LCD_CS_0;      // 关键！拉低片选，选中屏幕芯片
    LCD_BLK_1;     // 拉高背光，点亮屏幕
    SPI_SCK_1;     
    
    LCD_RST_0;
    delay_ms(100);
    LCD_RST_1;
    delay_ms(100);

	LCD_BLK_1;
	SPI_SCK_0;

	SPI_SCK_1; // 特别注意！！
	LCD_RST_0;
	delay_ms(1000);
	LCD_RST_1;
	delay_ms(1000);
	
	// 根据提供的指令表进行初始化
	lcd_send_cmd(0x11); // Sleep Out & Booster On
	delay_ms(120);		// DELAY120ms
	
	// 设置像素格式
	lcd_send_cmd(0x3A); // Interface Pixel Format
	lcd_send_data(0x05); // 16-bit/pixel
	
	// 设置内存访问控制
	lcd_send_cmd(0x36); // Memory Access Control
	lcd_send_data(0x08); // 默认方向
	
	// 关闭显示倒置
	lcd_send_cmd(0x20); // Display Inversion Off
	
	// 开启显示
	lcd_send_cmd(0x29); // Display On
}

void lcd_show_chn(u32 x, u32 y,uint8_t no, u32 fc, u32 bc,u32 font_size)
{
	u32 i,j;
	u8 tmp;

	lcd_addr_set(x, y, x + font_size-1, y + font_size-1);

	for (i = 0; i < (font_size*font_size/8); i++) // column loop
	{
		if(font_size==16)tmp = chinese_tbl_16[no][i];
		if(font_size==24)tmp = chinese_tbl_24[no][i];	
		if(font_size==32)tmp = chinese_tbl_32[no][i];
		
		for (j = 0;j < 8; j++)
		{
			if (tmp & (1<<j))
			{
				lcd_send_data(fc >> 8);
				lcd_send_data(fc);
			}
			
			else
			{
				lcd_send_data(bc);
				lcd_send_data(bc);
			}
		}
	}
}

void lcd_draw_point(u32 x, u32 y, u32 color)
{
	lcd_addr_set(x, y, x, y);

	lcd_send_data(color >> 8);
	lcd_send_data(color);
}


void lcd_draw_line(u16 x1,u16 y1,u16 x2,u16 y2,u16 color)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance;
	int incx,incy,uRow,uCol;
	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1;
	uRow=x1;//画线起点坐标
	uCol=y1;
	if(delta_x>0)incx=1; //设置单步方向 
	else if (delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;}
	if(delta_y>0)incy=1;
	else if (delta_y==0)incy=0;//水平线 
	else {incy=-1;delta_y=-delta_y;}
	if(delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y;
	for(t=0;t<distance+1;t++)
	{
		lcd_draw_point(uRow,uCol,color);//画点
		xerr+=delta_x;
		yerr+=delta_y;
		if(xerr>distance)
		{
			xerr-=distance;
			uRow+=incx;
		}
		if(yerr>distance)
		{
			yerr-=distance;
			uCol+=incy;
		}
	}
}

void lcd_draw_rectangle(u16 x1, u16 y1, u16 x2, u16 y2,u16 color)
{
	lcd_draw_line(x1,y1,x2,y1,color);
	lcd_draw_line(x1,y1,x1,y2,color);
	lcd_draw_line(x1,y2,x2,y2,color);
	lcd_draw_line(x2,y1,x2,y2,color);
}

void lcd_draw_circle(u16 x0,u16 y0,u8 r,u16 color)
{
	int a,b;
	
	a=0;b=r;
	
	while(a<=b)
	{
		lcd_draw_point(x0-b,y0-a,color);             //3           
		lcd_draw_point(x0+b,y0-a,color);             //0           
		lcd_draw_point(x0-a,y0+b,color);             //1                
		lcd_draw_point(x0-a,y0-b,color);             //2             
		lcd_draw_point(x0+b,y0+a,color);             //4               
		lcd_draw_point(x0+a,y0-b,color);             //5
		lcd_draw_point(x0+a,y0+b,color);             //6 
		lcd_draw_point(x0-b,y0+a,color);             //7
		a++;
		if((a*a+b*b)>(r*r))//判断要画的点是否过远
		{
			b--;
		}
	}
}


