#include "sys.h" 
#include "usart.h" 
#include "Zigbee.h"
#include "led.h"
#include "beep.h"
#include "timer.h"

//yes

int main(void) {
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);    //初始化延时函数 
	OLED_Init();
	LED_Init();
	BEEP_Init();
	Zigbee_Init(115200);
	TIM3_PWM_Init(500-1,84-1);//84M/84=1Mhz的计数频率,重装载值500，所以PWM频率为 1M/500=2Khz.(周期为500us)
	while (1){
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





