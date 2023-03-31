#include "24Cxx.h" 

#include "IIC.h"
#include "delay.h"
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
   *@retval	返回0:检测失败
   *@retval	返回1:检测成功
   */
u8 AT24CXX_Check(void)
{
	u8 temp;
	temp=AT24CXX_ReadOneByte(EE_TYPE);//避免每次开机都写AT24CXX
	if(temp==0X55)return 1;
	else
	{
		AT24CXX_WriteOneByte(EE_TYPE,0X55);
		temp=AT24CXX_ReadOneByte(EE_TYPE);
		if(temp==0X55)return 1;
	}
	return 0;
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
   *@brief	清空整个AT24CXX（置零）
   *@param	void
   *@retval	void
   */

void AT24CXX_Clear(void)
{				   	  	    
	u16 i;
	for(i = 0; i < EE_TYPE; i++){
		AT24CXX_WriteOneByte(i,0);
	}
}

