#include "myList.h"

void AddrCpy(u8 n, u8* dest, u8*src){
	u8 i;
	for(i = 0 ; i < n; i++){
		dest[i] = src[i];
	}
}

u8 AddrCmp(u8 n, u8* dest, u8*src){
	u8 i;
	for(i = 0; i < n; i++){
		if(dest[i] != src[i]) return 0;
	}
	return 1;
}

Device* CreateNode(u8 type, u8 onlineFlag, u8* LongAddr, u8* ShortAddr) {
	Device* newNode = (Device*)malloc(sizeof(Device));
	newNode->type = type;
	newNode->onlineFlag = onlineFlag;
	AddrCpy(8,newNode->LongAddr, LongAddr);//注意！！要用strcpy
	AddrCpy(2,newNode->ShortAddr, ShortAddr);//注意！！要用strcpy
	newNode->next = NULL;
	return newNode;
}

Device* CreateList(void) {
	//创建表头
	Device* headNode = (Device*)malloc(sizeof(Device));//指针变成了结构体变量
	headNode->type = 0;
	headNode->next = NULL;
	return headNode;
}

void InsertNodeByType(Device* headNode, u8 type, u8 onlineFlag, u8* LongAddr, u8* ShortAddr) {
	Device* posNode = headNode;
	Device* posNodeFront;
	while (posNode->type <= type && posNode->next != NULL) {
		posNodeFront = posNode;
		posNode = posNode->next;
	}//定位在type为指定数字或的最后一个
	Device* newNode = CreateNode(type,onlineFlag, LongAddr, ShortAddr);
	if(posNode->type > type){//如果不是最后一个
		posNodeFront->next = newNode;
		newNode->next = posNode;
	}else{
		posNode->next = newNode;
	}
}

void DeleteNodeByLongAddr(Device* headNode, u8* LongAddr) {
	Device* posNode = headNode->next;
	Device* posNodeFront = headNode;
	while (AddrCmp(8,posNode->LongAddr, LongAddr) != 0) {
		posNodeFront = posNode;
		posNode = posNode->next;
	}
	posNodeFront->next = posNode->next;
	free(posNode);
}

u8 CheckByLongAddr(Device* headNode, u8* LongAddr, u8* ShortAddr) {
	Device* posNode = headNode;
	if(posNode->next == NULL) return 0;//空链表，没有查到该长地址的设备
	while(posNode != NULL){
		if(AddrCmp(8,posNode->LongAddr, LongAddr) == 1) break;
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

u8 CheckByOnlineFlag(Device* headNode) {
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
void UpdateList(Device* headNode){

	Device* posNode = headNode->next;
	while (posNode != NULL){
		Zigbee_Change_Mode(0);
		Set_Send_Target(posNode->ShortAddr,0x01);
		Zigbee_Change_Mode(1);
		AckFlag = 0;
		WaitTime = 0;
		while(AckFlag != 1){
			if(WaitTime == 5){
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

void PrintList(Device* headNode) {
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
		WaitTime = 0;
		while(Esp32AckFlag == 0){
			if(WaitTime == 5){//超时退出
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
void SetNetStateByShorAddr(Device* headNode, u8* ShortAddr){
	Device* posNode = headNode->next;
	while(posNode != NULL){
		if(posNode->ShortAddr[0] == ShortAddr[0] && posNode->ShortAddr[1] == ShortAddr[1]){
			posNode->onlineFlag = 0;
			return ;
		}
		else posNode = posNode->next;
	}
}
