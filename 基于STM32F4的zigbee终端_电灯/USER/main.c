#include "sys.h" 
#include "usart.h" 
#include "Zigbee.h"
#include "led.h"
#include "beep.h"
#include "timer.h"

//yes

int main(void) {
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init(168);    //��ʼ����ʱ���� 
	OLED_Init();
	LED_Init();
	BEEP_Init();
	Zigbee_Init(115200);
	TIM3_PWM_Init(500-1,84-1);//84M/84=1Mhz�ļ���Ƶ��,��װ��ֵ500������PWMƵ��Ϊ 1M/500=2Khz.(����Ϊ500us)
	while (1){
		if(LEDmode == 0){
			TIM_SetCompare1(TIM3,PWMval);	//�޸ıȽ�ֵ���޸�ռ�ձ�
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





