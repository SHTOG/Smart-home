#include "myList.h"

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
	while(posNode != NULL){//如果当前命令有内容，就执行
		//处理数据
		if(posNode->DataDirection == 1){//是来自ESP32的命令
			/*设置命令发送方向*/
			Zigbee_Change_Mode(0);
			Set_Send_Target(posNode->DSAddr,0x01);//设置目标终端
			Zigbee_Change_Mode(1);
			/*设置完毕等待发送*/
			AckFlag = 0;//清零应答标志位
			WaitTime = 0;//倒计时重新开始计时
			while(AckFlag != 1){
				if(WaitTime == 3){//3秒没有收到终端应答
					SetDeviceOnlineFlagBySAddr(DeviceList,posNode->DSAddr);
					break;
				}
				Send_Custom_Data(USART1,Esp32CommandStreamList->type,Esp32CommandStreamList->len,Esp32CommandStreamList->Data);//发送指令
				AckJudge = 1;//允许delay到一半退出delay
				delay_ms(100);//等待终端的应答,目前是在内部植入了一个对AckJudge，后期可以用UCOS的任务轮转调度优化CPU资源
			}
			AckJudge = 0;//禁止delay到一半退出delay
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
Device* CreateDeviceNode(u8 type, u8 onlineFlag, u8* LongAddr, u8* ShortAddr) {
	Device* newNode = (Device*)malloc(sizeof(Device));
	newNode->type = type;
	newNode->onlineFlag = onlineFlag;
	ArrCpy(8,newNode->LongAddr, LongAddr);//注意！！要用strcpy
	ArrCpy(2,newNode->ShortAddr, ShortAddr);//注意！！要用strcpy
	newNode->next = NULL;
	return newNode;
}

Device* CreateDeviceList(void) {
	//创建表头
	Device* headNode = (Device*)malloc(sizeof(Device));//指针变成了结构体变量
	headNode->type = 0;
	headNode->next = NULL;
	return headNode;
}

void InsertDeviceNodeByType(Device* headNode, u8 type, u8 onlineFlag, u8* LongAddr, u8* ShortAddr) {
	Device* posNode = headNode;
	Device* posNodeFront;
	while (posNode->type <= type && posNode->next != NULL) {
		posNodeFront = posNode;
		posNode = posNode->next;
	}//定位在type为指定数字或的最后一个
	Device* newNode = CreateDeviceNode(type,onlineFlag, LongAddr, ShortAddr);
	if(posNode->type > type){//如果不是最后一个
		posNodeFront->next = newNode;
		newNode->next = posNode;
	}else{
		posNode->next = newNode;
	}
}

void DeleteDeviceNodeByLongAddr(Device* headNode, u8* LongAddr) {
	Device* posNode = headNode->next;
	Device* posNodeFront = headNode;
	while (ArrCmp(8,posNode->LongAddr, LongAddr) != 0) {
		posNodeFront = posNode;
		posNode = posNode->next;
	}
	posNodeFront->next = posNode->next;
	free(posNode);
}

u8 CheckDeviceNodeByLongAddr(Device* headNode, u8* LongAddr, u8* ShortAddr) {
	Device* posNode = headNode;
	if(posNode->next == NULL) return 0;//空链表，没有查到该长地址的设备
	while(posNode != NULL){
		if(ArrCmp(8,posNode->LongAddr, LongAddr) == 1) break;
		if(posNode->next == NULL) return 0;//没有查到该长地址的设备
		posNode = posNode->next;
	}
	//找到了,标志入网，更新短地址
	posNode->onlineFlag = 1;
	posNode->ShortAddr[0] = ShortAddr[0];
	posNode->ShortAddr[1] = ShortAddr[1];
	return 1;//查到了该长地址的设备
}

/**
  * @brief		查找链表中是否有未联网设备
  * @param		Device* headNode：链表表头
  * @retval	    1->有未联网设备
  * @retval	    0->无未联网设备
  */

u8 CheckDeviceNodeByOnlineFlag(Device* headNode) {
	Device* posNode = headNode->next;
	while (posNode != NULL){
		if(posNode->onlineFlag != 0) posNode = posNode->next;
		else{
			if(posNode->onlineFlag == 0){
				return 1;
			}
		}
	}
	return 0;
}


/**
  * @brief		更新链表中设备联网状态
  * @param		Device* headNode：链表表头
  * @retval	    void
  */
void UpdateDeviceList(Device* headNode){

	Device* posNode = headNode->next;
	while (posNode != NULL){
		Zigbee_Change_Mode(0);
		Set_Send_Target(posNode->ShortAddr,0x01);
		Zigbee_Change_Mode(1);
		AckFlag = 0;
		WaitTime = 0;
		while(AckFlag != 1){
			if(WaitTime == 3){
				posNode->onlineFlag = 0;
				break;
			}
			Send_Custom_Data(USART1,0xFF,0,NULL);//请求应答
			AckJudge = 1;
			delay_ms(100);//目前是在内部植入了一个AckJudge，后期可以用UCOS的任务轮转调度优化CPU资源
		}
		AckJudge = 0;
		posNode = posNode->next;
	}
}

/**
  * @brief		将链表内容输出到Esp
  * @param		Device* headNode：链表表头
  * @retval	    void
  */

void PrintDeviceList(Device* headNode) {
	Device* pMove = headNode->next;
	u8 Data[12];
	u8 i;
	while (pMove != NULL) {
		Data[0] = pMove->type;
		Data[1] = pMove->onlineFlag;
		for(i = 0; i < 8; i++){
			Data[2+i] = pMove->LongAddr[i];
		}
		Data[i] = pMove->ShortAddr[0];
		i++;
		Data[i] = pMove->ShortAddr[1];
		Esp32AckFlag = 0;
		EspWaitTime = 0;
		while(Esp32AckFlag == 0){
			if(EspWaitTime == 5){//超时退出,应该是APP不在线，esp寄了
				return ;
			}
			Send_Custom_Data(USART2,0x00,0,Data);//开始信号，等待应答
			delay_ms(300);//稍微等等
		}
		Send_Custom_Data(USART2,0x00,12,Data);
		pMove = pMove->next;
	}
}
/**
  * @brief		将链表中指定短地址设备设置为未联网
  * @param		Device* headNode：链表表头
  * @retval	    void
  */
void SetDeviceOnlineFlagBySAddr(Device* headNode, u8* ShortAddr){
	Device* posNode = headNode->next;
	u8 Data[12];
	u8 i;
	while(posNode != NULL){
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
/***************************************END***************************************/
