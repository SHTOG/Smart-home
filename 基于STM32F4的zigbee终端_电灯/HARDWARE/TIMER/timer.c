#include "timer.h"


//PWM脉宽
u16 PWMval = 100;

/**

  * @brief    通用定时器3中断初始化
  * @param    arr->自动重装值；psc->时钟预分频数
  * @retval   void
  */

void TIM3_Int_Init(u16 arr,u16 psc){
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE); ///使能 TIM3 时钟
	
	//初始化定时器参数
	TIM_TimeBaseStructure.TIM_Prescaler = psc;					//定时器分频
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//向上计数模式
	TIM_TimeBaseStructure.TIM_Period = arr;						//自动重装载值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;		//设置时钟分频因子
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE );//使能TM3更新中断
	
	//TIM3 中断优先级设置
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; 				//定时器 3 中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; 	//抢占优先级 1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; 		//响应优先级 3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;				//使能中断
	NVIC_Init(&NVIC_InitStructure);								//初始化 NVIC
	
	TIM_Cmd(TIM3, ENABLE); //使能 TIM3 外设
}

//定时器3中断服务函数
void TIM3_IRQHandler(void){
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET){ //溢出中断
		LED1=!LED1;//DS1翻转
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //清除中断标志位
	}
}

/**

  * @brief    通用定时器3输出PWM初始化(PF0)
  * @param    arr->自动重装值；psc->时钟预分频数
  * @retval   void
  */
void TIM3_PWM_Init(u32 arr,u32 psc){
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE); //TIM3 时钟使能  
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); 	//使能GPIOF时钟	
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource6,GPIO_AF_TIM3); //GPIOA6 复用为定时器3

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;           //GPIOF9
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure);              //初始化PF9

	TIM_TimeBaseStructure.TIM_Prescaler=psc;  //定时器分频
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseStructure.TIM_Period=arr;   //自动重装载值
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);//初始化定时器3

	//初始化TIM3 Channel1 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //输出极性:TIM输出比较极性低
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM1 4OC1

	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);  //使能TIM3在CCR1上的预装载寄存器

	TIM_ARRPreloadConfig(TIM3,ENABLE);//ARPE使能

	TIM_Cmd(TIM3, ENABLE);  //使能TIM3
}
	
