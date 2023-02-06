#include "Zigbee.h"


/*Begin of ȫ�ֱ���*/
u8 SelfLongAddr[8] = {0};//Zigbee�豸����ַ
u8 SelfShortAddr[2] = {0};//Zigbee�豸�̵�ַ Э������ʼ��Ϊ0xFF 0xFF���ն˳�ʼ��Ϊ0
u8 PANID[2] = {0};
u8 NetFlag = 0;
/*End of ȫ�ֱ���*/

/**
  * @brief		��ʼ����Zigbee�����Ĵ��ڲ���ʼ��Zigebee���ã���Ҫ�Ǳ�֤Zigbee��������״̬���ҽ��봫��ģʽ
  * @param		bound(������)->��Zigbeeģ��Ĳ����ʱ���һ�£�Ĭ��115200��
  * @retval	void
  */
  
void Zigbee_Init(u32 bound){
	delay_ms(5000);//��Zigbee������ȫ
    
	//���ڳ�ʼ��
	USART1_Init(bound);	//���ڳ�ʼ��������Ϊbound��Ĭ����115200��Zigbee�ĳ���Ĭ��ֵͳһ
	
	//��hex����ģʽ
	Zigbee_Change_Mode(0);
	//ָ��͸��Ŀ��ΪЭ����
	Set_Send_Target();
	//��ʼ����(ȷ������PANIDΪ0101�ľ���),����ȡ����ַ�Ͷ̵�ַ
	OpenNet(0x0101);
	
	//����͸��ģʽ
	Zigbee_Change_Mode(1);
	
	//���ͳ��̵�ַ��֪�п��Լ�������,�յ�Ӧ����˳�
	Upload_Address();
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
	return 0;
}

/**
  * @brief		����Ŀ��̵�ַΪ0101(��Э����)��Ŀ���ն�Ϊ01
  * @param		void
  * @retval		1->���óɹ�
  */

u8 Set_Send_Target(){
	u8 i;
	u8 SetDirection[] = {0x55, 0x08, 0x00, 0x11, 0x00, 0x01, 0x00, 0x00, 0x00, 0x10, 0x55, 0x07, 0x00, 0x11, 0x00, 0x02, 0x00, 0x01, 0x12};
	while(1){
		delay_ms(10);
		for(i = 0; i < 10;i++){
			USART_SendData(USART1, SetDirection[i]);         //�򴮿�1��������
			while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
		}
		if(USART1_RX_BUF[1] == 0x04 && USART1_RX_BUF[2] == 0x00 && USART1_RX_BUF[3] == 0x11 && USART1_RX_BUF[4] == 0x00 && USART1_RX_BUF[5] == 0x11){//55 04 00 11 00 11 
			break;
		}
	}
	while(1){
		delay_ms(10);
		for(i = 10; i < 19; i++){
			USART_SendData(USART1, SetDirection[i]);         //�򴮿�1��������
			while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
		}
		if(USART1_RX_BUF[1] == 0x04 && USART1_RX_BUF[2] == 0x00 && USART1_RX_BUF[3] == 0x11 && USART1_RX_BUF[4] == 0x00 && USART1_RX_BUF[5] == 0x11){//55 04 00 11 00 11 
			return 1;
		}
	}
}



/**
  * @brief		��Zigbeeģ������磨�նˣ�����һ��ָ��PANID���������磩
  * @param		Direction_PANID->���ֽڵ�ʮ������������ʾĿ�������PANID
  * @retval	    1->����򿪳ɹ�
  */

u8 OpenNet(u16 Direction_PANID){
	u8 i;
	u8 SetPANID[] = {0x55, 0x05, 0x00, 0x08, 0x01, 0x01, 0x08};
	u8 NetStart[] = {0x55, 0x04, 0x00, 0x02, 0x00, 0x02,};
	while(1){
		delay_ms(100);
		//����PANIDΪ01 01(���ﲻ���ж��Ƿ�д�룬��Ϊ����Ѿ��������Ļ���д��ʧ�ܵģ�����100ms�����������������)
		for(i = 0; i < 7; i++){
			USART_SendData(USART1, SetPANID[i]);         //�򴮿�1��������
			while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
		}
		while(NetFlag == 0){
			for(i = 0; i < 6;i++){
				USART_SendData(USART1, NetStart[i]);         //�򴮿�1��������
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
			}
			delay_ms(50);
		}
		NetFlag = 0;
		delay_ms(1000);//�ȸ�һ��������
		//������Ҫ��ȡPANID,�����ȡ���ˣ���ô�ͱ�ʾ�ɹ�������Э����Ҳ���ߣ�
//		Get_State();//���ﲻ��ѭ�������Բ��ܵ����������
		u8 GetState[] = {0x55, 0x03, 0x00, 0x00, 0x00};//��ѯZigbeeģ�鵱ǰ״̬
		for(u8 i = 0; i < 5;i++){
			USART_SendData(USART1, GetState[i]);         //�򴮿�1��������
			while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
		}
		delay_ms(50);
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
		}
		if(PANID[0] == Direction_PANID/256 && PANID[1] == Direction_PANID%256){
			return 1;
		}
	}
	
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
			PANID[0] = USART1_RX_BUF[15];
			PANID[1] = USART1_RX_BUF[16];
			SelfShortAddr[0] = USART1_RX_BUF[17];
			SelfShortAddr[1] = USART1_RX_BUF[18];
			return 1;
		}
	}
}

/**
  * @brief		��Э���������Լ��ĳ��̵�ַ
  * @param		void
  * @retval	    1->���ͳɹ���Э������Ӧ��
  */

u8 Upload_Address(void){
	while(1){
		Send_Custom_Data(0x00,2,SelfShortAddr);
		delay_ms(500);//�������ݽ����ӳ�,����Ƶ�����͵����п�����ӵ��
		if(USART1_RX_BUF[1] == 0xC2 && USART1_RX_BUF[2] == 0xC3 && USART1_RX_BUF[3] == 0xC4 && USART1_RX_BUF[12] == 0xFF && USART1_RX_BUF[13] == 0x02 && USART1_RX_BUF[14] == 0x4F && USART1_RX_BUF[15] == 0x4B){//C1 C2 C3 C4 70 E4 61 25 00 4B 12 00 FF 02 4F 4B 0D 0A
			return 1;
		}
	}
}


