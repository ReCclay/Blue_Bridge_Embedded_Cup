/*******************************************************************************
* 文件名：main.c
* 描  述：
* 作  者：CLAY
* 版本号：v1.0.0
* 日  期: 2019年2月18日
* 备  注：修改后的LCD例程
*         PC通过USART2发送数据到单片机，并显示到LCD上(PA2-TXD PA3-RXD) 
*		  注意发送新行是\r\n，剔除\r显示到屏幕上的乱码影响
*******************************************************************************
*/

#include "stm32f10x.h"
#include "lcd.h"
#include "e2prom.h"
#include "stdio.h"
#include "i2c.h"
#include "adc.h"
#include "rtc.h"
#include "usart2.h"

u32 TimingDelay = 0;
u8 RxdCnt = 0;
u8 RxdOver = 0;
u8 RxdBuf[20];
void Delay_Ms(u32 nTime);
u8 RTC_Flag = 0;

//Main Body
int main(void)
{
	u8 i;
	
	STM3210B_LCD_Init();
	LCD_Clear(Blue);
	LCD_SetBackColor(Blue);
	LCD_SetTextColor(White);
	
	SysTick_Config(SystemCoreClock/1000);

	USART2_Init();
	
	while(1)
	{
		if(RxdOver)
		{
			RxdOver = 0;
			LCD_ClearLine(Line5);//清除LCD的对应行
			LCD_DisplayStringLine(Line5, RxdBuf);
			USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//接收的数据处理完毕后打开接收中断
			for(i=0; i<20; i++)//清空缓冲区
				RxdBuf[i] = 0;
		}
	}
}

//
void Delay_Ms(u32 nTime)
{
	TimingDelay = nTime;
	while(TimingDelay != 0);	
}
