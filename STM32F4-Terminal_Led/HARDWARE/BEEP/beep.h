#ifndef __BEEP_H
#define __BEEP_H	 
#include "sys.h" 

//LED�˿ڶ���
#define BEEP PFout(8)	// ����������IO 

void BEEP_Init(void);//��ʼ��		
void BEEP_Test(u16 xms);
#endif

















