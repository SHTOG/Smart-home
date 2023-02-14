#include "Zigbee.h"


/*Begin of 全局变量*/
u8 SelfLongAddr[8] = {0};//Zigbee设备长地址
u8 SelfShortAddr[2] = {0};//Zigbee设备短地址 协调器初始化为0xFF 0xFF，终端初始化为0
u8 EnterModeFlag = 2;//Zigbee模块模式切换标志位，进入配置模式置0，透传模式置1
u8 PANID[2] = {0};//Zigbee局网PANID
u8 NetFlag = 0;//打开网络标志位，打开成功置1
u8 GetStateFlag = 0;//读取模块状态标志位，读取成功置1
u8 ReadySetTargetFlag = 0;//成功设置透传目标标志位，设置成功置1(与协调器的同名变量作用有一定出入)
u8 SetSendTargetFlag = 0;//设置透传目标标志位，分两步，先设置目标短地址，再设置目标端口，短地址设置成功后置1，端口设置成功后置0

/*End of 全局变量*/

/**
  * @brief		初始化与Zigbee相连的串口并初始化Zigebee设置，主要是保证Zigbee是有网的状态并且进入传输模式
  * @param		bound(波特率)->与Zigbee模块的波特率保持一致（默认115200）
  * @retval	void
  */
  
void Zigbee_Init(u32 bound){
	delay_ms(5000);//等Zigbee启动完全
    
	//串口初始化
	USART1_Init(bound);	//串口初始化波特率为bound，默认是115200与Zigbee的出厂默认值统一
	
	//进hex配置模式
	Zigbee_Change_Mode(0);
	//指定透传目标为协调器
	Set_Send_Target();
	//开始配网(确保进入PANID为0101的局网),并获取长地址和短地址
	OpenNet(0x0101);
	
	//进入透传模式
	Zigbee_Change_Mode(1);
	
	//发送长短地址告知中控自己已上网,收到应答才退出
	Upload_Address();
}

/**
  * @brief		切换Zigbee模式
  * @param		ModeNum -> 0为进入HEX指令模式，1为进入透传模式
  * @retval		void
  */

void Zigbee_Change_Mode(u8 modeNum){
	u8 EnterMode0[] = {0x2B, 0x2B, 0x2B};//Zigbee透传模式切换HEX指令模式 0为HEX指令模式
	u8 EnterMode1[] = {0x55, 0x07, 0x00, 0x11, 0x00, 0x03, 0x00, 0x01, 0x13};//Zigbee HEX指令模式切换透传模式 1为数据透传模式
	u8 i;
	if(modeNum == 0){
		while(EnterModeFlag != 0){
			delay_ms(100);
			for(i = 0; i < 3;i++){
				USART_SendData(USART1, EnterMode0[i]);         //向串口1发送数据
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
			}
		}
	}
	else if(modeNum == 1){
		//进入透传模式
		while(EnterModeFlag != 1){
			delay_ms(50);
			for(i = 0; i < 9;i++){
				USART_SendData(USART1, EnterMode1[i]);         //向串口1发送数据
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
			}
		}
	}
}

/**
  * @brief		设置目标短地址为0101(即协调器)和目标终端为01
  * @param		void
  * @retval		void
  */

void Set_Send_Target(){
	u8 i;
	u8 SetDirection[] = {0x55, 0x08, 0x00, 0x11, 0x00, 0x01, 0x00, 0x00, 0x00, 0x10, 0x55, 0x07, 0x00, 0x11, 0x00, 0x02, 0x00, 0x01, 0x12};
	while(SetSendTargetFlag != 1){
		delay_ms(10);
		for(i = 0; i < 10;i++){
			USART_SendData(USART1, SetDirection[i]);         //向串口1发送数据
			while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
		}
	}
	while(SetSendTargetFlag != 0){
		delay_ms(10);
		for(i = 10; i < 19; i++){
			USART_SendData(USART1, SetDirection[i]);         //向串口1发送数据
			while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
		}
	}
	delay_ms(10);
	ReadySetTargetFlag = 1;
}



/**
  * @brief		打开Zigbee模块的网络（终端：加入一个指定PANID的现有网络）
  * @param		Direction_PANID->四字节的十六进制数，表示目标网络的PANID
  * @retval	    1->网络打开成功
  */

void OpenNet(u16 Direction_PANID){
	u8 i;
	u8 SetPANID[] = {0x55, 0x05, 0x00, 0x08, 0x01, 0x01, 0x08};
	u8 NetStart[] = {0x55, 0x04, 0x00, 0x02, 0x00, 0x02,};
		u8 GetState[] = {0x55, 0x03, 0x00, 0x00, 0x00};//查询Zigbee模组当前状态
	while(PANID[0] != Direction_PANID/256 || PANID[1] != Direction_PANID%256){
		delay_ms(100);
		//设置PANID为01 01(这里不用判断是否写入，因为如果已经连过网的话是写入失败的，给个100ms防数据流过快就行了)
		for(i = 0; i < 7; i++){
			USART_SendData(USART1, SetPANID[i]);         //向串口1发送数据
			while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
		}
		while(NetFlag != 1){
			for(i = 0; i < 6;i++){
				USART_SendData(USART1, NetStart[i]);         //向串口1发送数据
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
			}
			delay_ms(50);
		}
		NetFlag = 0;
		delay_ms(1000);//等个一秒钟配网
		//这里主要获取PANID,如果获取到了，那么就表示成功入网（协调器也在线）
//		Get_State();//这里不能循环，所以不能调用这个函数
		for(u8 i = 0; i < 5;i++){
			USART_SendData(USART1, GetState[i]);         //向串口1发送数据
			while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
		}
		delay_ms(50);
		GetStateFlag = 0;
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
	while(GetStateFlag != 1){
		for(i = 0; i < 5;i++){
			USART_SendData(USART1, GetState[i]);         //向串口1发送数据
			while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
		}
	}
	GetStateFlag = 0;
}

/**
  * @brief		向协调器发送自己的长短地址
  * @param		void
  * @retval	    1->发送成功且协调器已应答
  */

void Upload_Address(void){
	while(1){
		AckFlag = 0;
		Send_Custom_Data(0x00,2,SelfShortAddr);
		delay_ms(500);//考虑数据接收延迟,避免频繁发送导致中控数据拥堵
		if(AckFlag == 1){
			AckFlag = 0;
			return ;
		}
	}
}


/**
  * @brief		分析Zigbee的反馈命令
  * @param		void
  * @retval	void
  */


void Zigbee_Analyse_Command_Data(){
	if (model == E18){
		if(USART1_RX_BUF[1] == 0x2A && USART1_RX_BUF[2] == 0x00 && USART1_RX_BUF[3] == 0x00 && USART1_RX_BUF[4] == 0x00){//55 2A 00 00 00
			SelfLongAddr[0] = USART1_RX_BUF[6];
			SelfLongAddr[1] = USART1_RX_BUF[7];
			SelfLongAddr[2] = USART1_RX_BUF[8];
			SelfLongAddr[3] = USART1_RX_BUF[9];
			SelfLongAddr[4] = USART1_RX_BUF[10];
			SelfLongAddr[5] = USART1_RX_BUF[11];
			SelfLongAddr[6] = USART1_RX_BUF[12];
			SelfLongAddr[7] = USART1_RX_BUF[13];
			PANID[0] = USART1_RX_BUF[15];
			PANID[1] = USART1_RX_BUF[16];
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
