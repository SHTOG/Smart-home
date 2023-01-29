#include "sys.h" 
#include "delay.h" 
#include "usart.h" 
#include "OLED.h"
#include "stdlib.h"
#include "Zigbee.h"
#include "led.h"
#include "beep.h"
#include "myList.h"
#include "AnalyseAndSend.h"
#include "timer.h"
#include "24cxx.h"
//#include "u8x8.h"
//#include "u8g2.h"

/*Begin of ȫ�ֱ���*/
u8 ReadFlag = 0;//״̬��ȡ�ɹ���־λ
Device* DeviceList;//�豸���̵�ַ���ݿ�
/*End of ȫ�ֱ���*/

/*Begin of ��������*/
//uint8_t u8x8_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);//u8g2�ص���������
/*End of ��������*/


int main(void) {
  	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init(168);    //��ʼ����ʱ���� 
	Zigbee_Init(115200);	
	OLED_Init();		//��ʼ��OLED
	OLED_Show_Chinese(2,1,welcome_C,5);//ϵͳ������ʾ��ӭ���棬������ʧ��ʾ�ɹ�����ϵͳ
	LED_Init();			//��ʼ���п�ָʾ�ƺ���
	BEEP_Init();		//��ʼ���пط���������
	DeviceList = createList();//���ﻹ��û��������籣�湦�ܣ�֮����load��������
	USART2_Init(115200);
	TIM3_Int_Init(10000-1,8400-1);//��ʱ��ʱ��84M����Ƶϵ��8400������84M/8400=10Khz�ļ���Ƶ�ʣ�����10000��Ϊ1s     
	OLED_Clear();
	u8 ladd[] = {1,1,1,1,1,1,1,1};
	u8 sadd3[] = {3,3};
	u8 sadd2[] = {2,2};
	u8 sadd1[] = {1,1};
	u8 sadd4[] = {4,4};
		insertNodeByType(DeviceList,03,ladd,sadd1);
		insertNodeByType(DeviceList,01,ladd,sadd2);
		insertNodeByType(DeviceList,03,ladd,sadd3);
		insertNodeByType(DeviceList,04,ladd,sadd4);
	while (1){
		LED_Test(GPIOF,GPIO_Pin_9,400);
		printList(DeviceList);
		delay_ms(5000);
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
