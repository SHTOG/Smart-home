#ifndef __LED_H 
#define __LED_H 
#include "sys.h" 
//LED �˿ڶ��� ��δ���������ؼ����� 2 ���궨�壺
#define LED0 PFout(9)  // DS0 
#define LED1 PFout(10)// DS1    
 
void LED_Init(void);//��ʼ��
void LED_Test(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin,u16 xms);//��ˮ��
#endif 
