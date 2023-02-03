#include "sys.h" 
#include "usart.h" 
#include "Zigbee.h"
#include "led.h"
#include "beep.h"
#include "timer.h"
#include "exti.h"

int main(void) {
	u8 i;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);    //初始化延时函数 
	OLED_Init();
	LED_Init();			//初始化LEDio口
	BEEP_Init();		//初始化蜂鸣器（测试用）
	EXTI0_Init();
	TIM2_Int_Init(10-1,8400-1);//定时器时钟84M，分频系数8400，所以84M/8400=10Khz的计数频率，计数10次为1ms
	Zigbee_Init(115200);
	TIM3_PWM_Init(500-1,84-1);//84M/84=1Mhz的计数频率,重装载值500，所以PWM频率为 1M/500=2Khz.(周期为500us)
	while (1){
	// 电灯根据模式工作
		for(i = 0; i < 4; i++){
			if(LEDmode[i] == 0){
				TIM_SetCompare[i](TIM3,PWMval[i]);	//修改比较值，修改占空比
			}
			else if(LEDmode[i] == 1){
				if(PWMval[i] >= 450) direction[i] = 0;
				if(PWMval[i] <= 50) direction[i] = 1;
				if(direction[i]) PWMval[i]++;
				else PWMval[i]--;
				TIM_SetCompare[i](TIM3,PWMval[i]);
			}
		}
		delay_ms(2);
		}
	
}





