#include "led.h"   


u8 LEDmode = 0;//LED��ģʽ��0Ϊ��ͨģʽ��1Ϊ������ģʽ
u8 direction = 1;//���������ȱ仯����1Ϊ������0Ϊ����

//��ʼ�� PF9 �� PF10 Ϊ�����.��ʹ���������ڵ�ʱ��            
//LED IO ��ʼ�� 
void LED_Init(void) 
{
    GPIO_InitTypeDef    GPIO_InitStructure; 
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);//ʹ�� GPIOF ʱ�� 
    //GPIOF9,F10 ��ʼ������ 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_0;//LED0 �� LED1 ��Ӧ IO �� 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ 
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//������� 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz 
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//���� 
    GPIO_Init(GPIOF, &GPIO_InitStructure);//��ʼ�� GPIO
	GPIO_SetBits(GPIOF,GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_0);//GPIOF0,F9,F10 ���øߣ����� 
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
