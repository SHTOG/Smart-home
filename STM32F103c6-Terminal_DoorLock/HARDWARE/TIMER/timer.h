#ifndef _TIMER_H
#define _TIMER_H
#include "sys.h"
extern u16 MilliSecond;//毫秒级计数器
extern u8 WaitTime;//等待时长，每秒增加1

void TIM2_Int_Init(u16 arr,u16 psc);
#endif
