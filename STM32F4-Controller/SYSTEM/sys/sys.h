#ifndef __SYS_H
#define __SYS_H	 
#include "stm32f4xx.h" 
#include "stdlib.h"
#include "string.h"

#define MAX_DATA_FROM_ESP32_LEN 20   //与esp32通信数据帧的有效数据

//设备信息链表结点
typedef struct myDevice{
    //数据域
    u8 type;            //电器类型
    u8 onlineFlag;      //入网标志，0-未入网，1-已入网
    u8 LongAddr[8];     //Zigbee设备长地址
    u8 ShortAddr[2];    //Zigbee设备短地址
    //指针域
    struct myDevice* next;
}Device;


//与Esp32间通信数据流链表结点
typedef struct CommandStreamWithEsp32{
    //数据域
    u8 DSAddr[2];//指令终端短地址
    u8 type;//指令终端设备类型码
    u8 len;//有效数据长度
    u8 Data[MAX_DATA_FROM_ESP32_LEN];//有效数据
    u8 DataDirection;//数据传输方向,1->来自ESP32   2->准备发往Esp32
    //指针域
    struct CommandStreamWithEsp32* next;
}Esp32CommandStream;


//来自终端的数据流链表结点
typedef struct CommandStreamFromTerminal{
    //数据域
    u8 type;//指令终端设备类型码
    u8 len;//有效数据长度
    u8 Data[MAX_DATA_FROM_ESP32_LEN];//有效数据
    //指针域
    struct CommandStreamFromTerminal* next;
}TerminalCommandStream;

extern u8 AckFlag;
extern u8 AckJudge;
extern Device* DeviceList;
extern u8 Esp32AckFlag;
extern u8 AckFlag;
extern void Send_Custom_Data(USART_TypeDef* USARTx, u8 type, u8 len, u8* Data);
extern u8  USART1_RX_BUF[200]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART1_RX_STA;         		//接收状态标记	
extern u8  USART2_RX_BUF[200]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART2_RX_STA;         		//接收状态标记	
extern Device* DeviceList;
extern u8 SelfLongAddr[8];
extern u8 BootedTimeFlag;
extern u8 AckFlag;
extern u8 USART1ReciveITEnable;
extern u8 USART2ReciveITEnable;
extern u8 CurtainDeep;
extern u8 AckJudge;
extern u8 Esp32AckFlag;
extern u8 SelfLongAddr[8];
extern u8 SelfShortAddr[2];
extern u8 NetFlag;
extern u8 EnterModeFlag;
extern u8 GetStateFlag;//读取模块状态标志位，读取成功置1
extern u8 SetSendTargetFlag;//设置透传目标标志位，分两步，先设置目标短地址，再设置目标端口，短地址设置成功后置1，端口设置成功后置0
extern u8 ReadySetTargetFlag;//成功设置透传目标标志位，1为已设置，0为待设置
extern u16 MilliSecond;//毫秒级计数器
extern u8 Second;//秒级计数器
extern u8 Minute;//分级计数器
extern u8 WaitTime;//秒级等待应答时间,置零时开始计时,计到3停止
extern u8 EspWaitTime;//秒级等待应答时间,置零时开始计时,计到5停止
extern Esp32CommandStream* Esp32CommandStreamList;//与Esp32间通信数据流链表


//0,不支持ucos
//1,支持ucos
#define SYSTEM_SUPPORT_OS		0		//定义系统文件夹是否支持UCOS
																	    
	 
//位带操作,实现51类似的GPIO控制功能
//具体实现思想,参考<<CM3权威指南>>第五章(87页~92页).M4同M3类似,只是寄存器地址变了.
//IO口操作宏定义
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//IO口地址映射
#define GPIOA_ODR_Addr    (GPIOA_BASE+20) //0x40020014
#define GPIOB_ODR_Addr    (GPIOB_BASE+20) //0x40020414 
#define GPIOC_ODR_Addr    (GPIOC_BASE+20) //0x40020814 
#define GPIOD_ODR_Addr    (GPIOD_BASE+20) //0x40020C14 
#define GPIOE_ODR_Addr    (GPIOE_BASE+20) //0x40021014 
#define GPIOF_ODR_Addr    (GPIOF_BASE+20) //0x40021414    
#define GPIOG_ODR_Addr    (GPIOG_BASE+20) //0x40021814   
#define GPIOH_ODR_Addr    (GPIOH_BASE+20) //0x40021C14    
#define GPIOI_ODR_Addr    (GPIOI_BASE+20) //0x40022014     

#define GPIOA_IDR_Addr    (GPIOA_BASE+16) //0x40020010 
#define GPIOB_IDR_Addr    (GPIOB_BASE+16) //0x40020410 
#define GPIOC_IDR_Addr    (GPIOC_BASE+16) //0x40020810 
#define GPIOD_IDR_Addr    (GPIOD_BASE+16) //0x40020C10 
#define GPIOE_IDR_Addr    (GPIOE_BASE+16) //0x40021010 
#define GPIOF_IDR_Addr    (GPIOF_BASE+16) //0x40021410 
#define GPIOG_IDR_Addr    (GPIOG_BASE+16) //0x40021810 
#define GPIOH_IDR_Addr    (GPIOH_BASE+16) //0x40021C10 
#define GPIOI_IDR_Addr    (GPIOI_BASE+16) //0x40022010 
 
//IO口操作,只对单一的IO口!
//确保n的值小于16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //输出 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //输入 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //输出 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //输入 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //输出 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //输入 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //输出 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //输入

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //输出 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //输入

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //输出 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //输入

#define PHout(n)   BIT_ADDR(GPIOH_ODR_Addr,n)  //输出 
#define PHin(n)    BIT_ADDR(GPIOH_IDR_Addr,n)  //输入

#define PIout(n)   BIT_ADDR(GPIOI_ODR_Addr,n)  //输出 
#define PIin(n)    BIT_ADDR(GPIOI_IDR_Addr,n)  //输入

//以下为汇编函数
void WFI_SET(void);		//执行WFI指令
void INTX_DISABLE(void);//关闭所有中断
void INTX_ENABLE(void);	//开启所有中断
void MSR_MSP(u32 addr);	//设置堆栈地址 
#endif











