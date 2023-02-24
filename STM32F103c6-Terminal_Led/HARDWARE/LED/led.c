#include "led.h"   


u8 LEDmode[8] = {0};//LED灯模式，0为普通模式，1为呼吸灯模式
u8 direction[8] = {1,1,1,1,1,1,1,1};//呼吸灯亮度变化方向，1为增亮，0为减灭

//初始化 PF9 和 PF10 为输出口.并使能这两个口的时钟            
//LED IO 初始化 
void LED_Init(void) 
{
    GPIO_InitTypeDef    GPIO_InitStructure; 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);//使能 GPIOB 时钟 
    //GPIOF9,F10 初始化设置 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;//LED1 对应 IO 口 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//50MHz 
    GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化 GPIO
	GPIO_SetBits(GPIOC,GPIO_Pin_13);//GPIOC13 设置高，灯灭 
}

void LED_Test(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin,u16 xms){
    GPIO_ResetBits(GPIOx,GPIO_Pin);
    delay_ms(xms);
    GPIO_SetBits(GPIOx,GPIO_Pin);
    delay_ms(xms);
}

void LED_Close(u8 number){
    LEDmode[number] = 0;
	PWMval[number] = 0;
}

void LED_Open(u8 number){
	PWMval[number] = 400;
}

void LED_Light_Plus(u8 number){
    if(PWMval[number] < 500) PWMval[number]+=50;
}

void LED_Light_Minus(u8 number){
    if(PWMval[number] > 0) PWMval[number]-=50;
}

void LED_Light_Set(u8 number, u8 value){
    PWMval[number] = value * 50;
}

void LED_Mode(u8 number, u8 mode){
	if(mode == 0){
		while(1){
			if(PWMval[number]%50 == 0) break;
			if(direction[number]) PWMval[number]++;
			else PWMval[number]--;
            TIM_SetCompare[number](TIM3,PWMval[number]);
			delay_ms(2);
		}
	}
    LEDmode[number] = mode;
}
