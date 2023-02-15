#include "usart.h"

//��ȡUSARTx->SR�ܱ���Ī������Ĵ���   
//���ջ���,���200���ֽ�.	
u8 USART1_RX_BUF[200];     
u8 USART2_RX_BUF[200];     

//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART1_RX_STA=0;       //����״̬���	
u16 USART2_RX_STA=0;       //����״̬���	

//�յ��п�Ӧ���־
u8 AckFlag = 0;


/**
  * @brief		��ʼ��IO ����1 
  * @param		bound->������
  * @retval	
  */

void USART1_Init(u32 bound){
   //GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //ʹ��GPIOAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//ʹ��USART1ʱ��
 
	//����1��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //GPIOA9����ΪUSART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10����ΪUSART1
	
	//USART1��GPIO����
 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9��GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//�ٶ�100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOA,&GPIO_InitStructure); //��ʼ��PA9��PA10

    //USART1 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;	//�շ�ģʽ
	USART_Init(USART1, &USART_InitStructure); //��ʼ������1
	
	USART_Cmd(USART1, ENABLE);  //ʹ�ܴ���1 
	USART_ClearFlag(USART1, USART_FLAG_TC);
		
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//��������ж�

	//Usart1 NVIC ����
  	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//����1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��NVIC�Ĵ���

	
}



/**
  * @brief		����1�жϷ������
  * @param		void
  * @retval		void
  */

void USART1_IRQHandler(void)
{
	u8 Res;

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		Res =USART_ReceiveData(USART1);//(USART1->DR);	//��ȡ���յ�������
		
		if((USART1_RX_STA&0x8000)==0)//����δ���
		{
			
			if(USART1_RX_BUF[0] == 0xC1){
				//������յ������Զ��崫������ݸ�ʽ
				if(USART1_RX_STA&0x4000)//���յ���0x0d
				{
					if(Res!=0x0a)USART1_RX_STA=0;//���մ���,���¿�ʼ
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
//			else if(USART1_RX_BUF[0] == 'O'){
//				USART1_RX_BUF[USART1_RX_STA&0X3FFF]=Res ;//�ѵ�ǰ���յ������ݷŵ�USART1_RX_BUF�����һλ	
//				USART1_RX_STA++;//���ݳ���++
//				if(USART1_RX_BUF[1] == 'K') USART1_RX_STA|=0x8000;//���������
//				else if(USART1_RX_STA >= 2){
//					//���մ���,���¿�ʼ
//					USART1_RX_STA=0;
//					USART1_RX_BUF[0] = 0;
//					USART1_RX_BUF[1] = 0;
//				} 
//			}
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
			if(USART1_RX_BUF[0] == 0x55) Zigbee_Analyse_Command_Data();
			//������յ������豸�䴫�������
			else if( USART1_RX_BUF[0] == 0xC1
				  && USART1_RX_BUF[1] == 0xC2
				  && USART1_RX_BUF[2] == 0xC3
				  && USART1_RX_BUF[3] == 0xC4) Analyse_Custom_Data();
			//���������ɱ�־��α���ռĴ���׼�����½���
			USART1_RX_STA=0;
			USART1_RX_BUF[0] = 0;
		}
  	}
}

void Analyse_Custom_Data(){
	u8 Ack[] = {'O','K'};
	if(USART1_RX_BUF[12] == 0xFF){ //�յ����пص�Ӧ��
		AckFlag = 1;
	}
	else if(USART1_RX_BUF[12] == 0x03){//ֻ��������Ϊ0x03�Ĳ���Ҫ����ִ��
		if(USART1_RX_BUF[14] == 0x00)		STMT_Open();
		else if(USART1_RX_BUF[14] == 0x01)	STMT_Close();
		else if(USART1_RX_BUF[14] == 0x02)	STMT_X_Pulse(0,16);
		else if(USART1_RX_BUF[14] == 0x03)	STMT_X_Pulse(1,16);
		Send_Custom_Data(0xFF,2,Ack);//ִ������п�һ��Ӧ��
	}
}

void Send_Custom_Data(u8 type, u8 len, u8* Data){
	int i;
	USART_SendData(USART1, 0xC1);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
	USART_SendData(USART1, 0xC2);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
	USART_SendData(USART1, 0xC3);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
	USART_SendData(USART1, 0xC4);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���

	for(i = 0; i < 8; i++){
		USART_SendData(USART1, SelfLongAddr[i]);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
	}

	USART_SendData(USART1, type);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���

	USART_SendData(USART1, len);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���

	for(i = 0; i < len; i++){
		USART_SendData(USART1, Data[i]);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
	}

	USART_SendData(USART1, 0x0D);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
	USART_SendData(USART1, 0x0A);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
}
