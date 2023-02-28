#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"
#include "led.h"
#include "mtxk.h"
#include "Zigbee.h"
#include "iwdg.h"

void TIM2_Int_Init(u16 arr,u16 psc);
void TIM3_Int_Init(u16 arr,u16 psc);

#endif
