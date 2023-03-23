#include "iic.h"

//移植性极高的软件IIC

void Soft_IIC_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(IIC_Port_RCC, ENABLE);

    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_OD;
    GPIO_InitStructure.GPIO_Pin   = SCL_Pin | SDA_Pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(IIC_Port, &GPIO_InitStructure);

    GPIO_SetBits(IIC_Port, SCL_Pin | SDA_Pin);
}

void Soft_IIC_Start(void)
{
    Soft_IIC_W_SDA(1);
    Soft_IIC_W_SCL(1);
    Soft_IIC_W_SDA(0);
    Soft_IIC_W_SCL(0);
}

void Soft_IIC_Stop(void)
{
    Soft_IIC_W_SDA(0);
    Soft_IIC_W_SCL(1);
    Soft_IIC_W_SDA(1);
}

void Soft_IIC_SendByte(uint8_t Byte)
{
    uint8_t i;
    for (i = 0; i < 8; i++) {
        Soft_IIC_W_SDA(Byte & (0x80 >> i));
        Soft_IIC_W_SCL(1);
        Soft_IIC_W_SCL(0);
    }
}

uint8_t Soft_IIC_ReceiveByte(void)
{
    uint8_t i, Byte = 0x00;
    Soft_IIC_W_SDA(1);
    for (i = 0; i < 8; i++) {
        Soft_IIC_W_SCL(1);
        if (Soft_IIC_R_SDA()) { Byte |= (0x80 >> i); }
        Soft_IIC_W_SCL(0);
    }
    return Byte;
}

void Soft_IIC_SendAck(uint8_t AckBit)
{
    Soft_IIC_W_SDA(AckBit);
    Soft_IIC_W_SCL(1);
    Soft_IIC_W_SCL(0);
}

uint8_t Soft_IIC_ReceiveAck(void)
{
    uint8_t AckBit;
    Soft_IIC_W_SDA(1);
    Soft_IIC_W_SCL(1);
    AckBit = Soft_IIC_R_SDA();
    Soft_IIC_W_SCL(0);
    return AckBit;
}

void Soft_IIC_W_SCL(uint8_t BitValue)
{
    GPIO_WriteBit(GPIOB, SCL_Pin, (BitAction)BitValue);
    IIC_Delay();
}

void Soft_IIC_W_SDA(uint8_t BitValue)
{
    GPIO_WriteBit(GPIOB, SDA_Pin, (BitAction)BitValue);
    IIC_Delay();
}

uint8_t Soft_IIC_R_SDA(void)
{
    uint8_t BitValue;
    BitValue = GPIO_ReadInputDataBit(GPIOB, SDA_Pin);
    IIC_Delay();
    return BitValue;
}