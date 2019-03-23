#include "stm32f10x.h"
#include "lcd.h"

u32 TimingDelay = 0;

void Delay_Ms(u32 nTime);

//Main Body
int main(void)
{

	STM3210B_LCD_Init();
	LCD_Clear(Blue);
	LCD_SetBackColor(Blue);
	LCD_SetTextColor(White);
	
	SysTick_Config(SystemCoreClock/1000);

	LCD_DrawLine(120,0,320,Horizontal);
	LCD_DrawLine(0,160,240,Vertical);
	Delay_Ms(1000);
	LCD_Clear(Blue);

	LCD_DrawRect(70,210,100,100);
	Delay_Ms(1000);
	LCD_Clear(Blue);

	LCD_DrawCircle(120,160,50);
	Delay_Ms(1000);

	LCD_Clear(Blue);
	LCD_DisplayStringLine(Line4 ,"    Hello,world.   ");
	Delay_Ms(1000);

	LCD_SetBackColor(White);
	LCD_DisplayStringLine(Line0,"                    ");	
	LCD_SetBackColor(Black);
	LCD_DisplayStringLine(Line1,"                    ");	
	LCD_SetBackColor(Grey);
	LCD_DisplayStringLine(Line2,"                    ");
	LCD_SetBackColor(Blue);
	LCD_DisplayStringLine(Line3,"                    ");
	LCD_SetBackColor(Blue2);
	LCD_DisplayStringLine(Line4,"                    ");
	LCD_SetBackColor(Red);						
	LCD_DisplayStringLine(Line5,"                    ");
	LCD_SetBackColor(Magenta);	
	LCD_DisplayStringLine(Line6,"                    ");
	LCD_SetBackColor(Green);	
	LCD_DisplayStringLine(Line7,"                    ");	
	LCD_SetBackColor(Cyan);	
	LCD_DisplayStringLine(Line8,"                    ");
	LCD_SetBackColor(Yellow);		
	LCD_DisplayStringLine(Line9,"                    ");	
	
	while(1);
}

//
void Delay_Ms(u32 nTime)
{
	TimingDelay = nTime;
	while(TimingDelay != 0);	
}
