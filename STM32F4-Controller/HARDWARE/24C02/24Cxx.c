#include "24Cxx.h" 

//初始化IIC接口
void AT24CXX_Init(void){
	IIC_Init();//IIC初始化
}

/**
   *@brief	在AT24CXX指定地址读出一个数据
   *@param	ReadAddr:开始读数的地址  对C02为0~255
   *@retval	读取到的数据
   */

u8 AT24CXX_ReadOneByte(u16 ReadAddr)
{
	u8 temp=0;
    IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);	   //发送写命令
		IIC_Wait_Ack();
		IIC_Send_Byte(ReadAddr>>8);//发送高地址
	}
	else{
		IIC_Send_Byte(0XA0+((ReadAddr/256)<<1));   //发送器件地址0XA0,写数据 	   
	}
	IIC_Wait_Ack(); 
    IIC_Send_Byte(ReadAddr%256);   //发送低地址
	IIC_Wait_Ack();	    
	IIC_Start();  	 	   
	IIC_Send_Byte(0XA1);           //进入接收模式			   
	IIC_Wait_Ack();	 
    temp=IIC_Read_Byte(0);		   
    IIC_Stop();//产生一个停止条件	    
	return temp;
}

/**
   *@brief	在AT24CXX指定地址写一个数据
   *@param	WriteAddr	:写入数据的目的地址  对C02为0~255
   *@param	DataToWrite	:要写入的数据
   *@retval	void
   */

void AT24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite)
{				   	  	    																 
    IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);	    //发送写命令
		IIC_Wait_Ack();
		IIC_Send_Byte(WriteAddr>>8);//发送高地址	  
	}else IIC_Send_Byte(0XA0+((WriteAddr/256)<<1));   //发送器件地址0XA0,写数据 	 
	IIC_Wait_Ack();	   
    IIC_Send_Byte(WriteAddr%256);   //发送低地址
	IIC_Wait_Ack(); 	 										  		   
	IIC_Send_Byte(DataToWrite);     //发送字节							   
	IIC_Wait_Ack();  		    	   
    IIC_Stop();//产生一个停止条件 
	delay_ms(10);//写完一个字节delay10ms为下一次写留空间 
}

/**
   *@brief	在AT24CXX里面的指定地址开始读出长度为Len的数据
   *@param	ReadAddr：开始读出的地址  对C02为0~255
   *@param	Len		:要读出数据的长度2,4
   *@retval	数据
   */

u32 AT24CXX_ReadLenByte(u16 ReadAddr,u8 Len)
{  	
	u8 t;
	u32 temp=0;
	for(t=0;t<Len;t++)
	{
		temp<<=8;
		temp+=AT24CXX_ReadOneByte(ReadAddr+Len-t-1); 	 				   
	}
	return temp;												    
}


/**
   *@brief	在AT24CXX里面的指定地址开始写入长度为Len的数据，该函数用于写入16bit或者32bit的数据.
   *@param	WriteAddr  :开始写入的地址  对C02为0~255
   *@param	DataToWrite:数据数组首地址  
   *@param	Len        :要写入数据的长度2,4
   *@retval	
   */

void AT24CXX_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len)
{  	
	u8 t;
	for(t=0;t<Len;t++)
	{
		AT24CXX_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
	}												    
}

/**
   *@brief	检查AT24CXX是否正常,这里用了24Cxx的最后一个地址(EE_TYPE)来存储标志字,更换24Cxx芯片时在头文件中修改EE_TYPE的宏定义即可
   *@param	void
   *@retval	返回1:检测失败
   *@retval	返回0:检测成功
   */

u8 AT24CXX_Check(void)
{
	u8 temp;
	temp=AT24CXX_ReadOneByte(EE_TYPE);//避免每次开机都写AT24CXX			   
	if(temp==0X55)return 0;		   
	else//排除第一次初始化的情况
	{
		temp=AT24CXX_ReadOneByte(EE_TYPE);
		if(temp==0X55)return 0;
	}
	return 1;											  
}

/**
   *@brief	在AT24CXX里面的指定地址开始读出指定个数的数据
   *@param	ReadAddr :开始读出的地址 对24c02为0~255
   *@param	pBuffer  :数据数组首地址
   *@param	NumToRead:要读出数据的个数
   *@retval	void
   */
void AT24CXX_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++=AT24CXX_ReadOneByte(ReadAddr++);	
		NumToRead--;
	}
}  

/**
   *@brief	在AT24CXX里面的指定地址开始写入指定个数的数据
   *@param	WriteAddr :开始写入的地址 对24c02为0~255
   *@param	pBuffer   :数据数组首地址
   *@param	NumToWrite:要写入数据的个数
   *@retval	void
   */
void AT24CXX_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite)
{
	while(NumToWrite--)
	{
		AT24CXX_WriteOneByte(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
	}
}

/**
   *@brief	在AT24CXX里面的指定地址开始写入链表
   *@param	WriteAddr	:开始写入的地址 对24c02为0~255
   *@param	headNode	:结构体指针
   *@retval	void
   */
void AT24CXX_Save_List(u16 WriteAddr,struct myDevice* headNode){
	u8 i;
	Device* posNode = headNode->next;
	AT24CXX_WriteOneByte(EE_TYPE-1,0X66);//24Cxx设备的在倒数第二位写66，表示已经保存过链表
	while(posNode != NULL){
		AT24CXX_WriteOneByte(WriteAddr++,posNode->type);
		for(i = 0; i < 8; i++){
			AT24CXX_WriteOneByte(WriteAddr++,posNode->LongAddr[i]);
		}
		for(i = 0; i < 2; i++){
			AT24CXX_WriteOneByte(WriteAddr++,posNode->ShortAddr[i]);
		}
		posNode = posNode->next;
	}
	AT24CXX_WriteOneByte(WriteAddr, 0xFF);//在链表的最末端写一个0xFF封尾

}


/**
   *@brief	在AT24CXX里面的指定地址开始读取链表
   *@param	ReadAddr:开始读取的地址 对24c02为0~255
   *@param	
   *@retval	Device	:读取到的结构体指针
   */

struct myDevice* AT24CXX_Load_List(u16 ReadAddr){
	u8 type,i;
	u8 LongAddr[8];
	u8 ShortAddr[2];
	Device* newList = CreateList();
	if(AT24CXX_ReadOneByte(EE_TYPE-1) == 0x66){//如果曾写过链表,则读取，否则就只是创建了一个新的链表
		while(AT24CXX_ReadOneByte(ReadAddr) != 0xFF){
		type = AT24CXX_ReadOneByte(ReadAddr++);
		for(i = 0; i < 8; i++){
			LongAddr[i] = AT24CXX_ReadOneByte(ReadAddr++);
		}
		for(i = 0; i < 2; i++){
			ShortAddr[i] = AT24CXX_ReadOneByte(ReadAddr++);
		}
		InsertNodeByType(newList, type,0, LongAddr, ShortAddr);
	}
	}
	return newList;
}




