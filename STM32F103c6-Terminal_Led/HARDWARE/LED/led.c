#include "led.h"   


u8 LEDmode[8] = {0};//LED��ģʽ��0Ϊ��ͨģʽ��1Ϊ������ģʽ
u8 direction[8] = {1,1,1,1,1,1,1,1};//���������ȱ仯����1Ϊ������0Ϊ����

//��ʼ�� PF9 �� PF10 Ϊ�����.��ʹ���������ڵ�ʱ��            
//LED IO ��ʼ�� 
void LED_Init(void) 
{
    GPIO_InitTypeDef    GPIO_InitStructure; 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);//ʹ�� GPIOB ʱ�� 
    //GPIOF9,F10 ��ʼ������ 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;//LED1 ��Ӧ IO �� 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//50MHz 
    GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ�� GPIO
	GPIO_SetBits(GPIOC,GPIO_Pin_13);//GPIOC13 ���øߣ����� 
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
