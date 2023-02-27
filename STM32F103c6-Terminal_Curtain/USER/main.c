#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "stmt.h"
#include "Zigbee.h"
#include "iwdg.h"
#include "timer.h"

int main(void){	
	delay_init();	    //��ʱ������ʼ��	  
	STMT_Init();		//��ʼ�����ģ��
	STMT_Turn_X_Deep(1,5);
	IWDG_Init(6,1562); 	//Լ10��ιһ�ι���10�벻ι������λ
	Zigbee_Init(115200);//���ں�Zigbeeһ���ʼ��
	TIM2_Int_Init(10000-1,7200-1);//��ʱ��ʱ��72M����Ƶϵ��7200������72M/7200=10Khz�ļ���Ƶ�ʣ�����10000��Ϊ1s
	while(1){
		LED0 = 0;
		IWDG_Feed();//ι��
		if(CurtainDeep < NewCurtainDeep){
			STMT_Turn_X_Deep(1,1);
			CurtainDeep++;
		}
		else if(CurtainDeep > NewCurtainDeep){
			STMT_Turn_X_Deep(0,1);
			CurtainDeep--;
		}
	}
}

