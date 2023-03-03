#ifndef __LED_H
#define __LED_H	 
#include "sys.h"
#include "delay.h"

#define LED1_GPIO GPIOB
#define LED1_Pin GPIO_Pin_9
#define LED1_Clock RCC_APB2Periph_GPIOB

#define LED2_GPIO GPIOC
#define LED2_Pin GPIO_Pin_13
#define LED2_Clock RCC_APB2Periph_GPIOC

#define LED1 PBout(9)// 用于提示终端正在寻找中控
#define LED2 PCout(13)// 调试直观观察程序进入主while循环



void LED_Init(void);//LED GPIO初始化
void LED1_Init(void);
void LED2_Init(void);
void LED_Test(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin,u16 xms);
#endif
