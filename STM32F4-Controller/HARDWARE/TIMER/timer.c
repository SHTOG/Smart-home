#include "timer.h"

/**

  * @brief    ͨ�ö�ʱ��2�жϳ�ʼ��
  * @param    arr->�Զ���װֵ
  * @param	  psc->ʱ��Ԥ��Ƶ��
  * @retval   void
  */

void TIM2_Int_Init(u16 arr,u16 psc){
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE); ///ʹ�� TIM2 ʱ��
	
	//��ʼ����ʱ������
	TIM_TimeBaseStructure.TIM_Prescaler = psc;					//��ʱ����Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_Period = arr;						//�Զ���װ��ֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;		//����ʱ�ӷ�Ƶ����
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE );//ʹ��TM2�����ж�
	
	//TIM3 �ж����ȼ�����
	NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn; 				//��ʱ�� 2 �ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x02; 	//��ռ���ȼ� 2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; 		//��Ӧ���ȼ� 3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;				//ʹ���ж�
	NVIC_Init(&NVIC_InitStructure);								//��ʼ�� NVIC
	
	TIM_Cmd(TIM2, ENABLE); //ʹ�� TIM2 ����
}


/**

  * @brief    ͨ�ö�ʱ��3�жϳ�ʼ��
  * @param    arr->�Զ���װֵ
  * @param	  psc->ʱ��Ԥ��Ƶ��
  * @retval   void
  */
 
void TIM3_Int_Init(u16 arr,u16 psc){
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE); ///ʹ�� TIM3 ʱ��
	
	//��ʼ����ʱ������
	TIM_TimeBaseStructure.TIM_Prescaler = psc;					//��ʱ����Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_Period = arr;						//�Զ���װ��ֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;		//����ʱ�ӷ�Ƶ����
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE );//ʹ��TM3�����ж�
	
	//TIM3 �ж����ȼ�����
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; 				//��ʱ�� 3 �ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; 	//��ռ���ȼ� 1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x02; 		//��Ӧ���ȼ� 2
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;				//ʹ���ж�
	NVIC_Init(&NVIC_InitStructure);								//��ʼ�� NVIC
	
	TIM_Cmd(TIM3, ENABLE); //ʹ�� TIM3 ����
}

//��ʱ��2�жϷ�����
//��Ҫ��¼��������ʱ��
//ÿ���봥��һ���ж�
void TIM2_IRQHandler(void){
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET){ //����ж�
		static u8 Second = 0;//�뼶������
		if(Second == 120){
			if(CheckByOnlineFlag(DeviceList)){
				Zigbee_Change_Mode(0);
				OpenNet();
				Zigbee_Change_Mode(1);
			}
			Second = 0;
		}
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);  //����жϱ�־λ
	}
}

//��ʱ��3�жϷ�����
//��Ҫ��¼��������ʱ��
//ÿ���봥��һ���ж�
void TIM3_IRQHandler(void){
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET){ //����ж�
		static u16 MilliSecond = 0;//���뼶������
		static u8 Second = 0;//�뼶������
		static u8 HalfSecond = 0;
		static u8 Minute = 0;//�ּ�������
		MilliSecond++;
		if(MilliSecond == 1000){
			Second++;//��������
			HalfSecond++;
			
			if(Second == 64){
				Minute++;
				if(Minute == 10){
					BootedTimeFlag = 1;
				}
				Second = 0;
			}
			MilliSecond = 0;
		}
		if(BootedTimeFlag == 0 && HalfSecond == 30){
			//ÿ30����APP�����������ݲ����������EEPROM
			//��������λ�ø��������͵�APP�ˡ�
			AT24CXX_Save_List(0,DeviceList);
			HalfSecond = 0;
		}
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //����жϱ�־λ
	}
}
