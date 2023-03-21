#include "usart.h"

//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART1_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART1_RX_STA=0;       //接收状态标记	  
  
//收到中控应答标志
u8 AckFlag = 0;

/**
  * @brief		初始化IO 串口1 
  * @param		bound->波特率
  * @retval	
  */

void USART1_Init(u32 bound)
{
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟

	//USART1_TX   GPIOA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9

	//USART1_RX	  GPIOA.10初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10  

	//USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART1, &USART_InitStructure); //初始化串口1
	USART_Cmd(USART1, ENABLE);                    //使能串口1 

	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//子优先级1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

	USART_ClearFlag(USART1, USART_FLAG_TC);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启串口接收中断

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
			else if(USART1_RX_BUF[0] == 'B'){
				if(Res != 'U' && Res != 'S' && Res != 'Y'){
					//接收错误,重新开始
					USART1_RX_STA=0;
					USART1_RX_BUF[1] = 0;
					USART1_RX_BUF[0]=Res ;//把当前接收到的数据放到USART1_RX_BUF的第一位
					USART1_RX_STA++;//数据长度++
				}					
				else {					
					USART1_RX_BUF[USART1_RX_STA&0X3FFF]=Res;
					USART1_RX_STA++;//数据长度++
				}
				if(USART1_RX_STA == 4 && EnterModeFlag == 0) EnterModeFlag = 1;
				else if(USART1_RX_STA > 4){
					//接收错误,重新开始
					USART1_RX_STA=0;
					USART1_RX_BUF[1] = 0;
					USART1_RX_BUF[0]=Res ;//把当前接收到的数据放到USART1_RX_BUF的第一位
					USART1_RX_STA++;//数据长度++
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

void Analyse_Custom_Data(){
	u8 Ack[] = {'O','K'};
	//把密文拿出
	u16 len = (USART1_RX_STA&0X3FFF) - 4;//获取加密数据长度
	u8 i;
	u8 teaKey[] = {'N','Z','o','k','G','u','z','T','n','F','s','6','D','C','H','4'};
	u8* Data = (u8*)malloc(sizeof(u8) * len);
	for(i = 0; i < len; i++){
		Data[i] = USART1_RX_BUF[4 + i];
	}
	//开始解密数据
	decrypt(Data,len,teaKey);
	if(Data[8] == 0xFF){//收到了设备应答命令
		if(Data[9] == 0x00){//中控在请求应答
			Send_Custom_Data(0xFF,2,Ack);//应答
		}
		else if(Data[10] == 0x4F && Data[11] == 0x4B){//收到了中控的应答
			AckFlag = 1;
		}
		else if(Data[10] == 0x00 && Data[11] == 0x00) {//拒绝入网
			APPJudgeFlag = 2;
		}
		else if(Data[10] == 0x00 && Data[11] == 0x01) {//同意入网
			APPJudgeFlag = 1;
		}
	}
//	else if(Data[8] == 0xXX){//移植时请修改0xXX为对应的设备码，并在else内写上收到指定指令所需执行的内容
//		Send_Custom_Data(0xXX,2,Ack);//这句话不可或缺，收到中控的控制指令后应立即回复，告诉中控自己确实收到了
//		xxxxxx;
//	}
	free(Data);
}

/**
  * @brief		将数据封装到私有协议并发送
  * @param		type:自身设备码
  * @param		len :有效数据长度
  * @param		Data :有效数据内容指针（如果是设备信息命令，Data指针指向自身短地址第一位)
  * @retval		void
  */
void Send_Custom_Data(u8 type, u8 len, u8* Data){
	u8 i = 0,j = 0,newDataLen = 10+len;
	u8* newData;
	u8 teaKey[] = {'N','Z','o','k','G','u','z','T','n','F','s','6','D','C','H','4'};
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
	if(type == 0x00){
		newData[i] = 0x01;//这里一定要修改为对应终端的设备类型码
		i++;
		len--;
	}
	for(; j<len ;i++,j++){
		newData[i] = Data[j];
	}
	for(; i < newDataLen; i++){
		newData[i] = 1;
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
	free(newData);
}

