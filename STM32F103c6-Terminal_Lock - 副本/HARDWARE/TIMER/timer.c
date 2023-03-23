#include "timer.h"

u16 WaitTime = 0;//�ȴ�ʱ����С��900ʱÿ������1�������ӵ�900ʱ�ж�Ϊ�пض���
u32 MilliSecond = 0;//���Ǻ��뼶����������ÿһ�����1

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

	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE );//ʹ��TIM2�����ж�

	//TIM2 �ж����ȼ�����
	NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn; 				//��ʱ�� 2 �ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x02; 	//��ռ���ȼ� 2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; 		//��Ӧ���ȼ� 3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;				//ʹ���ж�
	NVIC_Init(&NVIC_InitStructure);								//��ʼ�� NVIC

	TIM_Cmd(TIM2, ENABLE); //ʹ�� TIM2 ����
}

void TIM3_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��
	
	//��ʱ��TIM3��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�

	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���


	TIM_Cmd(TIM3, ENABLE);  //ʹ��TIMx					 
}



//��ʱ��2�жϷ�����
//
//ÿ�봥��һ���ж�
void TIM2_IRQHandler(void){
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET){ //����ж�
		WaitTime++;
		if(WaitTime == 900){//��15������WaitTime��û�б����㣬���ж�Ϊ�пص���
			AckFlag = 0;
			while(WaitTime == 900 && AckFlag != 1){//�յ��пص�Ӧ����յ��пص�����Ӧ����˳�ѭ��
				Send_Custom_Data(0x00,2,SelfShortAddr);//�����豸��Ϣ����
				delay_ms(500);
				IWDG_Feed();//ι��
			}
			WaitTime = 0;//ˢ�µȴ�ʱ��
		}
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);  //����жϱ�־λ
	}
}



//��ʱ��3�жϷ������
void TIM3_IRQHandler(void)   //TIM3�ж�
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET){  //���TIM3�����жϷ������
        MilliSecond++;
        if(MTXKCD < MilliSecond){
            MilliSecond = 0;
            MTXKCD = 0;

        }
    	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  //���TIMx�����жϱ�־	
    }
}

