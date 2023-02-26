#ifndef _TIMER_H
#define _TIMER_H
#include "sys.h"
#include "led.h"
#include "usart.h"
#include "myList.h"
#include "24Cxx.h"


void TIM2_Int_Init(u16 arr,u16 psc);
void TIM3_Int_Init(u16 arr,u16 psc);
#endif
