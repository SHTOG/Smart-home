#ifndef __ANALYSEANDSEND_H
#define __ANALYSEANDSEND_H			  	 
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "myList.h"
#include "24Cxx.h"

/*Begin of extern*/
extern struct myDevice* DeviceList;
extern u8 SelfLongAddr[8];
extern u8 BootedTimeFlag;

/*End of extern*/

void Analyse_Custom_Data(void);
void Send_Custom_Data(USART_TypeDef* USARTx, u8 type, u8 len, u8* Data);

#endif  
