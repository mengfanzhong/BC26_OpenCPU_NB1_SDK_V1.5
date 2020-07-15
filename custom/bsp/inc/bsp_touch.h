/*
*********************************************************************************************************
*
*	模块名称 : 电阻式触摸板驱动模块
*	文件名称 : bsp_touch.h
*	版    本 : V1.6
*	说    明 : 头文件
*
*	Copyright (C), 2013-2020, 华宏软件 
*
*********************************************************************************************************
*/

#ifndef __BSP_TOUCH_H
#define __BSP_TOUCH_H

#define TOUCH_FIFO_SIZE		10

typedef struct
{
	/* 2点校准 和 4点校准 */
    //触摸点坐标
	// u16 usAdcX1;	/* 左上角 */
	// u16 usAdcY1;
	// u16 usAdcX2;	/* 右下角 */
	// u16 usAdcY2;
	// u16 usAdcX3;	/* 左下角 */
	// u16 usAdcY3;
	// u16 usAdcX4;	/* 右上角 */
	// u16 usAdcY4;

	// u16 XYChange;	/* X, Y 是否交换  */

	u8 Enable;		/* 触摸检测使能标志 */

	u8 Event[TOUCH_FIFO_SIZE];	/* 触摸事件 */
	s16 XBuf[TOUCH_FIFO_SIZE];	/* 触摸坐标缓冲区 */
	s16 YBuf[TOUCH_FIFO_SIZE];	/* 触摸坐标缓冲区 */
	u8 Read;					/* 缓冲区读指针 */
	u8 Write;					/* 缓冲区写指针 */
}TOUCH_T;

/* 触摸事件 */
enum
{
	TOUCH_NONE = 0,		/* 无触摸 */
	TOUCH_DOWN = 1,		/* 按下 */
	TOUCH_MOVE = 2,		/* 移动 */
	TOUCH_RELEASE = 3	/* 释放 */
};

/* 全局参数 */
typedef struct
{
	// u32 ParamVer;			/* 参数区版本控制（可用于程序升级时，决定是否对参数区进行升级） */

//	/* LCD背光亮度 */
//	u8 ucBackLight;

	/* 触摸屏校准参数 */
	//{
		// u16 usAdcX1;	/* 左上角 */
		// u16 usAdcY1;
		// u16 usAdcX2;	/* 右下角 */
		// u16 usAdcY2;
		// u16 usAdcX3;	/* 左下角 */
		// u16 usAdcY3;
		// u16 usAdcX4;	/* 右上角 */
		// u16 usAdcY4;
       
		u8 XYChange;		/* X, Y 是否交换， 1表示i切换，0表示不切换  */
		   
		u8 TouchDirection;	/* 屏幕方向 0-3  0表示横屏，1表示横屏180° 2表示竖屏 3表示竖屏180° */	 
    
//		u16 usLcdX1;	/* 校准时，屏幕坐标 */
//		u16 usLcdY1;	/* 校准时，屏幕坐标 */
//		u16 usLcdX2;	/* 校准时，屏幕坐标 */
//		u16 usLcdY2;	/* 校准时，屏幕坐标 */
//		u16 usLcdX3;	/* 校准时，屏幕坐标 */
//		u16 usLcdY3;	/* 校准时，屏幕坐标 */
//		u16 usLcdX4;	/* 校准时，屏幕坐标 */
//		u16 usLcdY4;	/* 校准时，屏幕坐标 */	
	//}
}PARAM_T;

extern PARAM_T g_tParam;

/* 供外部调用的函数声明 */
void 	TOUCH_InitHard(void);

u8 		TOUCH_GetKey(s16 *_pX, s16 *_pY);

void 	TOUCH_CelarFIFO(void);

u8 		TOUCH_InRect(u16 _usX, u16 _usY, u16 _usRectX, u16 _usRectY, u16 _usRectH, u16 _usRectW);

void 	TOUCH_PutKey(u8 _ucEvent, u16 _usX, u16 _usY);

void 	TOUCH_CapScan(void);

/* 供外部调用的变量声明 */
extern 	TOUCH_T g_tTP;

#endif

/**************************************** 华宏软件 (END OF FILE) ****************************************/
