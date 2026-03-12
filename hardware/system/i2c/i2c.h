#ifndef __I2C_H__
#define __I2C_H__

void i2c_init(void);
void i2c_start(void);
void i2c_stop(void);
void i2c_send_byte(unsigned char byte);
unsigned char i2c_recv_byte(void);
void i2c_ack(unsigned char ack);
unsigned char i2c_wait_ack(void);

#endif
