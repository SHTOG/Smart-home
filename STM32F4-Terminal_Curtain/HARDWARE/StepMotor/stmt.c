#include "stmt.h"

/**

  * @brief    初始化五线四相步进电机IO口
  * @brief	  A+ -> E0       A- -> E1
  * @brief 	  B+ -> E2       B- -> E3
  * @param    void
  * @retval   void
  */

void STMT_Init(void){
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);//使能 GPIOF 时钟 
    //GPIOE0,E1,E2,E3 初始化设置 
	GPIO_InitTypeDef    GPIO_InitStructure; 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式 
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz 
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//下拉 
    GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化 GPIO
	GPIO_ResetBits(GPIOE,GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3);//先将IO口全部拉低
}

void STMT_Open(){
	STMT_Turn_X_Around(0,1);
}

void STMT_Close(){
	STMT_Turn_X_Around(1,1);
}

void STMT_Turn_X_Around(u8 dir,u8 x){
	u8 i;
	while(x){
		for(i = 0; i < x; i++){
			STMT_X_Pulse(dir,512);
		}
		x--;
	}
}

void STMT_X_Pulse(u8 dir,u16 x){//旋转x个脉冲
//	u16 CW[] = {0x1,0x2,0x4,0x8};//单四拍顺时针
	u16 CW[] = {0x3,0x6,0xC,0x9};//双四拍顺时针
	u8 i,j;
	u16 k;
	if(dir == 0){//逆时针旋转
		for(k = 0; k < x; k++){
			for(i = 4; i > 0; i--){
				for(j = 0; j < 4; j++){
					PEout(j) = (CW[i-1] >> j) & 0x01;
				}
				delay_ms(5);
			}
		}
	}
	else{
		for(k = 0; k < x; k++){
			for(i = 0; i < 4 ; i++){
				for(j = 0; j < 4; j++){
					PEout(j) = (CW[i] >> j) & 0x01;
				}
				delay_ms(5);
			}
		}
	}




	// STMTAP = 1;
	// STMTAM = 0;
	// STMTBP = 0;
	// STMTBM = 0;
	// delay_ms(5);
	// STMTAP = 0;
	// STMTAM = 1;
	// STMTBP = 0;
	// STMTBM = 0;
	// delay_ms(5);
	// STMTAP = 0;
	// STMTAM = 0;
	// STMTBP = 1;
	// STMTBM = 0;
	// delay_ms(5);
	// STMTAP = 0;
	// STMTAM = 0;
	// STMTBP = 0;
	// STMTBM = 1;
	// delay_ms(5);
}
