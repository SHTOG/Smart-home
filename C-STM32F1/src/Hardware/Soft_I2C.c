#include "stm32f10x.h" // Device header
#include "Delay.h"

#define I2C_Port     GPIOB
#define I2C_Port_RCC RCC_APB2Periph_GPIOB
#define SCL_Pin      GPIO_Pin_1
#define SDA_Pin      GPIO_Pin_0
#define I2C_Delay()  Delay_us(10)

void Soft_I2C_W_SCL(uint8_t BitValue)
{
    GPIO_WriteBit(GPIOB, SCL_Pin, (BitAction)BitValue);
    I2C_Delay();
}

void Soft_I2C_W_SDA(uint8_t BitValue)
{
    GPIO_WriteBit(GPIOB, SDA_Pin, (BitAction)BitValue);
    I2C_Delay();
}

uint8_t Soft_I2C_R_SDA(void)
{
    uint8_t BitValue;
    BitValue = GPIO_ReadInputDataBit(GPIOB, SDA_Pin);
    I2C_Delay();
    return BitValue;
}

void Soft_I2C_Init(void)
{
    RCC_APB2PeriphClockCmd(I2C_Port_RCC, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_OD;
    GPIO_InitStructure.GPIO_Pin   = SCL_Pin | SDA_Pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(I2C_Port, &GPIO_InitStructure);

    GPIO_SetBits(I2C_Port, SCL_Pin | SDA_Pin);
}

void Soft_I2C_Start(void)
{
    Soft_I2C_W_SDA(1);
    Soft_I2C_W_SCL(1);
    Soft_I2C_W_SDA(0);
    Soft_I2C_W_SCL(0);
}

void Soft_I2C_Stop(void)
{
    Soft_I2C_W_SDA(0);
    Soft_I2C_W_SCL(1);
    Soft_I2C_W_SDA(1);
}

void Soft_I2C_SendByte(uint8_t Byte)
{
    uint8_t i;
    for (i = 0; i < 8; i++) {
        Soft_I2C_W_SDA(Byte & (0x80 >> i));
        Soft_I2C_W_SCL(1);
        Soft_I2C_W_SCL(0);
    }
}

uint8_t Soft_I2C_ReceiveByte(void)
{
    uint8_t i, Byte = 0x00;
    Soft_I2C_W_SDA(1);
    for (i = 0; i < 8; i++) {
        Soft_I2C_W_SCL(1);
        if (Soft_I2C_R_SDA()) { Byte |= (0x80 >> i); }
        Soft_I2C_W_SCL(0);
    }
    return Byte;
}

void Soft_I2C_SendAck(uint8_t AckBit)
{
    Soft_I2C_W_SDA(AckBit);
    Soft_I2C_W_SCL(1);
    Soft_I2C_W_SCL(0);
}

uint8_t Soft_I2C_ReceiveAck(void)
{
    uint8_t AckBit;
    Soft_I2C_W_SDA(1);
    Soft_I2C_W_SCL(1);
    AckBit = Soft_I2C_R_SDA();
    Soft_I2C_W_SCL(0);
    return AckBit;
}
