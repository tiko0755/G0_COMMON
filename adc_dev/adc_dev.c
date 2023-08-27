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
#include "crc16.h"
#include "thread_delay.h"

#define ADC_CRC_INIT    (0xadc1)

/**********************************************************
 Private function
**********************************************************/
static  void adcDevStart(adcRsrc_T* r, u16 interval);
static void adcDevStop(adcRsrc_T* r);

static s32 adcDevPolling(adcRsrc_T* r);
static s32 adcDevRead(adcRsrc_T* r, u8 ch, s16* val);
static s32 adcDevReadRaw(adcRsrc_T* r, u8 ch, s16* val);
static void adcDevTimHandler(void* e);
static s32 adcSaveCal(adcRsrc_T* r);
static void adc_cal_reset(adcRsrc_T* r);
static void adcDev_cal_offset(adcRsrc_T* r, u8 ch);
static void adcDev_cal_gain(adcRsrc_T* r, u8 ch, u16 scaleTo);

/**********************************************************
 Public function
**********************************************************/
void ADC_Setup(
	adcDev_T *p,
	ADC_HandleTypeDef* hadc,
    appTmrDev_t* tmrid,
    u16 interval,
    IO_Read ioRead,
    IO_Write ioWrite,
    u16 ioBase
){
    s32 i;
    u16 romCRC,computedCRC;
    u8* buff;
    
	adcRsrc_T* r = &p->rsrc;
	memset(r, 0, sizeof(adcRsrc_T));
	r->hadc = hadc;
    r->tmr = tmrid;
    r->ioRead = ioRead;
    r->ioWrite = ioWrite;
    r->ioBase = ioBase;

    // read cal from rom
    if((ioRead!=NULL) && (ioWrite!=NULL)){
        ioRead(ioBase+0, (u8*)&romCRC, 2);
        buff = (u8*)r->cal;
        ioRead(ioBase+2, buff, sizeof(CAL32_T)*ADC_CH_COUNT);
        computedCRC = CRC16(buff, sizeof(CAL32_T)*ADC_CH_COUNT, ADC_CRC_INIT);
        log("<%s romCRC:0x%02x >", __func__, romCRC);
        log("<%s computedCRC:0x%02x >", __func__, computedCRC);
        if(computedCRC != romCRC){
            for(i=0;i<ADC_CH_COUNT;i++){
                r->cal[i].offset = 0;
                r->cal[i].gainDiv = 1;
                r->cal[i].gainMul = 1;
            }
            adcSaveCal(r);
        }
    }
    else{
        // initial cal
        for(i=0;i<ADC_CH_COUNT;i++){
            r->cal[i].offset = 0;
            r->cal[i].gainDiv = 1;
            r->cal[i].gainMul = 1;
        }
    }

	/* Run the ADC calibration */
	if (HAL_ADCEx_Calibration_Start(hadc) != HAL_OK){	  r->errorCode |= BIT(0);  }
//	p->Polling = adcDevPolling;
	p->read = adcDevRead;
	p->readRaw = adcDevReadRaw;
    p->start = adcDevStart;
    p->stop = adcDevStop;
    p->cal_gain = adcDev_cal_gain;
    p->cal_offset = adcDev_cal_offset;
    p->cal_save = adcSaveCal;
    p->cal_reset = adc_cal_reset;
    
    if(interval>0){
        adcDevStart(r,interval);
    }
}

/**********************************************************
 read data
**********************************************************/
static void adcDevTimHandler(void* e){
    adcDevPolling((adcRsrc_T*) e);
}

static s32 adcDevPolling(adcRsrc_T* r){
	u8 i;
	if(r->indx >= ADC_CH_COUNT){
        r->indx = 0;
        r->cnvtCycle++;
    }
	HAL_ADC_Start(r->hadc);
	HAL_ADC_PollForConversion(r->hadc, 5);
	for(i=ADC_BUFF_LEN-1;i>=1;i--){
		r->valSeries[r->indx][i] = r->valSeries[r->indx][i-1];
	}
	r->valSeries[r->indx][0] = HAL_ADC_GetValue(r->hadc);
	r->indx++;
	return 0;
}

static s32 adcDevRead(adcRsrc_T* r, u8 ch, s16* val){
    if(adcDevReadRaw(r,ch,val)<0){
        return -1;
    }
    *val -= r->cal[ch].offset;
    *val = *val*r->cal[ch].gainMul/r->cal[ch].gainDiv;
    return 0;
}

static s32 adcDevReadRaw(adcRsrc_T* r, u8 ch, s16* val){
	u8 i;
	u32 x = 0,min;
    min = MIN(r->cnvtCycle,ADC_BUFF_LEN);
    if(min == 0){
        return -1;
    }
	*val = r->valSeries[ch][0];
    return 0;
}

static s32 adcDevReadRaw_avr(adcRsrc_T* r, u8 ch, s16* val){
	u8 i;
	u32 x = 0,min;
    min = MIN(r->cnvtCycle,ADC_BUFF_LEN);
    if(min == 0){
        return -1;
    }
	for(i=0; i<min; i++){
		x += r->valSeries[ch][i];
	}
	*val = (x/min);
    return 0;
}

static  void adcDevStart(adcRsrc_T* r, u16 interval){
    r->indx = 0;
    r->cnvtCycle = 0;
    r->tmr->start(&r->tmr->rsrc, interval, POLLING_REPEAT, adcDevTimHandler, r);
    r->isStarted = 1;
}

static void adcDevStop(adcRsrc_T* r){
    r->tmr->stop(&r->tmr->rsrc);
    HAL_ADC_Stop(r->hadc);
    r->isStarted = 0;
}

static void adc_cal_reset(adcRsrc_T* r){
    s32 i;
    for(i=0;i<ADC_CH_COUNT;i++){
        r->cal[i].offset = 0;
        r->cal[i].gainMul = 1;
        r->cal[i].gainDiv = 1;
    }
}

static s32 adcSaveCal(adcRsrc_T* r){
    s32 ret = 0;
    u16 computedCRC;
    u8* buff = (u8*)r->cal;
    computedCRC = CRC16(buff, sizeof(CAL32_T)*ADC_CH_COUNT, ADC_CRC_INIT);
    ret += r->ioWrite(r->ioBase+0, (u8*)&computedCRC, 2);
    ret += r->ioWrite(r->ioBase+2, buff, sizeof(CAL32_T)*ADC_CH_COUNT);
    return ret;
}

static void adcDev_cal_offset(adcRsrc_T* r, u8 chX){
    u8 isStarted_temp = r->isStarted,indx,i,ch;
    s32 sum;

    adcDevStop(r);
    adcDevStart(r,100);
    while(r->cnvtCycle<ADC_CH_COUNT){
        thread_delay(0);
    }
    adcDevStop(r);
    
//    s32 min = MIN(ADC_BUFF_LEN, r->cnvtCount/ADC_CH_COUNT);

    for(ch=0;ch<ADC_CH_COUNT;ch++){
        if((chX<ADC_CH_COUNT) && (chX!=ch)){
            continue;
        }
        log_raw("ch%02d: ", ch);
        sum = 0;
        for(i=0; i<ADC_BUFF_LEN; i++){
            sum += r->valSeries[ch][i];
            log_raw("%04d ", r->valSeries[ch][i]);
        }
        log_raw("sum:%d\n", sum);
        r->cal[ch].offset = sum/ADC_BUFF_LEN;
    }
    
    // recover
    if(isStarted_temp){
        adcDevStart(r,r->tmr->rsrc.interval);
    }
}

static void adcDev_cal_gain(adcRsrc_T* r, u8 ch, u16 scaleTo){
    u8 isStarted_temp = r->isStarted,indx,i;
    s32 sum;

    adcDevStop(r);
    adcDevStart(r,100);
    while(r->cnvtCycle<ADC_CH_COUNT){
        thread_delay(0);
    }
    adcDevStop(r);

    sum = 0;
    for(i=0; i<ADC_BUFF_LEN; i++){
        sum += r->valSeries[ch][i];
    }
    r->cal[ch].gainMul = scaleTo;
    r->cal[ch].gainDiv = sum/ADC_BUFF_LEN - r->cal[ch].offset;
    
    // recover
    if(isStarted_temp){
        adcDevStart(r,r->tmr->rsrc.interval);
    }
}

/**********************************************************
 == THE END ==
**********************************************************/
