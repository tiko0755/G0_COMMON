/******************** (C) COPYRIGHT 2015 INCUBECN *****************************
* File Name          : rs485Dev.c
* Author             : Tiko Zhong
* Date First Issued  : 04/20/2020
* Description        : This file provides a set of functions needed to manage the
*                      communication using HAL_UARTxxx
********************************************************************************
* History:
* 04/20/2020: V0.1
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "rs485Dev.h"
#include "stdarg.h"
#include "stdio.h"
#include "string.h"
#include "crc16.h"

/* Public variables ---------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

static u8 rs485RxMonitor(Rs485Rsrc_t *pRsrc);
static u16 rs485RxFetchFrame(Rs485Rsrc_t *pRsrc, u8* frame, u16 frameLen);
static void rs485TxPolling(Rs485Rsrc_t *pRsrc);
static u16 rs485TxSend(Rs485Rsrc_t *pRsrc, const u8* BUF, u16 len);
static void rs485TxSendString(Rs485Rsrc_t *pRsrc, const char* FORMAT_ORG, ...);
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : uartSrscSetup
* Description    : 
* Input          : 
* Output         : None
* Return         : None
*******************************************************************************/
void setupRs485Dev(
    Rs485Dev_t *pDev,
    UART_HandleTypeDef* huart,
    u8* txPool, u16 txPoolLen,
    u8* rxPool,    u16    rxPoolLen,
    u8* rxDoubleBuff,    u16 rxBufLen,
    const PIN_T DE,
    const PIN_T DET,
    s8 (*beforeSend)(void),
    s8 (*afterSend)(UART_HandleTypeDef *huart)
){
    Rs485Rsrc_t *pRsrc = &pDev->rsrc;
    
    memset(&pRsrc->uartdev, 0, sizeof(UartDev_t));

    pRsrc->DE = DE;
    pRsrc->DET = DET;
    HAL_GPIO_WritePin(pRsrc->DE.GPIOx, pRsrc->DE.GPIO_Pin, GPIO_PIN_SET);
    
    setupUartDev(&pRsrc->uartdev, huart,
        txPool, txPoolLen,
        rxPool, rxPoolLen,
        rxDoubleBuff, rxBufLen
    );
    pRsrc->uartdev.rsrc.beforeSend = beforeSend;
    pRsrc->uartdev.rsrc.afterSend = afterSend;
    
    pDev->RxPolling = rs485RxMonitor;
    pDev->RxFetchFrame = rs485RxFetchFrame;
    pDev->Send = rs485TxSend;
    pDev->SendStr = rs485TxSendString;
    pDev->TxPolling = rs485TxPolling;

}

static u16 rs485TxSend(Rs485Rsrc_t *pRsrc, const u8* BUF, u16 len){
    UartDev_t* pUartDev = &pRsrc->uartdev;

    if(BUF == NULL || len == 0 )    return 0;
    pUartDev->TxSendFrame(&pUartDev->rsrc, BUF, len);

    return len;
}

static void rs485TxSendString(Rs485Rsrc_t *pRsrc, const char* FORMAT_ORG, ...){
    va_list ap;
    s16 bytes;
    UartDev_t* pUartDev = &pRsrc->uartdev;
    char buff[MAX_CMD_LEN]={0};
    
    if(FORMAT_ORG == NULL)    return ;
    va_start(ap, FORMAT_ORG);
    bytes = vsnprintf(buff, MAX_CMD_LEN, FORMAT_ORG, ap);
    va_end(ap);
    pUartDev->TxSendFrame(&pUartDev->rsrc, (u8*)buff, bytes);
}

static u8 rs485RxMonitor(Rs485Rsrc_t *pRsrc){
    return (pRsrc->uartdev.RxPolling(&pRsrc->uartdev.rsrc));
}

static void rs485TxPolling(Rs485Rsrc_t *pRsrc){
    pRsrc->uartdev.TxPolling(&pRsrc->uartdev.rsrc);
}

static u16 rs485RxFetchFrame(Rs485Rsrc_t *pRsrc, u8* frame, u16 frameLen){
    return (pRsrc->uartdev.RxFetchFrame(&pRsrc->uartdev.rsrc, frame, frameLen));
}

/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
