#ifndef __STMT_H
#define __STMT_H			  	 
#include "sys.h"
#include "delay.h"
#define STMTAP	PAout(0)
#define STMTAM	PAout(1)
#define STMTBP	PAout(2)
#define STMTBM	PAout(3)

#define MAXDEEP  128//��ȫ�򿪴�������Deep
#define MAXDEEPAROUND   5//�����ȼ�����ת5Ȧ��ȫ�򿪴���

extern u8 CurtainDeep;
extern u8 AllowChangeDeepFlag;
extern u8 NewCurtainDeep;

void STMT_Init(void);
void STMT_Turn_X_Deep(u8 dir, u8 x);
void STMT_Turn_X_Around(u8 dir,u8 x);
void STMT_X_Pulse(u8 dir,u16 x);

#endif
