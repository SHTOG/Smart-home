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

//收到中控应答标志
u8 AckFlag = 0;


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
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化NVIC寄存器

	
}



/**
  * @brief		串口1中断服务程序
  * @param		void
  * @retval		void
  */

void USART1_IRQHandler(void)
{
	u8 Res;

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		Res =USART_ReceiveData(USART1);//(USART1->DR);	//读取接收到的数据
		
		if((USART1_RX_STA&0x8000)==0)//接收未完成
		{
			
			if(USART1_RX_BUF[0] == 0xC1){
				//如果接收到的是自定义传输的数据格式
				if(USART1_RX_STA&0x4000)//接收到了0x0d
				{
					if(Res!=0x0a)USART1_RX_STA=0;//接收错误,重新开始
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
//			else if(USART1_RX_BUF[0] == 'O'){
//				USART1_RX_BUF[USART1_RX_STA&0X3FFF]=Res ;//把当前接收到的数据放到USART1_RX_BUF的最后一位	
//				USART1_RX_STA++;//数据长度++
//				if(USART1_RX_BUF[1] == 'K') USART1_RX_STA|=0x8000;//接收完成了
//				else if(USART1_RX_STA >= 2){
//					//接收错误,重新开始
//					USART1_RX_STA=0;
//					USART1_RX_BUF[0] = 0;
//					USART1_RX_BUF[1] = 0;
//				} 
//			}
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
			if(USART1_RX_BUF[0] == 0x55) Zigbee_Analyse_Command_Data();
			//如果接收到的是设备间传输的数据
			else if( USART1_RX_BUF[0] == 0xC1
				  && USART1_RX_BUF[1] == 0xC2
				  && USART1_RX_BUF[2] == 0xC3
				  && USART1_RX_BUF[3] == 0xC4) Analyse_Custom_Data();
			//归零接收完成标志和伪接收寄存器准备重新接收
			USART1_RX_STA=0;
			USART1_RX_BUF[0] = 0;
		}
  	}
}

void Analyse_Custom_Data(){
	u8 Ack[] = {'O','K'};
	if(USART1_RX_BUF[12] == 0xFF){ //收到了中控的应答
		AckFlag = 1;
	}
	else if(USART1_RX_BUF[12] == 0x03){//只有命令码为0x03的才需要分析执行
		if(USART1_RX_BUF[14] == 0x00)		STMT_Open();
		else if(USART1_RX_BUF[14] == 0x01)	STMT_Close();
		else if(USART1_RX_BUF[14] == 0x02)	STMT_X_Pulse(0,16);
		else if(USART1_RX_BUF[14] == 0x03)	STMT_X_Pulse(1,16);
		Send_Custom_Data(0xFF,2,Ack);//执行完给中控一个应答
	}
}

void Send_Custom_Data(u8 type, u8 len, u8* Data){
	int i;
	USART_SendData(USART1, 0xC1);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
	USART_SendData(USART1, 0xC2);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
	USART_SendData(USART1, 0xC3);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
	USART_SendData(USART1, 0xC4);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束

	for(i = 0; i < 8; i++){
		USART_SendData(USART1, SelfLongAddr[i]);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
	}

	USART_SendData(USART1, type);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束

	USART_SendData(USART1, len);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束

	for(i = 0; i < len; i++){
		USART_SendData(USART1, Data[i]);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
	}

	USART_SendData(USART1, 0x0D);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
	USART_SendData(USART1, 0x0A);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
}
