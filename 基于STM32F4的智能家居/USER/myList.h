#ifndef __MYLIST_H
#define __MYLIST_H			  	 
#include "delay.h"
#include "sys.h"
#include "string.h"
#include "stdlib.h"
#include "AnalyseAndSend.h"

typedef struct myDevice{
    // ˝æ›”Ú
    u8 type;
    u8 OnlineFlag;
    u8 LongAddr[8];
    u8 ShortAddr[2];
    //÷∏’Î”Ú
    struct myDevice* next;
}Device;

extern Device* DeviceList;

Device* createList(void);
Device* createNode(u8 type, u8* LongAddr,u8* ShortAddr) ;
void insertNodeByType(Device* headNode, u8 type, u8* LongAddr,u8* ShortAddr);
void deleteNodeByLongAddr(Device* headNode, u8* LongAddr);
u8 checkByLongAddr(Device* headNode, u8* LongAddr);
void printList(Device* headNode);
#endif  
