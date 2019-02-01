/*******************************************************************************
* 文件名：main.c
* 描  述：
* 作  者：CLAY
* 版本号：v1.0.0
* 日  期: 2019年2月1日
* 备  注：该例程用于测量外部输入信号的频率
*         
*******************************************************************************
*/

#include "config.h"

u8 str[20];
extern __IO uint32_t TIM3Freq;

int main(void)
{
	u32 i = 0;
	
	STM3210B_LCD_Init();
	LCD_Clear(Blue);//别忘了这一句总体清除
	LCD_SetBackColor(Blue);
	LCD_SetTextColor(White);
	
	TIM3_InputCaptureInit();
	
	while(1)
	{
		sprintf((char *)str, "TIM3Freq = %d           ", TIM3Freq);
		LCD_DisplayStringLine(Line1, str);
		for(i=0; i<500000; i++);
	}
}

