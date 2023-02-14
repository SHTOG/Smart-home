#include "Zigbee.h"


/*Begin of ȫ�ֱ���*/
u8 SelfLongAddr[8] = {0};//Zigbee�豸����ַ
u8 SelfShortAddr[2] = {0};//Zigbee�豸�̵�ַ Э������ʼ��Ϊ0xFF 0xFF���ն˳�ʼ��Ϊ0
u8 EnterModeFlag = 2;//Zigbeeģ��ģʽ�л���־λ����������ģʽ��0��͸��ģʽ��1
u8 PANID[2] = {0};//Zigbee����PANID
u8 NetFlag = 0;//�������־λ���򿪳ɹ���1
u8 GetStateFlag = 0;//��ȡģ��״̬��־λ����ȡ�ɹ���1
u8 ReadySetTargetFlag = 0;//�ɹ�����͸��Ŀ���־λ�����óɹ���1(��Э������ͬ������������һ������)
u8 SetSendTargetFlag = 0;//����͸��Ŀ���־λ����������������Ŀ��̵�ַ��������Ŀ��˿ڣ��̵�ַ���óɹ�����1���˿����óɹ�����0

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
  * @retval		void
  */

void Zigbee_Change_Mode(u8 modeNum){
	u8 EnterMode0[] = {0x2B, 0x2B, 0x2B};//Zigbee͸��ģʽ�л�HEXָ��ģʽ 0ΪHEXָ��ģʽ
	u8 EnterMode1[] = {0x55, 0x07, 0x00, 0x11, 0x00, 0x03, 0x00, 0x01, 0x13};//Zigbee HEXָ��ģʽ�л�͸��ģʽ 1Ϊ����͸��ģʽ
	u8 i;
	if(modeNum == 0){
		while(EnterModeFlag != 0){
			delay_ms(100);
			for(i = 0; i < 3;i++){
				USART_SendData(USART1, EnterMode0[i]);         //�򴮿�1��������
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
			}
		}
	}
	else if(modeNum == 1){
		//����͸��ģʽ
		while(EnterModeFlag != 1){
			delay_ms(50);
			for(i = 0; i < 9;i++){
				USART_SendData(USART1, EnterMode1[i]);         //�򴮿�1��������
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
			}
		}
	}
}

/**
  * @brief		����Ŀ��̵�ַΪ0101(��Э����)��Ŀ���ն�Ϊ01
  * @param		void
  * @retval		void
  */

void Set_Send_Target(){
	u8 i;
	u8 SetDirection[] = {0x55, 0x08, 0x00, 0x11, 0x00, 0x01, 0x00, 0x00, 0x00, 0x10, 0x55, 0x07, 0x00, 0x11, 0x00, 0x02, 0x00, 0x01, 0x12};
	while(SetSendTargetFlag != 1){
		delay_ms(10);
		for(i = 0; i < 10;i++){
			USART_SendData(USART1, SetDirection[i]);         //�򴮿�1��������
			while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
		}
	}
	while(SetSendTargetFlag != 0){
		delay_ms(10);
		for(i = 10; i < 19; i++){
			USART_SendData(USART1, SetDirection[i]);         //�򴮿�1��������
			while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
		}
	}
	delay_ms(10);
	ReadySetTargetFlag = 1;
}



/**
  * @brief		��Zigbeeģ������磨�նˣ�����һ��ָ��PANID���������磩
  * @param		Direction_PANID->���ֽڵ�ʮ������������ʾĿ�������PANID
  * @retval	    1->����򿪳ɹ�
  */

void OpenNet(u16 Direction_PANID){
	u8 i;
	u8 SetPANID[] = {0x55, 0x05, 0x00, 0x08, 0x01, 0x01, 0x08};
	u8 NetStart[] = {0x55, 0x04, 0x00, 0x02, 0x00, 0x02,};
		u8 GetState[] = {0x55, 0x03, 0x00, 0x00, 0x00};//��ѯZigbeeģ�鵱ǰ״̬
	while(PANID[0] != Direction_PANID/256 || PANID[1] != Direction_PANID%256){
		delay_ms(100);
		//����PANIDΪ01 01(���ﲻ���ж��Ƿ�д�룬��Ϊ����Ѿ��������Ļ���д��ʧ�ܵģ�����100ms�����������������)
		for(i = 0; i < 7; i++){
			USART_SendData(USART1, SetPANID[i]);         //�򴮿�1��������
			while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
		}
		while(NetFlag != 1){
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
		for(u8 i = 0; i < 5;i++){
			USART_SendData(USART1, GetState[i]);         //�򴮿�1��������
			while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
		}
		delay_ms(50);
		GetStateFlag = 0;
	}
	
}

/**
  * @brief		��ѯģ�鵱ǰ״̬����ȡ������
  * @param		void
  * @retval	    1->�ɹ���ȡ״̬
  */

void Get_State(void){
	u8 i;
	u8 GetState[] = {0x55, 0x03, 0x00, 0x00, 0x00};//��ѯZigbeeģ�鵱ǰ״̬
	while(GetStateFlag != 1){
		for(i = 0; i < 5;i++){
			USART_SendData(USART1, GetState[i]);         //�򴮿�1��������
			while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
		}
	}
	GetStateFlag = 0;
}

/**
  * @brief		��Э���������Լ��ĳ��̵�ַ
  * @param		void
  * @retval	    1->���ͳɹ���Э������Ӧ��
  */

void Upload_Address(void){
	while(1){
		AckFlag = 0;
		Send_Custom_Data(0x00,2,SelfShortAddr);
		delay_ms(500);//�������ݽ����ӳ�,����Ƶ�����͵����п�����ӵ��
		if(AckFlag == 1){
			AckFlag = 0;
			return ;
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
			NetFlag = 1;//�ж������Ѵ�
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
