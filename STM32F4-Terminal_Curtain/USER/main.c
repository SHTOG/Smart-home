#include "sys.h" 
#include "usart.h" 
#include "Zigbee.h"
#include "led.h"
#include "stmt.h"

int main(void) {
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);    //初始化延时函数 
	Zigbee_Init(115200);
	LED_Init();
	STMT_Init();
	LED1 = 0;
	STMT_Turn_X_Around(1,1);
	while (1){
		
	}
}





