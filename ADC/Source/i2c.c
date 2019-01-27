#include "i2c.h"

void I2CInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //使能GPIOB时钟       
    GPIO_SetBits(GPIOB, GPIO_Pin_6|GPIO_Pin_7);           //SCL和SDA初始输出高电平(先设置引脚电平可以避免IO初始化过程中可能产生的毛刺)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;  //选择SCL和SDA引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;      //选择开漏输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;     //输出速率10MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/* 产生总线起始信号 */
void I2CStart(void)
{
    I2C_SDA_OUT = 1; //首先确保SDA、SCL都是高电平
    I2C_SCL_OUT = 1;
    delay_us(5);
    I2C_SDA_OUT = 0; //先拉低SDA
    delay_us(5);
    I2C_SCL_OUT = 0; //再拉低SCL
}

/* 产生总线停止信号 */
void I2CStop(void)
{
    I2C_SCL_OUT = 0; //首先确保SDA、SCL都是低电平
    I2C_SDA_OUT = 0;
    delay_us(5);
    I2C_SCL_OUT = 1; //先拉高SCL
    delay_us(5);
    I2C_SDA_OUT = 1; //再拉高SDA
    delay_us(5);
}

/* I2C总线写操作，dat-待写入字节，返回值-从机应答位的值 */
u8 I2CWrite(u8 dat)
{
    int i;
    u8 ack;   //用于暂存应答位的值
    
    for (i=0; i<8; i++)  //循环将8bit数据输出到总线上
    {
        I2C_SDA_OUT = (dat&0x80) ? 1 : 0; //将最高位的值输出到SDA上
        delay_us(5);
        I2C_SCL_OUT = 1; //拉高SCL
        delay_us(5);
        I2C_SCL_OUT = 0; //再拉低SCL，完成一个位周期
        dat <<= 1;       //左移将次高位变为最高位，实现高位在先低位在后的发送顺序
    }
    I2C_SDA_OUT = 1;  //8位数据发送完后，主机释放SDA，以检测从机应答
    delay_us(5);
    I2C_SCL_OUT = 1;  //拉高SCL
    ack = I2C_SDA_IN; //读取此时的SDA值，即为从机的应答值
    delay_us(5);
    I2C_SCL_OUT = 0;  //再拉低SCL完成应答位，并保持住总线        
    delay_us(5);
    
    return (!ack); //应答值取反以符合通常的逻辑：
                   //0=不存在或忙或写入失败，1=存在且空闲或写入成功
}

/* I2C总线读取8位数据，返回值-读到的字节 */
u8 I2CRead(void)
{
    int i;
    u8 dat = 0; //数据接收变量赋初值0
    
    I2C_SDA_OUT = 1;    //首先确保主机释放SDA
    for (i=0; i<8; i++) //循环将总线上的8bit数据读入dat中
    {
        delay_us(5);
        I2C_SCL_OUT = 1;    //拉高SCL
        dat <<= 1;          //左移将己读到的位向高位移动，实现高位在先低位在后的接收顺序
        if(I2C_SDA_IN != 0) //读取SDA的值到dat最低位上
        {
            dat |= 0x01;    //SDA为1时设置dat最低位为1，SDA为0时无操作，即仍为初始值的0
        }
        delay_us(5);
        I2C_SCL_OUT = 0;    //再拉低SCL，以使从机发送出下一位
    }
    return dat;
}

/* I2C总线读操作，并发送非应答信号，返回值-读到的字节 */
u8 I2CReadNAK(void)
{
    u8 dat;
    
    dat = I2CRead();  //读取8位数据
    I2C_SDA_OUT = 1;  //8位数据读取完后，拉高SDA，发送非应答信号
    delay_us(5);
    I2C_SCL_OUT = 1;  //拉高SCL
    delay_us(5);
    I2C_SCL_OUT = 0;  //再拉低SCL完成非应答位，并保持住总线
    delay_us(5);
    
    return dat;
}

/* I2C总线读操作，并发送应答信号，返回值-读到的字节 */
u8 I2CReadACK(void)
{
    u8 dat;
    
    dat = I2CRead();  //读取8位数据
    I2C_SDA_OUT = 0;  //8位数据读取完后，拉低SDA，发送应答信号
    delay_us(5);
    I2C_SCL_OUT = 1;  //拉高SCL
    delay_us(5);
    I2C_SCL_OUT = 0;  //再拉低SCL完成应答位，并保持住总线
    delay_us(5);
    
    return dat;
}
