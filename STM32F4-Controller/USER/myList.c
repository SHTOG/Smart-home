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

Device* createNode(u8 type, u8 onlineFlag, u8* LongAddr, u8* ShortAddr) {
	Device* newNode = (Device*)malloc(sizeof(Device));
	newNode->type = type;
	newNode->onlineFlag = onlineFlag;
	AddrCpy(8,newNode->LongAddr, LongAddr);//ע�⣡��Ҫ��strcpy
	AddrCpy(2,newNode->ShortAddr, ShortAddr);//ע�⣡��Ҫ��strcpy
	newNode->next = NULL;
	return newNode;
}

Device* createList(void) {
	//������ͷ
	Device* headNode = (Device*)malloc(sizeof(Device));//ָ�����˽ṹ�����
	headNode->type = 0;
	headNode->next = NULL;
	return headNode;
}

void insertNodeByType(Device* headNode, u8 type, u8 onlineFlag, u8* LongAddr, u8* ShortAddr) {
	Device* posNode = headNode;
	Device* posNodeFront;
	while (posNode->type <= type && posNode->next != NULL) {
		posNodeFront = posNode;
		posNode = posNode->next;
	}//��λ��typeΪָ�����ֻ�����һ��
	Device* newNode = createNode(type,onlineFlag, LongAddr, ShortAddr);
	if(posNode->type > type){//����������һ��
		posNodeFront->next = newNode;
		newNode->next = posNode;
	}else{
		posNode->next = newNode;
	}
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
		if(posNode->next == NULL) return 0;//û�в鵽�ó���ַ���豸
		posNode = posNode->next;
	}
	//�ҵ���,��־���������¶̵�ַ
	posNode->onlineFlag = 1;
	posNode->ShortAddr[0] = ShortAddr[0];
	posNode->ShortAddr[1] = ShortAddr[1];
	return 1;//�鵽�˸ó���ַ���豸
}




