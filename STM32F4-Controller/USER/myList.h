#ifndef __MYLIST_H
#define __MYLIST_H		
#include "sys.h"	  	 
#include "delay.h"
#include "Zigbee.h"
#include "24Cxx.h"

TerminalStream* CreateTermStreamNode(u8 type, u8 len, u8* Data); 
TerminalStream* CreateTerminalStreamList(void);
void InsTerminalStreamNodeByEnd(TerminalStream* headNode,u8* SLAddr,u8 type, u8 len, u8* Data);
void HandleTerminalStream(TerminalStream* headNode);

Esp32CommandStream* CreateEsp32CommandStreamNode(u8* DSAddr, u8 type, u8 len, u8* Data, u8 DataDirection) ;
Esp32CommandStream* CreateEsp32CommandStreamList(void) ;
void InsertEsp32CommandStreamNodeByEnd(Esp32CommandStream* headNode,u8* DSAddr, u8 type, u8 len, u8* Data, u8 DataDirection);
void HandleEsp32CommandStream(Esp32CommandStream* headNode);

Device* CreateDeviceList(void);
Device* CreateDeviceNode(u8 type, u8 onlineFlag, u8* LongAddr,u8* ShortAddr) ;
void InsertDeviceNodeByType(Device* headNode, u8 type, u8 onlineFlag, u8* LongAddr,u8* ShortAddr);
void DeleteDeviceNodeByLongAddr(Device* headNode, u8* LongAddr);
u8 CheckDeviceNodeByLongAddr(Device* headNode, u8* LongAddr, u8* ShortAddr);
void UpdateDeviceList(Device* headNode);
void PrintDeviceList(Device* headNode);
void SetDeviceOnlineFlagBySAddr(Device* headNode, u8* ShortAddr);
#endif 
