/*
*********************************************************************************************************
*
*	模块名称 : 触摸板驱动模块
*	文件名称 : bsp_touch.c
*	版    本 : V2.0
*	说    明 : 支持电容触摸CST816S
*	修改记录 :
*		版本号  日期        作者    说明
*       v1.0   2020-04-29 孟凡忠  正式发布
*
*	Copyright (C), 2013-2020, 华宏软件 
*
*********************************************************************************************************
*/

#include "bsp.h"


/* 触屏模块用到的全局变量 */
TOUCH_T g_tTP;
/* 全局参数,先暂放在这里 */
PARAM_T g_tParam;

/*
*********************************************************************************************************
*	函 数 名: bsp_InitTouch
*	功能说明: 配置STM32和触摸相关的口线，使能硬件SPI2, 片选由软件控制
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void TOUCH_InitHard(void)
{
	/* 初始化电容触摸屏(CST816S) */
	CST816S_InitHard();				

	g_tParam.TouchDirection = 0; /* 默认为横屏 */
	
    g_tTP.Write = g_tTP.Read = 0;

	g_tTP.Enable = 1;
}

/*
*********************************************************************************************************
*	函 数 名: TOUCH_PutKey
*	功能说明: 将1个触摸点坐标值压入触摸FIFO缓冲区。用于电阻触摸屏。
*	形    参: _usX, _usY 坐标值
*	返 回 值: 无
*********************************************************************************************************
*/
void TOUCH_PutKey(u8 _ucEvent, u16 _usX, u16 _usY)
{
	u16 xx, yy;
	u16 x = 0, y = 0;

	g_tTP.Event[g_tTP.Write] = _ucEvent;

	/* 电容屏 (无需校准) */
	if (g_tCST816S.Enable == 1)
	{
		xx = _usX;
		yy = _usY;
	}	
	else	/* 电阻屏 */
	{
		// xx = TOUCH_TransX(_usX, _usY);
		// yy = TOUCH_TransY(_usX, _usY);
	}
	
	/* 横屏和竖屏方向识别 */
	if (g_tParam.TouchDirection > 3)
	{
		g_tParam.TouchDirection  = 0;
	}
	switch (g_tParam.TouchDirection)
	{
		case 0:	/* 校准触摸时，屏幕方向为0 */
			if (g_LcdDirection == 0)		/* 横屏 */
			{
				x = xx;
				y = yy;
			}
			else if (g_LcdDirection == 1)	/* 横屏180°*/
			{
				x = g_LcdWidth - xx - 1;
				y = g_LcdHeight - yy - 1;
			}
			else if (g_LcdDirection == 2)	/* 竖屏 */
			{
				y = xx;
				x = g_LcdWidth - yy - 1;
			}
			else if (g_LcdDirection == 3)	/* 竖屏180° */
			{
				y = g_LcdHeight - xx - 1;
				x = yy;
			}
			break;

		case 1:	/* 校准触摸时，屏幕方向为1 */
			if (g_LcdDirection == 0)		/* 横屏 */
			{
				x = g_LcdWidth - xx - 1;
				y = g_LcdHeight - yy - 1;
			}
			else if (g_LcdDirection == 1)	/* 横屏180°*/
			{
				x = xx;
				y = yy;
			}
			else if (g_LcdDirection == 2)	/* 竖屏 */
			{
				y = g_LcdHeight - xx - 1;
				x = yy;
			}
			else if (g_LcdDirection == 3)	/* 竖屏180° */
			{
				y = xx;
				x = g_LcdWidth - yy - 1;
			}
			break;

		case 2:	/* 校准触摸时，屏幕方向为2 */
			if (g_LcdDirection == 0)		/* 横屏 */
			{
				y = xx;
				x = g_LcdWidth - yy - 1;
			}
			else if (g_LcdDirection == 1)	/* 横屏180°*/
			{
				y = g_LcdHeight - xx - 1;
				x = yy;
			}
			else if (g_LcdDirection == 2)	/* 竖屏 */
			{
				x = xx;
				y = yy;
			}
			else if (g_LcdDirection == 3)	/* 竖屏180° */
			{
				x = g_LcdWidth - xx - 1;
				y = g_LcdHeight - yy - 1;
			}
			break;

		case 3:	/* 校准触摸时，屏幕方向为3 */
			if (g_LcdDirection == 0)		/* 横屏 */
			{
				y = xx;
				x = g_LcdWidth - yy - 1;
			}
			else if (g_LcdDirection == 1)	/* 横屏180°*/
			{
				y = g_LcdHeight - xx - 1;
				x = yy;
			}
			else if (g_LcdDirection == 2)	/* 竖屏 */
			{
				x = g_LcdWidth - xx - 1;
				y = g_LcdHeight - yy - 1;
			}
			else if (g_LcdDirection == 3)	/* 竖屏180° */
			{
				x = xx;
				y = yy;
			}
			break;

		default:
			g_tParam.TouchDirection = 0;	/* 方向参数无效时，纠正为缺省的横屏 */
			break;
	}

	g_tTP.XBuf[g_tTP.Write] = x;
	g_tTP.YBuf[g_tTP.Write] = y;

	if (++g_tTP.Write  >= TOUCH_FIFO_SIZE)
	{
		g_tTP.Write = 0;
	}
}

/*
*********************************************************************************************************
*	函 数 名: TOUCH_GetKey
*	功能说明: 从触摸FIFO缓冲区读取一个坐标值。
*	形    参:  无
*	返 回 值:
*			TOUCH_NONE      表示无事件
*			TOUCH_DOWN      按下
*			TOUCH_MOVE      移动
*			TOUCH_RELEASE	释放
*********************************************************************************************************
*/
u8 TOUCH_GetKey(s16 *_pX, s16 *_pY)
{
	u8 ret;

	if (g_tTP.Read == g_tTP.Write)
	{
		return TOUCH_NONE;
	}
	else
	{
		ret = g_tTP.Event[g_tTP.Read];
		*_pX = g_tTP.XBuf[g_tTP.Read];
		*_pY = g_tTP.YBuf[g_tTP.Read];

		if (++g_tTP.Read >= TOUCH_FIFO_SIZE)
		{
			g_tTP.Read = 0;
		}
		return ret;
	}
}

/*
*********************************************************************************************************
*	函 数 名: TOUCH_CelarFIFO
*	功能说明: 清除触摸FIFO缓冲区
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void TOUCH_CelarFIFO(void)
{
	// __set_PRIMASK(1);  		    /* 关中断 */
	g_tTP.Write = g_tTP.Read;
	// __set_PRIMASK(0);  		    /* 开中断 */
}

/*
*********************************************************************************************************
*	函 数 名: TOUCH_InRect
*	功能说明: 判断当前坐标是否位于矩形框内
*	形    参:  _usX, _usY: 输入坐标
*			_usRectX,_usRectY: 矩形起点
*			_usRectH、_usRectW : 矩形高度和宽度
*	返 回 值: 1 表示在范围内
*********************************************************************************************************
*/
u8 TOUCH_InRect(u16 _usX, u16 _usY,
	u16 _usRectX, u16 _usRectY, u16 _usRectH, u16 _usRectW)
{
	if ((_usX > _usRectX) && (_usX < _usRectX + _usRectW)
		&& (_usY > _usRectY) && (_usY < _usRectY + _usRectH))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/*
*********************************************************************************************************
*	函 数 名: TOUCH_CapScan
*	功能说明: I2C接口电容触摸板扫描函数,周期性调用.
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void TOUCH_CapScan(void)
{
	if (g_tCST816S.Enable == 1)
	{
		CST816S_Scan();
		return;
	}
}

/**************************************** 华宏软件 (END OF FILE) ****************************************/
