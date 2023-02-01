#include "timer.h"


//PWM����
u16 PWMval = 0;

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

  * @brief    ͨ�ö�ʱ��3���PWM��ʼ��(PA6)
  * @param    arr->�Զ���װֵ��psc->ʱ��Ԥ��Ƶ��
  * @retval   void
  */
void TIM3_PWM_Init(u32 arr,u32 psc){
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE); //TIM3 ʱ��ʹ��  
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); 	//ʹ��GPIOAʱ��	
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource6,GPIO_AF_TIM3); //GPIOA6 ����Ϊ��ʱ��3

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;           //GPIOA6
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//�ٶ�100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //����
	GPIO_Init(GPIOA,&GPIO_InitStructure);              //��ʼ��PA6

	TIM_TimeBaseStructure.TIM_Prescaler=psc;  //��ʱ����Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_Period=arr;   //�Զ���װ��ֵ
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);//��ʼ����ʱ��3

	//��ʼ��TIM3 Channel1 PWMģʽ	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //�������:TIM����Ƚϼ��Ե�
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM1 4OC1

	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);  //ʹ��TIM3��CCR1�ϵ�Ԥװ�ؼĴ���

	TIM_ARRPreloadConfig(TIM3,ENABLE);//ARPEʹ��

	TIM_Cmd(TIM3, ENABLE);  //ʹ��TIM3
}
	
//��ʱ��2�жϷ�����
//��Ҫ��¼��������ʱ��
//ÿ���봥��һ���ж�
// void TIM2_IRQHandler(void){
// 	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET){ //����ж�
// 		static u8 Second = 0;//�뼶������
// 		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);  //����жϱ�־λ
// 	}
// }
