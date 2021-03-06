/*
*********************************************************************************************************
*
*	模块名称 : 电池监测模块
*	文件名称 : battery.h
*	说    明 : 头文件
*
*	修改记录 :
*		版本号    日期        作者     说明
*		V1.0    2020/04/28   孟凡忠  正式发布
*
*	Copyright (C), 2015-2020, 华宏软件
*
*********************************************************************************************************
*/

#ifndef __BATTERY_H
#define __BATTERY_H

/* 电池充电状态 */
typedef enum 
{
	NOT_CHARGE = 0,		/* 没有充电 */
	CHARGE, 			/* 正在充电 */	
}CHARGE_STATE;

/* 电池结构体 */
typedef struct
{
	u8 charge;			/* 电池充电状态  */
	u32 level;			/* 电池电量 0 ~ 100 */
}BATTERY_T;

/* 供外部调用的函数声明 */
void Battery_InitHard(void);
void Battery_Scan(void);

/* 供外部调用的变量 */
extern BATTERY_T g_battery_t;

#endif

/***************************** 华宏软件 (END OF FILE) *********************************/

