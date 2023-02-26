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

#define model E18 //这里选择一下使用的模块时E180还是E18

/*Begin of extern*/

/*End of extern*/

void Zigbee_Init(u32 bound);
void Zigbee_Change_Mode(u8 modeNum);
void Get_State(void);
void Zigbee_Analyse_Command_Data(void);
void OpenNet(void);
void Set_Send_Target(u8* DSAddr,u8 DPort);
#endif
