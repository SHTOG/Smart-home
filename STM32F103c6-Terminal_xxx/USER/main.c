#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "Zigbee.h"
#include "led.h"
#include "iwdg.h"
#include "timer.h"

int main(void){	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//����ϵͳ�ж����ȼ���Ϊ2
	delay_init();	    //��ʱ������ʼ��
	LED_Init();	  		//�����õ�LED���ţ�����д���˾�ɾ
	IWDG_Init(6,1562); 	//Լ10��ιһ�ι���10�벻ι������λ
	USART1_Init(115200);//������ɾ�����в�����һ�н��ע��
//	Zigbee_Init(115200);//���ں�Zigbeeһ���ʼ��
	TIM2_Int_Init(10000-1,7200-1);//��ʱ��ʱ��72M����Ƶϵ��7200������72M/7200=10Khz�ļ���Ƶ�ʣ�����10000��Ϊ1s
	while(1){
		LED0 = 0;
		IWDG_Feed();//ι��
		
	}
}

