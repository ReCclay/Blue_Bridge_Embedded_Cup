#ifndef _EEPROM_H
#define _EEPROM_H

#include "config.h"

u8 E2ReadByte(u8 addr);
void E2WriteByte(u8 addr, u8 dat);

#endif
