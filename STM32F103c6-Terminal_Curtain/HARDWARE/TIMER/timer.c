#include "timer.h"

u16 MilliSecond = 0;//毫秒级计数器
u8 WaitTime = 0;//等待时长，每秒增加1

/**
  * @brief    通用定时器2中断初始化
  * @param    arr->自动重装值
  * @param	  psc->时钟预分频数
  * @retval   void
  */

void TIM2_Int_Init(u16 arr,u16 psc){
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE); ///使能 TIM2 时钟

	//初始化定时器参数
	TIM_TimeBaseStructure.TIM_Prescaler = psc;					//定时器分频
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//向上计数模式
	TIM_TimeBaseStructure.TIM_Period = arr;						//自动重装载值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;		//设置时钟分频因子
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE );//使能TIM2更新中断

	//TIM2 中断优先级设置
	NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn; 				//定时器 2 中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x02; 	//抢占优先级 2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; 		//响应优先级 3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;				//使能中断
	NVIC_Init(&NVIC_InitStructure);								//初始化 NVIC

	TIM_Cmd(TIM2, ENABLE); //使能 TIM2 外设
}



//定时器2中断服务函数
//每毫秒触发一次中断
void TIM2_IRQHandler(void){
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET){ //溢出中断
		MilliSecond++;

		if(Key1CD <20 )Key1CD++;
		if(LED1FlashTime > 0){
			if(MilliSecond % 500 == 0){
				LED1 = !LED1;
			}
		}

		if(MilliSecond == 1000){//以下语句每秒执行一次

			if(LED1FlashTime > 0){
				LED1FlashTime--;
				if(LED1FlashTime == 0xFF) LED1FlashTime = 0;
				if(LED1FlashTime == 0){
					LED1 = 1;
				}
			}

			if(WaitTime < 0xFF) WaitTime++;

			MilliSecond = 0;
		}
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);  //清除中断标志位
	}
}
