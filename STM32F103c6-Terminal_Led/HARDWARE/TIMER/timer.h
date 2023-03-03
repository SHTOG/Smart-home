#ifndef _TIMER_H
#define _TIMER_H
#include "sys.h"
#include "iwdg.h"
#include "usart.h"

void TIM2_Int_Init(u16 arr,u16 psc);
void TIM3_PWM_Init(u16 arr,u16 psc);
#endif
