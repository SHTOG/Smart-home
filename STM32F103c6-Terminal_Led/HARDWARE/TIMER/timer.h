#ifndef _TIMER_H
#define _TIMER_H
#include "sys.h"
#include "iwdg.h"
#include "usart.h"

extern u16 PWMval[8];
extern void (*TIM_SetCompare[8])(TIM_TypeDef* TIMx, uint16_t Compare1);
extern uint64_t MilliSecond;//�뼶������
extern uint64_t TI0CD;//�ⲿ�����ж�0����
extern uint64_t TI1CD;//�ⲿ�����ж�1����
extern uint64_t TI2CD;//�ⲿ�����ж�2����
extern uint64_t TI3CD;//�ⲿ�����ж�3����

void TIM2_Int_Init(u16 arr,u16 psc);
void TIM3_PWM_Init(u16 arr,u16 psc);
#endif
