#include "IIC.h"

void IIC_Init(void)
{			
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//ʹ��GPIOBʱ��

  //GPIOB8,B9��ʼ������
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��
	IIC_SCL=1;
	IIC_SDA=1;
}
void IIC_Start(void){
    SDA_OUT();//SDA�����
	//����IIC�����ǿ���״̬��û���˶���������SDA/SCL�������ߵ�Ƶ
    //���и�������������ϵڶ���ͼ��SDA���п�����RA���õģ����Ե���֮��SDA�п�����0�п�����1
	//����Ϊ�ˡ�S�����ģ���ܹ�ƴ�ϣ��Ȱ�SDA��1�����ߣ���Ϊʲô�أ���ΪSDA�п�����0�п�����1��
    IIC_SDA = 1;
    //��SCL������ƴͼ��������0��������ȷ��SDA��1���ٰ�SCL����
    IIC_SCL = 1;
    //�������Ǹտ�ʼ����������û�ã�������ǳ�����е�һ���ˣ���ʹ�õ�����������Ǿ���Ҫ�������������һ������
    //Ȼ��������ϵ�8��ͼ,�Ȱ�SDA���ͣ��ٰ�SCL����
    IIC_SDA = 0;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
    IIC_SCL = 0;//ǯס IIC ���ߣ�׼�����ͻ��������
}//���Ͼ���Start���������򵥡�д��

//�������� Stop����
void IIC_Stop(void){
	SDA_OUT();//sda �����
	IIC_SCL=0;
	//�ȱ�֤SDA��0,�����Ȱ�SDA����
    IIC_SDA = 0;
	delay_us(4);
    //�ٰ�SCL����
    IIC_SCL = 1;
    //Ȼ���ٰ�SDA����
    IIC_SDA = 1;//STOP:when CLK is high DATA change form low to high
	delay_us(4);
}
                
//�ȴ�Ӧ���źŵ��� 
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
u8 IIC_Wait_Ack(void){
	u8 ucErrTime=0;
    //�������ϵ�4ͼ���Ȱ�SDA���ߣ��ٰ�SCL����
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
	IIC_SCL=0;//ʱ����� 0          
	return 0;
}

//���� ACK Ӧ�� 
void IIC_Ack(void) { 
	IIC_SCL=0;
	SDA_OUT(); 
	IIC_SDA=0;
	delay_us(2); 
	IIC_SCL=1;
	delay_us(2); 
	IIC_SCL=0; 
} 
//������ ACK Ӧ��           
void IIC_NAck(void) { 
	IIC_SCL=0; 
	SDA_OUT(); 
	IIC_SDA=1;delay_us(2); 
	IIC_SCL=1;delay_us(2); 
	IIC_SCL=0; 
}

//������д Sendbyte����
void IIC_Send_Byte(u8 Byte){
    unsigned char i;
	SDA_OUT();
	//�Ȱ�Ҫ�����ֽڵ����λ����SDA���ٰ�SCL�ø�,�ٰ�SCL�õ�
    //IIC_SDA = Byte & 0x80;//��Byte���λȡ��������SDA
    //IIC_SCL = 1;
 	//IIC_SCL = 0;
    //���������� ѭ��8�Σ�������ֱ��ע�͵�����ģ���������д��
    for(i = 0; i < 8; i++){
        IIC_SDA=(Byte&0x80)>>7;//������Ҫ��0x80�����ֽţ�ʵ�ִӸߵ���λ���α���
        Byte<<=1; 	  
		delay_us(2);   //��TEA5767��������ʱ���Ǳ����
        IIC_SCL = 1;
		delay_us(2);
        IIC_SCL = 0;
		delay_us(2);
    }
}

//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
u8 IIC_Read_Byte(u8 ack)
{
	u8 i,receive=0;
	SDA_IN();//SDA����Ϊ����
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
        IIC_NAck();//����nACK
    else
        IIC_Ack(); //����ACK   
    return receive;
}
