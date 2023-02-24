#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h"
#include "Zigbee.h"
#include "led.h"
#include "tea.h"
#define USART_REC_LEN  			200  	//�����������ֽ��� 200

/*Begin of extern*/
extern u8  USART1_RX_BUF[200]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART1_RX_STA;         		//����״̬���
extern u8 AckFlag;
/*End of extern*/


void USART1_Init(u32 bound);
void Analyse_Custom_Data(void);
void Send_Custom_Data(u8 type, u8 len, u8* Data);
#endif


