#include "timer.h"

u16 MilliSecond = 0;//毫秒级计数器
u8 Second = 0;//秒级计数器
u8 Minute = 0;//分级计数器
u8 WaitTime = 0xFF;//秒级等待应答时间,置零时开始计时,计到255停止
u8 EspWaitTime = 5;//秒级等待应答时间,置零时开始计时,计到5停止

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
	TIM_ClearITPendingBit(TIM2,TIM_IT_Update);//清除中断位
	//TIM3 中断优先级设置
	NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn; 				//定时器 2 中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x02; 	//抢占优先级 2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x02; 		//响应优先级 2
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
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);//清除中断位
	//TIM3 中断优先级设置
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; 				//定时器 3 中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x03; 	//抢占优先级 3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; 		//响应优先级 3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;				//使能中断
	NVIC_Init(&NVIC_InitStructure);								//初始化 NVIC
	
	TIM_Cmd(TIM3, ENABLE); //使能 TIM3 外设
}


//定时器2中断服务函数
//提供系统时间测量
//每毫秒触发一次中断
void TIM2_IRQHandler(void){
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET){ //溢出中断
		MilliSecond++;
		if(MilliSecond == 1000){
			Second++;//秒数增加
			if(Second == 60){
				Minute++;//分数增加
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
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);  //清除中断标志位
	}
}


//定时器3中断服务函数
//根据设定时长执行指定函数
//每秒触发一次中断
u8 UpdateWaitTime = 0;
void TIM3_IRQHandler(void){
	u8 testData[3] = {23,3,90};//23.3℃、90%湿度
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET){ //溢出中断
		if(UpdateWaitTime == 10){//开机时和开机后每十分钟
			UpdateDeviceList(DeviceList);//更新链表内所有终端在网状态
			UpdateWaitTime = 0;
		}
		if(DeviceList->next != NULL && Second % 5 == 0 && PrintDeviceListFlag == 0)Send_Custom_Data(USART2,0x02,3,testData);//把温湿度数据发送到APP
		
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //清除中断标志位
	}
}
