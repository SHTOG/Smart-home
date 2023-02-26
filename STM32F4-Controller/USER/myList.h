#ifndef __MYLIST_H
#define __MYLIST_H		
#include "sys.h"	  	 
#include "delay.h"
#include "Zigbee.h"




Device* CreateList(void);
Device* CreateNode(u8 type, u8 onlineFlag, u8* LongAddr,u8* ShortAddr) ;
void InsertNodeByType(Device* headNode, u8 type, u8 onlineFlag, u8* LongAddr,u8* ShortAddr);
void DeleteNodeByLongAddr(Device* headNode, u8* LongAddr);
u8 CheckByLongAddr(Device* headNode, u8* LongAddr, u8* ShortAddr);
u8 CheckByOnlineFlag(Device* headNode);
void UpdateList(Device* headNode);
void PrintList(Device* headNode);
void SetNetStateByShorAddr(Device* headNode, u8* ShortAddr);
#endif  
