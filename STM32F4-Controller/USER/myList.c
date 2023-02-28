#include "myList.h"

/**

  * @brief    ���鿽��
  * @param    len : ������λ��(��С��dest��src��ָ����ĳ��ȣ�
  * @param    dest: ���鿽��Ŀ�ĵ�
  * @param    src : Դ����
  * @retval   void
  */

void ArrCpy(u8 len, u8* dest, u8*src){
	u8 i;
	for(i = 0 ; i < len; i++){
		dest[i] = src[i];
	}
}

/**

  * @brief    ����Ա�
  * @param    len : �Աȵ�ǰnλ��(��С��dest��src��ָ����ĳ��ȣ�
  * @param    dest: ����Ա�Ŀ�ĵ�
  * @param    src : Դ����
  * @retval   1   : ��������ǰlenλ��ͬ
  * @retval   0   : ǰlenλ�в�ͬԪ��
  */
u8 ArrCmp(u8 len, u8* dest, u8*src){
	u8 i;
	for(i = 0; i < len; i++){
		if(dest[i] != src[i]) return 0;
	}
	return 1;
}

/*********************************����Ϊ����Esp32��ͨ�����������������*********************************/
//�������
Esp32CommandStream* CreateEsp32CommandStreamNode(u8* DSAddr, u8 type, u8 len, u8* Data, u8 DataDirection) {
	Esp32CommandStream* newNode = (Esp32CommandStream*)malloc(sizeof(Esp32CommandStream));
	ArrCpy(2,newNode->DSAddr, DSAddr);//ע�⣡��Ҫ��strcpy
	newNode->type = type;
	newNode->len = len;
	ArrCpy(len,newNode->Data, Data);//ע�⣡��Ҫ��strcpy
	newNode->DataDirection = DataDirection;
	newNode->next = NULL;
	return newNode;
}

//��������
Esp32CommandStream* CreateEsp32CommandStreamList(void) {
	//��ͷ���
	Esp32CommandStream* headNode = (Esp32CommandStream*)malloc(sizeof(Esp32CommandStream));
	headNode->next = NULL;
	return headNode;
}

//ĩ�˲��뷨
void InsertEsp32CommandStreamNodeByEnd(Esp32CommandStream* headNode,u8* DSAddr, u8 type, u8 len, u8* Data, u8 DataDirection) {
	Esp32CommandStream* posNode = headNode;
	while (posNode->next != NULL) {
		posNode = posNode->next;
	}//��λ����������һ���ڵ�
	Esp32CommandStream* newNode = CreateEsp32CommandStreamNode(DSAddr, type, len, Data, DataDirection);
	posNode->next = newNode;//����
}

//������Esp32��ͨ�ŵ�������
void HandleEsp32CommandStream(Esp32CommandStream* headNode){
	Esp32CommandStream* posNode = headNode;//ִ�б���վ��ͷ���
	Esp32CommandStream* posNodeFront;//�󱸱�,��û����
	if(headNode->next == NULL) return;//�������û�����������Ǿ�ֱ���˳�������
	else{
		posNodeFront = headNode;//�󱸱�����
		posNode = headNode->next;//ִ�б�������һ������
	}
	while(posNode != NULL){//�����ǰ���������ݣ���ִ��
		//��������
		if(posNode->DataDirection == 1){//������ESP32������
			/*��������ͷ���*/
			Zigbee_Change_Mode(0);
			Set_Send_Target(posNode->DSAddr,0x01);//����Ŀ���ն�
			Zigbee_Change_Mode(1);
			/*������ϵȴ�����*/
			AckFlag = 0;//����Ӧ���־λ
			WaitTime = 0;//����ʱ���¿�ʼ��ʱ
			while(AckFlag != 1){
				if(WaitTime == 3){//3��û���յ��ն�Ӧ��
					SetDeviceOnlineFlagBySAddr(DeviceList,posNode->DSAddr);
					break;
				}
				Send_Custom_Data(USART1,Esp32CommandStreamList->type,Esp32CommandStreamList->len,Esp32CommandStreamList->Data);//����ָ��
				AckJudge = 1;//����delay��һ���˳�delay
				delay_ms(100);//�ȴ��ն˵�Ӧ��,Ŀǰ�����ڲ�ֲ����һ����AckJudge�����ڿ�����UCOS��������ת�����Ż�CPU��Դ
			}
			AckJudge = 0;//��ֹdelay��һ���˳�delay
		}
		else if(posNode->DataDirection == 2){//׼������ESP32������
			Send_Custom_Data(USART2,posNode->type,posNode->len,posNode->Data);
		}
		//ɾ���ý��
		posNodeFront->next = posNode->next;//�󱸱�����һ��ָ��ִ�б�����һ��
		free(posNode);//ִ�б�ԭ������
		posNode = posNodeFront->next;//ִ�б��ں󱸱���һ������
	}
	//�������
}
/***************************************END***************************************/


/*********************************����Ϊ���豸��Ϣ�����������*********************************/
//�������
Device* CreateDeviceNode(u8 type, u8 onlineFlag, u8* LongAddr, u8* ShortAddr) {
	Device* newNode = (Device*)malloc(sizeof(Device));
	newNode->type = type;
	newNode->onlineFlag = onlineFlag;
	ArrCpy(8,newNode->LongAddr, LongAddr);//ע�⣡��Ҫ��strcpy
	ArrCpy(2,newNode->ShortAddr, ShortAddr);//ע�⣡��Ҫ��strcpy
	newNode->next = NULL;
	return newNode;
}

Device* CreateDeviceList(void) {
	//������ͷ
	Device* headNode = (Device*)malloc(sizeof(Device));//ָ�����˽ṹ�����
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
	}//��λ��typeΪָ�����ֻ�����һ��
	Device* newNode = CreateDeviceNode(type,onlineFlag, LongAddr, ShortAddr);
	if(posNode->type > type){//����������һ��
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
	if(posNode->next == NULL) return 0;//������û�в鵽�ó���ַ���豸
	while(posNode != NULL){
		if(ArrCmp(8,posNode->LongAddr, LongAddr) == 1) break;
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
  * @brief		�����������豸����״̬
  * @param		Device* headNode�������ͷ
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
			if(EspWaitTime == 5){//��ʱ�˳�,Ӧ����APP�����ߣ�esp����
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
		if(posNode == NULL) return;//������û�з��������Ľ��
	}

	//����Esp32��Ҳ�Ǹ���APP�и��ն������ˣ�
	Esp32AckFlag = 0;
	EspWaitTime = 0;
	while(Esp32AckFlag == 0){
		if(EspWaitTime == 5){//��ʱ�˳�
			return ;
		}
		Send_Custom_Data(USART2,0x00,0,NULL);//��ʼ�źţ��ȴ�Ӧ��
		delay_ms(200);//��΢�ȵ�
	}
	//�������
	Data[0] = posNode->type;
	Data[1] = posNode->onlineFlag;
	for(i = 0; i < 8; i++){
		Data[2+i] = posNode->LongAddr[i];
	}
	Data[i] = posNode->ShortAddr[0];
	i++;
	Data[i] = posNode->ShortAddr[1];
	Send_Custom_Data(USART2,0x00,12,Data);//����Esp
}
/***************************************END***************************************/
