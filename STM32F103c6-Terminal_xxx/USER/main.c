#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "Zigbee.h"
#include "led.h"
#include "iwdg.h"
#include "timer.h"

int main(void){	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//设置系统中断优先级分为2
	delay_init();	    //延时函数初始化
	LED_Init();	  		//测试用的LED引脚，工程写完了就删
	IWDG_Init(6,1562); 	//约10秒喂一次狗，10秒不喂狗程序复位
	USART1_Init(115200);//测试完删除这行并把下一行解除注释
//	Zigbee_Init(115200);//串口和Zigbee一块初始化
	TIM2_Int_Init(10000-1,7200-1);//定时器时钟72M，分频系数7200，所以72M/7200=10Khz的计数频率，计数10000次为1s
	while(1){
		LED0 = 0;
		IWDG_Feed();//喂狗
		
	}
}

