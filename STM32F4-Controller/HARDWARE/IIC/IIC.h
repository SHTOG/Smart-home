#ifndef __IIC_H__
#define __IIC_H__
#include "sys.h"
#include "delay.h"

//SWIIC GPIO定义
#define IIC_SCL_GPIO	GPIOD
#define IIC_SCL_Pin		GPIO_Pin_2
#define IIC_SCL_GPIOCLK	RCC_AHB1Periph_GPIOD
#define IIC_SDA_GPIO	GPIOD
#define IIC_SDA_Pin		GPIO_Pin_3
#define IIC_SDA_GPIOCLK	RCC_AHB1Periph_GPIOD
//IO方向设置（仅适用于F4）
#define SDA_IN()  {unsigned char n = 0;while(IIC_SDA_Pin>>n != 1){n++;}IIC_SDA_GPIO->MODER&=~(3<<(n*2));IIC_SDA_GPIO->MODER|=0<<n*2;}	//IIC_SDA_GPIO_Pin3输入模式
#define SDA_OUT() {unsigned char n = 0;while(IIC_SDA_Pin>>n != 1){n++;}IIC_SDA_GPIO->MODER&=~(3<<(n*2));IIC_SDA_GPIO->MODER|=1<<n*2;} //IIC_SDA_GPIO_Pin3输出模式
//IO操作函数
#define IIC_SCL_High    GPIO_SetBits(IIC_SCL_GPIO,IIC_SCL_Pin) //SCL拉高
#define IIC_SCL_Low		GPIO_ResetBits(IIC_SCL_GPIO,IIC_SCL_Pin) //SCL拉低
#define IIC_SDA_High    GPIO_SetBits(IIC_SDA_GPIO,IIC_SDA_Pin) //SDA拉高
#define IIC_SDA_Low		GPIO_ResetBits(IIC_SDA_GPIO,IIC_SDA_Pin) //SDA拉低
#define READ_SDA   		GPIO_ReadInputDataBit(IIC_SDA_GPIO,IIC_SDA_Pin)  //读取SDA输入电频

void IIC_Init(void);
void IIC_Start(void);
void IIC_Stop(void);
void IIC_Send_Byte(u8 Byte);
u8 IIC_Read_Byte(u8 ack);
u8 IIC_Wait_Ack(void);
void IIC_Ack(void);
void IIC_NAck(void);
#endif
