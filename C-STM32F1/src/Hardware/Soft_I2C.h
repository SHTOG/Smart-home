#ifndef __SOFT_I2C_H
#define __SOFT_I2C_H

void Soft_I2C_Init(void);
void Soft_I2C_Start(void);
void Soft_I2C_Stop(void);
void Soft_I2C_SendByte(uint8_t Byte);
uint8_t Soft_I2C_ReceiveByte(void);
void Soft_I2C_SendAck(uint8_t AckBit);
uint8_t Soft_I2C_ReceiveAck(void);

#endif
