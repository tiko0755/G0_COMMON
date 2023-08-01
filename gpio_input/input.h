/**********************************************************
filename: input.h
**********************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _INPUT_H
#define _INPUT_H

#include "misc.h"

typedef struct {
    const PIN_T *PIN; u8 gpioLen;
    char name[DEV_NAME_LEN];
    u32 status[2];
    u32 enableFalling;
    u32 enableRaising;
    void (*fallingCallback)(u8 indx);
    void (*raisingCallback)(u8 indx);
    u16 tick;
} INPUT_RSRC_T;

typedef struct {
    INPUT_RSRC_T rsrc;
    void (*Polling) (INPUT_RSRC_T* pRsrc, u8 tick);
    s8 (*ReadPin)    (INPUT_RSRC_T* pRsrc, u8 pin);
}INPUT_DEV_T;

/* output variables for extern function --------------------------------------*/
void InputDevSetup(
    INPUT_DEV_T *pDev, 
    const PIN_T *gpio, u8 gpioLen
);

#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
