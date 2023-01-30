#ifndef __MYLIST_H
#define __MYLIST_H			  	 
#include "delay.h"
#include "sys.h"
#include "string.h"
#include "stdlib.h"

typedef struct myDevice{
    //������
    u8 type;            //��������
    u8 onlineFlag;      //������־��0-δ������1-������
    u8 LongAddr[8];     //Zigbee�豸����ַ
    u8 ShortAddr[2];    //Zigbee�豸�̵�ַ
    //ָ����
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
