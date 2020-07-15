/*
*********************************************************************************************************
*
*	模块名称 : 振动马达驱动模块
*	文件名称 : bsp_motor.h
*	说    明 : 头文件
*
*	Copyright (C), 2015-2020, 华宏软件
*
*********************************************************************************************************
*/

#ifndef __BSP_MOTOR_H
#define __BSP_MOTOR_H

typedef struct 
{
	u8  ucEnalbe;
	u8  ucState;
	u16 usRunTime;
	u16 usStopTime;
	u16 usCycle;
	u16 usCount;
	u16 usCycleCount;
}MOTOR_T;

/* 供外部调用的函数声明 */
void Motor_InitHard(void);
void Motor_Start(u16 _usRunTime, u16 _usStopTime, u16 _usCycle);
void Motor_Stop(void);
void Motor_Pro(void);

#endif
/*********************************** 华宏软件 (END OF FILE) ********************************************/

