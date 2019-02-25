#include "config.h"

/* 1/4微秒延时函数(含函数调用及返回时间共计耗时约1/4微妙@72MHz主频) */
void delay_qus(void)
{
	__ASM ("nop");
	__ASM ("nop");
	__ASM ("nop");
	__ASM ("nop");
	__ASM ("nop");
	__ASM ("nop");
	__ASM ("nop");
	__ASM ("nop");
}

/* 微秒延时函数，us-延时时间 */
void delay_us(u16 us)
{
	while (us--)
	{
		delay_qus();
		delay_qus();
		delay_qus();
		delay_qus();
	}
}

/* 毫秒延时函数，ms-延时时间 */
void delay_ms(u16 ms)
{
    while (ms--)
    {
        delay_us(1000);
    }
}
