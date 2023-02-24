#ifndef __LED_H 
#define __LED_H 
#include "sys.h" 
#include "delay.h"
#include "timer.h"
//LED 端口定义 这段代码里面最关键就是 2 个宏定义：
#define LED0 PBout(4)
#define LED1 PCout(13)


extern u8 LEDmode[8];//LED灯模式，0为普通模式，1为呼吸灯模式
extern u8 direction[8];//呼吸灯亮度变化方向，1为增亮，0为减灭

void LED_Init(void);//初始化
void LED_Test(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin,u16 xms);//流水灯
void LED_Close(u8 number);
void LED_Open(u8 number);
void LED_Light_Plus(u8 number);
void LED_Light_Minus(u8 number);
void LED_Light_Set(u8 number, u8 value);
void LED_Mode(u8 number, u8 mode);
#endif 
