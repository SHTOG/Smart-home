#ifndef __LED_H 
#define __LED_H 
#include "sys.h" 
#include "delay.h"
#include "timer.h"
//LED �˿ڶ��� ��δ���������ؼ����� 2 ���궨�壺
#define LED0 PFout(9)  // DS0 
#define LED1 PFout(10)// DS1    
 

extern u8 LEDmode;//LED��ģʽ��0Ϊ��ͨģʽ��1Ϊ������ģʽ
extern u8 direction;//���������ȱ仯����1Ϊ������0Ϊ����

void LED_Init(void);//��ʼ��
void LED_Test(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin,u16 xms);//��ˮ��
void LED_Close(void);
void LED_Open(void);
void LED_Light_Plus(void);
void LED_Light_Minus(void);
void LED_Light_Set(u8 value);
void LED_Mode(u8 mode);
#endif 
