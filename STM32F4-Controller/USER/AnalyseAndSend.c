#include "AnalyseAndSend.h"

u8 BootedTimeFlag = 0;//����ʱ����¼flag,������10���Ӿ���1

void Analyse_Custom_Data(){
	u8 DeviceLongAddr[8];
	u8 DeviceShortAddr[2];
	u8 i;//ѭ����
	u8 Ack[] = {'O','K'};
	u8 type;//�豸����
	if(USART1_RX_BUF[12] == 0x00){//�豸��Ϣָ��
		Send_Custom_Data(USART1,0xFF,2,Ack);//�Ȼ�Ӧ�����Լ�����
		for(i = 0; i < 8; i++){
			DeviceLongAddr[i] = USART1_RX_BUF[4 + i]; 
		}
		type = USART1_RX_BUF[14];
		DeviceShortAddr[0] = USART1_RX_BUF[15];
		DeviceShortAddr[1] = USART1_RX_BUF[16];
		if(CheckByLongAddr(DeviceList,DeviceLongAddr,DeviceShortAddr) == 0){
			InsertNodeByType(DeviceList,type,1,DeviceLongAddr,DeviceShortAddr);
			//��������λ�ø��������͵�APP�ˡ�
			if(BootedTimeFlag == 1){
				AT24CXX_Save_List(0,DeviceList);
			}
		}

	}
}

void Send_Custom_Data(USART_TypeDef* USARTx, u8 type, u8 len, u8* Data){
	if(USARTx == USART1){
		delay_ms(50);//Ī��������������������Ҫdelay��40ms�����ݲ��ܷ�������Ȼ��ʱ��ᷢ��ʧ��
		USART_SendData(USART1, 0xC1);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
		USART_SendData(USART1, 0xC2);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
		USART_SendData(USART1, 0xC3);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
		USART_SendData(USART1, 0xC4);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���

		for(u8 i = 0; i < 8; i++){
			USART_SendData(USART1, SelfLongAddr[i]);
			while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
		}

		USART_SendData(USART1, type);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���

		USART_SendData(USART1, len);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���

		for(u8 i = 0; i < len; i++){
			USART_SendData(USART1, Data[i]);
			while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
		}

		USART_SendData(USART1, 0x0D);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
		
		USART_SendData(USART1, 0x0A);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
	}
	if(USARTx == USART2){
		
		USART_SendData(USART2, 0xA1);
		while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
		USART_SendData(USART2, 0xA2);
		while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
		USART_SendData(USART2, 0xA3);
		while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
		USART_SendData(USART2, 0xA4);
		while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//�ȴ����ͽ���

		for(u8 i = 0; i < 8; i++){
			USART_SendData(USART2, SelfLongAddr[i]);
			while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
		}
		
		USART_SendData(USART2, type);
		while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//�ȴ����ͽ���

		USART_SendData(USART2, len);
		while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//�ȴ����ͽ���

		for(u8 i = 0; i < len; i++){
			USART_SendData(USART2, Data[i]);
			while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
		}

		USART_SendData(USART2, 0x0D);
		while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
		
		USART_SendData(USART2, 0x0A);
		while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//�ȴ����ͽ���

		
	}
}

