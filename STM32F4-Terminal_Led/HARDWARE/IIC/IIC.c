#include "IIC.h"
#include "delay.h"

void IIC_Init(void)
{			
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能GPIOB时钟

  //GPIOB8,B9初始化设置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化
	IIC_SCL=1;
	IIC_SDA=1;
}

//产生IIC起始信号
void IIC_Start(void){
    SDA_OUT();//SDA线输出
	//开局IIC总线是空闲状态，没有人动他，所以SDA/SCL是两个高电频
    //还有个情况：根据往上第二张图，SDA还有可能是RA调用的，所以调用之后SDA有可能是0有可能是1
	//所以为了“S”这个模块能够拼上，先把SDA置1（拉高）。为什么呢？因为SDA有可能是0有可能是1，
    IIC_SDA = 1;
    IIC_SCL = 1;
	delay_us(4);
    //而SCL在其他拼图结束后都是0，所以先确保SDA是1，再把SCL拉高
    //如果这就是刚开始，那上两句没用，如果这是程序进行到一半了，才使用到这个函数，那就需要按上面的流程来一遍拉高
    //然后根据往上第8个图,先把SDA拉低，再把SCL拉低
    IIC_SDA = 0;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
    IIC_SCL = 0;//钳住 IIC 总线，准备发送或接收数据
}//以上就是Start函数，“简单”写完

//接下来是 Stop函数
void IIC_Stop(void){
	SDA_OUT();//sda 线输出
	//先保证SDA是0,所以先把SDA拉低(SCL在每个函数结束时都是拉低的状态，这里就可以不写)
    IIC_SDA = 0;
	delay_us(4);
    //再把SCL拉高
    IIC_SCL = 1;
    //然后再把SDA拉高
    IIC_SDA = 1;//STOP:when CLK is high DATA change form low to high
}
                
//等待应答信号到来 
//返回值：1，接收应答失败
//        0，接收应答成功
unsigned char IIC_Wait_Ack(){
	unsigned char ucErrTime=0;
    //根据往上第4图，先把SDA拉高，再把SCL拉高
	SDA_IN();
    IIC_SDA = 1;
	delay_us(1);
    IIC_SCL = 1;
	delay_us(1);
	while(READ_SDA) {
		ucErrTime++; 
		if(ucErrTime>250){
			IIC_Stop(); 
			return 1;
		} 
	} 
	IIC_SCL=0;//时钟输出 0          
	return 0;
}

//产生 ACK 应答 
void IIC_Ack(void) { 
	IIC_SCL=0;
	SDA_OUT(); 
	IIC_SDA=0;
	delay_us(2); 
	IIC_SCL=1;
	delay_us(2); 
	IIC_SCL=0; 
} 
//不产生 ACK 应答           
void IIC_NAck(void) { 
	IIC_SCL=0; 
	SDA_OUT(); 
	IIC_SDA=1;delay_us(2); 
	IIC_SCL=1;delay_us(2); 
	IIC_SCL=0; 
}

//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	SDA_OUT(); 	    
    IIC_SCL=0;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
        IIC_SDA=(txd&0x80)>>7;
        txd<<=1; 	  
		delay_us(2);   //对TEA5767这三个延时都是必须的
		IIC_SCL=1;
		delay_us(2); 
		IIC_SCL=0;	
		delay_us(2);
    }	 
} 	

//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
        IIC_SCL=0; 
        delay_us(2);
		IIC_SCL=1;
        receive<<=1;
        if(READ_SDA)receive++;   
		delay_us(1); 
    }					 
    if (!ack)
        IIC_NAck();//发送nACK
    else
        IIC_Ack(); //发送ACK   
    return receive;
}

