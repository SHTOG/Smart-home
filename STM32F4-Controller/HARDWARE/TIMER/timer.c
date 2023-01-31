#include "timer.h"

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
	
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE );//使能TM2更新中断
	
	//TIM3 中断优先级设置
	NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn; 				//定时器 2 中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x02; 	//抢占优先级 2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; 		//响应优先级 3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;				//使能中断
	NVIC_Init(&NVIC_InitStructure);								//初始化 NVIC
	
	TIM_Cmd(TIM2, ENABLE); //使能 TIM2 外设
}


/**

  * @brief    通用定时器3中断初始化
  * @param    arr->自动重装值
  * @param	  psc->时钟预分频数
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
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x02; 		//响应优先级 2
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;				//使能中断
	NVIC_Init(&NVIC_InitStructure);								//初始化 NVIC
	
	TIM_Cmd(TIM3, ENABLE); //使能 TIM3 外设
}

//定时器2中断服务函数
//主要记录程序运行时长
//每毫秒触发一次中断
void TIM2_IRQHandler(void){
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET){ //溢出中断
		static u8 Second = 0;//秒级计数器
		if(Second == 120){
			if(CheckByOnlineFlag(DeviceList)){
				Zigbee_Change_Mode(0);
				OpenNet();
				Zigbee_Change_Mode(1);
			}
			Second = 0;
		}
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);  //清除中断标志位
	}
}

//定时器3中断服务函数
//主要记录程序运行时长
//每毫秒触发一次中断
void TIM3_IRQHandler(void){
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET){ //溢出中断
		static u16 MilliSecond = 0;//毫秒级计数器
		static u8 Second = 0;//秒级计数器
		static u8 HalfSecond = 0;
		static u8 Minute = 0;//分级计数器
		MilliSecond++;
		if(MilliSecond == 1000){
			Second++;//秒数增加
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
			//每30秒向APP发送链表内容并将链表存入EEPROM
			//这里留个位置给《链表发送到APP端》
			AT24CXX_Save_List(0,DeviceList);
			HalfSecond = 0;
		}
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //清除中断标志位
	}
}
