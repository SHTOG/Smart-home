#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "Zigbee.h"
#include "led.h"
#include "iwdg.h"
#include "timer.h"
#include "key.h"
#include "mtxk.h"
#include "stmt.h"

u8 ZigbeeOnlineFlag = 0;//在网Flag,置0表示没有连上协调器,置1表示已连上
u8 OnlineFlag = 0;//在网Flag,置0表示没有连上生态,置1表示已连上
u8 APPJudgeFlag = 0;//来自APP的入网判断标志位，如果为1，表示同意，为2表示拒绝，闲时置0

void Try_To_Link(){
	LED1FlashTime = 120;//闪烁两分钟,表示正在配对中
	Zigbee_Change_Mode(0);//Zigbee进入HEX指令模式
	if(ZigbeeOnlineFlag == 1) Zigbee_Restore_Factory_Setting();//Zigbee恢复出厂设置
	Zigbee_Restart();
	// delay_ms(1000);
	IWDG_Feed();//喂狗
	Zigbee_Set_Type_To_Active_Terminal();//设置模组类型为活跃终端
	Zigbee_Restart();
	delay_ms(100);
	Zigbee_Open_Net();//打开网络准备配对
	IWDG_Feed();//喂狗
	delay_ms(4000);//稍微等待联网
	IWDG_Feed();//喂狗
	WaitTime = 0;
	ZigbeeOnlineFlag = 0;
	while(ZigbeeOnlineFlag != 1){
		IWDG_Feed();//喂狗
		if(WaitTime >= 110){//超过110sZigbee没有连上协调器,判断为中控没开机，直接退出while
			break;
		}
		Zigbee_Get_State();//获取设备状态
		delay_ms(100);
	}
//	Zigbee_Close_Net();
	if(ZigbeeOnlineFlag == 1){//如果已经连上了协调器
		Zigbee_Restart();
		Zigbee_Set_Send_Target();//将透传目标改为协调器
		Zigbee_Change_Mode(1);//进入透传模式
		
		//等待APP同意或拒绝
		APPJudgeFlag = 0;
		WaitTime = 0;
		while(APPJudgeFlag == 0){
			if(WaitTime >= 120){//120s没收到应答，直接退出，表示为没有入网
				OnlineFlag = 0;
				return ;
			}
			Send_Custom_Data(0x00,3,SelfShortAddr);
			delay_ms(500);//考虑数据接收延迟,避免频繁发送导致中控数据拥堵
		}
		if(APPJudgeFlag == 1) OnlineFlag = 1;//入网成功
		else if(APPJudgeFlag == 2) OnlineFlag = 0;//入网失败
	}
	LED1FlashTime = 0;//指示灯停止闪烁
	LED1 = 1;//熄灯
}



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
	LED_Init();	  		//LED初始化
//	IWDG_Init(6,1562); 	//约10秒喂一次狗，10秒不喂狗程序复位
	Key_Init();			//按键初始化
	USART1_Init(115200);//初始化串口1(通信用，有功能需要用到串口请换别的串口号)
	TIM2_Int_Init(10-1,7200-1);//定时器时钟72M，分频系数7200，所以72M/7200=10Khz的计数频率，计数10次为1ms
//	Zigbee_Change_Mode(0);//Zigbee进入HEX指令模式
//	Zigbee_Get_State();//这个函数执行完会得到ZigbeeOnlineFlag的值
//	if(ZigbeeOnlineFlag == 1){
//		Zigbee_Set_Send_Target();//设置透传目标为中控
//		Zigbee_Change_Mode(1);
//		Zigbee_Update_OnlineFlag();//这个函数执行完会得到OnlineFlag的值，为后面的while(1)提供执行方向
//	}
	/*初始化函数请放下下面*/
	MTXK_Init();
	STMT_Init();

	/*初始化函数请放下上面*/

	/*测试:假设用户已经设定了密码200030*/
	PasswordLen = 6;
	RealPassword = (u8*)malloc(sizeof(u8)*PasswordLen);
	RealPassword[0] = 2;
	RealPassword[1] = 0;
	RealPassword[2] = 0;
	RealPassword[3] = 0;
	RealPassword[4] = 3;
	RealPassword[5] = 0;
	STMT_X_Pulse(0,128);//逆时针旋转半圈表示步进电机已经成功连接
	/*测试*/
	while(1){
		PBout(13) = 1; 
		/*以下代码禁止修改*/
		LED2 = 0;//测试用，可从核心板上直接观测到程序进入了while(1)
		if(Key1 == 0){//如果按键1被按下
			WaitTime = 0;
			while(Key1 == 0){//如果保持长按
				if(WaitTime >= 3){//如果长按超过了3s开始配网
					Try_To_Link();//尝试入网
					break;
				}
			}
		}
		IWDG_Feed();//喂狗
		/*以上代码禁止修改*/
		KeyNum = MTXK_Scan();
		delay_ms(20);
		while(KeyNum == MTXK_Scan() && KeyNum != 0);
		if(KeyNum != 0){
			USART_SendData(USART1,KeyNum);//测试用
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
					while(MTXK_Scan() == 1);
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
					index = 0;//清零正在输入的密码位数
					free(InputPassword);//释放指针内存
					InputPassword = (u8*)malloc(sizeof(u8)*PasswordLen);//给密码缓冲区分配空间
				}
			}
		}
		if(OnlineFlag == 1) {
			LED1 = 0;//测试用
			/*请在下面编写需要远程操控的代码*/
			
			/*请在上面编写需要远程操控的代码*/
		}
		else if(OnlineFlag == 0)LED1 = 1;//测试用
		
	}
}

