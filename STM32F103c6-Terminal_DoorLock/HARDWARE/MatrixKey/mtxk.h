#ifndef __MTXK_H
#define __MTXK_H			  	 
#include "sys.h"
#include "timer.h"

extern uint32_t MTXKCD;

void MTXK_Init(void);
u8 MTXK_Scan(void);

#endif  
