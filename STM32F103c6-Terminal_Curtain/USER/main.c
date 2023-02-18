#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "stmt.h"
#include "Zigbee.h"

int main(void){	
	delay_init();	    //��ʱ������ʼ��	  
	Zigbee_Init(115200);	//���ڳ�ʼ��
	STMT_Init();		//��ʼ�����ģ��
	STMT_Turn_X_Deep(1,5);
	while(1){
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

