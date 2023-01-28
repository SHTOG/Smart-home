#include "led.h"   
#include "delay.h"

//初始化 PF9 和 PF10 为输出口.并使能这两个口的时钟            
//LED IO 初始化 
void LED_Init(void) 
{            
    GPIO_InitTypeDef    GPIO_InitStructure; 
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);//使能 GPIOF 时钟 
    //GPIOF9,F10 初始化设置 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;//LED0 和 LED1 对应 IO 口 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式 
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz 
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉 
    GPIO_Init(GPIOF, &GPIO_InitStructure);//初始化 GPIO
	GPIO_SetBits(GPIOF,GPIO_Pin_9 | GPIO_Pin_10);//GPIOF9,F10 设置高，灯灭 
}
extern void Send_Custom_Data(u8 type, u8 len, u8* Data);
void LED_Test(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin,u16 xms){
	GPIO_ResetBits(GPIOx,GPIO_Pin);
    delay_ms(xms);
    GPIO_SetBits(GPIOx,GPIO_Pin);
    delay_ms(xms);
	
}
