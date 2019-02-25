/*******************************************************************************
* 文件名：main.c
* 描  述：
* 作  者：CLAY
* 版本号：v1.0.0
* 日  期: 2019年2月16日
* 备  注：该例程用于测量输入信号的频率 
*         输入管脚PA7(TIM3_CH2) 大于1K，会有些许的抖动误差
*******************************************************************************
*/

#include "config.h"

u8 str[20];
extern __IO uint32_t TIM3Freq;
extern u8 flagFreq;

int main(void)
{
	u32 i = 0;
	
	STM3210B_LCD_Init();
	LCD_Clear(Blue);//别忘了这一句总体清除
	LCD_SetBackColor(Blue);
	LCD_SetTextColor(White);
	
	TIM3_ICInit();
	
	while(1)
	{
		if(flagFreq)
		{
			flagFreq = 0;
			
			sprintf((char *)str, "TIM3Freq = %d           ", TIM3Freq);
			LCD_DisplayStringLine(Line1, str);
			for(i=0; i<500000; i++);//刷新延迟
		}	
	}
}

