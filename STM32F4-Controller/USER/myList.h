#ifndef __MYLIST_H
#define __MYLIST_H		
#include "sys.h"	  	 
#include "delay.h"
#include "Zigbee.h"
#include "24Cxx.h"
Scenes* CreateSceneNode(Scene* newScene);
Scenes* CreateSceneList(void);
void InsertSceneNodeByEnd(Scenes* SceneList, u8 SceneNameLen, u8* SceneName);
void InsertSceneMemberNodeByFlag_User(Scenes* SceneList, u8 SceneNameLen, u8* SceneName, u8 Flag, u8 DataLen, u8* Data);
void DeleteSceneMemberNodeByData_User(Scenes* SceneList, u8 SceneNameLen, u8* SceneName, u8 DataLen, u8* Data);
void DeleteSceneNodeBySceneName(Scenes* SceneList, u8 SceneNameLen, u8* SceneName);
void StartScene(Scenes* SceneList, u8 SceneNameLen, u8* SceneName);
void StopScene(Scenes* SceneList, u8 SceneNameLen, u8* SceneName);

Scene* CreateSceneMemberNode(u8 Flag, u8 DataLen, u8* Data);
Scene* CreateSceneMemberList(u8 DataLen,u8* Data);
void InsertSceneMemberNodeByFlag_Exe(Scene* headNode, u8 Flag, u8 DataLen, u8* Data);
void DeleteSceneMemberNodeByData_Exe(Scene* headNode, u8 DataLen, u8* Data) ;
void DeleteSceneMemberNodeByEnd(Scene* headNode);


TerminalStream* CreateTerminalStreamNode(u8* SLAddr, u8 type, u8 len, u8* Data);
TerminalStream* CreateTerminalStreamList(void);
void InsTerminalStreamNodeByEnd(TerminalStream* headNode,u8* SLAddr,u8 type, u8 len, u8* Data);
void HandleTerminalStream(TerminalStream* headNode);

Esp32CommandStream* CreateEsp32CommandStreamNode(u8* DSAddr, u8 type, u8 len, u8* Data, u8 DataDirection) ;
Esp32CommandStream* CreateEsp32CommandStreamList(void) ;
void InsertEsp32CommandStreamNodeByEnd(Esp32CommandStream* headNode,u8* DSAddr, u8 type, u8 len, u8* Data, u8 DataDirection);
void HandleEsp32CommandStream(Esp32CommandStream* headNode);

Device* CreateDeviceList(void);
Device* CreateDeviceNode(u8 type, u8 onlineFlag, u8* LongAddr, u8* ShortAddr, u8 PosNameLen, u8* PosName, u8 SerialNumber);
void InsertDeviceNodeByType(Device* headNode, u8 type, u8 onlineFlag, u8* LongAddr, u8* ShortAddr, u8 PosNameLen, u8* PosName, u8 SerialNumber);
void DeleteDeviceNodeByLongAddr(Device* headNode, u8* LongAddr);
u8 CheckDeviceNodeByLongAddr(Device* headNode, u8* LongAddr, u8* ShortAddr);
void UpdateDeviceList(Device* headNode);
void PrintDeviceList(Device* headNode);
void SetDeviceOnlineFlagBySAddr(Device* headNode, u8* ShortAddr);
 void SetDevicePositionByLAddr(Device* HeadNode, u8* LongAddr, u8 PosNameLen, u8* PosName, u8 SerialNumber);
#endif
