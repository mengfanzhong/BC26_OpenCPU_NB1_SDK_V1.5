/*
*********************************************************************************************************
*
*	模块名称 : ADC驱动模块
*	文件名称 : bsp_adc.c
*	版    本 : V1.0
*	说    明 : ADC电量监测模块
*
*	修改记录 :
*		版本号    日期        作者     说明
*		V1.0    2020/04/28   孟凡忠  正式发布
*
*	Copyright (C), 2015-2020, 华宏软件
*
*********************************************************************************************************
*/
#include "bsp.h"

#define ADC_SINGLE_READ   /* 单次读 */ 
static u32 ADC_CustomParam = 1;
//timer
// static u32 Stack_timer = 0x101; 
// static u32 ST_Interval = 1000;
// static s32 m_param1 = 0;

static void Callback_OnADCSampling(Enum_ADCPin adcPin, u32 adcValue, void *customParam)
{
    APP_DEBUG("<-- Callback_OnADCSampling: sampling voltage(mV)=%d  times=%d -->\r\n", adcValue, *((s32*)customParam))
    *((s32*)customParam) += 1;
}

static void Timer_handler(u32 timerId, void* param)
{
	 u32 value = 0;
	 Ql_ADC_Read(PIN_ADC0,&value);
	 APP_DEBUG("<---read adc voltage%d(mV)--->\r\n",value);
}

static void ADC_Program(void)
{
    Enum_PinName adcPin = PIN_ADC0;

    // Register callback foR ADC
    APP_DEBUG("<-- Register callback for ADC -->\r\n")
    Ql_ADC_Register(adcPin, Callback_OnADCSampling, (void *)&ADC_CustomParam);

    // Initialize ADC (sampling count, sampling interval)
    APP_DEBUG("<-- Initialize ADC (sampling count=5, sampling interval=200ms) -->\r\n")
    Ql_ADC_Init(adcPin, 5, 200);

    // Start ADC sampling
    APP_DEBUG("<-- Start ADC sampling -->\r\n")
    Ql_ADC_Sampling(adcPin, TRUE);

    // Stop  sampling ADC
    //Ql_ADC_Sampling(adcPin, FALSE);
}


/*
*********************************************************************************************************
*	函 数 名: bsp_InitADC
*	功能说明: 初始化ADC
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitADC( void )
{
   #ifdef  ADC_SINGLE_READ //single read
    //   Ql_Timer_Register(Stack_timer, Timer_handler, &m_param1);
    //   Ql_Timer_Start(Stack_timer,ST_Interval,TRUE);
	  
   #else //loop read
    ADC_Program();
   #endif 
}

/*
*********************************************************************************************************
*	函 数 名: bsp_SingleReadADC
*	功能说明: 单次读ADC
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
s32 bsp_SingleReadADC( u32 *value )
{
    s32 result;
	 result = Ql_ADC_Read(PIN_ADC0,value);
//	 APP_DEBUG("<---read adc result = %d voltage = %d(mV)--->\r\n",result,*value);
     return result;
}

/*********************************** 华宏软件 (END OF FILE) ********************************************/


