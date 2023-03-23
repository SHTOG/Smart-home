#include "usart.h"

//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART1_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART1_RX_STA=0;       //����״̬���	  
  
//�յ��п�Ӧ���־
u8 AckFlag = 0;

/**
  * @brief		��ʼ��IO ����1 
  * @param		bound->������
  * @retval	
  */

void USART1_Init(u32 bound)
{
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��

	//USART1_TX   GPIOA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9

	//USART1_RX	  GPIOA.10��ʼ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10  

	//USART ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART1, &USART_InitStructure); //��ʼ������1
	USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ���1 

	//Usart1 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//�����ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���

	USART_ClearFlag(USART1, USART_FLAG_TC);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�������ڽ����ж�

}

/**
  * @brief		����1�жϷ������
  * @param		void
  * @retval		void
  */

void USART1_IRQHandler(void){
	u8 Res;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET){  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
		
		Res =USART_ReceiveData(USART1);	//��ȡ���յ�������
		if((USART1_RX_STA&0x8000)==0){//����δ���
			if(USART1_RX_BUF[0] == 0xC1){
				//������յ������Զ��崫������ݸ�ʽ
				if(USART1_RX_STA&0x4000)//���յ���0x0d
				{
					if(Res!=0x0a){
						USART1_RX_STA &= 0xBFFF;
						USART1_RX_BUF[USART1_RX_STA&0X3FFF]=0X0D ;//��0x0D�ŵ�USART1_RX_BUF�ĵ����ڶ�λ
						USART1_RX_STA++;//���ݳ���++
						USART1_RX_BUF[USART1_RX_STA&0X3FFF]=Res ;//�ѵ�ǰ���յ������ݷŵ�USART1_RX_BUF�����һλ
						USART1_RX_STA++;//���ݳ���++
					}
					else USART1_RX_STA|=0x8000;	//��������� 
				}
				else //��û�յ�0X0D
				{
					if(Res==0x0d)USART1_RX_STA|=0x4000;
					else
					{
						USART1_RX_BUF[USART1_RX_STA&0X3FFF]=Res ;//�ѵ�ǰ���յ������ݷŵ�USART1_RX_BUF�����һλ	
						USART1_RX_STA++;//���ݳ���++
						if(USART1_RX_STA>(USART_REC_LEN-1))USART1_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
					}
				}
			}
			else if(USART1_RX_BUF[0] == 0x55){
				//����յ��ĵ�һ���ֽ�Ϊ0x55���ж�ΪZigbee������
				USART1_RX_BUF[USART1_RX_STA&0X3FFF]=Res ;//�ѵ�ǰ���յ������ݷŵ�USART1_RX_BUF�����һλ	
				USART1_RX_STA++;//���ݳ���++
				if(USART1_RX_BUF[1] + 2 == USART1_RX_STA) USART1_RX_STA|=0x8000;//���������
				else if(USART1_RX_BUF[1] + 2 < USART1_RX_STA){
					//���մ���,���¿�ʼ
					USART1_RX_STA=0;
					USART1_RX_BUF[0] = 0;
					USART1_RX_BUF[1] = 0;
				}
			}
			else if(USART1_RX_BUF[0] == 'B'){
				if(Res != 'U' && Res != 'S' && Res != 'Y'){
					//���մ���,���¿�ʼ
					USART1_RX_STA=0;
					USART1_RX_BUF[1] = 0;
					USART1_RX_BUF[0]=Res ;//�ѵ�ǰ���յ������ݷŵ�USART1_RX_BUF�ĵ�һλ
					USART1_RX_STA++;//���ݳ���++
				}					
				else {					
					USART1_RX_BUF[USART1_RX_STA&0X3FFF]=Res;
					USART1_RX_STA++;//���ݳ���++
				}
				if(USART1_RX_STA == 4 && EnterModeFlag == 0) EnterModeFlag = 1;
				else if(USART1_RX_STA > 4){
					//���մ���,���¿�ʼ
					USART1_RX_STA=0;
					USART1_RX_BUF[1] = 0;
					USART1_RX_BUF[0]=Res ;//�ѵ�ǰ���յ������ݷŵ�USART1_RX_BUF�ĵ�һλ
					USART1_RX_STA++;//���ݳ���++
				}
					
			}
			else if(USART1_RX_BUF[0] == 0){
				//�����һ���ֽ���0�����ж�Ϊ�տ�ʼ���յ�һ�ֽ�����
				USART1_RX_BUF[1] = 0;//������λZigbee�����ʱ����
				USART1_RX_BUF[0]=Res ;//�ѵ�ǰ���յ������ݷŵ�USART1_RX_BUF�ĵ�һλ
				USART1_RX_STA++;//���ݳ���++
			}
			else{
				//���մ���,���¿�ʼ
				USART1_RX_STA=0;
				USART1_RX_BUF[0] = 0;
				USART1_RX_BUF[1] = 0;
				USART1_RX_BUF[0]=Res ;//�ѵ�ǰ���յ������ݷŵ�USART1_RX_BUF�ĵ�һλ
				USART1_RX_STA++;//���ݳ���++
			}
		}
		if(USART1_RX_STA&0x8000){
			//�����ڽ�����һ������
			//������ڽ��յ���������Zigbee������
			if(USART1_RX_BUF[0] == 0x55)		Zigbee_Analyse_Command_Data();
			//������յ������豸�䴫�������
			else if( USART1_RX_BUF[0] == 0xC1
				  && USART1_RX_BUF[1] == 0xC2
				  && USART1_RX_BUF[2] == 0xC3
				  && USART1_RX_BUF[3] == 0xC4) 	Analyse_Custom_Data();
			//���������ɱ�־��α���ռĴ���׼�����½���
			USART1_RX_STA=0;
			USART1_RX_BUF[0] = 0;
		}
     } 
}

void Analyse_Custom_Data(){
	u8 Ack[] = {'O','K'};
	//�������ó�
	u16 len = (USART1_RX_STA&0X3FFF) - 4;//��ȡ�������ݳ���
	u8 i;
	u8 teaKey[] = {'N','Z','o','k','G','u','z','T','n','F','s','6','D','C','H','4'};
	u8* Data = (u8*)malloc(sizeof(u8) * len);
	for(i = 0; i < len; i++){
		Data[i] = USART1_RX_BUF[4 + i];
	}
	//��ʼ��������
	decrypt(Data,len,teaKey);
	if(Data[8] == 0xFF){//�յ����豸Ӧ������
		if(Data[9] == 0x00){//�п�������Ӧ��
			Send_Custom_Data(0xFF,2,Ack);//Ӧ��
		}
		else if(Data[10] == 0x4F && Data[11] == 0x4B){//�յ����пص�Ӧ��
			AckFlag = 1;
		}
		else if(Data[10] == 0x00 && Data[11] == 0x00) {//�ܾ�����
			APPJudgeFlag = 2;
		}
		else if(Data[10] == 0x00 && Data[11] == 0x01) {//ͬ������
			APPJudgeFlag = 1;
		}
	}
//	else if(Data[8] == 0xXX){//��ֲʱ���޸�0xXXΪ��Ӧ���豸�룬����else��д���յ�ָ��ָ������ִ�е�����
//		Send_Custom_Data(0xXX,2,Ack);//��仰���ɻ�ȱ���յ��пصĿ���ָ���Ӧ�����ظ��������п��Լ�ȷʵ�յ���
//		xxxxxx;
//	}
	free(Data);
}

/**
  * @brief		�����ݷ�װ��˽��Э�鲢����
  * @param		type:�����豸��
  * @param		len :��Ч���ݳ���
  * @param		Data :��Ч��������ָ�루������豸��Ϣ���Dataָ��ָ������̵�ַ��һλ)
  * @retval		void
  */
void Send_Custom_Data(u8 type, u8 len, u8* Data){
	u8 i = 0,j = 0,newDataLen = 10+len;
	u8* newData;
	u8 teaKey[] = {'N','Z','o','k','G','u','z','T','n','F','s','6','D','C','H','4'};
	while(newDataLen % 8 != 0){//�ѳ�֡ͷ֡β���ݴ�С���㵽��ռ�ڴ�Ϊ8�ֽڵı���
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
		newData[i] = 0x01;//����һ��Ҫ�޸�Ϊ��Ӧ�ն˵��豸������
		i++;
		len--;
	}
	for(; j<len ;i++,j++){
		newData[i] = Data[j];
	}
	for(; i < newDataLen; i++){
		newData[i] = 1;
	}

	//����
	encrypt(newData,newDataLen,teaKey);

	USART_SendData(USART1, 0xC1);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
	USART_SendData(USART1, 0xC2);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
	USART_SendData(USART1, 0xC3);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
	USART_SendData(USART1, 0xC4);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���

	for(i = 0; i < newDataLen; i++){
		USART_SendData(USART1, newData[i]);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
	}

	USART_SendData(USART1, 0x0D);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
	USART_SendData(USART1, 0x0A);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
	free(newData);
}

