#ifndef __ZIGBEE_H
#define __ZIGBEE_H	
#include "sys.h"
#include "usart.h"	
#include "delay.h"
#include "stdlib.h"
#include "led.h"
#include "beep.h"

#define E18  1
#define E180 2

#define model E18 //����ѡ��һ��ʹ�õ�ģ��ʱE180����E18

/*Begin of extern*/
extern u8 SelfLongAddr[8];//Zigbee�豸����ַ
extern u8 SelfShortAddr[2];//Zigbee�豸�̵�ַ Э������ʼ��Ϊ0xFF 0xFF���ն˳�ʼ��Ϊ0
extern u8 PANID[2];
/*End of extern*/

void Zigbee_Init(u32 bound);
void Zigbee_Change_Mode(u8 modeNum);
void Set_Send_Target(void);
void OpenNet(u16 Direction_PANID);
void Get_State(void);
void Upload_Address(void);
void Zigbee_Analyse_Command_Data(void);
#endif
