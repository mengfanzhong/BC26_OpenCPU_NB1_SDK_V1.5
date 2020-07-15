/*
*********************************************************************************************************
*
*	模块名称 : SPI总线驱动
*	文件名称 : bsp_spi_bus.h
*	版    本 : V1.1
*	说    明 : SPI总线底层驱动。提供SPI配置、收发数据、多设备共享SPI支持。通过宏切换是硬件SPI还是软件模拟
*	修改记录 :
*		版本号  日期        作者    说明
*       v1.0   2020-04-29 孟凡忠  正式发布
*
*	Copyright (C), 2013-2020, 华宏软件 
*
*********************************************************************************************************
*/

#include "bsp.h"

/***************************************************************************
* SPI通道号
****************************************************************************/

#define USR_SPI_CHANNAL		     (1)
	     
static void spi_init(u8 spi_type);

static void spi_GPIO_init(void);
/*
*********************************************************************************************************
*	函 数 名: bsp_InitSPIBus
*	功能说明: 配置SPI总线。 只包括 SCK、 MOSI、 MISO口线的配置。不包括片选CS，也不包括外设芯片特有的INT、BUSY等
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitSPIBus(void)
{
#ifdef SOFT_SPI    /* 使用软件SPI */
    /* 初始化模拟SPI引脚 */
    spi_GPIO_init();
#else
    /* 初始化BC26 SPI接口 */
    spi_init(1);   /* 参数为1 使用硬件SPI CS引脚为固定的 */   

#endif       
}

/*
*********************************************************************************************************
*	函 数 名: bsp_spiWrite
*	功能说明: 向SPI总线发送一个字节。
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
u8 bsp_spiWrite(u8 _ucByte)
{
#ifdef SOFT_SPI
	u8 i=0;
	for(i=8;i>0;i--)
	{
	    if(_ucByte & 0x80)	
	        Ql_GPIO_SetLevel(PINNAME_SPI_MOSI,PINLEVEL_HIGH);
        else 
            Ql_GPIO_SetLevel(PINNAME_SPI_MOSI,PINLEVEL_LOW);
	   
        Ql_GPIO_SetLevel(PINNAME_SPI_SCLK,PINLEVEL_LOW);      
        Ql_GPIO_SetLevel(PINNAME_SPI_SCLK,PINLEVEL_HIGH); 
        _ucByte <<= 1; 
	}
    return 1;

#else
    s32 ret;
    ret = Ql_SPI_Write(USR_SPI_CHANNAL,&_ucByte,1);
    if(ret == 1)
        return 1;
    else
        return 0; 

#endif
}

/*
*********************************************************************************************************
*	函 数 名: bsp_spi_Read
*	功能说明: SPI读一个字节数据
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
u8 bsp_spi_Read(void)
{
    u8  wrbuf = 0;
    u8  rdbuf;
    Ql_SPI_WriteRead(USR_SPI_CHANNAL, &wrbuf, 0, &rdbuf, 1);
    
    return rdbuf;
}

/*
*********************************************************************************************************
*	函 数 名: bsp_spi_WriteRead
*	功能说明: SPI读写数据
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
u8 bsp_spi_WriteRead(u8 *pwrBuf, u32 wrLen, u8 * prdBuf, u32 rdLen)
{
    s32 ret;
    ret = Ql_SPI_WriteRead(USR_SPI_CHANNAL, pwrBuf, wrLen, prdBuf, rdLen);
    if(ret == rdLen)
        return 1;
    else
        return 0;
}

/*
********************************************************************************
** 函数名称 ： spi_init
** 函数功能 ： SPI接口初始化
** 输    入	： 无
** 输    出	： 无
** 返    回	： 无
********************************************************************************
*/
static void spi_init(u8 spi_type)
{
    s32 ret;

    ret = Ql_SPI_Init(USR_SPI_CHANNAL,PINNAME_SPI_SCLK,PINNAME_SPI_MISO,PINNAME_SPI_MOSI,PINNAME_SPI_CS,spi_type);
    
    if(ret <0)
    {
        APP_DEBUG("\r\n<-- Failed!! Ql_SPI_Init fail , ret =%d-->\r\n",ret)
    }
    else
    {
        APP_DEBUG("\r\n<-- Ql_SPI_Init ret =%d -->\r\n",ret)  
    }
    ret = Ql_SPI_Config(USR_SPI_CHANNAL,1,1,0,25000); //config sclk about 25MHz;
    if(ret <0)
    {
        APP_DEBUG("\r\n<--Failed!! Ql_SPI_Config fail  ret=%d -->\r\n",ret)
    }
    else
    {
        APP_DEBUG("\r\n<-- Ql_SPI_Config  =%d -->\r\n",ret)
    }   


    // if (!spi_type)
    // {
    // 	Ql_GPIO_Init(PINNAME_SPI_CS,PINDIRECTION_OUT,PINLEVEL_HIGH,PINPULLSEL_PULLUP);	 //CS high
    // }
}

/*
********************************************************************************
** 函数名称 ： spi_GPIO_init
** 函数功能 ： 模拟SPI接口GPIO初始化
** 输    入	： 无
** 输    出	： 无
** 返    回	： 无
********************************************************************************
*/
static void spi_GPIO_init(void)
{
	/* 配置SPI_SCLK引脚 (输出,上拉,高电平) */
	Ql_GPIO_Init(PINNAME_SPI_SCLK,PINDIRECTION_OUT,PINLEVEL_HIGH,PINPULLSEL_PULLUP);

	/* 配置SPI_MISO引脚 (输入,上拉,低电平) */
	Ql_GPIO_Init(PINNAME_SPI_MISO,PINDIRECTION_IN,PINLEVEL_LOW,PINPULLSEL_DISABLE);

	/* 配置SPI_MOSI引脚 (输出,上拉,低电平) */
	Ql_GPIO_Init(PINNAME_SPI_MOSI,PINDIRECTION_OUT,PINLEVEL_LOW,PINPULLSEL_PULLUP);
}

/**************************************** 华宏软件 (END OF FILE) ****************************************/
