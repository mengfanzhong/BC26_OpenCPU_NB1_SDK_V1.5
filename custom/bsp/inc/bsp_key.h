/*
*********************************************************************************************************
*
*	模块名称 : 按键驱动模块
*	文件名称 : bsp_key.h
*	说    明 : 头文件
*
*	Copyright (C), 2015-2020, 华宏软件
*
*********************************************************************************************************
*/

#ifndef __BSP_KEY_H
#define __BSP_KEY_H

#define KEY_COUNT    1	   		/* 按键个数 */

/* 根据应用程序的功能重命名按键宏 */
#define KEY_DOWN_POWER		KEY_1_DOWN      /* 电源按键 */
#define KEY_UP_POWER		KEY_1_UP
#define KEY_LONG_POWER		KEY_1_LONG


/* 按键ID, 主要用于bsp_KeyState()函数的入口参数 */
typedef enum
{
	KID_K1 = 0,	
}KEY_ID_E;

/*
	按键滤波时间50ms, 单位10ms。
	只有连续检测到50ms状态不变才认为有效，包括弹起和按下两种事件
	即使按键电路不做硬件滤波，该滤波机制也可以保证可靠地检测到按键事件
*/
#define KEY_FILTER_TIME   5
#define KEY_LONG_TIME     100			/* 单位10ms， 持续1秒，认为长按事件 */

/*
	每个按键对应1个全局的结构体变量。
*/
typedef struct
{
	/* 下面是一个函数指针，指向判断按键手否按下的函数 */
	u8 (*IsKeyDownFunc)(void); /* 按键按下的判断函数,1表示按下 */

	u8  Count;			/* 滤波器计数器 */
	u16 LongCount;		/* 长按计数器 */
	u16 LongTime;		/* 按键按下持续时间, 0表示不检测长按 */
	u8  State;			/* 按键当前状态（按下还是弹起） */
	u8  RepeatSpeed;	/* 连续按键周期 */
	u8  RepeatCount;	/* 连续按键计数器 */
}KEY_T;

/*
	定义键值代码, 必须按如下次序定义每个键的按下、弹起和长按事件

	推荐使用enum, 不用#define，原因：
	(1) 便于新增键值,方便调整顺序，使代码看起来舒服点
	(2) 编译器可帮我们避免键值重复。
*/
typedef enum
{
	KEY_NONE = 0,			/* 0 表示没有按键事件 */

	KEY_1_DOWN,				/* 1键按下 */
	KEY_1_UP,				/* 1键弹起 */
	KEY_1_LONG,				/* 1键长按 */

}KEY_ENUM;

/* 按键FIFO用到变量 */
#define KEY_FIFO_SIZE	10
typedef struct
{
	u8 Buf[KEY_FIFO_SIZE];		/* 键值缓冲区 */
	u8 Read;					/* 缓冲区读指针1 */
	u8 Write;					/* 缓冲区写指针 */
//	u8 Read2;					/* 缓冲区读指针2 */
}KEY_FIFO_T;

/* 供外部调用的函数声明 */
void bsp_InitKey(void);
void bsp_KeyScan(void);
u8 bsp_GetKey(void);
void bsp_ClearKey(void);

#endif

/***************************** 华宏软件 (END OF FILE) *********************************/

