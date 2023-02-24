#ifndef __MYLIST_H
#define __MYLIST_H		
#include "sys.h"	  	 
#include "delay.h"

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

Device* CreateList(void);
Device* CreateNode(u8 type, u8 onlineFlag, u8* LongAddr,u8* ShortAddr) ;
void InsertNodeByType(Device* headNode, u8 type, u8 onlineFlag, u8* LongAddr,u8* ShortAddr);
void DeleteNodeByLongAddr(Device* headNode, u8* LongAddr);
u8 CheckByLongAddr(Device* headNode, u8* LongAddr, u8* ShortAddr);
u8 CheckByOnlineFlag(Device* headNode);
#endif  
