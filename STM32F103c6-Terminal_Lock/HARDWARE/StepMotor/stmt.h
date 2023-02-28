#ifndef __STMT_H
#define __STMT_H			  	 
#include "sys.h"
#include "delay.h"
#define STMTAP	PBout(15)  
#define STMTAM	PBout(14)
#define STMTBP	PBout(13)
#define STMTBM	PBout(12)

#define MAXDEEP  128//完全打开窗帘所需Deep
#define MAXDEEPAROUND   5//这里先假设电机转5圈完全打开窗帘

extern u8 CurtainDeep;
extern u8 AllowChangeDeepFlag;
extern u8 NewCurtainDeep;

void STMT_Init(void);
void STMT_Turn_X_Deep(u8 dir, u8 x);
void STMT_Turn_X_Around(u8 dir,u8 x);
void STMT_X_Pulse(u8 dir,u16 x);

#endif
