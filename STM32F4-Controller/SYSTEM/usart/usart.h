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
extern u8  USART1_RX_BUF[200]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART1_RX_STA;         		//接收状态标记	
extern u8  USART2_RX_BUF[200]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART2_RX_STA;         		//接收状态标记	
extern struct myDevice* DeviceList;
extern u8 SelfLongAddr[8];
extern u8 BootedTimeFlag;
extern u8 AckFlag;
extern u8 USART1ReciveITEnable;
extern u8 USART2ReciveITEnable;
extern u8 CurtainDeep;
extern u8 AckJudge;
/*End of extern*/



void USART1_Init(u32 bound);
void USART2_Init(u32 bound);
void Analyse_Custom_Data(u8 USARTNum);
void Send_Custom_Data(USART_TypeDef* USARTx, u8 type, u8 len, u8* Data);
#endif


