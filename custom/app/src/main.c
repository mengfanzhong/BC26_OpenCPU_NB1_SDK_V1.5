
/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of Quectel Co., Ltd. 2019
*
*****************************************************************************/
/*****************************************************************************
 *
 * Filename:
 * ---------
 *   main.c
 *
 * Project:
 * --------
 *   OpenCPU
 *
 * Description:
 * ------------
 *   This app demonstrates how to send AT command with RIL API, and transparently
 *   transfer the response through MAIN UART. And how to use UART port.
 *   Developer can program the application based on this example.
 * 
 ****************************************************************************/
//#ifdef __CUSTOMER_CODE__
#include "include.h"



#define MAX_TASK_NUM                    11
#define MSG_ID_USER_DATA                MSG_ID_USER_START+0x100
#define MSG_ID_MUTEX_TEST               MSG_ID_USER_START+0x101
#define MSG_ID_SEMAPHORE_TEST           MSG_ID_USER_START+0x102
#define MSG_ID_GET_ALL_TASK_PRIORITY    MSG_ID_USER_START+0x103
#define MSG_ID_GET_ALL_TASK_REMAINSTACK MSG_ID_USER_START+0x104

// static int  s_iPassTask = 1;



// Define the UART port and the receive data buffer
static Enum_SerialPort m_myUartPort  = UART_PORT0;
#define SERIAL_RX_BUFFER_LEN  2048
static u8 m_RxBuf_Uart[SERIAL_RX_BUFFER_LEN];

static void CallBack_UART_Hdlr(Enum_SerialPort port, Enum_UARTEventType msg, bool level, void* customizedPara);
static s32 ATResponse_Handler(char* line, u32 len, void* userData);

/**************************************************************
* 主任务
***************************************************************/
void proc_main_task(s32 taskId)
{ 
    s32 ret;
    ST_MSG msg;

    // Register & open UART port
    ret = Ql_UART_Register(m_myUartPort, CallBack_UART_Hdlr, NULL);
    if (ret < QL_RET_OK)
    {
        Ql_Debug_Trace("Fail to register serial port[%d], ret=%d\r\n", m_myUartPort, ret);
    }
    ret = Ql_UART_Open(m_myUartPort, 115200, FC_NONE);
    if (ret < QL_RET_OK)
    {
        Ql_Debug_Trace("Fail to open serial port[%d], ret=%d\r\n", m_myUartPort, ret);
    }

    bsp_Init();         /* 初始化硬件 */
//    Battery_InitHard(); /* 电池监测硬件初始化 */

    APP_DEBUG("OpenCPU: Customer Application\r\n");

    // START MESSAGE LOOP OF THIS TASK
    while(TRUE)
    {
        Ql_OS_GetMessage(&msg);
        switch(msg.message)
        {
        case MSG_ID_RIL_READY:
            APP_DEBUG("<-- RIL is ready -->\r\n");
            Ql_RIL_Initialize();

            break;
        case MSG_ID_URC_INDICATION:
            //APP_DEBUG("<-- Received URC: type: %d, -->\r\n", msg.param1);
            switch (msg.param1)
            {
            case URC_SYS_INIT_STATE_IND:
                APP_DEBUG("<-- Sys Init Status %d -->\r\n", msg.param2);
                break;
            case URC_SIM_CARD_STATE_IND:
                APP_DEBUG("<-- SIM Card Status:%d -->\r\n", msg.param2);
                break;            
            case URC_EGPRS_NW_STATE_IND:
                APP_DEBUG("<-- EGPRS Network Status:%d -->\r\n", msg.param2);
                break;
            case URC_CFUN_STATE_IND:
                APP_DEBUG("<-- CFUN Status:%d -->\r\n", msg.param2);
                break;  
            default:
                APP_DEBUG("<-- Other URC: type=%d\r\n", msg.param1);
                break;
            }
            break;
        default:
            break;
        }
    }

}

/**************************************************************
* 子任务1
***************************************************************/
void proc_subtask1(s32 TaskId)
{
    u32 cont = 0;
    u32 color = CL_GREEN;

    while(1)
    {   
        /* 10ms扫描按键 */
        bsp_KeyScan();      

        /* 振动马达处理 */
        Motor_Pro(); 

        if(++cont >= 100)
        {
            cont = 0;
            /* 1秒钟扫描一次电池状态 */
//            Battery_Scan(); 

            //  APP_DEBUG("<-- LCD_ClrScr-->\r\n"); 
            //  LCD_ClrScr(CL_WHITE); 
            //  APP_DEBUG("<-- END LCD_ClrScr-->\r\n"); 

 //           Test_FillRec();		//GUI矩形绘图测试
            LCD_ClrScr(color);				/* 清屏，显示全白 */  
            if(color == CL_RED)
                color = CL_GREEN;
            else if(color == CL_GREEN)
                color = CL_BLUE;
            else if(color == CL_BLUE)
                color = CL_RED;            

                       
        }

        /* 延时10ms */
        Ql_Sleep(10);       
    }    
}

/**************************************************************
* 子任务2
***************************************************************/
void proc_subtask2(s32 TaskId)
{
    bool keepGoing = TRUE;
    // ST_MSG subtask1_msg;
    
    // Ql_Debug_Trace("<--multitask: subtask1_entry-->\r\n");
    while(keepGoing)
    {   
        if(bsp_GetKey() == KEY_DOWN_POWER)  /* 电源按键按下 */
        {
            APP_DEBUG("<--K1 按下-->\r\n");
            Motor_Start(50,10,1);           /* 振动 */
        }

 //   	TOUCH_CapScan();	                /* I2C接口电容触摸屏扫描,10ms扫描一次 */

        Ql_Sleep(10);                       /* 延时10ms */
    }    
}

static s32 ReadSerialPort(Enum_SerialPort port, /*[out]*/u8* pBuffer, /*[in]*/u32 bufLen)
{
    s32 rdLen = 0;
    s32 rdTotalLen = 0;
    if (NULL == pBuffer || 0 == bufLen)
    {
        return -1;
    }
    Ql_memset(pBuffer, 0x0, bufLen);
    while (1)
    {
        rdLen = Ql_UART_Read(port, pBuffer + rdTotalLen, bufLen - rdTotalLen);
        if (rdLen <= 0)  // All data is read out, or Serial Port Error!
        {
            break;
        }
        rdTotalLen += rdLen;
        // Continue to read...
    }
    if (rdLen < 0) // Serial Port Error!
    {
        APP_DEBUG("Fail to read from port[%d]\r\n", port);
        return -99;
    }
    return rdTotalLen;
}

static void CallBack_UART_Hdlr(Enum_SerialPort port, Enum_UARTEventType msg, bool level, void* customizedPara)
{
    // u32 iTmp,iNum;
    // s32 iRet;
    
    // char *pData=NULL;
    // char *p=NULL;
    // char *q=NULL;

    //APP_DEBUG("CallBack_UART_Hdlr: port=%d, event=%d, level=%d, p=%x\r\n", port, msg, level, customizedPara);
    switch (msg)
    {
    case EVENT_UART_READY_TO_READ:
        {
            if (m_myUartPort == port)
            {
                s32 totalBytes = ReadSerialPort(port, m_RxBuf_Uart, sizeof(m_RxBuf_Uart));
                if (totalBytes <= 0)
                {
                    APP_DEBUG("<-- No data in UART buffer! -->\r\n");
                    return;
                }
                {// Read data from UART
                     s32 ret;
                    char* pCh = NULL;
                    
                    // Echo
                    Ql_UART_Write(m_myUartPort, m_RxBuf_Uart, totalBytes);

            // pData = m_RxBuf_Uart;
            // if(NULL != (p = Ql_strstr(pData, "Ql_OS_SendMessage")))
            // {                                          //Ql_osSendEvent=1,2
            //     p = Ql_strstr(p, "=");
            //     q = Ql_strstr(p, ",");
            //     if((p == NULL) || (q == NULL))
            //     {
            //         APP_DEBUG("\r\n<-- Ql_OS_SendMessage: Error parameter!-->\r\n");
            //         break;
            //     }
            //     p++; (*q) = 0; q++;

            //     iNum = Ql_atoi(p);
            //     iTmp = Ql_atoi(q);

            //     s_iPassTask = subtask1_id;

            //     iRet = Ql_OS_SendMessage(s_iPassTask,MSG_ID_USER_DATA,iNum, iTmp);
            //     if(iRet <0)
            //     {
            //         APP_DEBUG("\r\n<--failed!!, Ql_OS_SendMessage(1, %d, %d) fail,  ret=%d-->\r\n", iNum, iTmp, iRet);
            //     }
            //     APP_DEBUG("\r\n<--Ql_OS_SendMessage(%d, %d, %d) ret=%d-->\r\n",MSG_ID_USER_DATA,iNum, iTmp, iRet);
            //     break;
            // }

                    pCh = Ql_strstr((char*)m_RxBuf_Uart, "\r\n");
                    if (pCh)
                    {
                        *(pCh + 0) = '\0';
                        *(pCh + 1) = '\0';
                    }

                    // No permission for single <cr><lf>
                    if (Ql_strlen((char*)m_RxBuf_Uart) == 0)
                    {
                        return;
                    }
                    ret = Ql_RIL_SendATCmd((char*)m_RxBuf_Uart, totalBytes, ATResponse_Handler, NULL, 0);
                }
            }
            break;
        }
    case EVENT_UART_READY_TO_WRITE:
        break;
    default:
        break;
    }
}

static s32 ATResponse_Handler(char* line, u32 len, void* userData)
{
    APP_DEBUG("[ATResponse_Handler] %s\r\n", (u8*)line);
    
    if (Ql_RIL_FindLine(line, len, "OK"))
    {  
        return  RIL_ATRSP_SUCCESS;
    }
    else if (Ql_RIL_FindLine(line, len, "ERROR"))
    {  
        return  RIL_ATRSP_FAILED;
    }
    else if (Ql_RIL_FindString(line, len, "+CME ERROR"))
    {
        return  RIL_ATRSP_FAILED;
    }
    else if (Ql_RIL_FindString(line, len, "+CMS ERROR:"))
    {
        return  RIL_ATRSP_FAILED;
    }
    
    return RIL_ATRSP_CONTINUE; //continue wait
}

//#endif // __CUSTOMER_CODE__
