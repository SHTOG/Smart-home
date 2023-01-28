#include "IIC.h"
#include "Delay.h"
#include "OLED.h"
/*
	注意下，我自己写的OLED字符取模是行列式，u8g2是逐列式
*/
/**
   *@brief 初始化OLED
   *@param void
   *@retval void
   */
void OLED_Init(void){
	IIC_Init(); //初始化软件IIC
	delay_ms(500);
	OLED_Write_Cmd(0xae);//–turn off OLED panel 关闭显示
	OLED_Write_Cmd(0x00);//—set low column address设置起始列的低四位 0x0x
	OLED_Write_Cmd(0x10);//—set high column address设置起始列的高四位0x1x
	OLED_Write_Cmd(0x40);//–set start line address Set Mapping RAM Display Start Line (0x00~0x3F)
	OLED_Write_Cmd(0x81);//–set contrast control register设置对比度 
	OLED_Write_Cmd(0xff); // Set SEG Output Current Brightness对比度为oxff
	OLED_Write_Cmd(0xa1);//–Set SEG/Column Mapping 0xa0左右反置 0xa1正常
	OLED_Write_Cmd(0xc8);//Set COM/Row Scan Direction 0xc0上下反置 0xc8正常
	OLED_Write_Cmd(0xa6);//–set normal display
	OLED_Write_Cmd(0xa8);//–set multiplex ratio(1 to 64)
	OLED_Write_Cmd(0x3f);//–1/64 duty
	OLED_Write_Cmd(0xd3);//-set display offset Shift Mapping RAM Counter (0x00~0x3F)
	OLED_Write_Cmd(0x00);//-not offset
	OLED_Write_Cmd(0xd5);//–set display clock divide ratio/oscillator frequency
	OLED_Write_Cmd(0x80);//–set divide ratio, Set Clock as 100 Frames/Sec
	OLED_Write_Cmd(0xd9);//–set pre-charge period
	OLED_Write_Cmd(0xf1);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	OLED_Write_Cmd(0xda);//–set com pins hardware configuration
	OLED_Write_Cmd(0x12);
	OLED_Write_Cmd(0xdb);//–set vcomh
	OLED_Write_Cmd(0x40);//Set VCOM Deselect Level
	OLED_Write_Cmd(0x20);//-Set Page Addressing Mode (0x00/0x01/0x02)设置地址模式
	//水平寻址，垂直寻址，页寻址
	OLED_Write_Cmd(0x02);// 地址模式为页寻址
	OLED_Write_Cmd(0x8d);//–set Charge Pump enable/disable
	OLED_Write_Cmd(0x14);//–set(0x10) disable
	OLED_Write_Cmd(0xa4);// Disable Entire Display On (0xa4/0xa5)
	OLED_Write_Cmd(0xa6);// Disable Inverse Display On (0xa6写地址/a7)
	OLED_Write_Cmd(0xaf);//–turn on OLED panel开启显示
	delay_ms(100);

	OLED_Clear();//清屏
}

/**
   *@brief 写控制命令函数
   *@param 十六进制数据
   *@retval NULL
   */

void OLED_Write_Cmd(unsigned char cmd){
	IIC_Start();//启动
	IIC_Send_Byte(0X78);//写地址
	IIC_Wait_Ack();//接收从机应答
	IIC_Send_Byte(0X00);//发送控制命令字节
	IIC_Wait_Ack();//接收从机应答
	IIC_Send_Byte(cmd);//发送命令
	IIC_Wait_Ack();//接收从机应答
	IIC_Stop();//停止
}

/**
   *@brief 写数据函数
   *@param 十六进制数据
   *@retval void
   */

void OLED_Write_Byte(unsigned char byte){
	IIC_Start();//启动
	IIC_Send_Byte(0X78);//写地址
	IIC_Wait_Ack();//接收从机应答
	IIC_Send_Byte(0X40);//发送控制命令字节
	IIC_Wait_Ack();//接收从机应答
	IIC_Send_Byte(byte);//发送命令
	IIC_Wait_Ack();//接收从机应答
	IIC_Stop();//停止
}

/**
   *@brief 清屏
   *@param void
   *@retval void
   */
void OLED_Clear(void){
    unsigned char i,j;
	OLED_Put_Char_8x16(15,6);//光标放在最右下边，保证清屏函数
    OLED_Write_Cmd(0x20);
    OLED_Write_Cmd(0x02);

    for(i = 0; i < 8; i++){
        OLED_Write_Cmd(0xb0+i);
        OLED_Write_Cmd(0x00);
        OLED_Write_Cmd(0x10);
        for(j =0;j<128;j++){
            OLED_Write_Byte(0x00);
        }
    }
}

/**
   *@brief	设置显示位置,并设置地址模式为水平选址
   *@param	x -> colum; y -> page; t=0 -> 8*16 / t=1 -> 16*16
   *@retval
   */
void OLED_Put_Char_8x16(unsigned char x,unsigned char y){//x(0~15),y(0~6)
    unsigned char t = 0;
    OLED_Write_Cmd(0x20);
    OLED_Write_Cmd(0x00);//设置地址模式为水平选址
    //set page
    OLED_Write_Cmd(0x22);
    OLED_Write_Cmd(y);
    OLED_Write_Cmd(0x01+y);
    //set colum
    OLED_Write_Cmd(0x21);
    OLED_Write_Cmd((0x08+0x08*t)*x);
    OLED_Write_Cmd((0x08+0x08*t)*x+(0x07+0x08*t));
}
void OLED_Put_Char_16x16(unsigned char x,unsigned char y){//x(0~7),y(0~3)
    unsigned char t = 1;
    OLED_Write_Cmd(0x20);
    OLED_Write_Cmd(0x00);//设置地址模式为水平选址
    //set page
    OLED_Write_Cmd(0x22);
    OLED_Write_Cmd(y*2);
    OLED_Write_Cmd(0x01+y*2);
    //set colum
    OLED_Write_Cmd(0x21);
    OLED_Write_Cmd((0x08+0x08*t)*x);
    OLED_Write_Cmd((0x08+0x08*t)*x+(0x07+0x08*t));
}

/**
   *@brief 显示英文
   *@param x -> colum(0~15); y ->row(0~6);arr -> 传入的英文库名; length -> 库中前n位英文
   *@retval
   */

void OLED_Show_English(unsigned char x,unsigned char y,unsigned char arr[][16],unsigned char length){
	unsigned char i,j;
	for(i=0;i<length;i++){
		OLED_Put_Char_8x16(x+i,y);
		for(j = 0; j < 16; j++){
			OLED_Write_Byte(arr[i][j]);
		}
	}
}

/**
   *@brief 显示中文
   *@param x -> colum(0~7); y ->page(0~3);arr -> 传入的中文库名; length -> 库中前n位中文
   *@retval
   */

void OLED_Show_Chinese(unsigned char x,unsigned char y,const unsigned char arr[][32],unsigned char length){
	unsigned char i,j;
	for(i=0;i<length;i++){
		OLED_Put_Char_16x16(x+i,y);
			for(j = 0; j < 32; j++){
				OLED_Write_Byte(arr[i][j]);
			}
	}
}

/**
   *@brief 显示数字
   *@param x -> colum(0~15); y ->page(0~6); num ->具体数字(0~99); 
   *@retval
   */
//数字库
const unsigned char number[][16] = {
{0x00,0xE0,0x10,0x08,0x08,0x10,0xE0,0x00,0x00,0x0F,0x10,0x20,0x20,0x10,0x0F,0x00},/*"0",0*/
/* (8 X 16 , 宋体 )*/

{0x00,0x00,0x10,0x10,0xF8,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00},/*"1",1*/
/* (8 X 16 , 宋体 )*/

{0x00,0x70,0x08,0x08,0x08,0x08,0xF0,0x00,0x00,0x30,0x28,0x24,0x22,0x21,0x30,0x00},/*"2",2*/
/* (8 X 16 , 宋体 )*/

{0x00,0x30,0x08,0x08,0x08,0x88,0x70,0x00,0x00,0x18,0x20,0x21,0x21,0x22,0x1C,0x00},/*"3",3*/
/* (8 X 16 , 宋体 )*/

{0x00,0x00,0x80,0x40,0x30,0xF8,0x00,0x00,0x00,0x06,0x05,0x24,0x24,0x3F,0x24,0x24},/*"4",4*/
/* (8 X 16 , 宋体 )*/

{0x00,0xF8,0x88,0x88,0x88,0x08,0x08,0x00,0x00,0x19,0x20,0x20,0x20,0x11,0x0E,0x00},/*"5",5*/
/* (8 X 16 , 宋体 )*/

{0x00,0xE0,0x10,0x88,0x88,0x90,0x00,0x00,0x00,0x0F,0x11,0x20,0x20,0x20,0x1F,0x00},/*"6",6*/
/* (8 X 16 , 宋体 )*/

{0x00,0x18,0x08,0x08,0x88,0x68,0x18,0x00,0x00,0x00,0x00,0x3E,0x01,0x00,0x00,0x00},/*"7",7*/
/* (8 X 16 , 宋体 )*/

{0x00,0x70,0x88,0x08,0x08,0x88,0x70,0x00,0x00,0x1C,0x22,0x21,0x21,0x22,0x1C,0x00},/*"8",8*/
/* (8 X 16 , 宋体 )*/

{0x00,0xF0,0x08,0x08,0x08,0x10,0xE0,0x00,0x00,0x01,0x12,0x22,0x22,0x11,0x0F,0x00},/*"9",9*/
/* (8 X 16 , 宋体 )*/

};
void OLED_Show_Number(unsigned char x,unsigned char y, unsigned char num){
	unsigned char j;
	if(num <= 9){
		OLED_Put_Char_8x16(x,y);
		for(j = 0; j < 16; j++){
			OLED_Write_Byte(number[num][j]);
		}
	}
	else if(num > 9 && num < 100){
		OLED_Put_Char_8x16(x-1,y);
		for(j = 0; j < 16; j++){
			OLED_Write_Byte(number[num/10][j]);
		}
		OLED_Put_Char_8x16(x,y);
		for(j = 0; j < 16; j++){
			OLED_Write_Byte(number[num%10][j]);
		}
	}
	else{
		OLED_Put_Char_8x16(x,y);
		for(j = 0; j < 16; j++){
			OLED_Write_Byte(number[num%10][j]);
		}
		OLED_Put_Char_8x16(x-1,y);
		for(j = 0; j < 16; j++){
			OLED_Write_Byte(number[num/10%10][j]);
		}
		OLED_Put_Char_8x16(x-2,y);
		for(j = 0; j < 16; j++){
			OLED_Write_Byte(number[num/100][j]);
		}
	}
}


/**
 * @brief	显示图片
 * @param	length -> 图片长度(0~128); width -> 图片宽度(0~64)
 * @retval	
 */


void OLED_Show_Picture(unsigned char x,unsigned char y,unsigned char arr[][8],unsigned char length,unsigned char width){
	unsigned char i,j,k,len,wid;
	len = length / 8 ;
	length %= 8;
	wid = width / 8;
	for(i=0;i<wid;i++){
		for(j= 0; j < len; j++){
			OLED_Put_Char_8x16(x+j,y+i);
			for(k = 0; k < 8; k++){
				OLED_Write_Byte(arr[i*len+j][k]);
			}
		}
	}
}

/**
 * @brief	显示大号数字(16*32)
 * @param	length -> 图片长度(0~128); width -> 图片宽度(0~64)
 * @retval	
 */

const unsigned char bigNumber[][16] = {
{0x00,0x00,0x00,0x00,0x00,0x80,0xC0,0x40},
{0x40,0x40,0x80,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0xF0,0xFE,0x0F,0x01,0x00,0x00},
{0x00,0x00,0x01,0x07,0xFE,0xF0,0x00,0x00},
{0x00,0x00,0x3F,0xFF,0xC0,0x00,0x00,0x00},
{0x00,0x00,0x00,0x80,0xFF,0x3F,0x00,0x00},
{0x00,0x00,0x00,0x01,0x03,0x06,0x0C,0x08},
{0x08,0x08,0x06,0x03,0x01,0x00,0x00,0x00},/*"0",0*/
/* (16 X 32 , 宋体 )*/

{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80},
{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x01,0x01,0x01,0x01,0xFF},
{0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF},
{0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x08,0x08,0x08,0x0C,0x0F},
{0x0F,0x0C,0x08,0x08,0x08,0x00,0x00,0x00},/*"1",1*/
/* (16 X 32 , 宋体 )*/

{0x00,0x00,0x00,0x00,0x80,0x40,0x40,0x40},
{0x40,0x40,0xC0,0x80,0x80,0x00,0x00,0x00},
{0x00,0x00,0x1E,0x19,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0xC1,0xFF,0x3E,0x00,0x00},
{0x00,0x00,0x00,0x00,0x80,0x40,0x30,0x18},
{0x0C,0x06,0x03,0x01,0x00,0xC0,0x00,0x00},
{0x00,0x00,0x0E,0x0D,0x0C,0x0C,0x0C,0x0C},
{0x0C,0x0C,0x0C,0x0C,0x0E,0x03,0x00,0x00},/*"2",2*/
/* (16 X 32 , 宋体 )*/

{0x00,0x00,0x00,0x80,0x80,0x40,0x40,0x40},
{0x40,0xC0,0x80,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x0F,0x0F,0x00,0x00,0x00,0x00},
{0x00,0x80,0xC1,0x7F,0x3E,0x00,0x00,0x00},
{0x00,0x00,0xC0,0xC0,0x00,0x00,0x01,0x01},
{0x01,0x03,0x02,0x06,0xFC,0xF0,0x00,0x00},
{0x00,0x00,0x03,0x07,0x04,0x08,0x08,0x08},
{0x08,0x08,0x04,0x06,0x03,0x00,0x00,0x00},/*"3",3*/
/* (16 X 32 , 宋体 )*/

{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0xC0,0xC0,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x80,0x60,0x10,0x0C},
{0x03,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00},
{0x00,0x30,0x2C,0x26,0x21,0x20,0x20,0x20},
{0x20,0xFF,0xFF,0x20,0x20,0x20,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x08},
{0x08,0x0F,0x0F,0x08,0x08,0x08,0x00,0x00},/*"4",4*/
/* (16 X 32 , 宋体 )*/

{0x00,0x00,0x00,0x00,0xC0,0xC0,0xC0,0xC0},
{0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0x00,0x00},
{0x00,0x00,0x00,0xFF,0x00,0x80,0x40,0x40},
{0x40,0x40,0xC0,0x80,0x00,0x00,0x00,0x00},
{0x00,0x00,0xC0,0xC3,0x01,0x00,0x00,0x00},
{0x00,0x00,0x00,0x01,0xFF,0xFC,0x00,0x00},
{0x00,0x00,0x03,0x04,0x04,0x08,0x08,0x08},
{0x08,0x08,0x04,0x07,0x03,0x00,0x00,0x00},/*"5",5*/
/* (16 X 32 , 宋体 )*/

{0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x40},
{0x40,0x40,0x40,0x80,0x80,0x00,0x00,0x00},
{0x00,0x00,0xE0,0xFC,0x07,0x81,0xC0,0x40},
{0x40,0x40,0xC0,0x83,0x03,0x00,0x00,0x00},
{0x00,0x00,0x7F,0xFF,0x83,0x00,0x00,0x00},
{0x00,0x00,0x00,0x01,0xFF,0xFC,0x00,0x00},
{0x00,0x00,0x00,0x01,0x07,0x06,0x0C,0x08},
{0x08,0x08,0x0C,0x06,0x03,0x00,0x00,0x00},/*"6",6*/
/* (16 X 32 , 宋体 )*/

{0x00,0x00,0x00,0xC0,0xC0,0xC0,0xC0,0xC0},
{0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0x00,0x00},
{0x00,0x00,0x0E,0x03,0x00,0x00,0x00,0x00},
{0x00,0xE0,0x18,0x06,0x01,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0xFC},
{0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0x0F},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"7",7*/
/* (16 X 32 , 宋体 )*/

{0x00,0x00,0x00,0x00,0x80,0xC0,0x40,0x40},
{0x40,0x40,0xC0,0x80,0x00,0x00,0x00,0x00},
{0x00,0x00,0x3E,0x7F,0xF1,0xC0,0xC0,0x80},
{0x00,0x00,0x80,0x41,0x7F,0x1E,0x00,0x00},
{0x00,0xF0,0xFC,0x0E,0x02,0x01,0x01,0x01},
{0x03,0x07,0x0E,0x1E,0xFC,0xF0,0x00,0x00},
{0x00,0x01,0x03,0x06,0x04,0x08,0x08,0x08},
{0x08,0x08,0x04,0x06,0x03,0x01,0x00,0x00},/*"8",8*/
/* (16 X 32 , 宋体 )*/

{0x00,0x00,0x00,0x80,0x80,0x40,0x40,0x40},
{0x40,0x40,0x80,0x00,0x00,0x00,0x00,0x00},
{0x00,0xFC,0xFF,0x03,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x03,0xFE,0xF8,0x00,0x00},
{0x00,0x01,0x03,0x07,0x0C,0x08,0x08,0x08},
{0x08,0x04,0x06,0xE1,0x7F,0x1F,0x00,0x00},
{0x00,0x00,0x07,0x07,0x08,0x08,0x08,0x08},
{0x0C,0x06,0x03,0x01,0x00,0x00,0x00,0x00},/*"9",9*/
/* (16 X 32 , 宋体 )*/

{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0xE0},
{0xE0,0xC0,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01},
{0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x0F},
{0x0F,0x06,0x00,0x00,0x00,0x00,0x00,0x00},/*":",10*/
/* (16 X 32 , 宋体 )*/
};

void OLED_Show_BigNumber(unsigned char x,unsigned char y,unsigned char num){
	unsigned char i,j,k,len,wid;
	len = 2;
	wid = 4;
	for(i=0;i<wid;i++){
		for(j= 0; j < len; j++){
			OLED_Put_Char_8x16(x+j,y+i);
			for(k = 0; k < 8; k++){
				OLED_Write_Byte(bigNumber[8*num+i*len+j][k]);
			}
		}
	}
}
