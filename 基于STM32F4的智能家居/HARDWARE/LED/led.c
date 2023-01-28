#include "led.h"   
#include "delay.h"

//��ʼ�� PF9 �� PF10 Ϊ�����.��ʹ���������ڵ�ʱ��            
//LED IO ��ʼ�� 
void LED_Init(void) 
{            
    GPIO_InitTypeDef    GPIO_InitStructure; 
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);//ʹ�� GPIOF ʱ�� 
    //GPIOF9,F10 ��ʼ������ 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;//LED0 �� LED1 ��Ӧ IO �� 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ 
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//������� 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz 
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//���� 
    GPIO_Init(GPIOF, &GPIO_InitStructure);//��ʼ�� GPIO
	GPIO_SetBits(GPIOF,GPIO_Pin_9 | GPIO_Pin_10);//GPIOF9,F10 ���øߣ����� 
}
extern void Send_Custom_Data(u8 type, u8 len, u8* Data);
void LED_Test(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin,u16 xms){
	GPIO_ResetBits(GPIOx,GPIO_Pin);
    delay_ms(xms);
    GPIO_SetBits(GPIOx,GPIO_Pin);
    delay_ms(xms);
	
}
