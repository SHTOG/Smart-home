#include "sys.h"
#include "Zigbee.h"
#include "iwdg.h"
#include "mtxk.h"
#include "timer.h"
#include "stmt.h"
#include "nfc.h"

int main(void){	
	u8 i;//供循环语句使用
	u8 KeyNum = 0;
	u8 LockState = 0;//门锁开关状态，0表示已锁上，1表示正打开   后期必须想办法实时监测门锁的打开状态，实时更新这个变量
	u8 MTXKInputFlag = 0;//正在输入标志位，置1表示正在输入密码
	u8* RealPassword;//用户设定的密码
	u8 PasswordLen;//用户设定的密码长度
	u8* InputPassword;//指向正在输入的密码缓冲区
	u8 index = 0;//正在输入的密码位数(从0开始)
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//设置系统中断优先级分为2
	delay_init();	    //延时函数初始化
	LED_Init();	  		//测试用的LED引脚，工程写完了就删
	MTXK_Init();
	STMT_Init();
	IWDG_Init(6,1562); 	//约10秒喂一次狗，10秒不喂狗程序复位
	USART1_Init(115200);
//	Zigbee_Init(115200);//串口和Zigbee一块初始化
	TIM3_Int_Init(9,7199);//10Khz的计数频率，计数到10为1ms
	/*测试:假设用户已经设定了密码200030*/
	PasswordLen = 6;
	RealPassword = (u8*)malloc(sizeof(u8)*PasswordLen);
	RealPassword[0] = 2;
	RealPassword[1] = 0;
	RealPassword[2] = 0;
	RealPassword[3] = 0;
	RealPassword[4] = 3;
	RealPassword[5] = 0;
	STMT_X_Pulse(1,128);//逆时针旋转半圈表示开锁
	/*测试*/
	TIM2_Int_Init(10000-1,7200-1);//定时器时钟72M，分频系数7200，所以72M/7200=10Khz的计数频率，计数10000次为1s
	while(1){
		IWDG_Feed();//喂狗
		KeyNum = MTXK_Scan();
		if(KeyNum != 0){
			USART_SendData(USART1,KeyNum);
			if(KeyNum == 4){//锁门
				if(LockState == 1) STMT_X_Pulse(0,256);//逆时针旋转半圈表示开锁
			}
			else if(KeyNum == 15){//按下第15键即开始输入密码输入密码状态
				MTXKInputFlag = 1;
				index = 0;//清零正在输入的密码位数
				free(InputPassword);//释放指针内存
				InputPassword = (u8*)malloc(sizeof(u8)*PasswordLen);//给密码缓冲区分配空间
			}
			else if(KeyNum == 13){//退出密码输入模式
					MTXKInputFlag = 0;
			}
			if(MTXKInputFlag == 1){//正在输入密码中
				if(KeyNum == 1){
					InputPassword[index] = 7;
					index++;
				}
				else if(KeyNum == 2){
					InputPassword[index] = 8;
					index++;
				}
				else if(KeyNum == 3){
					InputPassword[index] = 9;
					index++;
				}
				else if(KeyNum == 5){
					InputPassword[index] = 4;
					index++;
				}
				else if(KeyNum == 6){
					InputPassword[index] = 5;
					index++;
				}
				else if(KeyNum == 7){
					InputPassword[index] = 6;
					index++;
				}
				else if(KeyNum == 9){
					InputPassword[index] = 1;
					index++;
				}
				else if(KeyNum == 10){
					InputPassword[index] = 2;
					index++;
				}
				else if(KeyNum == 11){
					InputPassword[index] = 3;
					index++;
				}
				else if(KeyNum == 14){
					InputPassword[index] = 0;
					index++;
				}
				else if(KeyNum == 8 && index > 0){//删去刚输入的1位数字
					index--;
				}
				if(index == PasswordLen){//当输入的密码长度与正确密码长度一致时,判断输入的密码是否正确
					for(i = 0; i < PasswordLen; i++){
						if(InputPassword[i] != RealPassword[i]) break;
					}
					if(i == PasswordLen){//输入的密码对得上
						STMT_X_Pulse(0,256);//顺时针旋转半圈表示开锁
					}
					else{//密码输入错误，清空输入密码缓冲区，继续等待输入密码
						index = 0;//清零正在输入的密码位数
						free(InputPassword);//释放指针内存
						InputPassword = (u8*)malloc(sizeof(u8)*PasswordLen);//给密码缓冲区分配空间
					}						
				}
			}
		}
		
	}
}			
		


