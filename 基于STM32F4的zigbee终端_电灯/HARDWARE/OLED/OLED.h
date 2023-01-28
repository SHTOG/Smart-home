#ifndef __OLED_H__
#define __OLED_H__
#include "OLEDfont.h"
void OLED_Write_Cmd(unsigned char cmd);
void OLED_Write_Byte(unsigned char byte);
void OLED_Clear(void);
void OLED_Init(void);
void OLED_Put_Char_8x16(unsigned char x,unsigned char y);
void OLED_Put_Char_16x16(unsigned char x,unsigned char y);
void OLED_Show_English(unsigned char x,unsigned char y,unsigned char arr[][16],unsigned char length);
void OLED_Show_Chinese(unsigned char x,unsigned char y,const unsigned char arr[][32],unsigned char length);
void OLED_Show_Number(unsigned char x,unsigned char y, unsigned char num);
void OLED_Show_Picture(unsigned char x,unsigned char y,unsigned char arr[][8],unsigned char length,unsigned char width);
void OLED_Show_BigNumber(unsigned char x,unsigned char y,unsigned char num);

#endif
