#ifndef _TIMER_H
#define _TIMER_H
#include "sys.h"
extern u16 MilliSecond;//���뼶������
extern u8 WaitTime;//�ȴ�ʱ����ÿ������1

void TIM2_Int_Init(u16 arr,u16 psc);
#endif
