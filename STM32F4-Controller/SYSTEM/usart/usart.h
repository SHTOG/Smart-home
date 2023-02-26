#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "stm32f4xx_conf.h"
#include "sys.h" 
#include "delay.h"
#include "usart.h"
#include "myList.h"
#include "24Cxx.h"
#include "Zigbee.h"
#include "beep.h"
#include "tea.h"

#define USART_REC_LEN  			200  	//定义最大接收字节数 200

/*Begin of extern*/

/*End of extern*/



void USART1_Init(u32 bound);
void USART2_Init(u32 bound);
void Analyse_APP_Data(void);
void Analyse_Custom_Data(void);
void Send_Custom_Data(USART_TypeDef* USARTx, u8 type, u8 len, u8* Data);
#endif


