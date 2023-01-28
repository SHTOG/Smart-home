#include "AnalyseAndSend.h"

/*Begin of extern*/
extern Device* DeviceList;
extern u8 SelfLongAddr[8];
/*End of extern*/

void Analyse_Custom_Data(){
	u8 DeviceLongAddr[8];
	u8 DeviceShortAddr[2];
	u8 i;//循环用
	u8 Ack[] = {'O','K'};
	u8 type;//设备类型
	if(USART1_RX_BUF[12] == 0x00){//设备信息指令
		Send_Custom_Data(USART1,0xFF,2,Ack);//先回应再做自己的事
		for(i = 0; i < 8; i++){
			DeviceLongAddr[i] = USART1_RX_BUF[4 + i]; 
		}
		type = USART1_RX_BUF[14];
		DeviceShortAddr[0] = USART1_RX_BUF[15];
		DeviceShortAddr[1] = USART1_RX_BUF[16];
		if(checkByLongAddr(DeviceList,DeviceLongAddr,DeviceShortAddr) == 0){
			insertNodeByType(DeviceList,type,DeviceLongAddr,DeviceShortAddr);
		}

	}
}

void Send_Custom_Data(USART_TypeDef* USARTx, u8 type, u8 len, u8* Data){
	if(USARTx == USART1){
		delay_ms(50);//莫名怪异的情况，这里至少要delay个40ms，数据才能发出，不然有时候会发送失败
		USART_SendData(USART1, 0xC1);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
		USART_SendData(USART1, 0xC2);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
		USART_SendData(USART1, 0xC3);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
		USART_SendData(USART1, 0xC4);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束

		for(u8 i = 0; i < 8; i++){
			USART_SendData(USART1, SelfLongAddr[i]);
			while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
		}

		USART_SendData(USART1, type);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束

		USART_SendData(USART1, len);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束

		for(u8 i = 0; i < len; i++){
			USART_SendData(USART1, Data[i]);
			while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
		}

		USART_SendData(USART1, 0x0D);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
		
		USART_SendData(USART1, 0x0A);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
	}
	if(USARTx == USART2){
		USART_SendData(USART2, 0xA1);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
		USART_SendData(USART2, 0xA2);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
		USART_SendData(USART2, 0xA3);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
		USART_SendData(USART2, 0xA4);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束

		for(u8 i = 0; i < 8; i++){
			USART_SendData(USART1, SelfLongAddr[i]);
			while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
		}

		
	}
}

