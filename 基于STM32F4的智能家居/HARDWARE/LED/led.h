#ifndef __LED_H 
#define __LED_H 
#include "sys.h" 
//LED 端口定义 这段代码里面最关键就是 2 个宏定义：
#define LED0 PFout(9)  // DS0 
#define LED1 PFout(10)// DS1    
 
void LED_Init(void);//初始化
void LED_Test(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin,u16 xms);//流水灯
#endif 
