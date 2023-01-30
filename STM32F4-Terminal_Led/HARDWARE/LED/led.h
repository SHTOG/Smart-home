#ifndef __LED_H 
#define __LED_H 
#include "sys.h" 
#include "delay.h"
#include "timer.h"
//LED 端口定义 这段代码里面最关键就是 2 个宏定义：
#define LED0 PFout(9)  // DS0 
#define LED1 PFout(10)// DS1    
 

extern u8 LEDmode;//LED灯模式，0为普通模式，1为呼吸灯模式
extern u8 direction;//呼吸灯亮度变化方向，1为增亮，0为减灭

void LED_Init(void);//初始化
void LED_Test(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin,u16 xms);//流水灯
void LED_Close(void);
void LED_Open(void);
void LED_Light_Plus(void);
void LED_Light_Minus(void);
void LED_Light_Set(u8 value);
void LED_Mode(u8 mode);
#endif 
