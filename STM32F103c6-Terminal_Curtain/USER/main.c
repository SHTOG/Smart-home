#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "stmt.h"
#include "Zigbee.h"
#include "iwdg.h"
#include "timer.h"

int main(void){	
	delay_init();	    //延时函数初始化	  
	STMT_Init();		//初始化电机模块
	STMT_Turn_X_Deep(1,5);
	IWDG_Init(6,1562); 	//约10秒喂一次狗，10秒不喂狗程序复位
	Zigbee_Init(115200);//串口和Zigbee一块初始化
	TIM2_Int_Init(10000-1,7200-1);//定时器时钟72M，分频系数7200，所以72M/7200=10Khz的计数频率，计数10000次为1s
	while(1){
		LED0 = 0;
		IWDG_Feed();//喂狗
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

