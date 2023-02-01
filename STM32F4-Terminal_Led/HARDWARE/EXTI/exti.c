#include "exti.h"


void EXTI0_Init(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//ʹ�� GPIOA ʱ�� 
    //GPIOA0��ʼ������ 
	GPIO_InitTypeDef    GPIO_InitStructure; 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;//A0��
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//��ͨ����ģʽ 
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//���� 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz 
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
    GPIO_Init(GPIOF, &GPIO_InitStructure);//��ʼ�� GPIO
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//ʹ��SYSCFGʱ��

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);//PA0 ���ӵ��ж���0

	/* ����EXTI_Line0 */
	EXTI_InitTypeDef   EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;//LINE0
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//�ж��¼�
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;//�����ش���
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;//ʹ��LINE0
	EXTI_Init(&EXTI_InitStructure);//����

	NVIC_InitTypeDef   NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;//�ⲿ�ж�0
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;//�����ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
	NVIC_Init(&NVIC_InitStructure);//����
	
	  
}

void EXTI0_IRQHandler(void){
    if(EXTI_GetITStatus(EXTI_Line0)!=RESET){//�ж�ĳ�����ϵ��ж��Ƿ���
		if(PWMval == 0){
			PWMval = 400;
		}
		else{
			PWMval = 0;
		}
    EXTI_ClearITPendingBit(EXTI_Line0); //��� LINE �ϵ��жϱ�־λ
    }
}












