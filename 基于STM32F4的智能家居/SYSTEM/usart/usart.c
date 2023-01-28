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
 * @brief		��ʼ��IO ����2
 * @param		bound->������
 * @retval	
 */

void USART2_Init(u32 bound){
  	//����һ�½ṹ��
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //ʹ��GPIOAʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//ʹ��USART2ʱ��

	//USART2��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2); //GPIOA2����ΪUSART2
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2); //GPIOA3����ΪUSART2
	
	//USART2��GPIO����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; //GPIOA2��GPIOA3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//�ٶ�100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOA,&GPIO_InitStructure); //��ʼ��PA2,PA3


   //USART2 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART2, &USART_InitStructure); //��ʼ������2
	
	USART_Cmd(USART2, ENABLE);  //ʹ�ܴ���2
	USART_ClearFlag(USART2, USART_FLAG_TC);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//��������ж�

	//Usart2 NVIC ����
 	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;//����2�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;//��ռ���ȼ�0��������ȼ���
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =2;		//�����ȼ�2
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
	u8 Res,ReciveITEnable = 1;

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET && ReciveITEnable )  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		Res =USART_ReceiveData(USART1);//(USART1->DR);	//��ȡ���յ�������
		
		if((USART1_RX_STA&0x8000)==0)//����δ���
		{
			
			if(USART1_RX_BUF[0] == 0xC1){
				GPIO_ResetBits(GPIOF,GPIO_Pin_9);
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
			else if(USART1_RX_BUF[0] == 'O'){
				USART1_RX_BUF[USART1_RX_STA&0X3FFF]=Res ;//�ѵ�ǰ���յ������ݷŵ�USART1_RX_BUF�����һλ	
				USART1_RX_STA++;//���ݳ���++
				if(USART1_RX_BUF[1] == 'K') USART1_RX_STA|=0x8000;//���������
				else if(USART1_RX_STA >= 2){
					//���մ���,���¿�ʼ
					USART1_RX_STA=0;
					USART1_RX_BUF[0] = 0;
					USART1_RX_BUF[1] = 0;
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
				USART1_RX_BUF[1] = 0;
				USART1_RX_BUF[2] = 0;
				USART1_RX_BUF[3] = 0;
				USART1_RX_BUF[0]=Res ;//�ѵ�ǰ���յ������ݷŵ�USART1_RX_BUF�ĵ�һλ
				USART1_RX_STA++;//���ݳ���++
			}
			
		}  
		if(USART1_RX_STA&0x8000){
			//�����ڽ�����һ������
			ReciveITEnable = 0;//�رմ���1�����ж�
			//������ڽ��յ���������Zigbee������
			if(USART1_RX_BUF[0] == 0x55) Zigbee_Analyse_Command_Data();
			//������յ������豸�䴫�������
			else if( USART1_RX_BUF[0] == 0xC1
			 && USART1_RX_BUF[1] == 0xC2
			 && USART1_RX_BUF[2] == 0xC3
			 && USART1_RX_BUF[3] == 0xC4) {Analyse_Custom_Data();}
			//���������ɱ�־��α���ռĴ���׼�����½���
			USART1_RX_STA=0;
			USART1_RX_BUF[0] = 0;
			ReciveITEnable = 1;//�򿪴���1�����ж�
		}
  	} 
} 

/**
  * @brief		����2�жϷ������
  * @param		void
  * @retval		void
  */
void USART2_IRQHandler(void){
	u8 Res,ReciveITEnable = 1;
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET && ReciveITEnable )  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		Res =USART_ReceiveData(USART2);//(USART2->DR);	//��ȡ���յ�������
		BEEP_Test(100);
		if(Res == 1){
			u8* data = (u8*)malloc(sizeof(u8)*1);
			data[0] = 1;
			Send_Custom_Data(USART1,0x01,1,data);//�����ã�ָʾmain����ִ����һ��
		}
		
		// if((USART2_RX_STA&0x8000)==0)//����δ���
		// {
			
		// 	if(USART2_RX_BUF[0] == 0xC1){
		// 		GPIO_ResetBits(GPIOF,GPIO_Pin_9);
		// 		//������յ������Զ��崫������ݸ�ʽ
		// 		if(USART2_RX_STA&0x4000)//���յ���0x0d
		// 		{
		// 			if(Res!=0x0a)USART2_RX_STA=0;//���մ���,���¿�ʼ
		// 			else USART2_RX_STA|=0x8000;	//��������� 
		// 		}
		// 		else //��û�յ�0X0D
		// 		{
		// 			if(Res==0x0d)USART2_RX_STA|=0x4000;
		// 			else
		// 			{
		// 				USART2_RX_BUF[USART2_RX_STA&0X3FFF]=Res ;//�ѵ�ǰ���յ������ݷŵ�USART2_RX_BUF�����һλ	
		// 				USART2_RX_STA++;//���ݳ���++
		// 				if(USART2_RX_STA>(USART_REC_LEN-1))USART2_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
		// 			}
		// 		}
		// 	}
		// 	else if(USART2_RX_BUF[0] == 0x55){
		// 		//����յ��ĵ�һ���ֽ�Ϊ0x55���ж�ΪZigbee������
		// 		USART2_RX_BUF[USART2_RX_STA&0X3FFF]=Res ;//�ѵ�ǰ���յ������ݷŵ�USART2_RX_BUF�����һλ	
		// 		USART2_RX_STA++;//���ݳ���++
		// 		if(USART2_RX_BUF[1] + 2 == USART2_RX_STA) USART2_RX_STA|=0x8000;//���������
		// 		else if(USART2_RX_BUF[1] + 2 < USART2_RX_STA){
		// 			//���մ���,���¿�ʼ
		// 			USART2_RX_STA=0;
		// 			USART2_RX_BUF[0] = 0;
		// 			USART2_RX_BUF[1] = 0;
		// 		}
		// 	}
		// 	else if(USART2_RX_BUF[0] == 'O'){
		// 		USART2_RX_BUF[USART2_RX_STA&0X3FFF]=Res ;//�ѵ�ǰ���յ������ݷŵ�USART2_RX_BUF�����һλ	
		// 		USART2_RX_STA++;//���ݳ���++
		// 		if(USART2_RX_BUF[1] == 'K') USART2_RX_STA|=0x8000;//���������
		// 		else if(USART2_RX_STA >= 2){
		// 			//���մ���,���¿�ʼ
		// 			USART2_RX_STA=0;
		// 			USART2_RX_BUF[0] = 0;
		// 			USART2_RX_BUF[1] = 0;
		// 		} 
		// 	}
		// 	else if(USART2_RX_BUF[0] == 0){
		// 		//�����һ���ֽ���0�����ж�Ϊ�տ�ʼ���յ�һ�ֽ�����
		// 		USART2_RX_BUF[1] = 0;//������λZigbee�����ʱ����
		// 		USART2_RX_BUF[0]=Res ;//USART2_RX_BUF
		// 		USART2_RX_STA++;//���ݳ���++
		// 	}
		// 	else{
		// 		//���յ��˴�������ݿ�ͷ,���½���
		// 		USART2_RX_STA=0;
		// 		USART2_RX_BUF[0] = 0;
		// 		USART2_RX_BUF[1] = 0;
		// 	}
			
		// }  
		// if(USART2_RX_STA&0x8000){
		// 	//�����ڽ�����һ������
		// 	ReciveITEnable = 0;//�رմ���1�����ж�
		// 	//������ڽ��յ���������Zigbee������
		// 	if(USART2_RX_BUF[0] == 0x55) Zigbee_Analyse_Command_Data();
		// 	//������յ������豸�䴫�������
		// 	else if( USART2_RX_BUF[0] == 0xC1
		// 	 && USART2_RX_BUF[1] == 0xC2
		// 	 && USART2_RX_BUF[2] == 0xC3
		// 	 && USART2_RX_BUF[3] == 0xC4) {Analyse_Custom_Data();}
		// 	//���������ɱ�־��α���ռĴ���׼�����½���
		// 	USART2_RX_STA=0;
		// 	USART2_RX_BUF[0] = 0;
		// 	ReciveITEnable = 1;//�򿪴���1�����ж�
		// }
 	} 
}


