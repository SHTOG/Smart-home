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
	AddrCpy(8,newNode->LongAddr, LongAddr);//ע�⣡��Ҫ��strcpy
	AddrCpy(2,newNode->ShortAddr, ShortAddr);//ע�⣡��Ҫ��strcpy
	newNode->next = NULL;
	return newNode;
}

Device* CreateList(void) {
	//������ͷ
	Device* headNode = (Device*)malloc(sizeof(Device));//ָ�����˽ṹ�����
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
	}//��λ��typeΪָ�����ֻ�����һ��
	Device* newNode = CreateNode(type,onlineFlag, LongAddr, ShortAddr);
	if(posNode->type > type){//����������һ��
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
	if(posNode->next == NULL) return 0;//������û�в鵽�ó���ַ���豸
	while(posNode != NULL){
		if(AddrCmp(8,posNode->LongAddr, LongAddr) == 1) break;
		if(posNode->next == NULL) return 0;//û�в鵽�ó���ַ���豸
		posNode = posNode->next;
	}
	//�ҵ���,��־���������¶̵�ַ
	posNode->onlineFlag = 1;
	posNode->ShortAddr[0] = ShortAddr[0];
	posNode->ShortAddr[1] = ShortAddr[1];
	return 1;//�鵽�˸ó���ַ���豸
}

/**
  * @brief		�����������Ƿ���δ�����豸
  * @param		Device* headNode�������ͷ
  * @retval	    1->��δ�����豸
  * @retval	    0->��δ�����豸
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
  * @brief		�����������豸����״̬
  * @param		Device* headNode�������ͷ
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
			Send_Custom_Data(USART1,0xFF,0,NULL);//����Ӧ��
			AckJudge = 1;
			delay_ms(100);//Ŀǰ�����ڲ�ֲ����һ��AckJudge�����ڿ�����UCOS��������ת�����Ż�CPU��Դ
		}
		AckJudge = 0;
		posNode = posNode->next;
	}
}

/**
  * @brief		���������������Esp
  * @param		Device* headNode�������ͷ
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
			if(WaitTime == 5){//��ʱ�˳�
				return ;
			}
			Send_Custom_Data(USART2,0x00,0,Data);//��ʼ�źţ��ȴ�Ӧ��
			delay_ms(300);//��΢�ȵ�
		}
		Send_Custom_Data(USART2,0x00,12,Data);
		pMove = pMove->next;
	}
}
/**
  * @brief		��������ָ���̵�ַ�豸����Ϊδ����
  * @param		Device* headNode�������ͷ
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
