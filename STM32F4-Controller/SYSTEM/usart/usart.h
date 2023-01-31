#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "stm32f4xx_conf.h"
#include "sys.h" 
#include "AnalyseAndSend.h"
#include "Zigbee.h"
#include "beep.h"

#define USART_REC_LEN  			200  	//�����������ֽ��� 200

/*Begin of extern*/
extern u8  USART1_RX_BUF[200]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART1_RX_STA;         		//����״̬���	
extern u8  USART2_RX_BUF[200]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART2_RX_STA;         		//����״̬���	
/*End of extern*/

void USART1_Init(u32 bound);
void USART2_Init(u32 bound);
#endif


