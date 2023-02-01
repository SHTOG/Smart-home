#include "exti.h"


void EXTI0_Init(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能 GPIOA 时钟 
    //GPIOA0初始化设置 
	GPIO_InitTypeDef    GPIO_InitStructure; 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;//A0口
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//普通输入模式 
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz 
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//下拉
    GPIO_Init(GPIOF, &GPIO_InitStructure);//初始化 GPIO
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//使能SYSCFG时钟

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);//PA0 连接到中断线0

	/* 配置EXTI_Line0 */
	EXTI_InitTypeDef   EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;//LINE0
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//中断事件
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;//上升沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;//使能LINE0
	EXTI_Init(&EXTI_InitStructure);//配置

	NVIC_InitTypeDef   NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;//外部中断0
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure);//配置
	
	  
}

void EXTI0_IRQHandler(void){
    if(EXTI_GetITStatus(EXTI_Line0)!=RESET){//判断某个线上的中断是否发生
		if(PWMval == 0){
			PWMval = 400;
		}
		else{
			PWMval = 0;
		}
    EXTI_ClearITPendingBit(EXTI_Line0); //清除 LINE 上的中断标志位
    }
}












