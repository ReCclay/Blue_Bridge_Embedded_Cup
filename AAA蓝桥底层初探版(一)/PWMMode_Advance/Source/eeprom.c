#include "i2c.h"
#include "eeprom.h"

/* 读取EEPROM中的一个字节，addr-字节地址 */
u8 E2ReadByte(u8 addr)
{
    u8 dat;
    
    do { //用寻址操作查询当前是否可进行读写
        I2CStart();
        if (I2CWrite(0x50<<1)) //寻址器件，应答则跳出循环，否则继续查询
        {
            break;
        }
        I2CStop();
    } while(1);
    I2CWrite(addr);           //写入存储地址
    I2CStart();               //发送重复启动信号
    I2CWrite((0x50<<1)|0x01); //寻址器件，后续为读操作
    dat = I2CReadNAK();       //读取一个字节数据
    I2CStop();
    
    return dat;
}

/* 向EEPROM中写入一个字节，addr-字节地址 */
void E2WriteByte(u8 addr, u8 dat)
{
    do { //用寻址操作查询当前是否可进行读写
        I2CStart();
        if (I2CWrite(0x50<<1)) //寻址器件，应答则跳出循环，否则继续查询
        {
            break;
        }
        I2CStop();
    } while(1);
    I2CWrite(addr);  //写入存储地址
    I2CWrite(dat);   //写入一个字节数据
    I2CStop();
}
