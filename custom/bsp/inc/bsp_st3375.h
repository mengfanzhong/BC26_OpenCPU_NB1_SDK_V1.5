/*
*********************************************************************************************************
*
*	模块名称 : RA8875芯片驱动模块器驱动模块
*	文件名称 : LCD_RA8875.h
*	版    本 : V1.6
*	说    明 : 头文件
*
*	Copyright (C), 2013-2020, 华宏软件 
*
*********************************************************************************************************
*/


#ifndef _LCD_ST3375_H
#define _LCD_ST3375_H



/* 可供外部模块调用的函数 */
void ST3375_InitHard(void);
void ST3375_SetDirection(u8 _dir);
void ST3375_DispOn(void);
void ST3375_DispOff(void);
void ST3375_PutPixel(u16 _usX, u16 _usY, u16 _usColor);
void ST3375_ClrScr(u16 _usColor);
void ST3375_DrawLine(u16 _usX1 , u16 _usY1 , u16 _usX2 , u16 _usY2 , u16 _usColor);
void ST3375_DrawRect(u16 _usX, u16 _usY, u16 _usHeight, u16 _usWidth, u16 _usColor);
void ST3375_FillRect(u16 _usX, u16 _usY, u16 _usHeight, u16 _usWidth, u16 _usColor);
void ST3375_DrawCircle(u16 _usX, u16 _usY, u16 _usRadius, u16 _usColor);
void ST3375_DispPicture(u16 StartX,u16 StartY,u16 EndX,u16 EndY,u16 *pic);

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
