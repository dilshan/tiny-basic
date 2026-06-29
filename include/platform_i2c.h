#ifndef __PLATFORM_I2C_HEADER__
#define __PLATFORM_I2C_HEADER__

void i2cInit();

void i2cStart();
void i2cRestart();

unsigned char i2cWrite(unsigned char data);
unsigned char i2cRead(unsigned char send_ack);

void i2cStop();

#endif /* __PLATFORM_I2C_HEADER__ */