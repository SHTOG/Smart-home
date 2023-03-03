#include "led.h"   


u8 LED1FlashTime = 0;//LED1��˸ʣ��ʱ��,��0ʱֹͣ��˸

//���ж���IO�ڿ��Ƶ�LED�ܵĳ�ʼ����
void LED_Init(void){
	LED1_Init();
	LED2_Init();
}

void LED1_Init(void){
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(LED1_Clock, ENABLE);	 		//ʹ�ܶ˿�ʱ��
	
	GPIO_InitStructure.GPIO_Pin = LED1_Pin;				//LED1�˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 	//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//IO���ٶ�Ϊ50MHz
	GPIO_Init(LED1_GPIO, &GPIO_InitStructure);			//�����趨������ʼ��
	GPIO_SetBits(LED1_GPIO,LED1_Pin);					//LED1�����
}


void LED2_Init(void){
	GPIO_InitTypeDef  GPIO_InitStructure;

 	RCC_APB2PeriphClockCmd(LED2_Clock, ENABLE);	 		//ʹ�ܶ˿�ʱ��

	GPIO_InitStructure.GPIO_Pin = LED2_Pin;	    		//LED2�˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 	//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//IO���ٶ�Ϊ50MHz
	GPIO_Init(LED2_GPIO, &GPIO_InitStructure);	  		//�����趨������ʼ��
	GPIO_SetBits(LED2_GPIO,LED2_Pin); 					//LED2 ����� 
}

/**
  * @brief    ��һ��led����˸�𵽲��Գ����������
  * @param    GPIO_TypeDef* GPIOx��uint16_t GPIO_Pin: led����io��
  * @param    led�����л�������ʱ�䣨ms��
  * @retval    
  */
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
