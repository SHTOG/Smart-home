#include "usart.h"

//读取USARTx->SR能避免莫名其妙的错误   
//接收缓冲,最大200个字节.	
u8 USART1_RX_BUF[200];     
u8 USART2_RX_BUF[200];     

//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART1_RX_STA=0;       //接收状态标记	
u16 USART2_RX_STA=0;       //接收状态标记	

//标志位
u8 USART1ReciveITEnable = 1;//串口1接收中断允许位,置1时允许，0为禁止
u8 USART2ReciveITEnable = 1;//串口2接收中断允许位,置1时允许，0为禁止
u8 BootedTimeFlag = 0;//开机时长记录flag,开机满10分钟就置1
u8 AckFlag = 0;//来自终端的应答标志位，收到应答置1处理完马上归0
u8 AckJudge = 0;//在delay函数内植入这个判断标志位，为1时delay_ms期间如果收到终端的应答就直接退出延时

/**
  * @brief		初始化IO 串口1 
  * @param		bound->波特率
  * @retval	
  */

void USART1_Init(u32 bound){
   //GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //使能GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//使能USART1时钟
 
	//串口1对应引脚复用映射
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //GPIOA9复用为USART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10复用为USART1
	
	//USART1的GPIO配置
 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9与GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化PA9，PA10

    //USART1 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;	//收发模式
	USART_Init(USART1, &USART_InitStructure); //初始化串口1
	
	USART_Cmd(USART1, ENABLE);  //使能串口1 
	USART_ClearFlag(USART1, USART_FLAG_TC);
		
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启相关中断

	//Usart1 NVIC 配置
  	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =2;		//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化NVIC寄存器

	
}

/**
 * @brief		初始化IO 串口2
 * @param		bound->波特率
 * @retval	
 */

void USART2_Init(u32 bound){
  	//声明一下结构体
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //使能GPIOA时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//使能USART2时钟

	//USART2对应引脚复用映射
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2); //GPIOA2复用为USART2
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2); //GPIOA3复用为USART2
	
	//USART2的GPIO配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; //GPIOA2与GPIOA3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化PA2,PA3


   //USART2 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART2, &USART_InitStructure); //初始化串口2
	
	USART_Cmd(USART2, ENABLE);  //使能串口2
	USART_ClearFlag(USART2, USART_FLAG_TC);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启相关中断

	//Usart2 NVIC 配置
 	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;//串口2中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;//抢占优先级0（最高优先级）
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =2;		//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化NVIC寄存器

	
}



/**
  * @brief		串口1中断服务程序
  * @param		void
  * @retval		void
  */

void USART1_IRQHandler(void){
	u8 Res;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET){  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
		
		Res =USART_ReceiveData(USART1);	//读取接收到的数据
		if((USART1_RX_STA&0x8000)==0){//接收未完成
			if(USART1_RX_BUF[0] == 0xC1){
				//如果接收到的是自定义传输的数据格式
				if(USART1_RX_STA&0x4000)//接收到了0x0d
				{
					if(Res!=0x0a){
						USART1_RX_STA &= 0xBFFF;
						USART1_RX_BUF[USART1_RX_STA&0X3FFF]=0X0D ;//把0x0D放到USART1_RX_BUF的倒数第二位
						USART1_RX_STA++;//数据长度++
						USART1_RX_BUF[USART1_RX_STA&0X3FFF]=Res ;//把当前接收到的数据放到USART1_RX_BUF的最后一位
						USART1_RX_STA++;//数据长度++
					}
					else USART1_RX_STA|=0x8000;	//接收完成了 
				}
				else //还没收到0X0D
				{
					if(Res==0x0d)USART1_RX_STA|=0x4000;
					else
					{
						USART1_RX_BUF[USART1_RX_STA&0X3FFF]=Res ;//把当前接收到的数据放到USART1_RX_BUF的最后一位	
						USART1_RX_STA++;//数据长度++
						if(USART1_RX_STA>(USART_REC_LEN-1))USART1_RX_STA=0;//接收数据错误,重新开始接收	  
					}
				}
			}
			else if(USART1_RX_BUF[0] == 0x55){
				//如果收到的第一个字节为0x55则判断为Zigbee的命令
				USART1_RX_BUF[USART1_RX_STA&0X3FFF]=Res ;//把当前接收到的数据放到USART1_RX_BUF的最后一位	
				USART1_RX_STA++;//数据长度++
				if(USART1_RX_BUF[1] + 2 == USART1_RX_STA) USART1_RX_STA|=0x8000;//接收完成了
				else if(USART1_RX_BUF[1] + 2 < USART1_RX_STA){
					//接收错误,重新开始
					USART1_RX_STA=0;
					USART1_RX_BUF[0] = 0;
					USART1_RX_BUF[1] = 0;
				}
			}
			else if(USART1_RX_BUF[0] == 0){
				//如果第一个字节是0，则判断为刚开始接收第一字节数据
				USART1_RX_BUF[1] = 0;//给数据位Zigbee命令的时候用
				USART1_RX_BUF[0]=Res ;//把当前接收到的数据放到USART1_RX_BUF的第一位
				USART1_RX_STA++;//数据长度++
			}
			else{
				//接收错误,重新开始
				USART1_RX_STA=0;
				USART1_RX_BUF[0] = 0;
				USART1_RX_BUF[1] = 0;
				USART1_RX_BUF[0]=Res ;//把当前接收到的数据放到USART1_RX_BUF的第一位
				USART1_RX_STA++;//数据长度++
			}
		}
		if(USART1_RX_STA&0x8000){
			//当串口接收完一串数据
			//如果串口接收到的是来自Zigbee的命令
			if(USART1_RX_BUF[0] == 0x55)		Zigbee_Analyse_Command_Data();
			//如果接收到的是设备间传输的数据
			else if( USART1_RX_BUF[0] == 0xC1
				  && USART1_RX_BUF[1] == 0xC2
				  && USART1_RX_BUF[2] == 0xC3
				  && USART1_RX_BUF[3] == 0xC4) 	Analyse_Custom_Data();
			//归零接收完成标志和伪接收寄存器准备重新接收
			USART1_RX_STA=0;
			USART1_RX_BUF[0] = 0;
		}
     } 
}

/**
  * @brief		串口2中断服务程序
  * @param		void
  * @retval		void
  */
void USART2_IRQHandler(void){
	u8 Res;
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET && USART2ReciveITEnable){//接收中断(接收到的数据必须是0x0d 0x0a结尾)
		Res = USART_ReceiveData(USART2);//(USART2->DR);	//读取接收到的数据
		if(USART2_RX_BUF[0] == 0xA1){//如果接收到的是来自ESP32的数据
			if(USART2_RX_STA&0x4000)//接收到了0x0d
			{
				if(Res!=0x0a)USART2_RX_STA=0;//接收错误,重新开始
				else USART2_RX_STA|=0x8000;	//接收完成了 
			}
			else //还没收到0X0D
			{
				if(Res==0x0d)USART2_RX_STA|=0x4000;
				else
				{
					USART2_RX_BUF[USART2_RX_STA&0X3FFF]=Res ;//把当前接收到的数据放到USART2_RX_BUF的最后一位	
					USART2_RX_STA++;//数据长度++
					if(USART2_RX_STA>(USART_REC_LEN-1))USART2_RX_STA=0;//接收数据错误,重新开始接收	  
				}
			}
		}
		else if(USART2_RX_BUF[0] == 0){
			//如果第一个字节是0，则判断为刚开始接收第一字节数据
			USART2_RX_BUF[0]=Res ;//把当前接收到的数据放到USART2_RX_BUF的第一位
			USART2_RX_STA++;//数据长度++
		}
		else{
			//接收错误,重新开始
			USART2_RX_STA=0;
			USART2_RX_BUF[0]=Res ;//把当前接收到的数据放到USART2_RX_BUF的第一位
			USART2_RX_STA++;//数据长度++
		}
		if(USART2_RX_STA&0x8000){//接收完成一段数据
			Analyse_APP_Data();//分析数据
			//归零接收完成标志和伪接收寄存器准备重新接收
			USART2_RX_STA=0;
			USART2_RX_BUF[0] = 0;
		}
 	} 
}

void Analyse_APP_Data(){
	if(USART2_RX_BUF[6] == 0xFF){
		if(USART2_RX_BUF[8] == 'O' && USART2_RX_BUF[9] == 'K' ){
			Esp32AckFlag = 1;
		}
	}
	else ReadySetTargetFlag = 0;//先设置目标短地址和端口，然后直接发送就OK
}

void Analyse_Custom_Data(){
	u8 DeviceLongAddr[8];
	u8 DeviceShortAddr[2];
	u8 i;//循环用
	u8 Ack[] = {'O','K'};
	u8 type;//设备类型

	//把密文拿出
	u16 len = (USART1_RX_STA&0X3FFF) - 4;//获取加密数据长度
	u8 teaKey[] = {'N','Z','o','k','G','u','z','T','n','F','s','6','D','C','H','4'};
	u8* Data = (u8*)malloc(sizeof(u8) * len);
	for(i = 0; i < len; i++){
		Data[i] = USART1_RX_BUF[4 + i];
	}
	//开始解密数据
	decrypt(Data,len,teaKey);

	//如果来自终端
	if(Data[8] == 0x00){//设备信息命令		

		//这里又出现了在串口1中断在此需求串口1中断
		//		Zigbee_Change_Mode(0);
		//		Set_Send_Target(DeviceShortAddr,0x01);
		//		Zigbee_Change_Mode(1);
		Send_Custom_Data(USART1,0xFF,2,Ack);//先回应再做自己的事

		for(i = 0; i < 8; i++){
			DeviceLongAddr[i] = Data[i]; 
		}
		type = Data[10];
		DeviceShortAddr[0] = Data[11];
		DeviceShortAddr[1] = Data[12];

		if(CheckByLongAddr(DeviceList,DeviceLongAddr,DeviceShortAddr) == 0){
			InsertNodeByType(DeviceList,type,1,DeviceLongAddr,DeviceShortAddr);
			//重新封装该设备信息
			Data = (u8*)malloc(sizeof(u8) * 12);
			Data[0] = type;
			Data[1] = 1;
			for(i = 0; i < 8; i++){
				Data[2+i] = DeviceLongAddr[i];
			}
			Data[10] = DeviceShortAddr[0];
			Data[11] = DeviceShortAddr[1];
			
			//该设备信息发送到APP端
			Esp32AckFlag = 0;
			WaitTime = 0;
			while(Esp32AckFlag == 0){
				if(WaitTime == 5){//超时退出
					free(Data);
					return ;
				}
				Send_Custom_Data(USART2,0x00,0,NULL);//开始信号，等待应答
				delay_ms(300);//稍微等等
			}
			Send_Custom_Data(USART2,0x00,12,Data);
			
			if(BootedTimeFlag == 1){
				AT24CXX_Save_List(0,DeviceList);
			}
		}
	}
	else if(Data[8] == 0xFF){//设备应答命令
		if(Data[10] == 'O' && Data[11] == 'K'){//设备应答ok
			AckFlag = 1;
		}
		
	}
	else if(Data[8] == 0x02){//暖通反馈命令
		Send_Custom_Data(USART2,0x02,Data[9],&Data[10]);//把温湿度数据发送到APP
	}
	free(Data);

}

void Send_Custom_Data(USART_TypeDef* USARTx, u8 type, u8 len, u8* Data){
	u8 i = 0,j = 0,newDataLen = 0;
	u8* newData;
	u8 teaKey[] = {'N','Z','o','k','G','u','z','T','n','F','s','6','D','C','H','4'};//秘钥

	if(USARTx == USART1){
		delay_ms(50);//Zigbee模块自带延迟
		
		//将发送的数据封装到一个数组内
		newDataLen = 10+len;
		while(newDataLen % 8 != 0){//把除帧头帧尾数据大小补足到所占内存为8字节的倍数
			newDataLen++;
		}
		newData = (u8*)malloc(sizeof(u8) * newDataLen);
		for(; i < 8; i++){
			newData[i] = SelfLongAddr[i];
		}
		newData[i] = type;
		i++;
		newData[i] = len;
		i++;
		for(; j<len ;i++,j++){
			newData[i] = Data[j];
		}
		for(; i < newDataLen; i++){
			newData[i] = 0;
		}
		
		//加密
		encrypt(newData,newDataLen,teaKey);

		USART_SendData(USART1, 0xC1);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
		USART_SendData(USART1, 0xC2);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
		USART_SendData(USART1, 0xC3);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
		USART_SendData(USART1, 0xC4);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束

		for(i = 0; i < newDataLen; i++){
			USART_SendData(USART1, newData[i]);
			while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
		}

		USART_SendData(USART1, 0x0D);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
		USART_SendData(USART1, 0x0A);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
	}

	if(USARTx == USART2){
		//发送帧头
		USART_SendData(USART2, 0xA1);
		while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//等待发送结束
		USART_SendData(USART2, 0xA2);
		while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//等待发送结束
		USART_SendData(USART2, 0xA3);
		while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//等待发送结束
		USART_SendData(USART2, 0xA4);
		while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//等待发送结束

		for(u8 i = 0; i < 2; i++){
			USART_SendData(USART2, SelfShortAddr[i]);
			while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//等待发送结束
		}
		
		USART_SendData(USART2, type);
		while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//等待发送结束

		USART_SendData(USART2, len);
		while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//等待发送结束

		for(i = 0; i < len; i++){
			USART_SendData(USART2, Data[i]);
			while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//等待发送结束
		}

		USART_SendData(USART2, 0x0D);
		while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//等待发送结束
		USART_SendData(USART2, 0x0A);
		while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//等待发送结束
	}

	free(newData);
}
