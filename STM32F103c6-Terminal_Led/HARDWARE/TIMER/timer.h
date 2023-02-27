#ifndef _TIMER_H
#define _TIMER_H
#include "sys.h"
#include "iwdg.h"
#include "usart.h"

extern u16 PWMval[8];
extern void (*TIM_SetCompare[8])(TIM_TypeDef* TIMx, uint16_t Compare1);
extern uint64_t MilliSecond;//秒级计数器
extern uint64_t TI0CD;//外部按键中断0消抖
extern uint64_t TI1CD;//外部按键中断1消抖
extern uint64_t TI2CD;//外部按键中断2消抖
extern uint64_t TI3CD;//外部按键中断3消抖

void TIM2_Int_Init(u16 arr,u16 psc);
void TIM3_PWM_Init(u16 arr,u16 psc);
#endif
