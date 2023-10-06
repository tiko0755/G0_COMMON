#include "input.h"
#include "string.h"

/**********************************************************
 private function
**********************************************************/
//static void inputPolling(INPUT_RSRC_T* pRsrc, u8 tick);
static s8 inputReadPin(INPUT_RSRC_T* pRsrc, u8 pin);
static void inputStart(INPUT_RSRC_T* pRsrc, u16 interval);
/**********************************************************
 public function
**********************************************************/
void InputDevSetup(
    INPUT_DEV_T *pDev, 
    const PIN_T *gpio, u8 gpioLen,
    appTmrDev_t* tObj
){
    INPUT_RSRC_T *pRsrc = &pDev->rsrc;
    memset(pRsrc, 0, sizeof(INPUT_RSRC_T));
    pRsrc->PIN = gpio;
    pRsrc->gpioLen = gpioLen;
    pRsrc->status[0] = 0xffffffff;
    pRsrc->status[1] = 0xffffffff;
    pRsrc->tmrObj = tObj;
    strcpy(pRsrc->name, "input");
    
    pDev->Start = inputStart;
    pDev->ReadPin = inputReadPin;
}

void inputPollingX(void* argV){
    u8 i;
    INPUT_RSRC_T *pRsrc = (INPUT_RSRC_T*)argV;
    pRsrc->tick += pRsrc->interval;
    if(pRsrc->tick < 20)    return;
    pRsrc->tick = 0;
    pRsrc->status[1] = pRsrc->status[0];
    pRsrc->status[0] = 0x00;
//    pRsrc->status[0] = 0xffffffff;
    for(i=0;i<pRsrc->gpioLen;i++){
        if(HAL_GPIO_ReadPin(pRsrc->PIN[i].GPIOx, pRsrc->PIN[i].GPIO_Pin) == GPIO_PIN_RESET)
        //    pRsrc->status[0] &= (0xffffffff^BIT(i));
        pRsrc->status[0] |= BIT(i);
        if((pRsrc->status[1]&BIT(i))>0 && (pRsrc->status[0]&BIT(i))==0){
            if((pRsrc->enableFalling & BIT(i)) && pRsrc->fallingCallback!=NULL)    pRsrc->fallingCallback(i);    }
        else if((pRsrc->status[1]&BIT(i))==0 && (pRsrc->status[0]&BIT(i))>0){
            if((pRsrc->enableRaising & BIT(i)) && pRsrc->raisingCallback!=NULL)    pRsrc->raisingCallback(i);    }
    }
}

static void inputStart(INPUT_RSRC_T* r, u16 interval){
    r->interval = interval;
    r->tmrObj->start(&r->tmrObj->rsrc, interval, POLLING_REPEAT, inputPollingX, r);
}



/*******************************************************************************
* Function Name  : inputFetch
* Description    : per 4ms timer call back, do inputFetch
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
//static void inputPolling(INPUT_RSRC_T* pRsrc, u8 tick){
//    u8 i;
//    pRsrc->tick += tick;
//    if(pRsrc->tick < 20)    return;
//    pRsrc->tick = 0;
//    pRsrc->status[1] = pRsrc->status[0];
//    pRsrc->status[0] = 0x00;
////    pRsrc->status[0] = 0xffffffff;
//    for(i=0;i<pRsrc->gpioLen;i++){
//        if(HAL_GPIO_ReadPin(pRsrc->PIN[i].GPIOx, pRsrc->PIN[i].GPIO_Pin) == GPIO_PIN_RESET)
//        //    pRsrc->status[0] &= (0xffffffff^BIT(i));
//        pRsrc->status[0] |= BIT(i);
//        if((pRsrc->status[1]&BIT(i))>0 && (pRsrc->status[0]&BIT(i))==0){
//            if((pRsrc->enableFalling & BIT(i)) && pRsrc->fallingCallback!=NULL)    pRsrc->fallingCallback(i);    }
//        else if((pRsrc->status[1]&BIT(i))==0 && (pRsrc->status[0]&BIT(i))>0){
//            if((pRsrc->enableRaising & BIT(i)) && pRsrc->raisingCallback!=NULL)    pRsrc->raisingCallback(i);    }
//    }
//}

/*******************************************************************************
* Function Name  : InputReadPin
* Description    : all sample 
* Input          : None
* Output         : 0: low(turn on), 1:high    2:shaking
* Return         : None
*******************************************************************************/
static s8 inputReadPin(INPUT_RSRC_T* pRsrc, u8 pin){
    if(pin>=pRsrc->gpioLen)    return -1;
    if(pRsrc->status[0] & BIT(pin))    return 1;
    else return 0;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

