/*
*********************************************************************************************************
*
*	模块名称 : cst816s电容触摸芯片驱动程序
*	文件名称 : bsp_cst816s.c
*	版    本 : V1.0
*	说    明 : cst816s触摸芯片驱动程序。
*		版本号  日期        作者    说明
*       v1.0   2020-04-29 孟凡忠  正式发布
*
*	Copyright (C), 2013-2020, 华宏软件 
*
*********************************************************************************************************
*/

#include "bsp.h"

CST816S_T g_tCST816S;

/* 使用I2C通道号 */
#define CS816S_I2C_CHANNAL	(1)

/* 定义GPIO端口 */
#define PINNAME_TP_INT	PINNAME_CTS_AUX

static void CST816S_ReadReg(u16 _usRegAddr, u8 *_pRegBuf, u8 _ucLen);

/*
*********************************************************************************************************
*	函 数 名: CST816S_InitHard
*	功能说明: 配置触摸芯片
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void CST816S_InitHard(void)
{	
	s32 ret;

	/* 初始化I2C接口 */
	{
        ret = Ql_IIC_Init(CS816S_I2C_CHANNAL,PINNAME_RI,PINNAME_DCD,1);
        if(ret < 0)
        {
            APP_DEBUG("\r\n<--Failed!! IIC controller Ql_IIC_Init channel 1 fail ret=%d-->\r\n",ret);
        }

		ret = Ql_IIC_Config(CS816S_I2C_CHANNAL,TRUE, CST816S_I2C_ADDR, I2C_FREQUENCY_400K);// just for the IIC controller
        if(ret < 0)
        {
            APP_DEBUG("\r\n<--Failed !! IIC controller Ql_IIC_Config channel 1 fail ret=%d-->\r\n",ret);
        }
	}

	/* 配置触摸笔中断引脚为输入 */
	{
 		Ql_GPIO_Init(PINNAME_TP_INT,PINDIRECTION_IN,PINLEVEL_HIGH,PINPULLSEL_PULLDOWN);
	}


#if 0	/* 打印全部的寄存器数据 for debug */
	{
		u8 i;
		u8 reg_value;
		
		printf_dbg("[FTS] Touch Chip\r\n");
		
		bsp_DelayMS(10);
		
		for (i = 0; i < 255; i++)
		{
			CST816S_ReadReg(i, &reg_value, 1);
			printf_dbg(" 0x%02X = 0x%02X\r\n", i, reg_value);
		}
	}
#endif	
	
#if 0
	{
		u8 reg_addr;
		u8 reg_value;
		
		//get some register information
		reg_addr = CST816S_REG_FW_VER;
		CST816S_ReadReg(reg_addr, &reg_value, 1);
		printf_dbg("[FTS] Firmware version = 0x%x\r\n", reg_value);
		
		reg_addr = CST816S_REG_POINT_RATE;
		CST816S_ReadReg(reg_addr, &reg_value, 1);
		printf_dbg("[FTS] report rate is %dHz.\r\n", reg_value * 10);
		
		reg_addr = CST816S_REG_THGROUP;
		CST816S_ReadReg(reg_addr, &reg_value, 1);
		printf_dbg("[FTS] touch threshold is %d.\r\n", reg_value * 4);
	}
#endif		
	g_tCST816S.TimerCount = 0;
	g_tCST816S.Enable = 1;
}

/*
*********************************************************************************************************
*	函 数 名: CST816S_PenInt
*	功能说明: 判断触摸按下
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
u8 CST816S_PenInt(void)
{
    /* 低电平表示有触摸中断 */
    if (Ql_GPIO_GetLevel(PINNAME_TP_INT) == 0) 
	{
		return 1;
	}
	return 0; 
}

/*
*********************************************************************************************************
*	函 数 名: CST816S_ReadReg
*	功能说明: 读1个或连续的多个寄存器
*	形    参: _usRegAddr : 寄存器地址
*			  _pRegBuf : 寄存器数据缓冲区
*			 _ucLen : 数据长度
*	返 回 值: 无
*********************************************************************************************************
*/
static void CST816S_ReadReg(u16 _usRegAddr, u8 *_pRegBuf, u8 _ucLen)
{
//	u8 i;
	s32 ret;
	u8 I2C_SendBuf[2] = {0}; 

	/* if IICtype=1(硬件IIC) ,1<Len<8 */
	if( _ucLen > 8 )return;

	/* 只使用低8位地址 */ 
	I2C_SendBuf[0] = (_usRegAddr & 0xff); 
  
	/* 先写寄存器地址然后读数据 */
	ret =  Ql_IIC_Write_Read(CS816S_I2C_CHANNAL,CST816S_I2C_ADDR,(u8*)I2C_SendBuf,1,(u8*)_pRegBuf,_ucLen);
    if(ret < 0)
    {	
		/* 读取错误 */
    	APP_DEBUG("\r\n<--Failed !! Ql_IIC_Read channel 0 fail ret=%d-->\r\n",ret);            
    }
}

/*
*********************************************************************************************************
*	函 数 名: CST816S_Timer1ms
*	功能说明: 每隔1ms调用1次
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void CST816S_Timer1ms(void)
{
	g_tCST816S.TimerCount++;
}

/*
*********************************************************************************************************
*	函 数 名: CST816S_Scan
*	功能说明: 读取触摸数据。读取全部的数据，需要 720us左右。放在主程序 bsp_Idle()中执行
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void CST816S_Scan(void)
{
#if 0
	u8 buf[CFG_POINT_READ_BUF];
	u8 i;
	static u8 s_tp_down = 0;
	u16 x, y;
	static u16 x_save, y_save;
	static u8 s_no_touch = 0;

	if (g_tCST816S.Enable == 0)
	{
		return;
	}
	
	/* 20ms 执行一次 */
	if (g_tCST816S.TimerCount < 10)
	{
		return;
	}

	g_tCST816S.TimerCount = 0;
	
	if (CST816S_PenInt() == 0)
	{
		/* INT引脚出现周期性的高低电平，周期18ms左右 */
		if (s_tp_down == 1)
		{
			if (++s_no_touch > 2)
			{
				s_no_touch = 0;
				s_tp_down = 0;
				TOUCH_PutKey(TOUCH_RELEASE, x_save, y_save);	/* 释放 */
			}
		}		
		return;
	}
	
	s_no_touch = 0;
	
	CST816S_ReadReg(2, buf, 1);		
	if ((buf[0] & 0x07) == 0)
	{
		if (s_tp_down == 1)
		{
			s_tp_down = 0;
			TOUCH_PutKey(TOUCH_RELEASE, x_save, y_save);	/* 释放 */
		}
		return;
	}
	
	/* 有触摸，读取完整的数据 */
	CST816S_ReadReg(0, buf, CFG_POINT_READ_BUF);
	
	g_tCST816S.Count = buf[2] & 0x07; 
	if (g_tCST816S.Count > CST816S_TOUCH_POINTS)
	{
    	g_tCST816S.Count = CST816S_TOUCH_POINTS;
	}
	
	g_tCST816S.Count = 0;
	//for (i = 0; i < CST816S_TOUCH_POINTS; i++)
	for (i = 0; i < 1; i++)
	{
		u8 pointid;
		
		pointid = (buf[5 + 6*i]) >> 4;
		if (pointid >= 0x0f)
		{
			break;
		}
		else
		{
			g_tCST816S.Count++;
        	g_tCST816S.X[i] = (int16_t)(buf[3 + 6*i] & 0x0F)<<8 | (int16_t)buf[4 + 6*i];
        	g_tCST816S.Y[i] = (int16_t)(buf[5 + 6*i] & 0x0F)<<8 | (int16_t)buf[6 + 6*i];
        	g_tCST816S.Event[i] = buf[0x3 + 6*i] >> 6;
        	g_tCST816S.id[i] = (buf[5 + 6*i])>>4;
    	}
    }

	/* 检测按下 */
	{
		if (g_tCST816S.ChipID == 0x55)       /* 4.3寸 480 * 272 */
		{
			x = g_tCST816S.Y[0];
			y = g_tCST816S.X[0];	
			
			/* 判断值域 */
			if (x > 479)
			{
				x = 479;
			}
			
			if (y > 271)
			{
				y = 271;
			}			
		}
		else if (g_tCST816S.ChipID == 0x0A)	/* 5.0寸 800 * 480 */
		{
			x = g_tCST816S.X[0];
			y = g_tCST816S.Y[0];	
			
			/* 判断值域 */
			if (x > 799)
			{
				x = 799;
			}			
			if (y > 479)
			{
				y = 479;
			}			
		}
		else	/* id == 0x06 表示7寸电容屏（FT芯片） */
		{
			x = g_tCST816S.X[0];
			y = g_tCST816S.Y[0];	
			
			/* 判断值域 */
			if (x > 799)
			{
				x = 799;
			}			
			if (y > 479)
			{
				y = 479;
			}			
		}
	}
	
	if (s_tp_down == 0)
	{
		s_tp_down = 1;
		
		TOUCH_PutKey(TOUCH_DOWN, x, y);
	}
	else
	{
		TOUCH_PutKey(TOUCH_MOVE, x, y);
	}
	x_save = x;	/* 保存坐标，用于释放事件 */
	y_save = y;

#if 0
	for (i = 0; i < CFG_POINT_READ_BUF; i++)
	{
		printf("%02X ", buf[i]);
	}
	printf("\r\n");
#endif

#if 0  /* 打印5个坐标点数据 */	
	printf("(%5d,%5d,%3d,%3d) ",  g_tCST816S.X[0], g_tCST816S.Y[0], g_tCST816S.Event[0],  g_tCST816S.id[0]);
	printf("(%5d,%5d,%3d,%3d) ",  g_tCST816S.X[1], g_tCST816S.Y[1], g_tCST816S.Event[1],  g_tCST816S.id[1]);
	printf("(%5d,%5d,%3d,%3d) ",  g_tCST816S.X[2], g_tCST816S.Y[2], g_tCST816S.Event[2],  g_tCST816S.id[2]);
	printf("(%5d,%5d,%3d,%3d) ",  g_tCST816S.X[3], g_tCST816S.Y[3], g_tCST816S.Event[3],  g_tCST816S.id[3]);
	printf("(%5d,%5d,%3d,%3d) ",  g_tCST816S.X[4], g_tCST816S.Y[4], g_tCST816S.Event[4],  g_tCST816S.id[4]);
	printf("\r\n");
#endif	

#endif
}

/*
*********************************************************************************************************
*	函 数 名: CST816S_ReadID
*	功能说明: 读芯片ID。
*	形    参: 无
*	返 回 值: 1字节芯片ID
*********************************************************************************************************
*/
u8 CST816S_ReadID(void)
{
	u8 id;

	CST816S_ReadReg(CST816S_REG_CHIP_ID, &id, 1);
	
	g_tCST816S.ChipID = id;		/* 保存id */
	return id;
}

/**************************************** 华宏软件 (END OF FILE) ****************************************/
