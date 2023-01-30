#ifndef __MYLIST_H
#define __MYLIST_H			  	 
#include "delay.h"
#include "sys.h"
#include "string.h"
#include "stdlib.h"

typedef struct myDevice{
    //数据域
    u8 type;            //电器类型
    u8 onlineFlag;      //入网标志，0-未入网，1-已入网
    u8 LongAddr[8];     //Zigbee设备长地址
    u8 ShortAddr[2];    //Zigbee设备短地址
    //指针域
    struct myDevice* next;
}Device;

extern Device* DeviceList;

Device* createList(void);
Device* createNode(u8 type, u8 onlineFlag, u8* LongAddr,u8* ShortAddr) ;
void insertNodeByType(Device* headNode, u8 type, u8 onlineFlag, u8* LongAddr,u8* ShortAddr);
void deleteNodeByLongAddr(Device* headNode, u8* LongAddr);
u8 checkByLongAddr(Device* headNode, u8* LongAddr, u8* ShortAddr);
void printList(Device* headNode);
#endif  
