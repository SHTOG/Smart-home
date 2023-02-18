#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "stmt.h"
#include "Zigbee.h"

int main(void){	
	delay_init();	    //延时函数初始化	  
	Zigbee_Init(115200);	//串口初始化
	STMT_Init();		//初始化电机模块
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

