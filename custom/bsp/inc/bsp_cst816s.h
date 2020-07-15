/*
*********************************************************************************************************
*
*	模块名称 : cst816s电容触摸芯片驱动程序
*	文件名称 : bsp_cst816s.h
*	说    明 : 头文件
*	版    本 : V1.0
*
*	Copyright (C), 2012-2020, 华宏软件
*********************************************************************************************************
*/
#ifndef _BSP_CST816S_H
#define _BSP_CST816S_H

/* 
* Device address : 0x15
* Frequency : from 10Khz to 400Khz
*/
/* I2C总线，器件ID */
#define CST816S_I2C_ADDR       		0x15

/* 支持的触摸点数 */
#define CST816S_TOUCH_POINTS   		2		

/* 寄存器地址 */
#define DEVICE_MODE           		0x00
#define GEST_ID               		0x01  	/* 手势码 */
#define FINGERNUM             		0x02	/* 手指个数 */
#define XPOSH	 					0x03	/* X坐标高4位 */
#define XPOSL	 					0x04	/* X坐标低8位 */
#define YPOSH	 					0x05	/* X坐标高4位 */
#define YPOSL	 					0x06	/* X坐标低8位 */

#define BPC0H  						0xB0 	/* BPC0值的高8位 */
#define BPC0L 						0xB1 	/* BPC0值的低8位 */
#define BPC1H 						0xB2 	/* BPC1值的高8位 */
#define BPC1L 						0xB3 	/* BPC1值的低8位 */

#define CST816S_REG_CHIP_ID   		0xA7	/* 芯片型号 */
#define CST816S_REG_PROJID 			0XA8	/* 工程编号 */
#define CST816S_REG_FW_VER     		0xA9	/* 固件版本 */

#define CST816S_REG_MOTIONMASK 		0xEC 	/* 动着MASK */
#define CST816S_REG_IRQPLUSEWIDTH 	0xED 	/* 中断低脉冲输出宽度 */
#define CST816S_REG_NORSCANPER 		0xEE 	/* 正常快速检测周期 */
#define CST816S_REG_MOTIONSLANGLE 	0xEF 	/* 手势检测滑动分区角度控制 */
#define CST816S_REG_LPSCANRAW1H 	0xF0 	/* 低功耗扫描1号通道的基准值的高8位 */ 
#define CST816S_REG_LPSCANRAW1L 	0xF1 	/* 低功耗扫描1号通道的基准值的低8位 */
#define CST816S_REG_LPSCANRAW2H 	0xF2 	/* 低功耗扫描1号通道的基准值的高8位 */
#define CST816S_REG_LPSCANRAW2L 	0xF3 	/* 低功耗扫描1号通道的基准值的低8位 */
#define CST816S_REG_LPAUTOWAKETIME 	0xF4 	/* 低功耗时自动重校正周期 */
#define CST816S_REG_LPSCANTH 		0xF5 	/* 低功耗扫描唤醒门限。 越小越灵敏	*/
#define CST816S_REG_LPSCANWIN 		0xF6 	/* 低功耗扫描量程。 越大越灵敏， 功耗越高 */
#define CST816S_REG_LPSCANFREQ 		0xF7 	/* 低功耗扫描频率。 越小越灵敏 */
#define CST816S_REG_LPSCANIDAC 		0xF8 	/* 低功耗扫描电流。 越小越灵敏 */
#define CST816S_REG_AUTOSLEEPTIME 	0xF9 	/* x秒内无触摸时， 自动进入低功耗模式 */
#define CST816S_REG_IRQCTL 			0xFA 	/* IRQ配置 */
#define CST816S_REG_AUTORESET 		0xFB 	/* x秒内有触摸但无有效手势时， 自动复位*/
#define CST816S_REG_LONGPRESSTIME 	0xFC 	/* 长按x秒后自动复位 */
#define CST816S_REG_IOCTL 			0xFD 	/* IO口配置 */
#define CST816S_REG_DISAUTOSLEEP 	0xFE 	/* 自动进入低功耗模式开关 */

//#define CFG_POINT_READ_BUF  (3 + 6 * (CST816S_TOUCH_POINTS))    // 33字节

#if 0  /* 以下内容摘抄自 Linux驱动 */
struct Upgrade_Info 
{
	u8 CHIP_ID;
	u8 FTS_NAME[20];
	u8 TPD_MAX_POINTS;
	u8 AUTO_CLB;
	u8 delay_aa;			/* delay of write FT_UPGRADE_AA */
	u8 delay_55;			/* delay of write FT_UPGRADE_55 */
	u8 upgrade_id_1;		/* upgrade id 1 */
	u8 upgrade_id_2;		/* upgrade id 2 */
	u8 delay_readid;		/* delay of read id */
	u8 delay_earse_flash;	/* delay of earse flash */
};

struct Upgrade_Info fts_updateinfo[] =
{
	{0x55,"FT5x06",TPD_MAX_POINTS_5,AUTO_CLB_NEED,50, 30, 0x79, 0x03, 10, 2000},

	{0x08,"FT5606",TPD_MAX_POINTS_5,AUTO_CLB_NEED,50, 10, 0x79, 0x06, 100, 2000},
	
	{0x0a,"FT5x16",TPD_MAX_POINTS_5,AUTO_CLB_NEED,50, 30, 0x79, 0x07, 10, 1500},

	{0x06,"FT6x06",TPD_MAX_POINTS_2,AUTO_CLB_NONEED,100, 30, 0x79, 0x08, 10, 2000},

	{0x36,"FT6x36",TPD_MAX_POINTS_2,AUTO_CLB_NONEED,10, 10, 0x79, 0x18, 10, 2000},
	{0x55,"FT5x06i",TPD_MAX_POINTS_5,AUTO_CLB_NEED,50, 30, 0x79, 0x03, 10, 2000},
	{0x14,"FT5336",TPD_MAX_POINTS_5,AUTO_CLB_NONEED,30, 30, 0x79, 0x11, 10, 2000},
	{0x13,"FT3316",TPD_MAX_POINTS_5,AUTO_CLB_NONEED,30, 30, 0x79, 0x11, 10, 2000},
	{0x12,"FT5436i",TPD_MAX_POINTS_5,AUTO_CLB_NONEED,30, 30, 0x79, 0x11, 10, 2000},
	{0x11,"FT5336i",TPD_MAX_POINTS_5,AUTO_CLB_NONEED,30, 30, 0x79, 0x11, 10, 2000},
	{0x54,"FT5x46",TPD_MAX_POINTS_5,AUTO_CLB_NONEED,2, 2, 0x54, 0x2c, 10, 2000},
	{0x58,"FT5x22",TPD_MAX_POINTS_5,AUTO_CLB_NONEED,2, 2, 0x58, 0x2c, 20, 2000},
	{0x59,"FT5x26",TPD_MAXPOINTS_10,AUTO_CLB_NONEED,30, 50, 0x79, 0x10, 1, 2000},
};
#endif

typedef struct
{
	u8  ChipID;
	u8  Enable;
	u8  TimerCount;
	u8  Count;			            /* 几个点按下 */
	u16 X[CST816S_TOUCH_POINTS];
	u16 Y[CST816S_TOUCH_POINTS];	
	u8  id[CST816S_TOUCH_POINTS];
	u8  Event[CST816S_TOUCH_POINTS];
}CST816S_T;

/* 供外部调用的函数声明 */
void    CST816S_InitHard(void);
u8      CST816S_ReadID(void);
u8      CST816S_PenInt(void);
u16     CST816S_ReadVersion(void);
void    CST816S_Scan(void);
void    CST816S_Timer1ms(void);

/* 供外部调用的变量声明 */
extern CST816S_T g_tCST816S;

#endif
