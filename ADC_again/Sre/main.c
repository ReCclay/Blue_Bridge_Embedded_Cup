/*******************************************************************************
* 文件名：main.c
* 描  述：
* 作  者：CLAY
* 版本号：v1.0.0
* 日  期: 2019年2月17日
* 备  注：修改后的LCD例程
*         滑变对应的PB0(ADC1_IN8)AD值，量化为0~3.3V显示在LCD上
*******************************************************************************
*/

#include "stm32f10x.h"
#include "lcd.h"
#include "e2prom.h"
#include "stdio.h"
#include "i2c.h"
#include "adc.h"

u32 TimingDelay = 0;

void Delay_Ms(u32 nTime);

//Main Body
int main(void)
{
	u8 str[20];
	u8 dat;
	float dat_ADC;

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
	
	ADC1_Init();
	
	
	while(1)
	{
		dat_ADC = Get_ADC();
		sprintf((char *)str, "ADC = %.2f V", dat_ADC * 3.3 / 4096);
		LCD_DisplayStringLine(Line2, str);
		Delay_Ms(500);
	}
}

//
void Delay_Ms(u32 nTime)
{
	TimingDelay = nTime;
	while(TimingDelay != 0);	
}
