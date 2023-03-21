#include "sys.h" 
#include "usart.h" 
#include "Zigbee.h"
#include "led.h"
#include "timer.h"
#include "exti.h"
#include "iwdg.h"
#include "key.h"

u8 ZigbeeOnlineFlag = 0;//����Flag,��0��ʾû������Э����,��1��ʾ������
u8 OnlineFlag = 0;//����Flag,��0��ʾû��������̬,��1��ʾ������
u8 APPJudgeFlag = 0;//����APP�������жϱ�־λ�����Ϊ1����ʾͬ�⣬Ϊ2��ʾ�ܾ�����ʱ��0

void Try_To_Link(){
	LED1FlashTime = 120;//��˸������,��ʾ���������
	Zigbee_Change_Mode(0);//Zigbee����HEXָ��ģʽ
	if(ZigbeeOnlineFlag == 1) Zigbee_Restore_Factory_Setting();//Zigbee�ָ���������
	Zigbee_Restart();
//	delay_ms(1000);
	IWDG_Feed();//ι��
	Zigbee_Set_Type_To_Active_Terminal();//����ģ������Ϊ��Ծ�ն�
	Zigbee_Restart();
	delay_ms(100);
	Zigbee_Open_Net();//������׼�����
	IWDG_Feed();//ι��
	delay_ms(4000);//��΢�ȴ�����
	IWDG_Feed();//ι��
	WaitTime = 0;
	ZigbeeOnlineFlag = 0;
	while(ZigbeeOnlineFlag != 1){
		IWDG_Feed();//ι��
		if(WaitTime >= 110){//����110sZigbeeû������Э����,�ж�Ϊ�п�û������ֱ���˳�while
			break;
		}
		Zigbee_Get_State();//��ȡ�豸״̬
		delay_ms(100);
	}
//	Zigbee_Close_Net();
	if(ZigbeeOnlineFlag == 1){//����Ѿ�������Э����
		Zigbee_Restart();
		Zigbee_Set_Send_Target();//��͸��Ŀ���ΪЭ����
		Zigbee_Change_Mode(1);//����͸��ģʽ
		
		//�ȴ�APPͬ���ܾ�
		APPJudgeFlag = 0;
		WaitTime = 0;
		while(APPJudgeFlag == 0){
			if(WaitTime >= 120){//120sû�յ�Ӧ��ֱ���˳�����ʾΪû������
				OnlineFlag = 0;
				return ;
			}
			Send_Custom_Data(0x00,3,SelfShortAddr);
			delay_ms(1000);//�������ݽ����ӳ�,����Ƶ�����͵����п�����ӵ��
		}
		if(APPJudgeFlag == 1) OnlineFlag = 1;//�����ɹ�
		else if(APPJudgeFlag == 2) OnlineFlag = 0;//����ʧ��
	}
	LED1FlashTime = 0;//ָʾ��ֹͣ��˸
	LED1 = 1;//Ϩ��
}

int main(void) {
	u8 i;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//����ϵͳ�ж����ȼ���Ϊ2
	delay_init();	    //��ʱ������ʼ��
	LED_Init();	  		//LED��ʼ��
//	IWDG_Init(6,1562); 	//Լ10��ιһ�ι���10�벻ι������λ
	Key_Init();			//������ʼ��
	USART1_Init(115200);//��ʼ������1(ͨ���ã��й�����Ҫ�õ������뻻��Ĵ��ں�)
	TIM2_Int_Init(10-1,7200-1);//��ʱ��ʱ��72M����Ƶϵ��7200������72M/7200=10Khz�ļ���Ƶ�ʣ�����10��Ϊ1ms
	Zigbee_Change_Mode(0);//Zigbee����HEXָ��ģʽ
	Zigbee_Get_State();//�������ִ�����õ�ZigbeeOnlineFlag��ֵ
	if(ZigbeeOnlineFlag == 1){
		Zigbee_Set_Send_Target();//����͸��Ŀ��Ϊ�п�
		Zigbee_Change_Mode(1);
		Zigbee_Update_OnlineFlag();//�������ִ�����õ�OnlineFlag��ֵ��Ϊ�����while(1)�ṩִ�з���
	}
	/*��ʼ���������������*/
	TIM3_PWM_Init(500-1,72-1);//72M/72=1Mhz�ļ���Ƶ��,��װ��ֵ500������PWMƵ��Ϊ 1M/500=2Khz.(����Ϊ500us)
	EXTI0_Init();
	/*��ʼ���������������*/
	LED2 = 0;//�����ã��ɴӺ��İ���ֱ�ӹ۲⵽���������while(1)
	while (1){
		if(Key1 == 0){//�������1������
			WaitTime = 0;
			while(Key1 == 0){//������ֳ���
				if(WaitTime >= 3){//�������������3s��ʼ����
					Try_To_Link();//��������
					break;
				}
			}
		}
		IWDG_Feed();//ι��
		/*���������д�Լ��ն˵Ĵ���*/
		// ��Ƹ���ģʽ����
		for(i = 0; i < 4; i++){
			if(LEDmode[i] == 0){
				TIM_SetCompare[i](TIM3,PWMval[i]);	//�޸ıȽ�ֵ���޸�ռ�ձ�
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
		/*���������д�Լ��ն˵Ĵ���*/
		if(OnlineFlag == 1){
			LED1 = 0;//������
			
		}
		else if(OnlineFlag == 0)LED1 = 1;
	}
	
}
