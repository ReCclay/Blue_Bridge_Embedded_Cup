/*******************************************************************************
* 文件名：main.c
* 描  述：
* 作  者：CLAY
* 版本号：v1.0.0
* 日  期: 2019年2月17日
* 备  注：修改后的LCD例程
*         E2PR0M在0x01这个地址写入数据0xAA，而后再读出至LCD
*******************************************************************************
*/

#include "stm32f10x.h"
#include "lcd.h"
#include "e2prom.h"
#include "stdio.h"
#include "i2c.h"

u32 TimingDelay = 0;

void Delay_Ms(u32 nTime);

//Main Body
int main(void)
{
	u8 str[20];
	u8 dat;

	STM3210B_LCD_Init();
	LCD_Clear(Blue);
	LCD_SetBackColor(Blue);
	LCD_SetTextColor(White);
	
	SysTick_Config(SystemCoreClock/1000);
	
	i2c_init();//FUCK,别忘了这个初始化
	E2Write(0x01, 0xAA);//在0x01这个地址写下,0xAA这个数据
	Delay_Ms(5);
	dat = E2Read(0x01);
	
	LCD_ClearLine(Line5);
	sprintf((char *)str, "data = %d", dat);
	LCD_DisplayStringLine(Line5, str);
	
	while(1);
}

//
void Delay_Ms(u32 nTime)
{
	TimingDelay = nTime;
	while(TimingDelay != 0);	
}
