#include "myList.h"
#include "delay.h"
#include "Zigbee.h"
#include "24Cxx.h"

u8 APPOpenNetCountDown = 0;//APP开放终端入网倒计时（单位秒），当APP打开入网许可时，倒计时增加到120，这期间终端发来的设备信息命令才会被分析执行
u8 APPJudgeFlag = 0;//来自APP的入网判断标志位，如果为1，表示同意，为2表示拒绝，闲时置0
/**
  * @brief    数组拷贝
  * @param    len : 拷贝的位数(需小于dest和src所指数组的长度）
  * @param    dest: 数组拷贝目的地
  * @param    src : 源数组
  * @retval   void
  */
void ArrCpy(u8 len, u8* dest, u8*src){
	u8 i;
	for(i = 0 ; i < len; i++){
		dest[i] = src[i];
	}
}

/**
  * @brief    数组对比
  * @param    len : 对比的前n位数(需小于dest和src所指数组的长度）
  * @param    dest: 数组对比目的地
  * @param    src : 源数组
  * @retval   1   : 两个数组前len位相同
  * @retval   0   : 前len位有不同元素
  */
u8 ArrCmp(u8 len, u8* dest, u8*src){
	u8 i;
	for(i = 0; i < len; i++){
		if(dest[i] != src[i]) return 0;
	}
	return 1;
}

/*********************************以下为对传感器数据的链表操作函数*********************************/
//创建结点
SensingData* CreateSensingDataNode(u8* SLAddr, u8 type, u8 newDataLen, u8* newData){
	SensingData* newNode = (SensingData*)malloc(sizeof(SensingData));
	newNode->type = type;
	ArrCpy(8,newNode->SLAddr,SLAddr);
	newNode->next = NULL;
	return newNode;
}

//创建表头
SensingData* CreateSensingDataList(void){
	SensingData* headNode = (SensingData*)malloc(sizeof(SensingData));//指针变成了结构体变量
	headNode->next = NULL;
	return headNode;
}


/**
  * @brief		更新传感数据(如果链表中已经有对应传感器，就更新数据，没有的话，就增加结点并更新数据)
  * @param		headNode	：	传感数据链表
  * @param		SLAddr		：	指定传感器长地址
  * @param		type		：	指定传感器型号
  * @param		newDataLen	：	新的传感数据长度
  * @param		newData		：	新的传感数据
  * @retval	    void
  */
void UpdateSensingData(SensingData* headNode, u8* SLAddr, u8 type, u8 newDataLen, u8* newData){
	SensingData* posNode = headNode;
	SensingData* newNode;
	while(posNode->next != NULL){
		posNode = posNode->next;
		if(posNode->type && ArrCmp(2,posNode->SLAddr,SLAddr) == 1){
			//找到了对应传感器
			ArrCpy(newDataLen,posNode->Data,newData);
			return;
		}
	}
	//没找到指定长地址的传感器
	//尾插法把此设备插入
	newNode = CreateSensingDataNode(SLAddr, type, newDataLen, newData);
	posNode->next = newNode;
}

/**
  * @brief		对比指定长地址传感数据
  * @param		headNode：	传感数据链表
  * @param		SLAddr	：	指定传感器长地址
  * @param		Data	：	待对比传感数据
  * @retval	    0：最新数据小于待对比数据
  * @retval	    1：最新数据大于待对比数据
  * @retval	    1：最新数据等于待对比数据
  * @retval	    3：链表内无指定传感器
  */

u8 CmpSensingData(SensingData* headNode, u8* SLAddr, u8* Data){
	SensingData* posNode;
	if(headNode->next == NULL){
		//如果传感数据链表为空，直接退出
		return 3;
	}
	else posNode = headNode->next;
	while(posNode != NULL){
		//遍历链表结点，直到找到对应长地址传感器
		if(ArrCmp(8,SLAddr,posNode->SLAddr) == 1){
			if(posNode->type == 0x05){
				//如果是独立温度传感器
				if(posNode->Data[0] > Data[0]) return 1;
				else if(posNode->Data[0] < Data[0]) return 0;
				else{
					if(posNode->Data[1] > Data[1]) return 1;
					else if(posNode->Data[1] < Data[1]) return 0;
					else return 2;
				}
			}
		}
		else posNode = posNode->next;
	}
	return 3;
}
/***********************************************END******************************************/

/*********************************以下为对所有场景的链表操作函数*********************************/
//创建结点
Scenes* CreateSceneNode(Scene* newScene) {
	Scenes* newNode = (Scenes*)malloc(sizeof(Scenes));
	newNode->SceneHeadNode = newScene;
	newNode->next = NULL;
	return newNode;
}

//创建表头
Scenes* CreateSceneList(){
	Scenes* headNode = (Scenes*)malloc(sizeof(Scenes));//指针变成了结构体变量
	headNode->SceneHeadNode = NULL;
	headNode->next = NULL;
	return headNode;
}

//插入新场景
void InsertSceneNodeByEnd(Scenes* SceneList, u8 SceneNameLen, u8* SceneName){
	Scenes* posNode = SceneList;
	Scene* newScene;
	Scenes* newNode;
	while (posNode->next != NULL) {
		posNode = posNode->next;
		if(ArrCmp(SceneNameLen,posNode->SceneHeadNode->Data,SceneName)){//如果发现了同名场景，直接退出不创建了
			return;
		}
	}
	newScene = CreateSceneMemberList(SceneNameLen,SceneName);
	newNode = CreateSceneNode(newScene);
	posNode->next = newNode;
}

/**
  * @brief		根据Flag指定单场景插入成员(条件or命令)(供程序员调用)
  * @param		SceneList：	场景链表
  * @param		SceneNameLen：	场景名字节长度
  * @param		SceneName	：	场景名
  * @param		Flag		：	条件or命令区分标志位
  * @param		DataLen		：	指定Data长度
  * @param		Data		：	指定Data
  * @retval	    void
  */
void InsertSceneMemberNodeByFlag_User(Scenes* SceneList, u8 SceneNameLen, u8* SceneName, u8 Flag, u8 DataLen, u8* Data){
	Scenes* posNode;
	if(SceneList->next == NULL){
		return;//场景链表是空的,直接退出了
	}
	posNode = SceneList->next;
	while (ArrCmp(SceneNameLen,posNode->SceneHeadNode->Data, SceneName) != 1) {
		if(posNode->next == NULL){
			return;//没有找到指定Data结点
		}
		posNode = posNode->next;
	}
	//找到了对应场景名的场景
	InsertSceneMemberNodeByFlag_Exe(posNode->SceneHeadNode,Flag,DataLen,Data);//向该场景插入成员
}

/**
  * @brief		根据Data指定单场景删除成员(条件or命令)(供程序员调用)
  * @param		SceneList：	场景链表
  * @param		SceneNameLen：	场景名字节长度
  * @param		SceneName	：	场景名
  * @param		DataLen		：	指定Data长度
  * @param		Data		：	指定Data
  * @retval	    void
  */
void DeleteSceneMemberNodeByData_User(Scenes* SceneList, u8 SceneNameLen, u8* SceneName, u8 DataLen, u8* Data) {
	Scenes* posNode;
	if(SceneList->next == NULL){
		return;//设备信息链表是空的,直接退出了
	}
	//如果链表内有设备
	posNode = SceneList->next;
	while (ArrCmp(SceneNameLen,posNode->SceneHeadNode->Data, SceneName) != 1) {
		if(posNode->next == NULL){
			return;//没有找到指定Data结点
		}
		posNode = posNode->next;
	}
	//找到了指定场景
	DeleteSceneMemberNodeByData_Exe(posNode->SceneHeadNode,DataLen,Data);
}

/**
  * @brief		指定单场景
  * @param		SceneList	：场景链表
  * @param		SceneNameLen：场景名长度
  * @param		SceneName   ：场景名
  * @retval	    void
  */
void DeleteSceneNodeBySceneName(Scenes* SceneList, u8 SceneNameLen, u8* SceneName) {
	Scenes* posNode;
	Scenes* posNodeFront;
	if(SceneList->next == NULL){
		return;//场景链表是空的,直接退出了
	}
	//如果链表内有设备
	posNodeFront = SceneList;
	posNode = SceneList->next;
	while (ArrCmp(SceneNameLen,posNode->SceneHeadNode->Data, SceneName) != 1) {
		if(posNode->next == NULL){
			return;//没有找到指定Data结点
		}
		posNodeFront = posNode;
		posNode = posNode->next;
	}
	//找到了指定的场景
	while(posNode->next != NULL){
		DeleteSceneMemberNodeByEnd(posNode->SceneHeadNode);
	}
	posNodeFront->next = posNode->next;
	free(posNode->SceneHeadNode);
	free(posNode);
}

/**
  * @brief		启用单场景
  * @param		SceneList	：场景链表
  * @param		SceneNameLen：场景名长度
  * @param		SceneName   ：场景名
  * @retval	    void
  */
void StartScene(Scenes* SceneList, u8 SceneNameLen, u8* SceneName){
	Scenes* posNode;
	if(SceneList->next == NULL){
		//如果场景链表是空的，直接退出
		return;
	}
	posNode = SceneList->next;
	while(ArrCmp(SceneNameLen,posNode->SceneHeadNode->Data,SceneName) != 1){
		posNode = posNode->next;
		if(posNode == NULL){
			//如果场景链表内没有指定场景,直接退出
			return;
		}
	}
	//找到了指定名称的场景
	posNode->SceneHeadNode->Flag = 1;//启用场景
}

/**
  * @brief		禁用单场景
  * @param		SceneList	：场景链表
  * @param		SceneNameLen：场景名长度
  * @param		SceneName   ：场景名
  * @retval	    void
  */
void StopScene(Scenes* SceneList, u8 SceneNameLen, u8* SceneName){
	Scenes* posNode;
	if(SceneList->next == NULL){
		//如果场景链表是空的，直接退出
		return;
	}
	posNode = SceneList->next;
	while(ArrCmp(SceneNameLen,posNode->SceneHeadNode->Data,SceneName) != 1){
		posNode = posNode->next;
		if(posNode == NULL){
			//如果场景链表内没有指定场景,直接退出
			return;
		}
	}
	//找到了指定名称的场景
	posNode->SceneHeadNode->Flag = 0;//禁用场景
}

/**
  * @brief		遍历执行场景链表
  * @param		SceneList	：场景链表
  * @retval	    void
  */
void HandleScenes(Scenes* SceneList){
	Scenes* posNode = SceneList->next;
	Scene* posScene;//此变量为posNode所包含的单场景
	u8 CmpFlag;
	HandleEsp32CommandStream(Esp32CommandStreamList);//处理与Esp32间通信的数据流
	HandleTerminalStream(TerminalStreamList);//处理等待发往终端数据流
	while(posNode != NULL){
		//如果当前定位结点不是空的
		posScene = posNode->SceneHeadNode;
		if(posScene->Flag == 1){//场景被启用，但还未运行
			posScene = posScene->next;//指向该单场景的第一个结点
			while(posScene != NULL){//如果该节点不为空
				if(posScene->Flag == 0){//如果是触发条件
					if(ArrCmp(8,SelfLongAddr,&posScene->Data[0])){//如果是时间限定条件
						if(posScene->Data[8] == 0){//如果是定时执行
							if(posScene->Data[9]*60 + posScene->Data[10] != BJTimeInSecond / 60) break;//如果没到指定时间，退出对当前场景的判断
						}
						else if(posScene->Data[8] == 1){//如果是时间段内执行
							if(posScene->Data[9]*3600 + posScene->Data[10]*60 + posScene->Data[11] > BJTimeInSecond || posScene->Data[12]*3600 + posScene->Data[13]*60 + posScene->Data[14] <BJTimeInSecond) break;//不在指定时间范围内，退出对当前场景的判断
						}
					}
					else{
						CmpFlag = CmpSensingData(SensingDataList,&posScene->Data[0],&posScene->Data[9]);
						if(CmpFlag != posScene->Data[8]) break;//如果对比结果与条件不符，退出对当前场景的判断
					}
				}
				else if(posScene->Flag == 1){//如果是执行指令(能遍历到Flag == 1说明前面的条件都通过了)
					/*设置命令发送方向*/
					Zigbee_Change_Mode(0);
					Set_Send_Target(&posScene->Data[0],0x01);//设置目标终端
					Zigbee_Change_Mode(1);
					/*设置完毕等待发送*/
					AckFlag = 0;//清零应答标志位
					WaitTime = 0;//倒计时重新开始计时
					while(AckFlag != 1){
						if(WaitTime >= 3){//3秒没有收到终端应答
							SetDeviceOnlineFlagBySAddr(DeviceList,&posScene->Data[0]);
							break;
						}
						Send_Custom_Data(USART1,posScene->Data[2],posScene->Data[3],&posScene->Data[4]);//发送指令
						AckJudge = 1;//允许delay到一半退出delay
						delay_ms(500);//等待终端的应答,目前是在内部植入了一个对AckJudge，后期可以用UCOS的任务轮转调度优化CPU资源
					}
					AckJudge = 0;//禁止delay到一半退出delay
				}
				posScene = posScene->next;
			}
		}
		posNode = posNode->next;
	}
}
/***********************************************END******************************************/


/*********************************以下为对场景信息链表操作函数*********************************/
//创建结点
Scene* CreateSceneMemberNode(u8 Flag, u8 DataLen, u8* Data) {
	Scene* newNode = (Scene*)malloc(sizeof(Scene));
	newNode->Flag = Flag;
	newNode->DataLen = DataLen;
	ArrCpy(DataLen,newNode->Data, Data);
	newNode->next = NULL;
	return newNode;
}

//创建表头
Scene* CreateSceneMemberList(u8 DataLen,u8* Data){
	Scene* headNode = (Scene*)malloc(sizeof(Scene));//指针变成了结构体变量
	headNode->Flag = 0;
	headNode->DataLen = DataLen;
	ArrCpy(DataLen,headNode->Data,Data);
	headNode->next = NULL;
	return headNode;
}
/**
  * @brief		根据Flag指定单场景插入成员(条件or命令)(供程序自调用)
  * @param		headNode：	链表表头
  * @param		Flag：	  	条件or命令区分标志位
  * @param		DataLen：	指定Data长度
  * @param		Data：	  	指定Data
  * @retval	    void
  */
void InsertSceneMemberNodeByFlag_Exe(Scene* headNode, u8 Flag, u8 DataLen, u8* Data){
	Scene* posNode = headNode;
	Scene* posNodeFront;
	Scene* newNode;
	if(headNode->next == NULL){
		newNode = CreateSceneMemberNode(Flag,DataLen,Data);//创建新结点
		headNode->next = newNode;
		return;
	}
	//如果这不是场景链表的第一个设备,执行以下指令
	
	do{
		posNodeFront = posNode;
		posNode = posNode->next;
		if(ArrCmp(DataLen,posNode->Data,Data) == 1){//如果已经有同一个条件或指令，那么不增加了直接退出
			return;
		}
	}
	while (posNode->Flag <= Flag && posNode->next != NULL);
	//定位在Flag为指定值的最后一个或链表最后一个
	newNode = CreateSceneMemberNode(Flag,DataLen,Data);//创建新结点
	//如果不是最后一个
	if(posNode->Flag > Flag){
		posNodeFront->next = newNode;
		newNode->next = posNode;
	}
	//如果是最后一个
	else{
		posNode->next = newNode;
	}
}

/**
  * @brief		根据Data指定单场景删除成员(条件or命令)(供程序自调用)
  * @param		headNode：链表表头
  * @param		DataLen	：指定Data长度
  * @param		Data	：指定Data
  * @retval	    void
  */
void DeleteSceneMemberNodeByData_Exe(Scene* headNode, u8 DataLen, u8* Data) {
	Scene* posNode;
	Scene* posNodeFront;
	if(headNode->next == NULL){
		return;//设备信息链表是空的,直接退出了
	}
	//如果链表内有设备
	posNodeFront = headNode;
	posNode = headNode->next;
	while (ArrCmp(DataLen,posNode->Data, Data) != 1) {
		if(posNode->next == NULL){
			return;//没有找到指定Data结点
		}
		posNodeFront = posNode;
		posNode = posNode->next;
	}
	posNodeFront->next = posNode->next;
	free(posNode);
}

/**
  * @brief		删除场景链表中最后一个结点
  * @param		headNode：链表表头
  * @retval	    void
  */
void DeleteSceneMemberNodeByEnd(Scene* headNode) {
	Scene* posNode = headNode;
	if(headNode->next == NULL){
		return;//设备信息链表是空的,直接退出了
	}
	
	while (posNode->next != NULL) {
		posNode = posNode->next;
	}
	free(posNode);
}
/***********************************************END********************************************/

/*********************************以下为对来自终端信息流处理函数*********************************/
//创建结点
TerminalStream* CreateTerminalStreamNode(u8* SLAddr, u8 type, u8 len, u8* Data){
	TerminalStream* newNode = (TerminalStream*)malloc(sizeof(TerminalStream));
	ArrCpy(8,newNode->SLAddr, SLAddr);
	newNode->type = type;
	newNode->len = len;
	ArrCpy(len,newNode->Data, Data);
	newNode->next = NULL;
	return newNode;
}

//创建链表
TerminalStream* CreateTerminalStreamList(void) {
	//表头结点
	TerminalStream* headNode = (TerminalStream*)malloc(sizeof(TerminalStream));
	headNode->next = NULL;
	return headNode;
}

//末端插入法
void InsTerminalStreamNodeByEnd(TerminalStream* headNode,u8* SLAddr,u8 type, u8 len, u8* Data) {
	TerminalStream* posNode = headNode;
	if(posNode->next == NULL){
		TerminalStream* newNode = CreateTerminalStreamNode(SLAddr,type, len, Data);
		posNode->next = newNode;
		return;
	}
	while (posNode->next != NULL) {
		if(ArrCmp(len,posNode->next->Data,Data) == 1 && posNode->next->len == len && posNode->next->type == type && ArrCmp(8,posNode->next->SLAddr,SLAddr) == 1) return;
		posNode = posNode->next;
		
	}//定位在链表的最后一个节点
	TerminalStream* newNode = CreateTerminalStreamNode(SLAddr,type, len, Data);
	posNode->next = newNode;//接上
}

//处理与终端间通信的数据流
void HandleTerminalStream(TerminalStream* headNode){
	u8 Ack[] = {'O','K'};
	u8 AllowAck[] = {0x00,0x01};//同意终端入网有效数据
	u8 RefuseAck[] = {0x00,0x00};//拒绝终端入网有效数据
	u8 i;
	u8 Data[12];
	TerminalStream* posNode = headNode;//执行兵，站在头结点
	TerminalStream* posNodeFront;//后备兵,还没出场
	if(headNode->next == NULL) return;//如果链表没有数据流，那就直接退出函数了
	else{
		posNodeFront = headNode;//后备兵出场
		posNode = headNode->next;//执行兵走向下一条命令
	}
	while(posNode != NULL){//如果当前命令有内容，就执行
		//处理数据
		if(posNode->type == 0x00){//设备信息命令
			if(CheckDeviceNodeByLongAddr(DeviceList,posNode->SLAddr,&posNode->Data[1]) == 1){//如果链表内已经有该终端，更新下数据
				Send_Custom_Data(USART1,0xFF,2,Ack);//先回应再做自己的事
			}
			else if(APPOpenNetCountDown > 0){//如果还在智能终端允许入网倒计时内,执行以下语句
				//重新封装该设备信息
				Data[0] = posNode->Data[0];
				Data[1] = 1;
				for(i = 0; i < 8; i++){
					Data[2+i] = posNode->SLAddr[i];
				}
				Data[10] = posNode->Data[1];
				Data[11] = posNode->Data[2];
				//将设备信息发给APP	
				Esp32AckFlag = 0;
				while(Esp32AckFlag != 1){
					Send_Custom_Data(USART2,0x00,12,Data);
					delay_ms(300);//稍微等等
				}
				//等待APP的同意或拒绝信号
				APPJudgeFlag = 0;
				WaitTime = 0;
				while(APPJudgeFlag == 0){//等待APP的回应，有1分钟时间
					if(WaitTime >= 60){
						break;//退出循环，表示APP回应超时APPJudgeFlag仍为0
					}
				}
				if(APPJudgeFlag == 1){//表示APP已同意
					//设置透传目标为对应设备
					Zigbee_Change_Mode(0);
					Set_Send_Target(&posNode->Data[1],0x01);
					Zigbee_Change_Mode(1);
					Send_Custom_Data(USART1,0xFF,2,AllowAck);//同意
					//纳入链表
					InsertDeviceNodeByType(DeviceList,Data[0],1,posNode->SLAddr,&posNode->Data[1],0,NULL,0);
					AT24CXX_Save_List(0,DeviceList,SceneList);//即时存入EEPROM
				}
				else if(APPJudgeFlag == 2){//表示APP已拒绝
					//设置透传目标为对应设备
					Zigbee_Change_Mode(0);
					Set_Send_Target(&posNode->Data[1],0x01);
					Zigbee_Change_Mode(1);
					Send_Custom_Data(USART1,0xFF,2,RefuseAck);//拒绝
				}
			}
		}
		//删除该结点
		posNodeFront->next = posNode->next;//后备兵的下一步指向执行兵的下一步
		free(posNode);//执行兵原地蒸发
		posNode = posNodeFront->next;//执行兵在后备兵下一步复活
	    HandleEsp32CommandStream(Esp32CommandStreamList);//处理完一个终端的事情，就处理与Esp32间通信的数据流
	}
	//链表空了
}
/***************************************END***************************************/


/*********************************以下为对与Esp32间通信数据链表操作函数*********************************/
//创建结点
Esp32CommandStream* CreateEsp32CommandStreamNode(u8* DSAddr, u8 type, u8 len, u8* Data, u8 DataDirection) {
	Esp32CommandStream* newNode = (Esp32CommandStream*)malloc(sizeof(Esp32CommandStream));
	ArrCpy(2,newNode->DSAddr, DSAddr);//注意！！要用strcpy
	newNode->type = type;
	newNode->len = len;
	ArrCpy(len,newNode->Data, Data);//注意！！要用strcpy
	newNode->DataDirection = DataDirection;
	newNode->next = NULL;
	return newNode;
}

//创建链表
Esp32CommandStream* CreateEsp32CommandStreamList(void) {
	//表头结点
	Esp32CommandStream* headNode = (Esp32CommandStream*)malloc(sizeof(Esp32CommandStream));
	headNode->next = NULL;
	return headNode;
}

//末端插入法
void InsertEsp32CommandStreamNodeByEnd(Esp32CommandStream* headNode,u8* DSAddr, u8 type, u8 len, u8* Data, u8 DataDirection) {
	Esp32CommandStream* posNode = headNode;
	while (posNode->next != NULL) {
		if(ArrCmp(len,posNode->next->Data,Data) == 1 && ArrCmp(2,posNode->next->DSAddr,DSAddr) == 1 && posNode->next->DataDirection == DataDirection) {
			//如果同一指令重复接收那就只接收第一次
			LED0 = 0;
			return;
		}
		posNode = posNode->next;
	}//定位在链表的最后一个节点
	Esp32CommandStream* newNode = CreateEsp32CommandStreamNode(DSAddr, type, len, Data, DataDirection);
	posNode->next = newNode;//接上
}

//处理与Esp32间通信的数据流
void HandleEsp32CommandStream(Esp32CommandStream* headNode){
	Esp32CommandStream* posNode = headNode;//执行兵，站在头结点
	Esp32CommandStream* posNodeFront;//后备兵,还没出场
	if(headNode->next == NULL) return;//如果链表没有数据流，那就直接退出函数了
	else{
		posNodeFront = headNode;//后备兵出场
		posNode = headNode->next;//执行兵走向下一条命令
	}
	while(posNode != NULL){//如果当前结点有内容，就执行
		//处理数据
		if(posNode->DataDirection == 1){//如果是来自ESP32的命令
			if(posNode->DSAddr[0] == 0x00 && posNode->DSAddr[1] == 0x00){//如果是APP对中控的命令
				if(posNode->type == 0x00){//如果是设备信息命令
					if(posNode->Data[0] == 0x00 && posNode->Data[9] != 0){//如果是给指定终端定位的命令
						SetDevicePositionByLAddr(DeviceList,&posNode->Data[1],posNode->Data[9],&posNode->Data[10],posNode->Data[posNode->len - 1]);
					}
				}
				else if(posNode->type == 0xFE){//如果是场景操作命令
					if(posNode->Data[0] == 0x00){//删除指定场景
						DeleteSceneNodeBySceneName(SceneList,posNode->Data[1],&posNode->Data[2]);
					}
					else if(posNode->Data[0] == 0x01){//创建指定场景
						InsertSceneNodeByEnd(SceneList,posNode->Data[1],&posNode->Data[2]);
					}
					else if(posNode->Data[0] == 0x02){//场景删除条件
						DeleteSceneMemberNodeByData_User(SceneList,posNode->Data[1],&posNode->Data[2],posNode->Data[posNode->Data[1]+2],&posNode->Data[posNode->Data[1]+3]);
					}
					else if(posNode->Data[0] == 0x03){//场景增加条件
						InsertSceneMemberNodeByFlag_User(SceneList,posNode->Data[1],&posNode->Data[2],0,posNode->Data[posNode->Data[1]+2],&posNode->Data[posNode->Data[1]+3]);
					}
					else if(posNode->Data[0] == 0x04){//场景删除指令
						DeleteSceneMemberNodeByData_User(SceneList,posNode->Data[1],&posNode->Data[2],posNode->Data[posNode->Data[1]+2],&posNode->Data[posNode->Data[1]+3]);
					}
					else if(posNode->Data[0] == 0x05){//场景增加指令
						InsertSceneMemberNodeByFlag_User(SceneList,posNode->Data[1],&posNode->Data[2],1,posNode->Data[posNode->Data[1]+2],&posNode->Data[posNode->Data[1]+3]);
					}
					else if(posNode->Data[0] == 0x06){//场景禁用指令
						StopScene(SceneList,posNode->Data[1],&posNode->Data[2]);
					}
					else if(posNode->Data[0] == 0x07){//场景启用指令
						StartScene(SceneList,posNode->Data[1],&posNode->Data[2]);
					}
					if(posNode->Data[0] >= 0 && posNode->Data[0] <= 5){
						AT24CXX_Save_List(0,DeviceList,SceneList);//及时存入EEPROM
					}
				}
			}
			else{//如果是APP对终端的命令
				/*设置命令发送方向*/
				Zigbee_Change_Mode(0);
				Set_Send_Target(posNode->DSAddr,0x01);//设置目标终端
				Zigbee_Change_Mode(1);
				/*设置完毕等待发送*/
				AckFlag = 0;//清零应答标志位
				WaitTime = 0;//倒计时重新开始计时
				while(AckFlag != 1){
					if(WaitTime >= 3){//3秒没有收到终端应答
						SetDeviceOnlineFlagBySAddr(DeviceList,posNode->DSAddr);
						break;
					}
					Send_Custom_Data(USART1,posNode->type,posNode->len,posNode->Data);//发送指令
					AckJudge = 1;//允许delay到一半退出delay
					delay_ms(500);//等待终端的应答,目前是在内部植入了一个对AckJudge，后期可以用UCOS的任务轮转调度优化CPU资源
				}
				AckJudge = 0;//禁止delay到一半退出delay
			}
		}
		else if(posNode->DataDirection == 2){//准备发往ESP32的数据
			Send_Custom_Data(USART2,posNode->type,posNode->len,posNode->Data);
		}
		//删除该结点
		posNodeFront->next = posNode->next;//后备兵的下一步指向执行兵的下一步
		free(posNode);//执行兵原地蒸发
		posNode = posNodeFront->next;//执行兵在后备兵下一步复活
	}
	//链表空了
}
/***************************************END***************************************/


/*********************************以下为对设备信息链表操作函数*********************************/
//创建结点
Device* CreateDeviceNode(u8 type, u8 onlineFlag, u8* LongAddr, u8* ShortAddr, u8 PosNameLen, u8* PosName, u8 SerialNumber) {
	Device* newNode = (Device*)malloc(sizeof(Device));
	newNode->type = type;
	newNode->onlineFlag = onlineFlag;
	ArrCpy(8,newNode->LongAddr, LongAddr);
	ArrCpy(2,newNode->ShortAddr, ShortAddr);
	if(PosNameLen != 0) ArrCpy(18,newNode->PosName,PosName);
	newNode->PosNameLen = PosNameLen;
	newNode->SerialNumber = SerialNumber;
	newNode->next = NULL;
	return newNode;
}

Device* CreateDeviceList(void){
	//创建表头
	Device* headNode = (Device*)malloc(sizeof(Device));//指针变成了结构体变量
	headNode->type = 0;
	headNode->next = NULL;
	return headNode;
}

void InsertDeviceNodeByType(Device* headNode, u8 type, u8 onlineFlag, u8* LongAddr, u8* ShortAddr, u8 PosNameLen, u8* PosName, u8 SerialNumber){
	Device* posNode = headNode;
	Device* posNodeFront;
	while (posNode->type <= type && posNode->next != NULL) {
		posNodeFront = posNode;
		posNode = posNode->next;
	}//定位在type为指定数字或的最后一个
	Device* newNode = CreateDeviceNode(type,onlineFlag, LongAddr, ShortAddr,PosNameLen,PosName,SerialNumber);
	if(posNode->type > type){//如果不是最后一个
		posNodeFront->next = newNode;
		newNode->next = posNode;
	}else{
		posNode->next = newNode;
	}
}

void DeleteDeviceNodeByLongAddr(Device* headNode, u8* LongAddr) {
	Device* posNode;
	Device* posNodeFront;
	if(headNode->next == NULL){
		return;//设备信息链表是空的,直接退出了
	}
	//如果链表内有设备
	posNodeFront = headNode;
	posNode = headNode->next;
	while (ArrCmp(8,posNode->LongAddr, LongAddr) != 1) {
		if(posNode->next == NULL){
			return;//没有找到指定长地址设备
		}
		posNodeFront = posNode;
		posNode = posNode->next;
	}
	posNodeFront->next = posNode->next;
	free(posNode);
}

/**
  * @brief		查找链表中是否有目标长地址的终端
  * @param		headNode：链表表头
  * @param		LongAddr：查询条件->长地址
  * @param		ShortAddr：更新后短地址短地址
  * @retval	    1->有
  * @retval	    0->无
  */

u8 CheckDeviceNodeByLongAddr(Device* headNode, u8* LongAddr, u8* ShortAddr) {
	Device* posNode = headNode;
	if(posNode->next == NULL) return 0;//空链表，没有查到该长地址的设备
	while(posNode != NULL){
		if(ArrCmp(8,posNode->LongAddr, LongAddr) == 1) break;
		if(posNode->next == NULL) return 0;//没有查到该长地址的设备
		posNode = posNode->next;
	}
	//找到了,标志入网
	posNode->onlineFlag = 1;
	if(ShortAddr[0] == 0x00 && ShortAddr[1] == 0x00){
		//如果不打算更新短地址
		//就直接退出了
		return 1;
	}
	else{
		//还要更新短地址的话就更新
		//更新短地址
		posNode->ShortAddr[0] = ShortAddr[0];
		posNode->ShortAddr[1] = ShortAddr[1];
		return 1;//查到了该长地址的设备
	}
	
}


/**
  * @brief		更新链表中设备联网状态
  * @param		Device* headNode：链表表头
  * @retval	    void
  */
void UpdateDeviceList(Device* headNode){
	u8 i;
	u8 changeFlag = 0;//终端在网情况发是否生变动标志位,置1表示有
	Device* posNode = headNode;
	u8 Data[12];
	if(headNode->next == NULL) return;//如果设备链表为空，那就直接退出
	posNode = posNode->next;
	while (posNode != NULL){
		Zigbee_Change_Mode(0);
		Set_Send_Target(posNode->ShortAddr,0x01);
		Zigbee_Change_Mode(1);
		AckFlag = 0;
		WaitTime = 0;
		while(AckFlag != 1){
			if(WaitTime >= 2){//等待超过2s
				posNode->onlineFlag = 0;//置0在网标志位
				//封装该不在线设备信息给到APP
				Data[0] = posNode->type;
				Data[1] = posNode->onlineFlag;
				for(i = 0; i < 8; i++){
					Data[2+i] = posNode->LongAddr[i];
				}
				Data[i] = posNode->ShortAddr[0];
				i++;
				Data[i] = posNode->ShortAddr[1];
				Esp32AckFlag = 0;
				EspWaitTime = 0;
				while(Esp32AckFlag == 0){
					if(EspWaitTime == 3){//超时退出,Esp32繁忙
						break; 
					}
					Send_Custom_Data(USART2,0x00,12,Data);//发送数据
					delay_ms(200);//稍微等等
				}
				changeFlag = 1;//终端在线情况发生变动
				break;
			}
			Send_Custom_Data(USART1,0xFF,0,NULL);//请求应答
			AckJudge = 1;
			delay_ms(500);//目前是在内部植入了一个AckJudge，后期可以用UCOS的任务轮转调度优化CPU资源
		}
		if(AckFlag == 1 && posNode->onlineFlag == 0){
			posNode->onlineFlag = 1;
			changeFlag = 1;//终端在线情况发生变动
		}
		AckJudge = 0;
		posNode = posNode->next;
	}
	if(changeFlag == 1){
		
	}
}

/**
  * @brief		将链表内容输出到Esp
  * @param		Device* headNode：链表表头
  * @retval	    void
  */

void PrintDeviceList(Device* headNode) {
	Device* posNode = headNode->next;
	u8 Data[12];
	u8 i;
	while (posNode != NULL) {
		Data[0] = posNode->type;
		Data[1] = posNode->onlineFlag;
		for(i = 0; i < 8; i++){
			Data[2+i] = posNode->LongAddr[i];
		}
		Data[i] = posNode->ShortAddr[0];
		i++;
		Data[i] = posNode->ShortAddr[1];
		Esp32AckFlag = 0;
		EspWaitTime = 0;
		while(Esp32AckFlag == 0){
			if(EspWaitTime == 5){//超时退出,APP不在线
				return ;
			}
			Send_Custom_Data(USART2,0x00,12,Data);//发送数据
			delay_ms(200);//稍微等等
		}
		delay_ms(1000);//这里延时一个1s
		posNode = posNode->next;
	}
}

/**
  * @brief		将链表中指定短地址设备设置为未联网
  * @param		Device* headNode：链表表头
  * @retval	    void
  */
void SetDeviceOnlineFlagBySAddr(Device* headNode, u8* ShortAddr){
	Device* posNode;
	u8 Data[12];
	u8 i;
	if(headNode->next == NULL) return;//如果链表为空就直接拜拜
	else posNode = headNode->next;
	while(1){
		if(ArrCmp(2,posNode->ShortAddr, ShortAddr) == 1){
			posNode->onlineFlag = 0;
			break ;
		}
		else posNode = posNode->next;
		if(posNode == NULL) return;//链表内没有符合条件的结点
	}

	//告诉Esp32（也是告诉APP有个终端下线了）
	Esp32AckFlag = 0;
	EspWaitTime = 0;
	while(Esp32AckFlag == 0){
		if(EspWaitTime == 5){//超时退出
			return ;
		}
		Send_Custom_Data(USART2,0x00,0,NULL);//开始信号，等待应答
		delay_ms(200);//稍微等等
	}
	//打包数据
	Data[0] = posNode->type;
	Data[1] = posNode->onlineFlag;
	for(i = 0; i < 8; i++){
		Data[2+i] = posNode->LongAddr[i];
	}
	Data[i] = posNode->ShortAddr[0];
	i++;
	Data[i] = posNode->ShortAddr[1];
	Send_Custom_Data(USART2,0x00,12,Data);//发给Esp
}

/**
  * @brief		根据长地址设置指定生态内设备的所处空间及编号
  * @param		HeadNode：链表表头
  * @param		LongAddr：指定终端长地址 
  * @param		PosNameLen：空间名长度
  * @param		PosName：空间名指针
  * @param		SerialNumber：编号
  * @retval	    void
  */

 void SetDevicePositionByLAddr(Device* HeadNode, u8* LongAddr, u8 PosNameLen, u8* PosName, u8 SerialNumber){
	Device* posNode;
	if(HeadNode->next == NULL) return;//如果链表为空就直接拜拜
	else posNode = HeadNode->next;
	while(1){
		if(ArrCmp(8,posNode->LongAddr, LongAddr) == 1){
			break ;//找到了指定长地址的终端
		}
		else posNode = posNode->next;
		if(posNode == NULL) return;//链表内没有符合条件的结点也拜拜
	}
	posNode->PosNameLen = PosNameLen;
	ArrCpy(PosNameLen,posNode->PosName,PosName);
	posNode->SerialNumber = SerialNumber;
 }
/***************************************END***************************************/
