#include "mtxk.h"
#include "timer.h"
u16 MTXKCD = 0;//���ھ����������

//�������IOռ��������������:1~4(A0~3)��5~8(A4~7)
void MTXK_Init(void){
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PA,PB�˿�ʱ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;//�˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 							//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 							//IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);					 							//�����趨������ʼ��GPIOB
	GPIO_SetBits(GPIOA,GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3);			//�����

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;	//�˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;									//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//�����趨������ʼ��GPIOA
}

u8 MTXK_Scan(void){
	u8 temp;
	u8 col = 1;
	GPIO_ResetBits(GPIOA,GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);//��������
	temp = GPIO_ReadInputData(GPIOA) & 0x00FF;
	if(temp != 0x0F){//˵���а���������
		if(MTXKCD <= MilliSecond){
			MTXKCD = MilliSecond + 150;
			temp >>= 4;
			while((temp & 0x01) != 0x01){//��ɨ��
				temp >>= 1;
				col++;
			}//����col�е�ƽΪ��
			//��ɨ��
			GPIO_ResetBits(GPIOA,GPIO_Pin_0);//���͵�һ��
			temp = GPIO_ReadInputData(GPIOA) & 0x00FF;//��ȡPA��0~7λ���
			GPIO_SetBits(GPIOA,GPIO_Pin_0);//��������
			if((temp | 0x0F) == 0x0F){
				switch(col){
					case 1: return 1;
					case 2: return 2;
					case 3: return 3;
					case 4:	return 4;
					default:return 0;
				}
			}
			GPIO_ResetBits(GPIOA,GPIO_Pin_1);//���͵ڶ���
			temp = GPIO_ReadInputData(GPIOA) & 0x00FF;//��ȡPA��0~7λ���
			GPIO_SetBits(GPIOA,GPIO_Pin_1);//��������
			if((temp | 0x0F) == 0x0F){
				switch(col){
					case 1: return 5;
					case 2: return 6;
					case 3: return 7;
					case 4:	return 8;
					default:return 0;
				}
			}
			GPIO_ResetBits(GPIOA,GPIO_Pin_2);//���͵�����
			temp = GPIO_ReadInputData(GPIOA) & 0x00FF;//��ȡPA��0~7λ���
			GPIO_SetBits(GPIOA,GPIO_Pin_2);//��������
			if((temp | 0x0F) == 0x0F){
				switch(col){
					case 1: return 9;
					case 2: return 10;
					case 3: return 11;
					case 4:	return 12;
					default:return 0;
				}
			}
			GPIO_ResetBits(GPIOA,GPIO_Pin_3);//���͵�����
			temp = GPIO_ReadInputData(GPIOA) & 0x00FF;//��ȡPA��0~7λ���
			GPIO_SetBits(GPIOA,GPIO_Pin_3);//��������
			if((temp | 0x0F) == 0x0F){
				switch(col){
					case 1: return 13;
					case 2: return 14;
					case 3: return 15;
					case 4:	return 16;
					default:return 0;
				}
			}
		}
		else return 0;
	}
	return 0;
}
