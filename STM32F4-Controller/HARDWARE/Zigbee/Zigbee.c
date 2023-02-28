#include "Zigbee.h"

/*Begin of 全局变量*/
u8 SelfShortAddr[] = {0xFF,0xFF};//设备短地址:协调器初始FFFF，其他初始0000
u8 SelfLongAddr[8] = {0x70, 0xE4, 0x61, 0x25, 0x00, 0x4B, 0x12, 0x00};//指定协调器的长地址
u8 NetFlag = 0;
u8 EnterModeFlag = 2;//Zigbee模块模式切换标志位，进入配置模式置0，透传模式置1
u8 GetStateFlag = 0;//读取模块状态标志位，读取成功置1
u8 ReadySetTargetFlag = 1;//成功设置透传目标标志位，1为已设置，0为待设置
u8 SetSendTargetFlag = 0;//设置透传目标标志位，分两步，先设置目标短地址，再设置目标端口，短地址设置成功后置1，端口设置成功后置0
/*End of 全局变量*/

/**
  * @brief		初始化与Zigbee相连的串口并初始化Zigebee设置，主要是保证Zigbee是有网的状态并且进入传输模式
  * @param		bound(波特率)->与Zigbee模块的波特率保持一致
  * @retval	void
  */

void Zigbee_Init(u32 bound){
    //串口初始化
	USART1_Init(bound);	//串口初始化波特率为bound，默认是115200与Zigbee的出厂默认值统一
	//进入配置模式
	Zigbee_Change_Mode(0);
	//开始配网
	OpenNet();
	//读取设备信息
	Get_State();
	//进入透传模式
	Zigbee_Change_Mode(1);
}
/**
  * @brief		打开Zigbee模块的网络（协调器：如果未建立网络，则建立一个新网络；如果已建立网络，则再调用此函数时为开放网络180s，180s内终端和路由器可加入）
  * @param		void
  * @retval		1->网络打开成功
  */

void OpenNet(){
	u8 NetStart[] = {0x55, 0x04, 0x00, 0x02, 0x00, 0x02,};//开始配网:协调器执行该命令会打开入网允许权限,路由和终端节点执行该命令会尝试加入一个协调器创建的网络
	u8 i;
	while(NetFlag == 0){
		for(i = 0; i < 6;i++){
			USART_SendData(USART1, NetStart[i]);         //向串口1发送数据
			while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
		}
		delay_ms(50);
	}
	NetFlag = 0;
}

/**
  * @brief		切换Zigbee模式
  * @param		ModeNum -> 0为进入HEX指令模式，1为进入透传模式
  * @retval	0为成功进入指令模式，1为成功进入透传模式
  */


void Zigbee_Change_Mode(u8 modeNum){
	u8 EnterMode0[] = {0x2B, 0x2B, 0x2B};//Zigbee透传模式切换HEX指令模式 0为HEX指令模式
	u8 EnterMode1[] = {0x55, 0x07, 0x00, 0x11, 0x00, 0x03, 0x00, 0x01, 0x13};//Zigbee HEX指令模式切换透传模式 1为数据透传模式
	u8 i;
	if(modeNum == 0){
		//进入配置模式
		while(EnterModeFlag != 0){
			delay_ms(10);
			for(i = 0; i < 3;i++){
				USART_SendData(USART1, EnterMode0[i]);         //向串口1发送数据
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
			}
			delay_ms(50);
		}
	}
	else if(modeNum == 1){
		//进入透传模式
		while(EnterModeFlag != 1){
			delay_ms(10);
			for(i = 0; i < 9;i++){
				USART_SendData(USART1, EnterMode1[i]);         //向串口1发送数据
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
			}
			delay_ms(50);
		}
	}
}

/**
  * @brief		查询模组当前状态（读取参数）
  * @param		void
  * @retval	    1->成功获取状态
  */

void Get_State(void){
	u8 i;
	u8 GetState[] = {0x55, 0x03, 0x00, 0x00, 0x00};//查询Zigbee模组当前状态
	while(GetStateFlag == 1){
		delay_ms(10);
		for(i = 0; i < 5;i++){
			USART_SendData(USART1, GetState[i]);         //向串口1发送数据
			while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
		}
	}
	GetStateFlag = 0;
}

/**
  * @brief		分析Zigbee的反馈命令
  * @param		void
  * @retval	void
  */


void Zigbee_Analyse_Command_Data(){
	if (model == E18){
		if(USART1_RX_BUF[1] == 0x2A && USART1_RX_BUF[2] == 0x00 && USART1_RX_BUF[3] == 0x00 && USART1_RX_BUF[4] == 0x00 && USART1_RX_BUF[5] == 0x00){//55 2A 00 00 00 00
			SelfLongAddr[0] = USART1_RX_BUF[6];
			SelfLongAddr[1] = USART1_RX_BUF[7];
			SelfLongAddr[2] = USART1_RX_BUF[8];
			SelfLongAddr[3] = USART1_RX_BUF[9];
			SelfLongAddr[4] = USART1_RX_BUF[10];
			SelfLongAddr[5] = USART1_RX_BUF[11];
			SelfLongAddr[6] = USART1_RX_BUF[12];
			SelfLongAddr[7] = USART1_RX_BUF[13];
			SelfShortAddr[0] = USART1_RX_BUF[17];
			SelfShortAddr[1] = USART1_RX_BUF[18];
			GetStateFlag = 1;
		}
		else if(USART1_RX_BUF[1] == 0x04 && USART1_RX_BUF[2] == 0x00 && USART1_RX_BUF[3] == 0x02 && USART1_RX_BUF[4] == 0x00 && USART1_RX_BUF[5] == 0x02){//55 04 00 02 00 02
			NetFlag = 1;//判断网络已打开
		}
		else if(ReadySetTargetFlag == 1 && USART1_RX_BUF[1] == 0x04 && USART1_RX_BUF[2] == 0x00 && USART1_RX_BUF[3] == 0x11 && USART1_RX_BUF[4] == 0x00 && USART1_RX_BUF[5] == 0x11){//55 04 00 11 00 11 
			EnterModeFlag = 1;
		}
		else if(ReadySetTargetFlag == 0 && USART1_RX_BUF[1] == 0x04 && USART1_RX_BUF[2] == 0x00 && USART1_RX_BUF[3] == 0x11 && USART1_RX_BUF[4] == 0x00 && USART1_RX_BUF[5] == 0x11){//55 04 00 11 00 11 
			if(SetSendTargetFlag == 0) SetSendTargetFlag = 1;
			else SetSendTargetFlag = 0;
		}
		else if(USART1_RX_BUF[1] == 0x03 && USART1_RX_BUF[2] == 0xFF && USART1_RX_BUF[3] == 0xFE && USART1_RX_BUF[4] == 0x01){//55 03 FF FE 01
			if(EnterModeFlag != 0) EnterModeFlag = 0;
		}
	}
	else if(model == E180){
		
	}
}

/**
  * @brief		设置目标短地址和目标端口
  * @param		DSAddr:目标短地址
  * @param		DPort:目标端口
  * @retval		1->设置成功
  */

void Set_Send_Target(u8* DSAddr,u8 DPort){
	u8 SetDSAddr[] = {0x55,0x08,0x00,0x11,0x00,0x01,0x00,DSAddr[0],DSAddr[1],0x00};
	u8 SetDPort[] = {0x55,0x07,0x00,0x11,0x00,0x02,0x00,DPort,0x00};
	u8 temp = 0,i;
	//计算一下上面两个命令的校验码
	for(i = 0; i < SetDSAddr[1] - 1; i++){
		temp = temp^SetDSAddr[2+i];
	}
    SetDSAddr[9] = temp;
	temp = 0;
	for(i = 0; i < SetDPort[1] - 1; i++){
		temp = temp^SetDPort[2+i];
	}
	SetDPort[8] = temp;
	//设置目标短地址
	ReadySetTargetFlag = 0;
	while(SetSendTargetFlag == 0){
		delay_ms(50);
		for(i = 0; i < 10;i++){
			USART_SendData(USART1, SetDSAddr[i]);         //向串口1发送数据
			while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
		}
	}
	//设置目标端口
	while(SetSendTargetFlag == 1){
		delay_ms(50);
		for(i = 0; i < 9; i++){
			USART_SendData(USART1, SetDPort[i]);         //向串口1发送数据
			while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
		}
	}
	ReadySetTargetFlag = 1;
}

