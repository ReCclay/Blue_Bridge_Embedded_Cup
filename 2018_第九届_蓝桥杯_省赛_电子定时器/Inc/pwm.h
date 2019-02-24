#ifndef _PWM_H
#define _PWM_H

/*最后一个参数表示的是是否使能输出PWM*/
void PWM_Init(u32 freq, u8 duty, u8 en);

#endif
