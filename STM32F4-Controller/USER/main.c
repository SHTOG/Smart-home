#include "sys.h" 
#include "delay.h" 
#include "usart.h" 
#include "OLED.h"
#include "stdlib.h"
#include "Zigbee.h"
#include "led.h"
#include "beep.h"
#include "myList.h"
#include "timer.h"
#include "24cxx.h"
//#include "u8x8.h"
//#include "u8g2.h"

/*Begin of 全局变量*/
u8 ReadFlag = 0;//状态读取成功标志位
Device* DeviceList = NULL;//设备长短地址数据链表
u8 Esp32AckFlag = 0;
/*End of 全局变量*/

/*Begin of 函数声明*/
//uint8_t u8x8_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);//u8g2回调函数声明
/*End of 函数声明*/


int main(void) {
	u8 i;
	u8 DSAddr[2];
	u8 type;
	u8 len;
	u8* Data;
  	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分为2
	delay_init(168);    //初始化延时函数 
	OLED_Init();		//初始化OLED
	OLED_Show_Chinese(2,1,welcome_C,5);//系统开机显示始迎界面，界面消失表示成功进入系统
	LED_Init();			//初始化中控指示灯函数
	Zigbee_Init(115200);	
	BEEP_Init();		//初始化中控蜂鸣器函数
	AT24CXX_Init();
	USART2_Init(115200);
	while(AT24CXX_Check()){
		LED_Test(GPIOF,GPIO_Pin_9,200);
	}
	AT24CXX_Clear();//测试用
	DeviceList = AT24CXX_Load_List(0);//从24Cxx的首地址开始读取链表，如果24Cxx没写过链表就等于调用了CreateList
	TIM3_Int_Init(10-1,8400-1);//定时器时钟84M，分频系数8400，所以84M/8400=10Khz的计数频率，计数10次为1ms     
	TIM2_Int_Init(10000-1,8400-1);//定时器时钟84M，分频系数8400，所以84M/8400=10Khz的计数频率，计数10000次为1s   
//	OLED_Clear();
//	PrintList(DeviceList);//开机向APP发送一次链表 //测试时注释掉这一行
	while (1){
		LED1 = 1;
		delay_ms(1000);
		if(ReadySetTargetFlag == 0){
			USART2ReciveITEnable = 0;//关闭串口中断保证赋值完整
			DSAddr[0] = USART2_RX_BUF[4];
			DSAddr[1] = USART2_RX_BUF[5];
			type = USART2_RX_BUF[6];
			len = USART2_RX_BUF[7];
			Data = (u8*)malloc(sizeof(u8)*len);
			for(i = 0; i < len ; i++){
				Data[i] = USART2_RX_BUF[8+i];
			}
			USART2ReciveITEnable = 1;//打开串口中断
			Zigbee_Change_Mode(0);
			Set_Send_Target(DSAddr,0x01);
			Zigbee_Change_Mode(1);
			AckFlag = 0;
			WaitTime = 0;//等待倒计时开始计时
			while(AckFlag != 1){
				if(WaitTime == 5){
					SetNetStateByShorAddr(DeviceList,DSAddr);
					break;
				}
				Send_Custom_Data(USART1,type,len,Data);
				AckJudge = 1;
				delay_ms(100);//目前是在内部植入了一个对AckJudge，后期可以用UCOS的任务轮转调度优化CPU资源

			}
			AckJudge = 0;
			free(Data);
		}
		LED1 = 0;
  	}
}



//uint8_t u8x8_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr){
//  switch(msg)
//  {
//    case U8X8_MSG_GPIO_AND_DELAY_INIT:	// called once during init phase of u8g2/u8x8
//      break;							// can be used to setup pins
//    case U8X8_MSG_DELAY_NANO:			// delay arg_int * 1 nano second
//      break;
//    case U8X8_MSG_DELAY_100NANO:		// delay arg_int * 100 nano seconds
//		__NOP();
//      break;
//    case U8X8_MSG_DELAY_10MICRO:		// delay arg_int * 10 micro seconds
//		delay_us(10);
//      break;
//    case U8X8_MSG_DELAY_MILLI:			// delay arg_int * 1 milli second
//		delay_ms(1);
//      break;
//    case U8X8_MSG_DELAY_I2C:				// arg_int is the I2C speed in 100KHz, e.g. 4 = 400 KHz
//		delay_us(5);
//      break;							// arg_int=1: delay by 5us, arg_int = 4: delay by 1.25us
//    case U8X8_MSG_GPIO_D0:				// D0 or SPI clock pin: Output level in arg_int
//    //case U8X8_MSG_GPIO_SPI_CLOCK:
//      break;
//    case U8X8_MSG_GPIO_D1:				// D1 or SPI data pin: Output level in arg_int
//    //case U8X8_MSG_GPIO_SPI_DATA:
//      break;
//    case U8X8_MSG_GPIO_D2:				// D2 pin: Output level in arg_int
//      break;
//    case U8X8_MSG_GPIO_D3:				// D3 pin: Output level in arg_int
//      break;
//    case U8X8_MSG_GPIO_D4:				// D4 pin: Output level in arg_int
//      break;
//    case U8X8_MSG_GPIO_D5:				// D5 pin: Output level in arg_int
//      break;
//    case U8X8_MSG_GPIO_D6:				// D6 pin: Output level in arg_int
//      break;
//    case U8X8_MSG_GPIO_D7:				// D7 pin: Output level in arg_int
//      break;
//    case U8X8_MSG_GPIO_E:				// E/WR pin: Output level in arg_int
//      break;
//    case U8X8_MSG_GPIO_CS:				// CS (chip select) pin: Output level in arg_int
//      break;
//    case U8X8_MSG_GPIO_DC:				// DC (data/cmd, A0, register select) pin: Output level in arg_int
//      break;
//    case U8X8_MSG_GPIO_RESET:			// Reset pin: Output level in arg_int
//      break;
//    case U8X8_MSG_GPIO_CS1:				// CS1 (chip select) pin: Output level in arg_int
//      break;
//    case U8X8_MSG_GPIO_CS2:				// CS2 (chip select) pin: Output level in arg_int
//      break;
//    case U8X8_MSG_GPIO_I2C_CLOCK:		// arg_int=0: Output low at I2C clock pin
//		PBout(8) = arg_int;
//      break;							// arg_int=1: Input dir with pullup high for I2C clock pin
//    case U8X8_MSG_GPIO_I2C_DATA:			// arg_int=0: Output low at I2C data pin
//		PBout(9) = arg_int;
//      break;							// arg_int=1: Input dir with pullup high for I2C data pin
//    case U8X8_MSG_GPIO_MENU_SELECT:
//      u8x8_SetGPIOResult(u8x8, /* get menu select pin state */ 0);
//      break;
//    case U8X8_MSG_GPIO_MENU_NEXT:
//      u8x8_SetGPIOResult(u8x8, /* get menu next pin state */ 0);
//      break;
//    case U8X8_MSG_GPIO_MENU_PREV:
//      u8x8_SetGPIOResult(u8x8, /* get menu prev pin state */ 0);
//      break;
//    case U8X8_MSG_GPIO_MENU_HOME: 
//      u8x8_SetGPIOResult(u8x8, /* get menu home pin state */ 0);
//      break;
//    default:
//      u8x8_SetGPIOResult(u8x8, 1);			// default return value
//      break;
//  }
//  return 1;
//}
