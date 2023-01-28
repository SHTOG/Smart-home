#ifndef __ZIGBEE_H
#define __ZIGBEE_H	
#include "sys.h"
#include "usart.h"	
#include "delay.h"
#include "stdlib.h"
#include "OLED.h"
#include "led.h"
#include "beep.h"

#define E18  1
#define E180 2

#define model E18 //����ѡ��һ��ʹ�õ�ģ��ʱE180����E18

/*Begin of extern*/
extern u8 SelfLongAddr[8];
extern u8 SelfShortAddr[2];
extern u8 NetFlag;
/*End of extern*/

void Zigbee_Init(u32 bound);
u8 Zigbee_Change_Mode(u8 modeNum);
u8 Get_State(void);
void Zigbee_Analyse_Command_Data(void);
void Analyse_State_Data(void);
void OpenNet(void);
#endif
