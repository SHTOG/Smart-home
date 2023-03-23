#ifndef __IIC_H
#define __IIC_H
#include "sys.h"
#include "delay.h"

//移植性极高的软件IIC
#define IIC_Port     GPIOB
#define IIC_Port_RCC RCC_APB2Periph_GPIOB
#define SCL_Pin      GPIO_Pin_1
#define SDA_Pin      GPIO_Pin_0
#define IIC_Delay()  delay_us(10)

void Soft_IIC_Init(void);
void Soft_IIC_Start(void);
void Soft_IIC_Stop(void);
void Soft_IIC_SendByte(uint8_t Byte);
uint8_t Soft_IIC_ReceiveByte(void);
void Soft_IIC_SendAck(uint8_t AckBit);
uint8_t Soft_IIC_ReceiveAck(void);
void Soft_IIC_W_SCL(uint8_t BitValue);
void Soft_IIC_W_SDA(uint8_t BitValue);
uint8_t Soft_IIC_R_SDA(void);
#endif
