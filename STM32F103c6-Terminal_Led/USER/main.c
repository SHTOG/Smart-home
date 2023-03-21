#include "sys.h" 
#include "usart.h" 
#include "Zigbee.h"
#include "led.h"
#include "timer.h"
#include "exti.h"
#include "iwdg.h"
#include "key.h"

u8 ZigbeeOnlineFlag = 0;//在网Flag,置0表示没有连上协调器,置1表示已连上
u8 OnlineFlag = 0;//在网Flag,置0表示没有连上生态,置1表示已连上
u8 APPJudgeFlag = 0;//来自APP的入网判断标志位，如果为1，表示同意，为2表示拒绝，闲时置0

void Try_To_Link(){
	LED1FlashTime = 120;//闪烁两分钟,表示正在配对中
	Zigbee_Change_Mode(0);//Zigbee进入HEX指令模式
	if(ZigbeeOnlineFlag == 1) Zigbee_Restore_Factory_Setting();//Zigbee恢复出厂设置
	Zigbee_Restart();
//	delay_ms(1000);
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
			delay_ms(1000);//考虑数据接收延迟,避免频繁发送导致中控数据拥堵
		}
		if(APPJudgeFlag == 1) OnlineFlag = 1;//入网成功
		else if(APPJudgeFlag == 2) OnlineFlag = 0;//入网失败
	}
	LED1FlashTime = 0;//指示灯停止闪烁
	LED1 = 1;//熄灯
}

int main(void) {
	u8 i;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//设置系统中断优先级分为2
	delay_init();	    //延时函数初始化
	LED_Init();	  		//LED初始化
//	IWDG_Init(6,1562); 	//约10秒喂一次狗，10秒不喂狗程序复位
	Key_Init();			//按键初始化
	USART1_Init(115200);//初始化串口1(通信用，有功能需要用到串口请换别的串口号)
	TIM2_Int_Init(10-1,7200-1);//定时器时钟72M，分频系数7200，所以72M/7200=10Khz的计数频率，计数10次为1ms
	Zigbee_Change_Mode(0);//Zigbee进入HEX指令模式
	Zigbee_Get_State();//这个函数执行完会得到ZigbeeOnlineFlag的值
	if(ZigbeeOnlineFlag == 1){
		Zigbee_Set_Send_Target();//设置透传目标为中控
		Zigbee_Change_Mode(1);
		Zigbee_Update_OnlineFlag();//这个函数执行完会得到OnlineFlag的值，为后面的while(1)提供执行方向
	}
	/*初始化函数请放下下面*/
	TIM3_PWM_Init(500-1,72-1);//72M/72=1Mhz的计数频率,重装载值500，所以PWM频率为 1M/500=2Khz.(周期为500us)
	EXTI0_Init();
	/*初始化函数请放下上面*/
	LED2 = 0;//测试用，可从核心板上直接观测到程序进入了while(1)
	while (1){
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
		/*请在下面编写自己终端的代码*/
		// 电灯根据模式工作
		for(i = 0; i < 4; i++){
			if(LEDmode[i] == 0){
				TIM_SetCompare[i](TIM3,PWMval[i]);	//修改比较值，修改占空比
			}
			else if(LEDmode[i] == 1){
				if(PWMval[i] >= 450) direction[i] = 0;
				if(PWMval[i] <= 50) direction[i] = 1;
				if(direction[i]) PWMval[i]++;
				else PWMval[i]--;
				TIM_SetCompare[i](TIM3,PWMval[i]);
			}
		}
		delay_ms(2);
		/*请在上面编写自己终端的代码*/
		if(OnlineFlag == 1){
			LED1 = 0;//测试用
			
		}
		else if(OnlineFlag == 0)LED1 = 1;
	}
	
}
