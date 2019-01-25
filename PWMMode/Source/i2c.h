#ifndef _I2C_H
#define _I2C_H

#include "config.h"

#define I2C_SCL_OUT   PB_OUT(6)
#define I2C_SDA_OUT   PB_OUT(7)
#define I2C_SDA_IN    PB_IN(7)

void I2CInit(void);
void I2CStart(void);
void I2CStop(void);
u8 I2CReadNAK(void);
u8 I2CReadACK(void);
u8 I2CWrite(u8 dat);


#endif
