#ifndef __PN532_IIC_H
#define __PN532_IIC_H			  	 
#include "sys.h"
#include "iic.h"
#include "delay.h"

#define PN532_RESET_RCC             RCC_APB2Periph_GPIOB
#define PN532_RESET_PORT            GPIOB
#define PN532_RESET_PIN             GPIO_Pin_8

#define PN532_IRQ_RCC               RCC_APB2Periph_GPIOB
#define PN532_IRQ_PORT              GPIOB
#define PN532_IRQ_PIN               GPIO_Pin_5

#define PN532_PREAMBLE              0x00
#define PN532_STARTCODE1            0x00
#define PN532_STARTCODE2            0xFF

#define Master_TO_PN532              0xD4
#define PN532_TO_Master              0xD5

/*Miscellaneous*/
#define CMD_DIAGONOSE               0x00
#define CMD_GET_FW_VERSION          0x02
#define CMD_GET_GEN_STATUS          0x04
#define CMD_READ_REG                0x06
#define CMD_WRITE_REG               0x08
#define CMD_READ_GPIO               0x0C
#define CMD_WRITE_GPIO              0x0E
#define CMD_SET_SERIAL_RATE         0x10
#define CMD_SET_PARAM               0x12
#define CMD_SAM_CONFIG              0x14
#define CMD_POWERDOWN               0x16

/*RF Communication*/
#define CMD_RFConfig                0x32
#define CMD_RFRegationTest          0x58

/*Initiator*/
#define CMD_IN_JUMP_FOR_DEP         0x56
#define CMD_In_JUMP_FOR_PSL         0x46
#define CMD_IN_LIST_PASSIVE_TARGET  0x4a
#define CMD_IN_ATR                  0x50
#define CMD_IN_PSL                  0x4E
#define CMD_IN_DATA_EXCHANGE        0x40
#define CMD_IN_COMM_THRU            0x42
#define CMD_IN_DESELECT             0x44
#define CMD_IN_RELEASE              0x52
#define CMD_IN_SELECET              0x54
#define CMD_IN_AUTOPOLL             0x60

#define FIND_NFCCARD_MAXNUM         0x01
#define NFC_106K_PROTOCOL           0x00

#define  PN532_I2C_ADDR             (0x48) 

#define PN532_IIC_Init()              Soft_IIC_Init();//IIC函数调用源
typedef enum
{
    PN532_OK = 0,
    PN532_TIMEOUT = -1,
    PN532_DATA_ERROR = -2,
}PN532_RES;

void PN532_GPIO_Init(void);
void PN532_Reset(void);
PN532_RES PN532_Get_Version(uint8_t *ver);
PN532_RES PN532_SAMConfig(void);
PN532_RES PN532_ReadPassiveTargetID(uint8_t *card_id, uint8_t *id_len);









//#define PN532_MAX_BUFFERSIZE 200

////一个PN532数据帧
//typedef struct t_PN532_FRAME_INFO{
//    u8  Type;        /* 00:ack, ff:nack, d4:mcu->pn532(d4), d5:mcu<-pn532(d5) */        
//    u8  DataSize;    /* DATA's length */    
//    u8  Data[PN532_MAX_BUFFERSIZE];  /* DATA */
//    u8  Cmd;
//}PN532FrameInfo;

////对于整个工程来说的一个总体PN532结构体对象
//typedef struct t_PN532{
//    vu32 StatusFlag;    
//    vu8  RxBufferSize;
//    vu8  TxBufferSize;
//    vu8  RxCurrentPos;
//    PN532FrameInfo RxFrameInfo;
//    PN532FrameInfo TxFrameInfo;

//    u8  RxBuffer[PN532_MAX_BUFFERSIZE];
//    u8  TxBuffer[PN532_MAX_BUFFERSIZE];
//}PN532;

#endif  
