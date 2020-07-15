/*
*********************************************************************************************************
*
*	模块名称 : BSP模块(For STM32F1XX)
*	文件名称 : bsp.c
*	版    本 : V1.0
*	说    明 : 这是硬件底层驱动程序模块的主文件。主要提供 bsp_Init()函数供主程序调用。主程序的每个c文件可以在开
*			  头	添加 #include "bsp.h" 来包含所有的外设驱动模块。
*
*	修改记录 :
*		版本号  日期        作者    说明
*       v1.0   2020-04-29 孟凡忠  正式发布
*
*	Copyright (C), 2013-2020, 华宏软件 
*
*********************************************************************************************************
*/
#include "bsp.h"

// void ShowHardwareInfo(void);/* 串口和液晶屏显示硬件信息 */

/*
*********************************************************************************************************
*	函 数 名: bsp_Init
*	功能说明: 初始化硬件设备。只需要调用一次。该函数配置CPU寄存器和外设的寄存器并初始化一些全局变量。
*			 全局变量。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_Init(void)
{

//	bsp_InitUart(); 	/* 初始化串口 */
	
//	bsp_InitLed(); 		/* 初始LED指示灯端口 */
	
	bsp_InitKey();		/* 初始化按键 */
	
    Motor_InitHard();    /* 初始化振动马达硬件*/
	
//	bsp_InitRTC();		/* 初始化CPU内部RTC */
	
//	bsp_InitI2C();       /* 配置I2C总线 */

	bsp_InitSPIBus();	/* 配置SPI总线，必须在初始化串行Flash之前执行本函数 */
	
//	bsp_InitSFlash();	/* 初始化串行Flash. 该函数会识别串行FLASH型号 */
	
	LCD_InitHard();	     /* 初始化显示器硬件 */

//	TOUCH_InitHard();    /* 初始化触摸 */


//	ShowHardwareInfo();		/* 串口和液晶屏显示硬件信息 */	
}

/*
*********************************************************************************************************
*	函 数 名: showMF_ID
*	功能说明: 显示MF芯片ID。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
// void showMF_ID(void)
// {
// 	uint16_t x, y;
// 	char buf[128];
// 	FONT_T tFont;
	
// 	tFont.FontCode = FC_ST_16;		/* 字体代码 16点阵 */
// 	tFont.FrontColor = CL_WHITE;	/* 字体颜色 */
// 	tFont.BackColor = CL_BLACK;		/* 文字背景颜色 */
// 	tFont.Space = 0;				/* 文字间距，单位 = 像素 */

// 	RTC_ReadClock();				/* 读时钟，结果在 g_tRTC */

// 	x = 10;
// 	y = LCD_GetHeight() - 50;

// 	sprintf(buf, "MF芯片ID:%02X-%02X-%02X-%02X",
// 		g_tMF.ChipID[0],g_tMF.ChipID[1],g_tMF.ChipID[2],g_tMF.ChipID[3]);
// 	LCD_DispStr(x, y, buf, &tFont);	
// }

// /*
// *********************************************************************************************************
// *	函 数 名: ShowHardwareInfo
// *	功能说明: 显示时钟。
// *	形    参: 无
// *	返 回 值: 无
// *********************************************************************************************************
// */
// void showClock(void)
// {
// 	uint16_t x, y;
// 	char buf[128];
// 	FONT_T tFont;
	
// 	tFont.FontCode = FC_ST_16;		/* 字体代码 16点阵 */
// 	tFont.FrontColor = CL_WHITE;	/* 字体颜色 */
// 	tFont.BackColor = CL_BLACK;		/* 文字背景颜色 */
// 	tFont.Space = 0;				/* 文字间距，单位 = 像素 */

// 	RTC_ReadClock();				/* 读时钟，结果在 g_tRTC */

// 	x = 5;
// 	y = LCD_GetHeight() - 20;

// 	sprintf(buf, "%4d-%02d-%02d %02d:%02d:%02d 星期%s",
// 		g_tRTC.Year, g_tRTC.Mon, g_tRTC.Day, g_tRTC.Hour, g_tRTC.Min, g_tRTC.Sec,WEEK_STR[g_tRTC.Week]);
// 	LCD_DispStr(x, y, buf, &tFont);	
// }

// /*
// *********************************************************************************************************
// *	函 数 名: ShowHardwareInfo
// *	功能说明: 串口和液晶屏显示硬件信息。
// *	形    参: 无
// *	返 回 值: 无
// *********************************************************************************************************
// */
// void ShowHardwareInfo(void)	
// {
	
// 	char LCD_TYPE[50];
// 	char temp[255];
// 	FONT_T font_t=
// 	{
// 		FC_ST_16,		/* 字体代码 FONT_CODE_E  */
// 		CL_WHITE,		/* 字体颜色 */
// 		CL_BLACK,		/* 文字背景颜色，透明 */
// 		0,				/* 文字间距，单位 = 像素 */		
// 	};
	
// 	printf ("\r\n型号:%s\r\n总大小:%d\r\n页大小:%d\r\n",g_tSF.ChipName,g_tSF.TotalSize,g_tSF.PageSize);
	
// 	LCD_ClrScr(CL_BLACK); /* 清屏 */
	
// 	LCD_GetChipDescribe(LCD_TYPE);
// 	printf ("\r\nLCD类型:%s\r\n",LCD_TYPE);
	
	
// 	sprintf (temp,"FLASH型号:%s;总大小:%d;页大小:%d;",g_tSF.ChipName,g_tSF.TotalSize,g_tSF.PageSize);
	
// 	LCD_DispStr(50,100,temp, &font_t);
	
	
// 	sprintf (temp,"FRAM型号:%s;总大小:%d;ID号:0x%x",g_tFRAM.ChipName,g_tFRAM.TotalSize,*(uint32_t *)&g_tFRAM.ChipID);
	
// 	LCD_DispStr(50,200,temp, &font_t);

// 	showMF_ID();/* 显示MF芯片ID */
	
// 	showClock();/* 显示时间 */

// }


/***************************** 华宏软件 (END OF FILE) *********************************/
