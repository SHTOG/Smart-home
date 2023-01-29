#include "led.h"   


u8 LEDmode = 0;//LED灯模式，0为普通模式，1为呼吸灯模式
u8 direction = 1;//呼吸灯亮度变化方向，1为增亮，0为减灭

//初始化 PF9 和 PF10 为输出口.并使能这两个口的时钟            
//LED IO 初始化 
void LED_Init(void) 
{
    GPIO_InitTypeDef    GPIO_InitStructure; 
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);//使能 GPIOF 时钟 
    //GPIOF9,F10 初始化设置 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_0;//LED0 和 LED1 对应 IO 口 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式 
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz 
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉 
    GPIO_Init(GPIOF, &GPIO_InitStructure);//初始化 GPIO
	GPIO_SetBits(GPIOF,GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_0);//GPIOF0,F9,F10 设置高，灯灭 
}

void LED_Test(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin,u16 xms){
    GPIO_ResetBits(GPIOx,GPIO_Pin);
    delay_ms(xms);
    GPIO_SetBits(GPIOx,GPIO_Pin);
    delay_ms(xms);
}

void LED_Close(){
	PWMval = 0;
}

void LED_Open(){
	PWMval = 500;
}

void LED_Light_Plus(){
    if(PWMval < 500) PWMval+=50;
}

void LED_Light_Minus(){
    if(PWMval > 0) PWMval-=50;
}

void LED_Light_Set(u8 value){
    PWMval = value * 50;
}

void LED_Mode(u8 mode){
	if(mode == 0){
		while(1){
			if(direction) PWMval++;
			else PWMval--;
			TIM_SetCompare1(TIM3,PWMval);
			delay_ms(5);
			if(PWMval%50 == 0) break;
		}
	}
    LEDmode = mode;
}
