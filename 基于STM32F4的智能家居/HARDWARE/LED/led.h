#ifndef __LED_H 
#define __LED_H 
#include "sys.h" 
#include "delay.h"
#define LED0 PFout(9)  // DS0 
#define LED1 PFout(10)// DS1 
#define LED2 PAout(6)// 测试用外接led
 
void LED_Init(void);//初始化
void LED_Test(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin,u16 xms);//流水灯
#endif 
