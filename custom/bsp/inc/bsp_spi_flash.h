/*
*********************************************************************************************************
*
*	模块名称 : SPI接口串行FLASH 读写模块
*	文件名称 : bsp_spi_flash.h
*	版    本 : V1.0
*	说    明 : 头文件
*
*	Copyright (C), 2013-2020, 华宏软件
*
*********************************************************************************************************
*/

#ifndef _BSP_SPI_FLASH_H
#define _BSP_SPI_FLASH_H

#define SF_MAX_PAGE_SIZE	(4 * 1024)

/* 定义串行Flash ID */
enum
{
	SST25VF016B_ID 	= 0xBF2541,
	MX25L1606E_ID  	= 0xC22015,
	W25Q64_ID    	= 0xEF4017,
	W25Q128_ID   	= 0xEF4018,
	W25Q64FWZPIG_ID = 0xEF6017,
};

typedef struct
{
	u32 ChipID;		        /* 芯片ID */
	s8 ChipName[16];		/* 芯片型号字符串，主要用于显示 */
	u32 TotalSize;		    /* 总容量 */
	u16 PageSize;		    /* 页面大小 */
}SFLASH_T;

void 	bsp_InitSFlash(void);
u32 	sf_ReadID(void);
void 	sf_EraseChip(void);
void 	sf_EraseSector(u32 _uiSectorAddr);
void 	sf_PageWrite(u8 * _pBuf, u32 _uiWriteAddr, u16 _usSize);
u8 		sf_WriteBuffer(u8* _pBuf, u32 _uiWriteAddr, u16 _usWriteSize);
void 	sf_ReadBuffer(u8 * _pBuf, u32 _uiReadAddr, u32 _uiSize);
void 	sf_ReadInfo(void);

extern SFLASH_T g_tSF;

#endif

/***************************** 华宏软件 (END OF FILE) *********************************/
