#include "stmt.h"
#include "delay.h"

u8 CurtainDeep = 0;//记录窗帘实际开闭的程度(0~128)
u8 AllowChangeDeepFlag = 0;//更新CurtainDeep允许位
u8 NewCurtainDeep = 0;//记录窗帘准备开闭的程度(0~128)

/**

  * @brief    初始化五线四相步进电机IO口
  * @brief	  A+ -> B15       A- -> B14
  * @brief 	  B+ -> B13       B- -> B12
  * @param    void
  * @retval   void
  */

void STMT_Init(void){
	GPIO_InitTypeDef    GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//使能 GPIO时钟 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//使能 GPIO时钟 
    //GPIO初始化设置  
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;				//推挽输出 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;				//50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化 GPIOB
	GPIO_SetBits(GPIOB, GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);//先将IO口全部拉低
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化 GPIOA
	GPIO_SetBits(GPIOA, GPIO_Pin_8);//先将IO口全部拉低
}

void STMT_Turn_X_Deep(u8 dir, u8 x){
	static u8 OneDeepToPulse = MAXDEEPAROUND * 512 / MAXDEEP;
	while(x){
		STMT_X_Pulse(dir,OneDeepToPulse);
		x--;
	}
}

void STMT_Turn_X_Around(u8 dir,u8 x){
	while(x){
		STMT_X_Pulse(dir,512);
		x--;
	}
}

void STMT_X_Pulse(u8 dir,u16 x){//旋转x个脉冲
//	u16 CW[] = {0x1,0x2,0x4,0x8};//单四拍顺时针
	u16 CW[] = {0x3,0x6,0xC,0x9};//双四拍顺时针
	u8 i;
	u16 k;
	if(dir == 1){//逆时针旋转
		for(k = 0; k < x; k++){
			for(i = 4; i > 0; i--){
				STMTAP = (CW[i-1] >> 0) & 0x01;
				STMTAM = (CW[i-1] >> 1) & 0x01;
				STMTBP = (CW[i-1] >> 2) & 0x01;
				STMTBM = (CW[i-1] >> 3) & 0x01;
				delay_ms(5);
			}
		}
	}
	else{
		for(k = 0; k < x; k++){
			for(i = 0; i < 4 ; i++){
				STMTAP = (CW[i] >> 0) & 0x01;
				STMTAM = (CW[i] >> 1) & 0x01;
				STMTBP = (CW[i] >> 2) & 0x01;
				STMTBM = (CW[i] >> 3) & 0x01;
				delay_ms(5);
			}
		}
	}
}
