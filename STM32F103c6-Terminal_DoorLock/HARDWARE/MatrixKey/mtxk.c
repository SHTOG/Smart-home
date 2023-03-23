#include "mtxk.h"
#include "timer.h"
u16 MTXKCD = 0;//用于矩阵键盘消抖

//矩阵键盘IO占用情况：矩阵键盘:1~4(A0~3)、5~8(A4~7)
void MTXK_Init(void){
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);	 //使能PA,PB端口时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;//端口设置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 							//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 							//IO口速度为50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);					 							//根据设定参数初始化GPIOB
	GPIO_SetBits(GPIOA,GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3);			//输出高

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;	//端口设置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;									//下拉输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);//根据设定参数初始化GPIOA
}

u8 MTXK_Scan(void){
	u8 temp;
	u8 col = 1;
	GPIO_ResetBits(GPIOA,GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);//拉低列线
	temp = GPIO_ReadInputData(GPIOA) & 0x00FF;
	if(temp != 0x0F){//说明有按键被按下
		if(MTXKCD <= MilliSecond){
			MTXKCD = MilliSecond + 150;
			temp >>= 4;
			while((temp & 0x01) != 0x01){//列扫描
				temp >>= 1;
				col++;
			}//发现col列电平为高
			//行扫描
			GPIO_ResetBits(GPIOA,GPIO_Pin_0);//拉低第一行
			temp = GPIO_ReadInputData(GPIOA) & 0x00FF;//获取PA的0~7位情况
			GPIO_SetBits(GPIOA,GPIO_Pin_0);//重新拉高
			if((temp | 0x0F) == 0x0F){
				switch(col){
					case 1: return 1;
					case 2: return 2;
					case 3: return 3;
					case 4:	return 4;
					default:return 0;
				}
			}
			GPIO_ResetBits(GPIOA,GPIO_Pin_1);//拉低第二行
			temp = GPIO_ReadInputData(GPIOA) & 0x00FF;//获取PA的0~7位情况
			GPIO_SetBits(GPIOA,GPIO_Pin_1);//重新拉高
			if((temp | 0x0F) == 0x0F){
				switch(col){
					case 1: return 5;
					case 2: return 6;
					case 3: return 7;
					case 4:	return 8;
					default:return 0;
				}
			}
			GPIO_ResetBits(GPIOA,GPIO_Pin_2);//拉低第三行
			temp = GPIO_ReadInputData(GPIOA) & 0x00FF;//获取PA的0~7位情况
			GPIO_SetBits(GPIOA,GPIO_Pin_2);//重新拉高
			if((temp | 0x0F) == 0x0F){
				switch(col){
					case 1: return 9;
					case 2: return 10;
					case 3: return 11;
					case 4:	return 12;
					default:return 0;
				}
			}
			GPIO_ResetBits(GPIOA,GPIO_Pin_3);//拉低第四行
			temp = GPIO_ReadInputData(GPIOA) & 0x00FF;//获取PA的0~7位情况
			GPIO_SetBits(GPIOA,GPIO_Pin_3);//重新拉高
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
