#include "sys.h" 
#include "usart.h" 
#include "Zigbee.h"
#include "led.h"
#include "beep.h"
#include "timer.h"
#include "exti.h"

int main(void) {
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init(168);    //��ʼ����ʱ���� 
	OLED_Init();
	LED_Init();			//��ʼ��LEDio��
	BEEP_Init();		//��ʼ���������������ã�
	Zigbee_Init(115200);
	TIM3_PWM_Init(500-1,84-1);//84M/84=1Mhz�ļ���Ƶ��,��װ��ֵ500������PWMƵ��Ϊ 1M/500=2Khz.(����Ϊ500us)
	EXTI0_Init();
//	TIM2_Int_Init(10000-1,8400-1);//��ʱ��ʱ��84M����Ƶϵ��8400������84M/8400=10Khz�ļ���Ƶ�ʣ�����10000��Ϊ1s
	while (1){
		//��Ƹ���ģʽ����
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





