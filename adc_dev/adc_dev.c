/**********************************************************
filename: adcDev.c
**********************************************************/

/************************����ͷ�ļ�***************************************************/
#include "string.h"
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#include "adc_dev.h"
#include "user_log.h"
/**********************************************************
 Private function
**********************************************************/
static  void adcDevStart(adcRsrc_T* r, u16 interval);
static void adcDevStop(adcRsrc_T* r);

static s8 adcDevPolling(adcRsrc_T* r);
static u16 adcDevRead(adcRsrc_T* r, u8 ch);
static u16 adcDevReadAvr(adcRsrc_T* r, u8 ch);
static void adcDevTimHandler(void* e);
/**********************************************************
 Public function
**********************************************************/
void ADC_Setup(
	adcDev_T *p,
	ADC_HandleTypeDef* hadc,
    appTmrDev_t* tmrid,
    IO_Read ioRead,
    IO_Write ioWrite,
    u16 ioBase
){
    s32 i;
	adcRsrc_T* r = &p->rsrc;
	memset(r, 0, sizeof(adcRsrc_T));
	r->hadc = hadc;
    r->tmr = tmrid;
    r->ioRead = ioRead;
    r->ioWrite = ioWrite;
    r->ioBase = ioBase;
    // initial cal
    for(i=0;i<ADC_CH_COUNT;i++){
        r->cal[i].offset = 0;
        r->cal[i].gainDiv = 1;
        r->cal[i].gainMul = 1;
    }
    // read cal from rom

	/* Run the ADC calibration */
	if (HAL_ADCEx_Calibration_Start(hadc) != HAL_OK){	  r->errorCode |= BIT(0);  }
//	p->Polling = adcDevPolling;
	p->Read = adcDevRead;
	p->ReadAvr = adcDevReadAvr;
    p->start = adcDevStart;
    p->stop = adcDevStop;
}

/**********************************************************
 read data
**********************************************************/
static void adcDevTimHandler(void* e){
    adcDevPolling((adcRsrc_T*) e);
}

static s8 adcDevPolling(adcRsrc_T* r){
	u8 i;
	if(r->indx >= ADC_CH_COUNT)	r->indx = 0;
	HAL_ADC_Start(r->hadc);
	HAL_ADC_PollForConversion(r->hadc, 2);
	for(i=ADC_BUFF_LEN-1;i>=1;i--){
		r->adcSeries[r->indx][i] = r->adcSeries[r->indx][i-1];
	}
	r->adcSeries[r->indx][0] = HAL_ADC_GetValue(r->hadc);
	r->indx++;
	return 0;
}

static u16 adcDevRead(adcRsrc_T* r, u8 ch){
	return(r->adcSeries[ch][0]);
}

static u16 adcDevReadAvr(adcRsrc_T* r, u8 ch){
	u8 i;
	u32 x = 0;
	for(i=0;i<ADC_BUFF_LEN;i++){
		x += r->adcSeries[ch][i];
	}
	return(x/ADC_BUFF_LEN);
}

static  void adcDevStart(adcRsrc_T* r, u16 interval){
    r->indx = 0;
    r->tmr->start(&r->tmr->rsrc, interval, POLLING_REPEAT, adcDevTimHandler, r);
}

static void adcDevStop(adcRsrc_T* r){
    r->tmr->stop(&r->tmr->rsrc);
    HAL_ADC_Stop(r->hadc);
}


/**********************************************************
 == THE END ==
**********************************************************/
