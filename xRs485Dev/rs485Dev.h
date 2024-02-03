/******************** (C) COPYRIGHT 2015 INCUBECN *****************************
* File Name          : rs485Dev.h
* Author             : Tiko Zhong
* Date First Issued  : 04/20/2020
* Description        : This file provides a set of functions needed to manage the
*                      communication using HAL_UARTxxx
********************************************************************************
* History:
* 04/20/2020: V0.1
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _RS485_DEV_H
#define _RS485_DEV_H

/* Includes ------------------------------------------------------------------*/
#include "misc.h"
#include "uartdev.h"

/* Exported types ------------------------------------------------------------*/
typedef struct{
    UartDev_t uartdev;
    const PIN_T *DE, *DET;
    //u8 squ;
    u16 tick;
    // cb

}Rs485Rsrc_t;

typedef struct{
    Rs485Rsrc_t rsrc;
//    u8 (*RxPolling)        (Rs485Rsrc_t *pRsrc);
    u16 (*RxFetchFrame)    (Rs485Rsrc_t *pRsrc, u8* frame, u16 frameLen);
//    u16 (*TxMonitor)    (Rs485Rsrc_t *pRsrc);
    u16 (*Send)            (Rs485Rsrc_t *pRsrc, const u8* BUF, u16 len);    
    void (*SendStr)    (Rs485Rsrc_t *pRsrc, const char* FORMAT_ORG, ...);
//    void (*TxPolling)    (Rs485Rsrc_t *pRsrc);
}Rs485Dev_t;


/* Exported variables --------------------------------------------------------*/
//extern u8 rxBufIndx;
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void setupRs485Dev(
    Rs485Dev_t *pDev, 
    UART_HandleTypeDef* huart,
		appTmrDev_t* tObj,
    u8* txPool, u16 txPoolLen,
    u8* rxPool,    u16    rxPoolLen,
    u8* rxDoubleBuff,    u16 rxBufLen,	u16 rxPollingTim,
    const PIN_T* DE,
    const PIN_T* DET,
    s8 (*beforeSend)(void),
    s8 (*afterSend)(UART_HandleTypeDef *huart)
);

    
//void setupRs485Dev1(
//    Rs485Dev_t *pDev, 
//    UartDev_t *pUartDev
//);

#endif /* _MY_UART_H */

/******************* (C) COPYRIGHT 2015 INCUBECN *****END OF FILE****/
