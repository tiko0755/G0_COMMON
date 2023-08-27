/**********************************************************
filename: adcDev.h
**********************************************************/
#ifndef _ADC_DEV_H_
#define _ADC_DEV_H_

#include "misc.h"
#include "usr_typedef.h"
#include "app_timer.h"

/*****************************************************************************
 @ typedefs
****************************************************************************/
#define ADC_CH_COUNT	8		// amount of adc channel
#define	ADC_BUFF_LEN	4		// adc result stack length

/*
Hardware configuration:
Scan Conversion Mode: Enable
Continue Conversion Mode: Disable
Discontinuous Conversion Mode: Enable
Number Of Discontinuous Conversions: 1
*/

#pragma pack(push,4)        // push current align bytes, and then set 4 bytes align

typedef struct{
	ADC_HandleTypeDef* hadc;
    appTmrDev_t* tmr;
    IO_Read ioRead;
    IO_Write ioWrite;
	u32 errorCode;
	u16 valSeries[ADC_CH_COUNT][ADC_BUFF_LEN];
    CAL32_T cal[ADC_CH_COUNT];
    u16 ioBase,cnvtCycle;
	u8 indx,isStarted;
}adcRsrc_T;

typedef struct{
	adcRsrc_T rsrc;
	//basic
    void (*start)(adcRsrc_T* r, u16 interval);
    void (*stop)(adcRsrc_T* r);
    void (*cal_offset)(adcRsrc_T* r, u8 ch);
    void (*cal_gain)(adcRsrc_T* r, u8 ch, u16 scaleTo);
    void (*cal_reset)(adcRsrc_T*);
    s32 (*cal_save)(adcRsrc_T*);
	s32 (*read)(adcRsrc_T* r, u8 ch, s16* val);
	s32 (*readRaw)(adcRsrc_T* r, u8 ch, s16* val);
}adcDev_T;

#pragma pack(pop)           //recover align bytes from 4 bytes

void ADC_Setup(
	adcDev_T *p,
	ADC_HandleTypeDef* hadc,
    appTmrDev_t* tmrid,
    u16 interval,
    IO_Read ioRead,
    IO_Write ioWrite,
    u16 ioBase
);

#endif
