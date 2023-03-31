#ifndef __STMT_H
#define __STMT_H			  	 
#include "sys.h"
#define STMTAP	PAout(8)  
#define STMTAM	PBout(15)
#define STMTBP	PBout(14)
#define STMTBM	PBout(13)

#define MAXDEEP  128//完全打开窗帘所需Deep
#define MAXDEEPAROUND   5//这里先假设电机转5圈完全打开窗帘

void STMT_Init(void);
void STMT_Turn_X_Deep(u8 dir, u8 x);
void STMT_Turn_X_Around(u8 dir,u8 x);
void STMT_X_Pulse(u8 dir,u16 x);

#endif
