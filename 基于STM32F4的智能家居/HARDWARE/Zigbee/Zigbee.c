#include "Zigbee.h"



/*Begin of ȫ�ֱ���*/
u8 SelfShortAddr[] = {0xFF,0xFF};//�豸�̵�ַ:Э������ʼFFFF��������ʼ0000
u8 SelfLongAddr[8] = {0x70, 0xE4, 0x61, 0x25, 0x00, 0x4B, 0x12, 0x00};//ָ��Э�����ĳ���ַ
u8 NetFlag = 0;
/*End of ȫ�ֱ���*/

/**
  * @brief		��ʼ����Zigbee�����Ĵ��ڲ���ʼ��Zigebee���ã���Ҫ�Ǳ�֤Zigbee��������״̬���ҽ��봫��ģʽ
  * @param		bound(������)->��Zigbeeģ��Ĳ����ʱ���һ��
  * @retval	void
  */

void Zigbee_Init(u32 bound){
	delay_ms(5000);//��Zigbee������ȫ
    //���ڳ�ʼ��
	USART1_Init(bound);	//���ڳ�ʼ��������Ϊbound��Ĭ����115200��Zigbee�ĳ���Ĭ��ֵͳһ
	//��������ģʽ
	Zigbee_Change_Mode(0);
	//��ʼ����
	OpenNet();
	//��ȡ�豸��Ϣ
	Get_State();
	PFout(9) = 0;
	//����͸��ģʽ
	Zigbee_Change_Mode(1);
	PFout(10) = 0;
}
/**
  * @brief		��Zigbeeģ������磨Э���������δ�������磬����һ�������磻����ѽ������磬���ٵ��ô˺���ʱΪ��������180s��180s���ն˺�·�����ɼ��룩
  * @param		void
  * @retval		1->����򿪳ɹ�
  */

void OpenNet(){
	u8 NetStart[] = {0x55, 0x04, 0x00, 0x02, 0x00, 0x02,};//��ʼ����:Э����ִ�и���������������Ȩ��,·�ɺ��ն˽ڵ�ִ�и�����᳢�Լ���һ��Э��������������
	u8 i;
	while(NetFlag == 0){
		for(i = 0; i < 6;i++){
			USART_SendData(USART1, NetStart[i]);         //�򴮿�1��������
			while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
		}
		delay_ms(50);
	}
	NetFlag = 0;
}

/**
  * @brief		�л�Zigbeeģʽ
  * @param		ModeNum -> 0Ϊ����HEXָ��ģʽ��1Ϊ����͸��ģʽ
  * @retval	0Ϊ�ɹ�����ָ��ģʽ��1Ϊ�ɹ�����͸��ģʽ
  */


u8 Zigbee_Change_Mode(u8 modeNum){
	u8 EnterMode0[] = {0x2B, 0x2B, 0x2B};//Zigbee͸��ģʽ�л�HEXָ��ģʽ 0ΪHEXָ��ģʽ
	u8 EnterMode1[] = {0x55, 0x07, 0x00, 0x11, 0x00, 0x03, 0x00, 0x01, 0x13};//Zigbee HEXָ��ģʽ�л�͸��ģʽ 1Ϊ����͸��ģʽ
	u8 i;
	if(modeNum == 0){
		while(1){
			delay_ms(100);
			for(i = 0; i < 3;i++){
				USART_SendData(USART1, EnterMode0[i]);         //�򴮿�1��������
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
			}
			if(USART1_RX_BUF[1] == 0x03 && USART1_RX_BUF[2] == 0xFF && USART1_RX_BUF[3] == 0xFE && USART1_RX_BUF[4] == 0x01){//55 03 FF FE 01
				return 0;
			}
		}
	}
	else if(modeNum == 1){
		//����͸��ģʽ
		while(1){
			delay_ms(50);
			for(i = 0; i < 9;i++){
				USART_SendData(USART1, EnterMode1[i]);         //�򴮿�1��������
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
			}
			if(USART1_RX_BUF[1] == 0x04 && USART1_RX_BUF[2] == 0x00 && USART1_RX_BUF[3] == 0x11 && USART1_RX_BUF[4] == 0x00 && USART1_RX_BUF[5] == 0x11){//55 04 00 11 00 11 
				return 1;
			}
		}
	}
	return 2;
}

/**
  * @brief		��ѯģ�鵱ǰ״̬����ȡ������
  * @param		void
  * @retval	    1->�ɹ���ȡ״̬
  */

u8 Get_State(void){
	u8 i;
	u8 GetState[] = {0x55, 0x03, 0x00, 0x00, 0x00};//��ѯZigbeeģ�鵱ǰ״̬
	while(1){
		for(i = 0; i < 5;i++){
			USART_SendData(USART1, GetState[i]);         //�򴮿�1��������
			while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
		}
		if(USART1_RX_BUF[1] == 0x2A && USART1_RX_BUF[2] == 0x00 && USART1_RX_BUF[3] == 0x00 && USART1_RX_BUF[4] == 0x00){//55 2A 00 00 00
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
			return 1;
		}
	}
}

/**
  * @brief		����Zigbee�ķ�������
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
		}
		if(USART1_RX_BUF[1] == 0x04 && USART1_RX_BUF[2] == 0x00 && USART1_RX_BUF[3] == 0x02 && USART1_RX_BUF[4] == 0x00 && USART1_RX_BUF[5] == 0x02){//55 04 00 02 00 02
			NetFlag = 1;//�ж������Ѵ�
		}
	}
	else if(model == E180){
		
	}
}


