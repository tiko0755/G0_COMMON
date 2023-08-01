/**********************************************************
filename: output.h
**********************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _OUTPUT_H
#define _OUTPUT_H

#include "misc.h"

#define OUTPUT_POLLING_TIME 16

typedef enum{
    PIN_RESET = 0,
    PIN_SET = 1
} OUTPUT_STATUS;

typedef struct {
    char name[DEV_NAME_LEN];
    const PIN_T* gpio;
    u8 gpioLen;
    u16 status;
} OUTPUT_RSRC_T;

typedef struct {    
    OUTPUT_RSRC_T rsrc;
    void (*WritePinHEX)(OUTPUT_RSRC_T* pRsrc, u16 pinStatus);
    void (*WritePin)(OUTPUT_RSRC_T* pRsrc, u8 pinIndx, OUTPUT_STATUS level);
    void (*TogglePin)(OUTPUT_RSRC_T* pRsrc, u8 pinIndx);
} OUTPUT_DEV_T;

/* output variables for extern function --------------------------------------*/
DEV_STATUS outputDevSetup(
    OUTPUT_DEV_T *pDev, 
    const PIN_T* gpio,
    u8 gpioLen,
    u16 initStatus
);
#endif
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
