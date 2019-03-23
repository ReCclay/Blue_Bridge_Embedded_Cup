#ifndef KEY_H
#define KEY_H

#define KEY1 GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)
#define KEY2 GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8)
#define KEY3 GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1)
#define KEY4 GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_2)


void KEY_Init(void);
void KeyDriver(void);
void KeyScan(void);


#endif
