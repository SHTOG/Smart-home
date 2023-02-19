#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "stmt.h"
#include "Zigbee.h"
#include "iwdg.h"

int main(void){	
	delay_init();	    //��ʱ������ʼ��	  
	STMT_Init();		//��ʼ�����ģ��
	STMT_Turn_X_Deep(1,5);
	IWDG_Init(6,1562); 	//Լ10��ιһ�ι���10�벻ι������λ
	Zigbee_Init(115200);//���ں�Zigbeeһ���ʼ��
	IWDG_Init(6,778); 	//Լ5��ιһ�ι���5�벻ι������λ
	while(1){
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

