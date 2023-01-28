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

Device* createNode(u8 type, u8* LongAddr, u8* ShortAddr) {
	Device* newNode = (Device*)malloc(sizeof(Device));
	newNode->type = type;
	AddrCpy(8,newNode->LongAddr, LongAddr);//注意！！要用strcpy
	AddrCpy(2,newNode->ShortAddr, ShortAddr);//注意！！要用strcpy
	newNode->next = NULL;
	return newNode;
}

Device* createList(void) {
	u8 i;
	//创建表头
	Device* headNode = (Device*)malloc(sizeof(Device));//指针变成了结构体变量
	headNode->type = 0;
	Device* pMove = (Device*)malloc(sizeof(Device));//指针变成了结构体变量
	pMove = headNode;
	for (i = 1; i <= 5; i++) {
		Device* newNode = (Device*)malloc(sizeof(Device));
		newNode->type = i;
		newNode->OnlineFlag = 0;
		newNode->next = NULL;
		newNode->ShortAddr[0] = 0;
		newNode->ShortAddr[1] = 0;
		pMove->next = newNode;
		pMove = newNode;
	}
	Device* newNode = (Device*)malloc(sizeof(Device));
	newNode->type = 0xFF;
	newNode->next = NULL;
	pMove->next = newNode;
	pMove = newNode;
	return headNode;
}

void insertNodeByType(Device* headNode, u8 type, u8* LongAddr, u8* ShortAddr) {
	Device* pMove = headNode;
	while (pMove->next->type <= type) {
		pMove = pMove->next;
	}//定位在type为指定数字的前一个(这个位置的type是小于type的)
	Device* newNode = createNode(type, LongAddr, ShortAddr);
	newNode->OnlineFlag = 1;
	newNode->next = pMove->next;
	pMove->next = newNode;
}

void deleteNodeByLongAddr(Device* headNode, u8* LongAddr) {
	Device* posNode = headNode->next;
	Device* posNodeFront = headNode;
	while (AddrCmp(8,posNode->LongAddr, LongAddr) != 0) {
		posNodeFront = posNode;
		posNode = posNode->next;
	}
	posNodeFront->next = posNode->next;
	free(posNode);
}

u8 checkByLongAddr(Device* headNode, u8* LongAddr, u8* ShortAddr) {
	Device* posNode = headNode->next;
	while (AddrCmp(8,posNode->LongAddr, LongAddr) == 0){
		if(posNode->next == NULL) return 0;//没有查到该长地址的设备
		posNode = posNode->next;
	}
	//找到了,标志入网，更新短地址
	posNode->OnlineFlag = 1;
	posNode->ShortAddr[0] = ShortAddr[0];
	posNode->ShortAddr[1] = ShortAddr[1];
	return 1;//查到了该长地址的设备
}

void printList(Device* headNode) {
	Device* pMove = headNode->next;
	while (pMove->type != 0xFF) {
		if (pMove->ShortAddr[0] != 0 || pMove->ShortAddr[1] != 0) {
			Send_Custom_Data(USART2,0xFF,8,pMove->LongAddr);
			Send_Custom_Data(USART2,0xFF,2,pMove->ShortAddr);
		}
		pMove = pMove->next;
	}
}

