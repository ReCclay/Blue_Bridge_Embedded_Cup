/*******************************************************************************
* 文件名：main.c
* 描  述：
* 作  者：CLAY
* 版本号：v1.0.0
* 日  期: 2019年1月25日
* 备  注：串口发送20字符以内的信息显示到LCD，并回显到串口助手
*         
*******************************************************************************
*/

#include "stm32f10x.h"
#include "led.h"
#include "key.h"
#include "timer.h"
#include "beep.h"
#include "lcd.h"
#include "stdio.h"
#include "usart.h"


int main(void)
{
	u8 i;
	u8 str[25];
	u8 temp = 30;
	float AO = 3.845;
	
	STM3210B_LCD_Init();
	LCD_Clear(Blue);
	LEDInit();
	KeyInit();
	BeepInit();
	TIM2Init(2000, 72);//定时2ms
	USART2Init(9600);
	
	LCD_DisplayStringLine(Line1,(u8*) "qwertyuioplkjhgfdsazxcvb");
	sprintf((char*)str,"temp=%d   A0=%.1f  ",temp, AO);
	LCD_DisplayStringLine(Line2,str);
	
	while(1)
	{	
		KeyDriver();
		if(RxdOverFlag)
		{
			RxdOverFlag = 0;
			LCD_ClearLine(Line5);
			LCD_DisplayStringLine(Line5, RxdBuf);
			USART2_SendByte(RxdBuf);
			for(i=0; i<50; i++) RxdBuf[i] = 0;//清空串口接收缓冲区
			USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启串口接收中断，处理下一帧数据
		} 
	}
}

void KeyAction(int code)
{
	if(code == 1)//按下B1，切换灯状态，蜂鸣器鸣叫0.1s
	{
		GPIOC->ODR ^= (1<<8);//PC8不断取反
		GPIOD->ODR |= (1<<2);//PD2置1，使能573锁存器
		GPIOD->ODR &= ~(1<<2);//PD2清0，关闭573锁存器
		Beep(100);
	}
	else if(code == 2)
	{
		Beep(-1);
	}
	else if(code == 3)
	{
		Beep(0);
	}
}


