/**********************************************************
filename: tmc2160a_dev.h
**********************************************************/
#ifndef __TMC2160A_DEV_H__
#define __TMC2160A_DEV_H__

#include "misc.h"
#include "tmc2160.h"

#define TMC2160A_USING_NAME    1

/*****************************************************************************
 @ typedefs
****************************************************************************/
#pragma pack(push,4)        // push current align bytes, and then set 4 bytes align
typedef struct{
#if TMC2160A_USING_NAME
    char name[DEV_NAME_LEN];
#endif
    const PIN_T *CS;
    const PIN_T *EN;
    const PIN_T *DIA0;
    const PIN_T *DIA1;
		
    SPI_HandleTypeDef* hspi;
    // PWM generator
    TIM_HandleTypeDef* htim;
    uint32_t ch;
    // eeprom
    u16 ioBase;
    s32 (*ioWrite)(u16 addr, const u8 *pDat, u16 nBytes);
    s32 (*ioRead)(u16 addr, u8 *pDat, u16 nBytes);
    // local var
    u16 mA;
    u16 mStep;
    
    // lower 
    TMC2160TypeDef obj;
    ConfigurationTypeDef conf;
}TMC2160A_rsrc_t;

typedef struct{
    TMC2160A_rsrc_t rsrc;
    
    void (*Enable) (TMC2160A_rsrc_t* pRsrc);
    void (*Disable) (TMC2160A_rsrc_t* pRsrc);
    void (*SetMicroStep)    (TMC2160A_rsrc_t* pRsrc, u16 mSteps);
    void (*SetCurrent)    (TMC2160A_rsrc_t* pRsrc, u16 mA);
    void (*Save)    (TMC2160A_rsrc_t* pRsrc);
    void (*Default)    (TMC2160A_rsrc_t* pRsrc);    
    
    // LOWER API
    void (*polling)(TMC2160A_rsrc_t* r, u8 tick);
    void (*readWriteArray)(TMC2160A_rsrc_t* r, uint8_t *data, size_t length);
    
}TMC2160A_dev_t;

#pragma pack(pop)        //recover align bytes from 4 bytes

void TMC2160A_dev_Setup(
    TMC2160A_dev_t *pDev, 
    const char* NAME,
    const PIN_T* CS,
    const PIN_T* EN,
    const PIN_T* DIA0,
    const PIN_T* DIA1,
    SPI_HandleTypeDef* hspi, 
    u8 ch,
    u16 ioBase,
    s32 (*ioWrite)(u16 addr, const u8 *pDat, u16 nBytes),
    s32 (*ioRead)(u16 addr, u8 *pDat, u16 nBytes)
);

#endif
