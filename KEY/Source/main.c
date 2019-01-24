/*******************************************************************************
* 文件名：main.c
* 描  述：
* 作  者：CLAY
* 版本号：v1.0.0
* 日  期: 2019年1月24日
* 备  注：按键B1控制LED1的亮灭
*         
*******************************************************************************
*/

#include "stm32f10x.h"
#include "led.h"
#include "key.h"
#include "timer.h"


int main(void)
{
	LEDInit();
	KeyInit();
	TIM2Init(2000, 72);//定时2ms
	
	while(1)
	{	
		KeyDriver();
	}
}

void KeyAction(int code)
{
	if(code == 1)
	{
		GPIOC->ODR ^= (1<<8);//PC8不断取反
		GPIOD->ODR |= (1<<2);//PD2置1，使能573锁存器
		GPIOD->ODR &= ~(1<<2);//PD2清0，关闭573锁存器
	}
}


