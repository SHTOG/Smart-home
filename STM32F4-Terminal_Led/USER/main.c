#include "sys.h" 
#include "usart.h" 
#include "Zigbee.h"
#include "led.h"
#include "beep.h"
#include "timer.h"
#include "exti.h"

int main(void) {
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);    //初始化延时函数 
	OLED_Init();
	LED_Init();			//初始化LEDio口
	BEEP_Init();		//初始化蜂鸣器（测试用）
	Zigbee_Init(115200);
	TIM3_PWM_Init(500-1,84-1);//84M/84=1Mhz的计数频率,重装载值500，所以PWM频率为 1M/500=2Khz.(周期为500us)
	EXTI0_Init();
//	TIM2_Int_Init(10000-1,8400-1);//定时器时钟84M，分频系数8400，所以84M/8400=10Khz的计数频率，计数10000次为1s
	while (1){
		//电灯根据模式工作
		if(LEDmode == 0){
			TIM_SetCompare1(TIM3,PWMval);	//修改比较值，修改占空比
		}
		else if(LEDmode == 1){
			if(PWMval == 500) direction = 0;
			if(PWMval == 50) direction = 1;
			if(direction) PWMval++;
			else PWMval--;
			TIM_SetCompare1(TIM3,PWMval);
			delay_ms(5);
		}
  	}
	
}





