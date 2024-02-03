/******************** (C) COPYRIGHT 2015 INCUBECN *****************************
* File Name          : uartdev.c
* Author             : Tiko Zhong
* Date First Issued  : 04/20/2020
* Description        : This file provides a set of functions needed to manage the
*                      communication using HAL_UARTxxx
********************************************************************************
* History:
* 04/20/2020: V0.1
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "uartDev.h"
#include "stdarg.h"
#include "stdio.h"
#include "string.h"
#include "crc16.h"
#include "board.h"
#include "user_log.h"
/* Public variables ---------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define UART_FRAM_HEAD   (0xed98ba)
#define UART_INTERVAL    (1)
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void uartPolling(void* e);
static u16 uartTxPolling(UartRsrc_t *r);
static u8 uartRxMonitor(UartRsrc_t *r);
static u16 uartRxFetchLine(UartRsrc_t *r, u8* line, u16 len);
static u16 uartTxSendFrame(UartRsrc_t *r, const u8* BUF, u16 len);
static s16 uartSend(UartRsrc_t *r, const u8* BUF, u16 len);

#if    UART_ALL_FUNCTION
static void uartTxSendString(UartRsrc_t *r, const char* FORMAT_ORG, ...);
static s16 uartSendSync(UartRsrc_t *r, const u8* BUF, u16 len);
static void uartTxSendStringSync(UartRsrc_t *r, const char* FORMAT_ORG, ...);
#endif

static void uartStartRecv(UartRsrc_t *r);
static s32 uartIsTxRBuffEmpty(UartRsrc_t *r);
/*******************************************************************************
* Function Name  : uartSrscSetup
* Description    : 
* Input          : 
* Output         : None
* Return         : None
*******************************************************************************/
void setupUartDev(
    UartDev_t *pDev, 
    UART_HandleTypeDef* huart,
    appTmrDev_t* tObj,
    u8* txPool, u16 txPoolLen,
    u8* rxPool,    u16    rxPoolLen,
    u8* rxDoubleBuff,    u16 rxBufLen, u16 rxPollingTim
){
    UartRsrc_t *r = &pDev->rsrc;
    r->huart = huart;
    r->tmr = tObj;

    r->rxPool = rxPool;
    r->rxPoolLen = rxPoolLen;
    
    r->rxBuf0 = rxDoubleBuff;
    r->rxBuf1 = rxDoubleBuff + rxBufLen;
    r->rxBufLen = rxBufLen;
    
    r->txPool = txPool;
    r->txPoolLen = txPoolLen;
    r->rxPollingTim = rxPollingTim;

    r->rxCurBuf = r->rxBuf0;
    r->rxNxtBuf = r->rxBuf1;
    
    r->afterSend = NULL;
    r->beforeSend = NULL;
    
    //register op
    pDev->TxPolling = uartTxPolling;
    pDev->RxPolling = uartRxMonitor;
    pDev->RxFetchLine = uartRxFetchLine;
    
    pDev->TxSendFrame = uartTxSendFrame;
    pDev->Send = uartSend;
    pDev->IsTxRBuffEmpty = uartIsTxRBuffEmpty;

#if    UART_ALL_FUNCTION
    pDev->SendSync = uartSendSync;
    pDev->SendStr = uartTxSendString;
    pDev->SendStrSync = uartTxSendStringSync;
#endif

    pDev->StartRcv = uartStartRecv;
    
    RingBuffer_Init(&r->txRB, r->txPool, 1, r->txPoolLen);
}

void setupUartDev_2(
    UartDev_t *pDev, 
    UART_HandleTypeDef* huart,
	appTmrDev_t* tObj,
    u8* txPool, u16 txPoolLen,
    u8* rxPool,    u16    rxPoolLen,
    u8* rxDoubleBuff,    u16 rxBufLen, u16 rxPollingTim,
    s8 (*beforeSend)(void),
    s8 (*afterSend)(UART_HandleTypeDef *huart)
){
    setupUartDev(
        pDev, huart, tObj, 
        txPool, txPoolLen,
        rxPool, rxPoolLen,
        rxDoubleBuff, rxBufLen, rxPollingTim
    );
    UartRsrc_t *r = &pDev->rsrc;
    r->beforeSend = beforeSend;
    r->afterSend = afterSend;
}

static void uartStartRecv(UartRsrc_t *r){
    memset(r->rxBuf0,0,r->rxBufLen*2);
    memset(r->rxPool,0,r->rxPoolLen);
    RingBuffer_Init(&r->rxRB, r->rxPool, 1, r->rxPoolLen);    
    r->rxCurBuf = r->rxBuf0;
    r->rxNxtBuf = r->rxBuf1;
    while(HAL_UART_Receive_IT(r->huart, r->rxCurBuf, r->rxBufLen) != HAL_OK){
    }
    r->tmr->start(&r->tmr->rsrc, UART_INTERVAL, POLLING_REPEAT, uartPolling, r);
}

static void uartPolling(void* e){
    UartRsrc_t *r = (UartRsrc_t*)e;
    uartTxPolling(r);
    r->tick += UART_INTERVAL;
    if(r->tick < r->rxPollingTim){
        return;
    }
    r->tick = 0;
    uartRxMonitor(r);
}

static u16 uartTxPolling(UartRsrc_t *r){
    s32 bytes;
    if(r->huart->gState & BIT(0))    return 0;    
    if(RingBuffer_IsEmpty(&r->txRB))    return 0;    
    if(r->beforeSend && r->beforeSend()<0)    return 0;
    bytes = RingBuffer_PopMult(&r->txRB, (u8*)r->txBuff, UART_TX_BUFF_LEN);
    if(bytes>0){
        while(HAL_UART_Transmit_IT(r->huart, r->txBuff, bytes) != HAL_OK){}
    }
    return bytes;
}

static s16 uartSend(UartRsrc_t *r, const u8* BUF, u16 len){
    u16 sentBytes;
    if(BUF == NULL || len==0)    return 0;
    for(sentBytes = 0; sentBytes < len; ){
        sentBytes += RingBuffer_InsertMult(&r->txRB, (void*)&BUF[sentBytes], len-sentBytes);
    }
    return sentBytes;
}

#if    UART_ALL_FUNCTION
static s16 uartSendSync(UartRsrc_t *r, const u8* BUF, u16 len){
    u16 sentBytes;
    for(sentBytes = 0; 1; ){
        sentBytes += RingBuffer_InsertMult(&r->txRB, (void*)&BUF[sentBytes], len-sentBytes);
        uartTxPolling(r);
        if(RingBuffer_IsEmpty(&r->txRB))    break;    
    }
    return sentBytes;
}

static void uartTxSendString(UartRsrc_t *r, const char* FORMAT_ORG, ...){
    va_list ap;
    s16 bytes;
    char buff[512]={0};
    
    if(FORMAT_ORG == NULL)    return ;
    va_start(ap, FORMAT_ORG);
    bytes = vsnprintf(buff, 512, FORMAT_ORG, ap);
    va_end(ap);
    
    uartSend(r, (u8*)buff, bytes);
}

static void uartTxSendStringSync(UartRsrc_t *r, const char* FORMAT_ORG, ...){
    va_list ap;
    s16 bytes;
    char buff[512]={0};
    
    if(FORMAT_ORG == NULL)    return ;
    va_start(ap, FORMAT_ORG);
    bytes = vsnprintf(buff, 512, FORMAT_ORG, ap);
    va_end(ap);
    
    uartSendSync(r, (u8*)buff, bytes);
}
#endif

static u16 uartTxSendFrame(UartRsrc_t *r, const u8* BUF, u16 len){
    u8 buff[6];
    
    if(BUF == NULL || len==0)    return 0;
    buff[0] = UART_FRAM_HEAD&0xff;
    buff[1] = (UART_FRAM_HEAD>>8)&0xff;    
    buff[2] = (UART_FRAM_HEAD>>16)&0xff;
    buff[3] = crc8(BUF, len);
    buff[4] = len;
    uartSend(r, buff, 5);
    uartSend(r, BUF, len);
    return (len+5);
}

static u8 uartRxMonitor(UartRsrc_t *r){
    s16 bytesReceived;
    u8* pTmp, abandond;
    UART_HandleTypeDef *huart = r->huart;

  /* Disable RXNE, PE and ERR (Frame error, noise error, overrun error) interrupts */
    __HAL_UART_DISABLE_IT(huart, UART_IT_RXNE|UART_IT_PE);
    bytesReceived = huart->RxXferSize - huart->RxXferCount;
    if(bytesReceived > 0){
        //restart uart
        huart->pRxBuffPtr = r->rxNxtBuf;
        huart->RxXferCount = r->rxBufLen;
    }
    __HAL_UART_ENABLE_IT(huart, UART_IT_RXNE|UART_IT_PE);

    if(bytesReceived <= 0)    return 0;
    
    // only keep the last received
    while(RingBuffer_GetFree(&r->rxRB) < bytesReceived){
        RingBuffer_Pop(&r->rxRB, &abandond);
        r->errorCode |= BIT(0);
    }
    RingBuffer_InsertMult(&r->rxRB, r->rxCurBuf, bytesReceived);
    
    pTmp = r->rxCurBuf;
    r->rxCurBuf = r->rxNxtBuf;
    r->rxNxtBuf = pTmp;
    
    return (bytesReceived);
}

static s32 uartIsTxRBuffEmpty(UartRsrc_t *r){
    return(RingBuffer_IsEmpty(&r->txRB));
}

u16 fetchLineFromRingBufferU8(RINGBUFF_T* rb, u8* line, u16 len){
    return(fetchLineFromRingBuffer(rb, (char*)line, len));
}

u16 fetchLineFromRingBuffer(RINGBUFF_T* rb, char* line, u16 len){
    u8 ret = 0;
    char *p = NULL;
    s32 i,lineLen=0, bytes, count;
       
    count = RingBuffer_GetCount(rb);
    if((count <= 0) || (line==NULL) || (len==0))    return 0;

    // only take the lase receive
    while(count > len){
        RingBuffer_Pop(rb, line);
        count = RingBuffer_GetCount(rb);
    }
    bytes = RingBuffer_PopMult(rb, line, len);
    RingBuffer_Flush(rb);

    // seek for end code
    for(i=0;i<bytes;i++){
        p = strstr(&line[i], CMD_END);    //be careful!! p can be out off buff, because 'line' may not end with '\0'
        if(p){
            // test overfloat memory
            if(p > (line+bytes-strlen(CMD_END))){
                p = NULL;
                break;
            }    
            lineLen = p-(char*)&line[i]+strlen(CMD_END);
            count = bytes - (i+lineLen);
            if(count > 0){
                RingBuffer_InsertMult(rb, &line[i+lineLen], count);
            }
            if(i>0){
                memmove(line, &line[i], (lineLen>len?len:lineLen));
                line[lineLen] = 0;
            }
            ret = 1;
            break;
        }
    }
    
    if(p==NULL){    RingBuffer_InsertMult(rb, line, bytes);    }

    if(ret > 0){
        return strlen(line);
    }
    return 0;
}

s32 fetchLineFromRingBufferX(RINGBUFF_T* rb, const u8* SYMBOL, u8 symbol_len, u8* line, u16 len){
    s32 ret, i, j, ii, count, smbLen;
    u8 x,symbol[MAX_LINE_SYMBOL_LEN] = {0};
    
    count = RingBuffer_GetCount(rb);
    if((count < smbLen)|| (SYMBOL==NULL) || (line==NULL) || (len==0))    return 0;
    
    smbLen = MIN(MAX_LINE_SYMBOL_LEN, symbol_len); 

    j = -1;        // if j be set, it stands for the len of line
    memset(symbol, 0, smbLen);
    i = RingBuffer_PopMult(rb, symbol, smbLen);         // pop first
    if(i != smbLen){
        RingBuffer_InsertMult(rb, symbol, i);
        return -1;
    }
    do{
        if(memcmp(symbol, SYMBOL, smbLen) == 0){
            j = i;
        }
        
        RingBuffer_Insert(rb, &symbol[0]);
        for(ii=0; ii<smbLen-1; ii++){
            symbol[ii] = symbol[ii+1];
        }
        // will pop 1 bytes, or fail will return 0
        if(RingBuffer_Pop(rb, &symbol[smbLen-1])==0){
            break;
        }
        i++;
    }while(i < count);
    RingBuffer_InsertMult(rb, symbol, smbLen);         // push finally

    if(j < 0){    return -1;    }

    // count bytes should be abandoned due to the lack of line
    count = j-len;
    for(i=0;i<count;i++){
        RingBuffer_Pop(rb, &x);
        j--;
    }
    ret = RingBuffer_PopMult(rb, line, j);

    return ret;
}

//better if len equ rxRB' pool len
static u16 uartRxFetchLine(UartRsrc_t *r, u8* line, u16 len){
    return(fetchLineFromRingBufferX(&r->rxRB, (const u8*)CMD_END, strlen(CMD_END), (u8*)line, len));
}

u16 fetchFrameFromRingBuffer(RINGBUFF_T* rb, u8* frame, u16 fLen){
    s32 i,j,k;
    u8 crc0,crc1;
    u8 *buff = frame;
    u16 len = 0;
    s16 bytes;
    
    if(RingBuffer_GetCount(rb) < (3+2+1))    return 0;    
        
    bytes = RingBuffer_PopMult(rb, buff, fLen);
    RingBuffer_Flush(rb);
    
    j = UART_FRAM_HEAD;
    for(i=0;i<bytes;i++){
        if(buff[i] != (0xff&j)){
            j = UART_FRAM_HEAD;
        }
        j >>= 8;
        if(j==0){
            // meet HEAD
            crc0 = buff[i+1];
            len = buff[i+2];
            crc1 = crc8(&buff[i+3], len);
            if(crc0 == crc1){
//                log("<%s 'crc match' >", __func__);
                for(k=0;k<fLen;k++){
                    if(k >= len){    buff[k] = 0;   }
                    else{    buff[k] = buff[i+3+k];   }
                }
//                log("<%s buff:%s >", __func__, buff);
                // give back to ringbuffer
                if((i+3+len) < bytes){
                    RingBuffer_InsertMult(rb, &buff[i+3+len], bytes-(i+3+len));
//                    log("<%s giveB0:%d >", __func__, RingBuffer_GetCount(rb));
                }
                break;
            }
            len = 0;
            RingBuffer_InsertMult(rb, &buff[i-2], bytes-(i-2));
//            log("<%s giveB1:%d >", __func__, RingBuffer_GetCount(rb));
            break;
        }
    }
    if(j){
        // match any HEAD
        // only give back the last 3 bytes
//        log("<%s buff:%s >", __func__, buff);
        RingBuffer_InsertMult(rb, &buff[bytes-3], 3);
//        log("<%s giveB2:%d >", __func__, RingBuffer_GetCount(rb));
    }
    return len;
}

/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
