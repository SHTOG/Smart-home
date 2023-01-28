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
extern u8 SelfLongAddr[8];//Zigbee设备长地址
extern u8 SelfShortAddr[2];//Zigbee设备短地址 协调器初始化为0xFF 0xFF，终端初始化为0
extern u8 PANID[2];
extern u8 NetFlag;
/*End of extern*/

void Zigbee_Init(u32 bound);
u8 Zigbee_Change_Mode(u8 modeNum);
u8 Set_SendDirection(void);
u8 OpenNet(u16 Direction_PANID);
u8 Get_State(void);
u8 Upload_Address(void);
#endif
