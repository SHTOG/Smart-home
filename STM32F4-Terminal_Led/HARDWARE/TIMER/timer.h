#ifndef _TIMER_H
#define _TIMER_H
#include "sys.h"
#include "led.h"
extern u16 PWMval;

void TIM2_Int_Init(u16 arr,u16 psc);
void TIM3_PWM_Init(u32 arr,u32 psc);
#endif
