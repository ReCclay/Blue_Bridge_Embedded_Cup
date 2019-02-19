#include "stm32f10x.h"
#include "i2c.h"

void E2Write(u8 addr, u8 dat)
{
	I2CStart();
	I2CSendByte(0xA0);
	I2CWaitAck();
	I2CSendByte(addr);
	I2CWaitAck();
	I2CSendByte(dat);
	I2CWaitAck();
	I2CStop();
}

u8 E2Read(u8 addr)
{
	u8 tmp;
	
	I2CStart();
	I2CSendByte(0xA0);
	I2CWaitAck();
	I2CSendByte(addr);
	I2CWaitAck();
	
	I2CStart();
	I2CSendByte(0xA1);
	I2CWaitAck();
	tmp = I2CReceiveByte();
	I2CWaitAck();
	I2CStop();
	
	return tmp;
}

