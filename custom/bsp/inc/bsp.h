/*
*********************************************************************************************************
*
*	模块名称 : BSP模块
*	文件名称 : bsp.h
*	说    明 : 这是底层驱动模块所有的h文件的汇总文件。 应用程序只需 #include bsp.h 即可，
*			  不需要#include 每个模块的 h 文件
*
*	Copyright (C), 2013-2014, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef _BSP_H_
#define _BSP_H

#include <string.h>

#include "custom_feature_def.h"
#include "ril.h"
#include "ril_util.h"
#include "ql_stdlib.h"
#include "ql_error.h"
#include "ql_trace.h"
#include "ql_uart.h"
#include "ql_spi.h"
#include "ql_iic.h"
#include "ql_system.h"
#include "ql_timer.h"
#include "ql_adc.h"
#include "ql_pwm.h"
#include "ql_memory.h"


#define DEBUG_ENABLE 1
#if DEBUG_ENABLE > 0
#define DEBUG_PORT  UART_PORT0
#define DBG_BUF_LEN   512
static char DBG_BUFFER[DBG_BUF_LEN];
#define APP_DEBUG(FORMAT,...) {\
    Ql_memset(DBG_BUFFER, 0, DBG_BUF_LEN);\
    Ql_sprintf(DBG_BUFFER,FORMAT,##__VA_ARGS__); \
    if (UART_PORT2 == (DEBUG_PORT)) \
    {\
        Ql_Debug_Trace(DBG_BUFFER);\
    } else {\
        Ql_UART_Write((Enum_SerialPort)(DEBUG_PORT), (u8*)(DBG_BUFFER), Ql_strlen((const char *)(DBG_BUFFER)));\
    }\
}
#else
#define APP_DEBUG(FORMAT,...) 
#endif

/* 通过取消注释或者添加注释的方式控制是否包含底层驱动模块 */
#include "bsp_key.h"
#include "bsp_motor.h"
#include "bsp_adc.h"
#include "bsp_charge.h"
#include "bsp_spi_bus.h"
#include "bsp_spi_flash.h"
#include "bsp_st3375.h"
#include "bsp_tft_lcd.h"
#include "bsp_cst816s.h"
#include "bsp_touch.h"

/* 提供给其他C文件调用的函数 */
 void bsp_Init(void);

#endif

/***************************** 华宏软件 (END OF FILE) *********************************/
