#include "stm32f4xx.h"
#include "sys.h"
#include <stdio.h>
#include "delay/delay.h"

// 引脚定义：SCL 是时钟线，SDA 是数据线
#define SCL_W PCout(9)  // SCL 时钟线，接在 GPIOB 的第 15 脚
#define SDA_W PCout(7)  // SDA 数据线输出，接在 GPIOD 的第 10 脚
#define SDA_R PCin(7)   // SDA 数据线输入，接在 GPIOD 的第 10 脚

// I2C 初始化函数
// 作用：配置 I2C 通信需要用到的 GPIO 引脚
void i2c_init(void)
{
    // 第一步：打开 GPIO 的时钟，否则引脚不工作
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);	// 打开 GPIOB 的时钟
	// 第二步：配置引脚参数
	GPIO_InitTypeDef GPIO_InitStructure;
	
	// 配置 SDA 数据线的输出功能（GPIOD.10）
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;            // 选择第 10 脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;         // 设置为输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;        // 推挽输出（可以输出高低电平）
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;      // 速度设为 2MHz，I2C 不需要太快
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;          // 上拉电阻，保证空闲时为高电平
	GPIO_Init(GPIOC, &GPIO_InitStructure);                // 应用配置到 GPIOD
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;            // 选择第 10 脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;         // 设置为输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;        // 推挽输出（可以输出高低电平）
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;      // 速度设为 2MHz，I2C 不需要太快
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;          // 上拉电阻，保证空闲时为高电平
	GPIO_Init(GPIOC, &GPIO_InitStructure);                // 应用配置到 GPIOD

	// 第三步：初始化引脚状态，I2C 空闲时两根线都应该是高电平
	SCL_W=1;  // SCL 设为高
	SDA_W=1;  // SDA 设为高	
}

// 配置 SDA 引脚的模式
// 注意：I2C 通信中，SDA 线既需要输出数据（主机发），也需要输入数据（主机收）
// 所以需要根据通信方向切换引脚模式
// 参数 pin_mode：GPIO_Mode_IN（输入）或 GPIO_Mode_OUT（输出）
void sda_pin_mode(GPIOMode_TypeDef pin_mode)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;            // 选择第 10 脚
	GPIO_InitStructure.GPIO_Mode = pin_mode;              // 设置为输入或输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;        // 推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;      // 速度 2MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;          // 上拉
	GPIO_Init(GPIOC, &GPIO_InitStructure);                // 应用配置
}

// 发送 I2C 起始信号
// I2C 协议规定：当 SCL 是高电平时，SDA 由高变低就是起始信号
// 起始信号表示要开始一次通信了
void i2c_start(void)
{
    sda_pin_mode(GPIO_Mode_OUT); // 确保 SDA 是输出模式，能控制电平
	
	// 先把两根线都拉高
	SDA_W=1;
	SCL_W=1;
	delay_us(5); // 等一下，让信号稳定
	
	// 关键：SCL 高的时候，SDA 从高变低
	SDA_W=0;     // 产生起始信号
	delay_us(5); // 等一下
	
	// 拉低 SCL，准备发送数据
	// 因为 I2C 协议中，数据只能在 SCL 低电平时变化
	SCL_W=0;
    delay_us(5); // 等一下
}

// 发送 I2C 停止信号
// I2C 协议规定：当 SCL 是高电平时，SDA 由低变高就是停止信号
// 停止信号表示这次通信结束了
void i2c_stop(void)
{
    sda_pin_mode(GPIO_Mode_OUT); // 确保 SDA 是输出模式
	
	SCL_W=1;     // 先把 SCL 拉高
    SDA_W=0;     // 把 SDA 拉低
	delay_us(5); // 等一下
	
	// 关键：SCL 高的时候，SDA 从低变高
	SDA_W=1;     // 产生停止信号
	delay_us(5); // 等一下
}

// 发送一个字节的数据
// I2C 协议规定，发送数据时要先发送高位（左边的 bit），再发送低位
// 参数 byte：要发送的一个字节数据
void i2c_send_byte(uint8_t byte)
{
    sda_pin_mode(GPIO_Mode_OUT); // 确保 SDA 是输出模式
    SCL_W=0;     // 拉低 SCL，准备发送数据
	SDA_W=0;     // 先初始化为低电平
	delay_us(5); // 等一下

    // 从高位到低位发送，i=7 是最高位（第8位），i=0 是最低位（第1位）
    for(int i=7;i>=0;i--)
    {
        // 判断当前位是否为 1
        // 1<<i 就是把 1 左移 i 位，比如 i=7 就是 10000000
        if(byte & (1<<i))
	        SDA_W=1;  // 如果是 1，就把 SDA 拉高
	    else
	        SDA_W=0;  // 如果是 0，就把 SDA 拉低
        
        delay_us(5); // 等一下，让数据稳定
        SCL_W=1;     // 把 SCL 拉高，这时数据就被对方读取了
        delay_us(5); // 等一下，让对方读完
        SCL_W=0;     // 把 SCL 拉低，准备发送下一位
        delay_us(5); // 等一下
    }
}

// 接收一个字节的数据
// I2C 协议规定，接收数据时也是高位在前
// 返回值：接收到的字节数据
uint8_t i2c_recv_byte()
{
    uint8_t d=0; // 接收缓冲区，初始化为 0
    
    // 从低位到高位接收，i=0 是最低位，i=7 是最高位
    // 因为最后要把数据组合成一个字节
    for(int i=0;i<8;i++)
    {
        SCL_W=1;     // 把 SCL 拉高，这时对方会把数据放到 SDA 线上
        delay_us(5); // 等一下，让数据稳定
        
        if(SDA_R)    // 读取 SDA 的状态
            d|=1<<i; // 如果是 1，就把对应的位设置为 1
        // 如果是 0，就不管，因为初始值已经是 0 了

        SCL_W=0;     // 把 SCL 拉低，准备接收下一位
        delay_us(5); // 等一下
    }
    return d; // 返回接收到的字节
}

// 发送应答信号
// 在 I2C 通信中，接收数据的一方要告诉发送方：我收到了
// 参数 ack：0 表示应答（ACK，收到了），1 表示非应答（NACK，没收到或不要了）
void i2c_ack(uint8_t ack)// 告诉对方是否接收成功，是否继续发送下一个字节
{
    sda_pin_mode(GPIO_Mode_OUT); // 确保 SDA 是输出模式
    
    SCL_W=0;     // 拉低 SCL，准备发送应答
	SDA_W=0;     // 初始化 SDA
	delay_us(5); // 等一下

    if(ack)      // 根据参数设置应答信号
        SDA_W=1; // 1 表示非应答（NACK），告诉对方我不要数据了
    else
        SDA_W=0; // 0 表示应答（ACK），告诉对方我收到了，继续发
    delay_us(5); // 等一下

    SCL_W=1;     // 把 SCL 拉高，应答信号生效
    delay_us(5); // 等一下
    SCL_W=0;     // 把 SCL 拉低，结束应答
    delay_us(5); // 等一下
}

// 等待应答信号
// 发送数据后，要等对方回应，确认是否收到
// 返回值：0 表示应答（ACK，对方收到了），1 表示非应答（NACK，对方没收到）
uint8_t i2c_wait_ack(void)
{
    uint8_t ack=0;
    
    sda_pin_mode(GPIO_Mode_IN); // 把 SDA 设置为输入模式，才能读取对方的应答
    SCL_W=1;     // 把 SCL 拉高，应答信号有效
    delay_us(5); // 等一下，让信号稳定
    
    if(SDA_R)    // 读取 SDA 的状态
        ack=1;   // 1 表示非应答（NACK），对方没收到
    else
        ack=0;   // 0 表示应答（ACK），对方收到了
    
    SCL_W=0;     // 把 SCL 拉低，结束应答检测
    delay_us(5); // 等一下
    
    return ack; // 返回应答状态
}

// 向 I2C 设备发送数据
// 比如要往设备的某个寄存器里写数据，就用这个函数
// 参数 addr：要写入的寄存器地址
// 参数 buf：要发送的数据的缓冲区指针
// 参数 len：要发送的数据长度
// 返回值：0 表示成功，-1 表示失败
int i2c_send_data(uint8_t addr, uint8_t *buf,uint8_t len )
{
    uint8_t ack;      // 用于保存应答状态
	uint8_t *p=buf;   // 指针，用来遍历缓冲区

    // 第一步：发送起始信号，表示要开始通信了
	i2c_start();
	
	// 第二步：告诉设备我要和你通信（设备寻址）
	// 0xA0 是设备地址，后面的 0 表示是写操作
	i2c_send_byte(0xA0);
	
	// 第三步：等设备回应，确认它在听
	ack=i2c_wait_ack();
	
	if(ack) // 如果没回应
	{
		printf("device address fail\r\n");// 设备没反应，可能不在总线上
		return -1;
	}

    // 第四步：告诉设备我要写哪个寄存器
    i2c_send_byte(addr);
    ack=i2c_wait_ack();
    if(ack)
    {
        printf("register address fail\r\n");// 寄存器地址不对
        return -1;
    }

    // 第五步：发送数据
    while(len--)
    {
        i2c_send_byte(*p);  // 发送一个字节
        ack=i2c_wait_ack(); // 等设备确认收到
        if(ack)
        {
            printf("data send fail\r\n");// 数据发送失败
            return -1;
        }
        p++; // 指针指向下一个字节
    }

    // 第六步：发送停止信号，表示通信结束
	i2c_stop();

    return 0; // 成功
}

// 从 I2C 设备读取数据
// 比如要读设备的某个寄存器的值，就用这个函数
// 参数 addr：要读取的寄存器地址
// 参数 buf：用来存放读取数据的缓冲区指针
// 参数 len：要读取的数据长度
// 返回值：0 表示成功，-1 表示失败
uint32_t i2c_read_data(uint8_t addr, uint8_t *buf,uint8_t len )
{
     uint8_t ack;      // 用于保存应答状态
	uint8_t *p=buf;    // 指针，用来遍历缓冲区
	
	// 第一步：发送起始信号
	i2c_start();
	
	// 第二步：先告诉设备我要和你通信（写操作，因为要先告诉它读哪个寄存器）
	i2c_send_byte(0xA0); // 0xA0 是设备地址，最后一位 0 表示写
	
	// 第三步：等设备回应
	ack=i2c_wait_ack();
	
	if(ack)
	{
		printf("device address fail with write\r\n");
		return -1;
	}

    // 第四步：告诉设备我要读哪个寄存器
    i2c_send_byte(addr);
    ack=i2c_wait_ack();
    if(ack)
    {
        printf("register address fail with read\r\n");
        return -1;
    }

    // 第五步：发送重复起始信号，因为接下来要改成读操作
    // 重复起始信号就是再发一次起始信号，不发停止信号
    i2c_start();

    // 第六步：告诉设备现在我要读数据了
    i2c_send_byte(0xA1); // 0xA1 是设备地址，最后一位 1 表示读
    ack=i2c_wait_ack();
    if(ack)
    {
        printf("device address fail with read\r\n");
        return -1;
    }

    // 第七步：读取数据
    len-=1; // 最后一个字节需要发送非应答信号
    while(len--)
    {
        *p=i2c_recv_byte();  // 读取一个字节
        // 注意：这里没有调用 i2c_wait_ack()，因为是我们接收数据
        // 我们要告诉设备是否继续发送
        i2c_ack(0); // 发送应答信号，告诉设备继续发
        p++;        // 指针指向下一个位置
    }
    // 读取最后一个字节
    *p=i2c_recv_byte();
    i2c_ack(1); // 发送非应答信号，告诉设备数据读完了
    i2c_stop(); // 发送停止信号

    return 0; // 成功
}
