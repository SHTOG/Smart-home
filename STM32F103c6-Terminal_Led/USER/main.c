#include "sys.h" 
#include "usart.h" 
#include "Zigbee.h"
#include "led.h"
#include "timer.h"
#include "exti.h"
#include "iwdg.h"

int main(void) {
	u8 i;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init();    //��ʼ����ʱ���� 
	LED_Init();			//��ʼ��LEDio��
	EXTI0_Init();
	TIM2_Int_Init(10-1,8400-1);//��ʱ��ʱ��84M����Ƶϵ��8400������84M/8400=10Khz�ļ���Ƶ�ʣ�����10��Ϊ1ms
	IWDG_Init(6,1562); 	//Լ10��ιһ�ι���10�벻ι������λ
	Zigbee_Init(115200);//���ں�Zigbeeһ���ʼ��
	IWDG_Init(6,778); 	//Լ5��ιһ�ι���5�벻ι������λ
	TIM3_PWM_Init(500-1,72-1);//72M/72=1Mhz�ļ���Ƶ��,��װ��ֵ500������PWMƵ��Ϊ 1M/500=2Khz.(����Ϊ500us)
	LED1 = 0;//������
	while (1){
	// ��Ƹ���ģʽ����
		for(i = 0; i < 4; i++){
			if(LEDmode[i] == 0){
				TIM_SetCompare[i](TIM3,PWMval[i]);	//�޸ıȽ�ֵ���޸�ռ�ձ�
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
