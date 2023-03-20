#include "timer.h"

u16 MilliSecond = 0;//���뼶������
u8 Second = 0;//�뼶������
u8 Minute = 0;//�ּ�������
u8 WaitTime = 0xFF;//�뼶�ȴ�Ӧ��ʱ��,����ʱ��ʼ��ʱ,�Ƶ�255ֹͣ
u8 EspWaitTime = 5;//�뼶�ȴ�Ӧ��ʱ��,����ʱ��ʼ��ʱ,�Ƶ�5ֹͣ

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
	TIM_ClearITPendingBit(TIM2,TIM_IT_Update);//����ж�λ
	//TIM3 �ж����ȼ�����
	NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn; 				//��ʱ�� 2 �ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x02; 	//��ռ���ȼ� 2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x02; 		//��Ӧ���ȼ� 2
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
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);//����ж�λ
	//TIM3 �ж����ȼ�����
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; 				//��ʱ�� 3 �ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x03; 	//��ռ���ȼ� 3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; 		//��Ӧ���ȼ� 3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;				//ʹ���ж�
	NVIC_Init(&NVIC_InitStructure);								//��ʼ�� NVIC
	
	TIM_Cmd(TIM3, ENABLE); //ʹ�� TIM3 ����
}


//��ʱ��2�жϷ�����
//�ṩϵͳʱ�����
//ÿ���봥��һ���ж�
void TIM2_IRQHandler(void){
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET){ //����ж�
		MilliSecond++;
		if(MilliSecond == 1000){
			Second++;//��������
			if(Second == 60){
				Minute++;//��������
				UpdateWaitTime++;
				if(Minute == 60){
					Minute = 0;
				}
				Second = 0;
			}
			if(WaitTime < 0xFF) WaitTime++;
			if(EspWaitTime < 5) EspWaitTime++;
			if(APPOpenNetCountDown > 0) APPOpenNetCountDown--;
			MilliSecond = 0;
		}
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);  //����жϱ�־λ
	}
}


//��ʱ��3�жϷ�����
//�����趨ʱ��ִ��ָ������
//ÿ�봥��һ���ж�
u8 UpdateWaitTime = 0;
void TIM3_IRQHandler(void){
	u8 testData[3] = {23,3,90};//23.3�桢90%ʪ��
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET){ //����ж�
		if(UpdateWaitTime == 10){//����ʱ�Ϳ�����ÿʮ����
			UpdateDeviceList(DeviceList);//���������������ն�����״̬
			UpdateWaitTime = 0;
		}
		if(DeviceList->next != NULL && Second % 5 == 0 && PrintDeviceListFlag == 0)Send_Custom_Data(USART2,0x02,3,testData);//����ʪ�����ݷ��͵�APP
		
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //����жϱ�־λ
	}
}
