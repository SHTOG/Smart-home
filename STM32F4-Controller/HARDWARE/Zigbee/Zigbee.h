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
extern u8 SelfLongAddr[8];
extern u8 SelfShortAddr[2];
extern u8 NetFlag;
extern u8 EnterModeFlag;
extern u8 GetStateFlag;//读取模块状态标志位，读取成功置1
extern u8 SetSendTargetFlag;//设置透传目标标志位，分两步，先设置目标短地址，再设置目标端口，短地址设置成功后置1，端口设置成功后置0
extern u8 ReadySetTargetFlag;//成功设置透传目标标志位，1为已设置，0为待设置
/*End of extern*/

void Zigbee_Init(u32 bound);
void Zigbee_Change_Mode(u8 modeNum);
void Get_State(void);
void Zigbee_Analyse_Command_Data(void);
void OpenNet(void);
void Set_Send_Target(u8* DSAddr,u8 DPort);
#endif
