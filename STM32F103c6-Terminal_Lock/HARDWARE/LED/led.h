#ifndef __LED_H
#define __LED_H	 
#include "sys.h"
#include "delay.h"

#define LED0 PBout(5)// PB5
#define LED1 PEout(5)// PE5

void LED_Init(void);//初始化
void LED_Test(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin,u16 xms);
#endif
