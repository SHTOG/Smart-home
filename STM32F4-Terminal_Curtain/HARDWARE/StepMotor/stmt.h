#ifndef __STMT_H
#define __STMT_H			  	 
#include "sys.h"
#include "delay.h"
#define STMTAP	PEout(0)
#define STMTAM	PEout(1)
#define STMTBP	PEout(2)
#define STMTBM	PEout(3)
void STMT_Init(void);
void STMT_Open(void);
void STMT_Close(void);
void STMT_Turn_X_Around(u8 dir,u8 x);
void STMT_X_Pulse(u8 dir,u16 x);

#endif  
