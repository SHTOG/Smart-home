#ifndef __MYLIST_H
#define __MYLIST_H		
#include "sys.h"	  	 
#include "delay.h"
#include "Zigbee.h"



Esp32CommandStream* CreateEsp32CommandStreamNode(u8* DSAddr, u8 type, u8 len, u8* Data, u8 DataDirection) ;
Esp32CommandStream* CreateEsp32CommandStreamList(void) ;
void InsertEsp32CommandStreamNodeByEnd(Esp32CommandStream* headNode,u8* DSAddr, u8 type, u8 len, u8* Data, u8 DataDirection);
void DeleteEsp32CommandStreamNodeByLongAddr(Esp32CommandStream* headNode, u8* LongAddr) ;
void HandleEsp32CommandStream(Esp32CommandStream* headNode);

Device* CreateDeviceList(void);
Device* CreateDeviceNode(u8 type, u8 onlineFlag, u8* LongAddr,u8* ShortAddr) ;
void InsertDeviceNodeByType(Device* headNode, u8 type, u8 onlineFlag, u8* LongAddr,u8* ShortAddr);
void DeleteDeviceNodeByLongAddr(Device* headNode, u8* LongAddr);
u8 CheckDeviceNodeByLongAddr(Device* headNode, u8* LongAddr, u8* ShortAddr);
u8 CheckDeviceNodeByOnlineFlag(Device* headNode);
void UpdateDeviceList(Device* headNode);
void PrintDeviceList(Device* headNode);
void SetDeviceOnlineFlagBySAddr(Device* headNode, u8* ShortAddr);
#endif  
