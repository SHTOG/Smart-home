#ifndef __MYLIST_H
#define __MYLIST_H		
#include "sys.h"	  	 
#include "delay.h"

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

Device* CreateList(void);
Device* CreateNode(u8 type, u8 onlineFlag, u8* LongAddr,u8* ShortAddr) ;
void InsertNodeByType(Device* headNode, u8 type, u8 onlineFlag, u8* LongAddr,u8* ShortAddr);
void DeleteNodeByLongAddr(Device* headNode, u8* LongAddr);
u8 CheckByLongAddr(Device* headNode, u8* LongAddr, u8* ShortAddr);
u8 CheckByOnlineFlag(Device* headNode);
#endif  
