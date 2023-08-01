#include "output.h"
//#include "stdarg.h"
#include "string.h"
//#include "stdio.h"
/* using external variables ---------------------------------------------------------*/

//const u8 REMAP[16] = {0,1,2,3,4,5,6,7,15,14,13,12,11,10,9,8};
static void outputWritePinHEX(OUTPUT_RSRC_T* pRsrc, u16 hex);
static void outputWritePin(OUTPUT_RSRC_T* pRsrc, u8 pin, OUTPUT_STATUS status);
static void outputTogglePin(OUTPUT_RSRC_T* pRsrc, u8 pin);

//static void outputPolling(OUTPUT_RSRC_T* pRsrc);
//static s8 autoTogglePin (OUTPUT_RSRC_T* pRsrc, u8 pinIndx, u16 mSec);
//static s8 stopTogglePin (OUTPUT_RSRC_T* pRsrc, u8 pinIndx, u16 status);

//static u16 remap(u16 x);
/*******************************************************************************
* Function Name  : outputDevSetup
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
DEV_STATUS outputDevSetup(
    OUTPUT_DEV_T *pDev, 
    const PIN_T* gpio,
    u8 gpioLen,
    u16 initStatus
){
    OUTPUT_RSRC_T* pRsrc = &pDev->rsrc;
    memset(pRsrc,0,sizeof(OUTPUT_DEV_T));
    pRsrc->gpio = gpio;
    pRsrc->gpioLen = gpioLen;
    outputWritePinHEX(pRsrc, initStatus);
    pRsrc->status = initStatus;
    strcpy(pRsrc->name, "output");
    
    pDev->WritePinHEX = outputWritePinHEX;
    pDev->WritePin = outputWritePin;
    pDev->TogglePin = outputTogglePin;

    return DEV_SUCCESS;
}

/*******************************************************************************
* Function Name  : outputPolling
* Description    : per OUTPUT_POLLING_TIME(ms) timer call back, do inputFetch
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
//#define MAX_TOGGLE_TMR  60000 
//static void outputPolling(OUTPUT_RSRC_T* pRsrc){
//    u8 i;
//    u16 tmp = pRsrc->status;
//    //auto toggle
//    for(i=0;i<16;i++){
//        if(pRsrc->autoToggleTmr[i] == 0)    continue;
//        pRsrc->autoToggleTick[i] += OUTPUT_POLLING_TIME;
//        if(pRsrc->autoToggleTick[i] > pRsrc->autoToggleTmr[i]){
//            pRsrc->autoToggleTick[i] = 0;
//            tmp ^= (1U<<i);
//        }
//        else if(pRsrc->autoToggleTick[i] > MAX_TOGGLE_TMR)
//            pRsrc->autoToggleTick[i] = MAX_TOGGLE_TMR;
//    }
//    if(tmp^pRsrc->status)    outputWritePinHEX(pRsrc, tmp);
//}

/*******************************************************************************
* Function Name  : outputWritePinHEX
* Description    : per 4ms timer call back, do inputFetch
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void outputWritePinHEX(OUTPUT_RSRC_T* pRsrc, u16 hex){
    u8 i;
    for(i=0;i<pRsrc->gpioLen;i++){
        if(hex & BIT(i))    HAL_GPIO_WritePin(pRsrc->gpio[i].GPIOx, pRsrc->gpio[i].GPIO_Pin, GPIO_PIN_SET);
        else    HAL_GPIO_WritePin(pRsrc->gpio[i].GPIOx, pRsrc->gpio[i].GPIO_Pin, GPIO_PIN_RESET);
    }
    pRsrc->status = hex;
}

/*******************************************************************************
* Function Name  : Output_WritePin
* Description    : per 4ms timer call back, do inputFetch
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void outputWritePin(OUTPUT_RSRC_T* pRsrc, u8 pin, OUTPUT_STATUS level){
    if(pin >= pRsrc->gpioLen)    return;
    if(level){
        HAL_GPIO_WritePin(pRsrc->gpio[pin].GPIOx, pRsrc->gpio[pin].GPIO_Pin, GPIO_PIN_SET);
        pRsrc->status |= BIT(pin);
    }
    else{
        HAL_GPIO_WritePin(pRsrc->gpio[pin].GPIOx, pRsrc->gpio[pin].GPIO_Pin, GPIO_PIN_RESET);
        pRsrc->status &= (0xffffffff^BIT(pin));
    }
}

/*******************************************************************************
* Function Name  : Output_TogglePin
* Description    : to toggle pin
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void outputTogglePin(OUTPUT_RSRC_T* pRsrc, u8 pin){
    if(pin >= pRsrc->gpioLen)    return;
    if(pRsrc->status & BIT(pin)){
        HAL_GPIO_WritePin(pRsrc->gpio[pin].GPIOx, pRsrc->gpio[pin].GPIO_Pin, GPIO_PIN_RESET);
        pRsrc->status &= (0xffffffff^BIT(pin));
    }
    else{
        HAL_GPIO_WritePin(pRsrc->gpio[pin].GPIOx, pRsrc->gpio[pin].GPIO_Pin, GPIO_PIN_SET);    
        pRsrc->status |= BIT(pin);
    }
}

/*******************************************************************************
* Function Name  : autoTogglePin
* Description    : to toggle pin
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
//static s8 autoTogglePin (OUTPUT_RSRC_T* pRsrc, u8 pinIndx, u16 msec){
//    u8 i;

//    if(pinIndx == 0xff){
//        for(i=0;i<32;i++){
//            pRsrc->autoToggleTmr[i] = msec;
//            pRsrc->autoToggleTick[i] = 0;
//        }
//        return 0;
//    }
//    else if(pinIndx<32){
//        pRsrc->autoToggleTmr[pinIndx] = msec;
//        pRsrc->autoToggleTick[pinIndx] = 0;
//        return 1;
//    }
//    else return -1;
//}

/*******************************************************************************
* Function Name  : stopTogglePin
* Description    : to toggle pin
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
//static s8 stopTogglePin (OUTPUT_RSRC_T* pRsrc, u8 pinIndx, u16 status){
//    u8 i;

//    if(pinIndx == 0xff){
//        for(i=0;i<32;i++)
//            pRsrc->autoToggleTmr[i] = 0;
//        outputWritePinHEX(pRsrc, status);
//        return 0;
//    }
//    else if(pinIndx<32){
//        pRsrc->autoToggleTmr[pinIndx] = 0;
//        if(status==0)    outputWritePin(pRsrc, pinIndx, PIN_RESET);
//        else    outputWritePin(pRsrc, pinIndx, PIN_SET);
//        return 1;
//    }
//    else return -1;
//}

//static u16 remap(u16 x){
//    u8 i;
//    u16 buf = 0;    
//    //remap
//    for(i=0;i<16;i++){
//        if(x & (1U<<i))    buf |= (1U<<REMAP[i]);
//    }
//    return buf;
//}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
