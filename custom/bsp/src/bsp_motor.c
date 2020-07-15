/*
*********************************************************************************************************
*
*	模块名称 : 振动马达驱动模块
*	文件名称 : bsp_motor.c
*	版    本 : V1.0
*	说    明 : 振动马达驱动
*
*	修改记录 :
*		版本号    日期        作者     说明
*		V1.0    2020/04/21   孟凡忠  正式发布
*
*	Copyright (C), 2015-2020, 华宏软件
*
*********************************************************************************************************
*/
#include "bsp.h"

/* 定义振动马达引脚 */
#define    MOTOR_PINNAME           PINNAME_RTS_AUX

static MOTOR_T g_tMotor;		/* 定义振动马达全局结构体变量 */

/*
*********************************************************************************************************
*	函 数 名: MOTOR_ENABLE
*	功能说明: 马达振动
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void MOTOR_ENABLE(void)
{
	Ql_GPIO_SetLevel(MOTOR_PINNAME, PINLEVEL_HIGH);  
}

/*
*********************************************************************************************************
*	函 数 名: MOTOR_DISABLE
*	功能说明: 马达停止
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void MOTOR_DISABLE(void)
{
	Ql_GPIO_SetLevel(MOTOR_PINNAME, PINLEVEL_LOW);   
}

/*
*********************************************************************************************************
*	函 数 名: Motor_InitHard
*	功能说明: 初始化硬件
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void Motor_InitHard(void)
{
    /* 初始化K1引脚 ( 输出,低电平,带上拉 ) */
    Ql_GPIO_Init(MOTOR_PINNAME, PINDIRECTION_OUT, PINLEVEL_LOW, PINPULLSEL_PULLUP);
    APP_DEBUG("<-- Initialize Motor pin (PINNAME_STATUS): output, low level, PULL up -->\r\n");
}

/*
*********************************************************************************************************
*	函 数 名: Motor_Start
*	功能说明: 开始振动。
*	形    参：_usRunTime  : 振动时间，单位10ms; 0 表示不振动
*			  _usStopTime : 停止时间，单位10ms; 0 表示持续振动
*			 _usCycle 	  : 振动次数， 0 表示持续振动
*	返 回 值: 无
*********************************************************************************************************
*/
void Motor_Start(u16 _usRunTime, u16 _usStopTime, u16 _usCycle)
{
	if (_usRunTime == 0)
	{
		return;
	}

	g_tMotor.usRunTime = _usRunTime;
	g_tMotor.usStopTime = _usStopTime;
	g_tMotor.usCycle = _usCycle;
	g_tMotor.usCount = 0;
	g_tMotor.usCycleCount = 0;
	g_tMotor.ucState = 0;
	g_tMotor.ucEnalbe = 1;			/* 设置完全局参数后再使能振动标志 */

	MOTOR_ENABLE();					/* 开始振动 */
}

/*
*********************************************************************************************************
*	函 数 名: Motor_Stop
*	功能说明: 停止振动。
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void Motor_Stop(void)
{
	g_tMotor.ucEnalbe = 0;

	MOTOR_DISABLE();	/* 必须在清控制标志后再停止振动，避免停止后在中断中又开启 */
}

/*
*********************************************************************************************************
*	函 数 名: Motor_Pro
*	功能说明: 每隔10ms调用1次该函数，用于控制马达振动。
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void Motor_Pro(void)
{
	if ((g_tMotor.ucEnalbe == 0) || (g_tMotor.usStopTime == 0))
	{
		return;
	}

	if (g_tMotor.ucState == 0)
	{
		if (g_tMotor.usStopTime > 0)	/* 间断振动 */
		{
			if (++g_tMotor.usCount >= g_tMotor.usRunTime)
			{
				MOTOR_DISABLE();		/* 停止振动 */
				g_tMotor.usCount = 0;
				g_tMotor.ucState = 1;
			}
		}
		else
		{
			;	/* 不做任何处理，连续振动 */
		}
	}
	else if (g_tMotor.ucState == 1)
	{
		if (++g_tMotor.usCount >= g_tMotor.usStopTime)
		{
			/* 连续振动时，直到调用stop停止为止 */
			if (g_tMotor.usCycle > 0)
			{
				if (++g_tMotor.usCycleCount >= g_tMotor.usCycle)
				{
					/* 循环次数到，停止振动 */
					g_tMotor.ucEnalbe = 0;
				}

				if (g_tMotor.ucEnalbe == 0)
				{
					g_tMotor.usStopTime = 0;
					return;
				}
			}

			g_tMotor.usCount = 0;
			g_tMotor.ucState = 0;

			MOTOR_ENABLE();					/* 开始振动 */
		}
	}
}

/*********************************** 华宏软件 (END OF FILE) ********************************************/
