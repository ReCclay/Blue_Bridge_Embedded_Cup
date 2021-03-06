/*******************************************************************************
* 文件名：main.c
* 描  述：
* 作  者：CLAY
* 版本号：v1.0.0
* 日  期: 2019年1月23日20:01:26
* 备  注：延时实现PC8对应的LED1闪烁
*         
*******************************************************************************
*/

#include "stm32f10x.h"
#include "led.h"


int main(void)
{
	int i;
	
	LEDInit();
	
	while(1)
	{
		GPIOD->ODR |= (1<<2);//PD2置1，使能573锁存器
		GPIOC->ODR ^= (1<<8);//PC8不断取反
		GPIOD->ODR &= ~(1<<2);//PD2清0，关闭573锁存器
		for(i=0; i<500000; i++);
	}
}


