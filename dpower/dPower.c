/**********************************************************
filename: dpower.c
**********************************************************/

/************************����ͷ�ļ�***************************************************/
#include "string.h"
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "dpower.h"
#include "user_log.h"
#include "crc16.h"
#include "thread_delay.h"
#include "gpioDecal.h"

#include "main.h"

/**********************************************************
 Private function
**********************************************************/
static void dpowerRegulatorEn(dPower_rsrc_t* r);
static void dpowerRegulatorDis(dPower_rsrc_t* r);
static s32 dpowerSetVolt(dPower_rsrc_t* r, u16 mv);
static s32 dpowerSetCurr(dPower_rsrc_t* r, s32 ua);
static s32 dpowerInfo(dPower_rsrc_t* r, u16* mv, s32* ua);
static s32 dpowerPowerUp(dPower_rsrc_t* r, u16 mv_dischg, s32 max_ua_dischg, s32 ua_chg);
static s32 dpowerCal_offset_blocking(dPower_rsrc_t* r, u8 ch, u16 timeout);
static s32 dpowerCal_gain_blocking(dPower_rsrc_t* r, u8 ch, s32 scaleTo, u16 timeout);
static void dpowerCal_reset(dPower_rsrc_t* r, u8 ch, s16 offset, s32 mul, s32 div);
static s32 dpowerCal_save(dPower_rsrc_t* r);
static void dpowerSetVPwmDuty(dPower_rsrc_t* r, u16 duty);
static void dpowerSetIPwmDuty(dPower_rsrc_t* r, u16 duty);
/**********************************************************
 Public function
**********************************************************/
s32 dPower_dev_Setup(
	dPower_dev_t *d,           // object
    const PIN_T* EN,        // dpowerery enable pin
    
    TIM_HandleTypeDef* vset_pwm_tim, // set voltage
    u32 vset_pwm_ch,
    
    TIM_HandleTypeDef* iset_pwm_tim, // set load current
    u32 iset_pwm_ch,

    s32 (*readADC)(DPOWER_CH ch, s32* val, u8 tz),
    IO_Read ioRead,
    IO_Write ioWrite,
    u16 ioBase
){
    s32 i;
    u16 romCRC,computedCRC;
    u8* buff;
    
    memset(d, 0, sizeof(dPower_dev_t));
		dPower_rsrc_t* r = &d->rsrc;

    r->EN = EN;
    r->VSET_PWM_TIM = vset_pwm_tim;
    r->VSET_PWM_CH = vset_pwm_ch;
    r->ISET_PWM_TIM = iset_pwm_tim;
    r->ISET_PWM_CH = iset_pwm_ch;
    
    r->readADC = readADC;
    r->ioRead = ioRead;
    r->ioWrite = ioWrite;
    r->ioBase = ioBase;
    
    as_OUTPUT_PP_PULLUP_LOW(EN);           
    
    d->Off = dpowerSwitchOff;
    d->SetDischarged = dpowerSetDischarged;
    d->SetChargedCurr = dpowerSetChargedCurr;
    d->Info = dpowerInfo;
    d->PowerUp = dpowerPowerUp;

    d->Cal_offset_blocking = dpowerCal_offset_blocking;
    d->Cal_gain_blocking = dpowerCal_gain_blocking;
    d->Cal_reset = dpowerCal_reset;
    
    d->SetPwmDuty_load = dpowerSetPwmDuty_load;
    d->SetPwmDuty_reg = dpowerSetPwmDuty_reg;
    
    // read from EEPROM
    if((ioRead!=NULL) && (ioWrite!=NULL)){
        ioRead(ioBase+0, (u8*)&romCRC, 2);
        buff = (u8*)r->cal;
        ioRead(ioBase+2, buff, sizeof(CAL32_T)*r->adcChs);
        computedCRC = CRC16(buff, sizeof(CAL32_T)*r->adcChs, CRC_INIT);
        if(computedCRC != romCRC){
            log("<%s romCRC:0x%02x >", __func__, romCRC);
            log("<%s computedCRC:0x%02x >", __func__, computedCRC);
            for(i=0;i<r->adcChs;i++){
                r->cal[i].offset = 0;
                r->cal[i].gainDiv = 1;
                r->cal[i].gainMul = 1;
            }
            dpowerCal_save(r);
        }
    }
    else{
        // initial cal
        for(i=0;i<r->adcChs;i++){
            r->cal[i].offset = 0;
            r->cal[i].gainDiv = 1;
            r->cal[i].gainMul = 1;
        }
    }
    
    if (HAL_TIM_PWM_Start(r->VSET_PWM_TIM, r->VSET_PWM_CH) != HAL_OK)
    {
        /* PWM Generation Error */
        log("<%s 'err@HAL_TIM_PWM_Start LOAD_ISET_PWM_TIM' >", __func__);
    }
    else{
        dpowerSetPwmDuty_reg(r,0);
    }

    if (HAL_TIM_PWM_Start(r->ISET_PWM_TIM, r->ISET_PWM_CH) != HAL_OK)
    {
        /* PWM Generation Error */
        log("<%s 'err@HAL_TIM_PWM_Start LOAD_ISET_PWM_TIM' >", __func__);
    }
    else{
        dpowerSetPwmDuty_load(r,0);
    }


    
    return 0;
}

/**********************************************************

**********************************************************/
// hardware Interface
static void dpowerRegulatorEn(dPower_rsrc_t* r){
    HAL_GPIO_WritePin(r->EN->GPIOx, r->EN->GPIO_Pin, GPIO_PIN_RESET);
}

static void dpowerRegulatorDis(dPower_rsrc_t* r){
    HAL_GPIO_WritePin(r->EN->GPIOx, r->EN->GPIO_Pin, GPIO_PIN_SET);
}

static s32 dpowerSetVolt(dPower_rsrc_t* r, u16 mv){
    return 0;
}

static s32 dpowerSetCurr(dPower_rsrc_t* r, s32 ua){
    return 0;
}

static s32 dpowerInfo(dPower_rsrc_t* r, u16* mv, s32* ua){
    s32 i,j,k,err = 0;
    s32 adcRslt = 0;

    return err;
}

static s32 dpowerPowerUp(dPower_rsrc_t* r, u16 mv_dischg, s32 max_ua_dischg, s32 ua_chg){
    return 0;
}

// Calibration
static s32 dpowerCal_offset_blocking(dPower_rsrc_t* r, u8 ch, u16 timeout){
    UNUSED(timeout);
    s32 i,j,k=0;

    
    // read adc convert result
    s32 adcRslt = 0;
    if(r->readADC(ch, &adcRslt, 8) < 0){
        return -5;
    }  
    r->cal[ch].offset = adcRslt;
    dpowerCal_save(r);
    
    dpowerCurrPos_ma(r); // return to ma
    return 0;
}

#define dpower_ADC_MAX  (0X0001FFFF)
static s32 dpowerCal_gain_blocking(dPower_rsrc_t* r, u8 ch, s32 scaleTo, u16 timeout){
    UNUSED(timeout);
    s32 i,j,k=0;
    
    // read adc convert result
    s32 adcRslt = 0;
    if(r->readADC(ch, &adcRslt, 8) < 0){
        return -5;
    }  
    log("<%s adcRslt:%d >", __func__, adcRslt);
		
    if(abs(scaleTo) >= (0x7fffffff/dpower_ADC_MAX)){
        r->cal[ch].gainMul = (0x7fffffff/dpower_ADC_MAX);
        r->cal[ch].gainDiv = (0x7fffffff/dpower_ADC_MAX)*(adcRslt - r->cal[ch].offset)/scaleTo;
    }
    else{
        r->cal[ch].gainMul = scaleTo;
        r->cal[ch].gainDiv = adcRslt - r->cal[ch].offset;
    }

//    r->cal[ch].gainDiv = adcRslt -  r->cal[ch].offset;
//    r->cal[ch].gainMul = scaleTo;
    dpowerCal_save(r);
    
    dpowerCurrPos_ma(r);
    return 0;
}

static void dpowerCal_reset(dPower_rsrc_t* r, u8 ch, s16 offset, s32 mul, s32 div){
    if(ch > DPWR_CH_CURR){
        return;
    }
    r->cal[ch].offset = offset;
    r->cal[ch].gainDiv = div;
    r->cal[ch].gainMul = mul;
    dpowerCal_save(r);
}

static s32 dpowerCal_save(dPower_rsrc_t* r){
    s32 ret = 0;
    u16 computedCRC;
    u8* buff = (u8*)r->cal;
    computedCRC = CRC16(buff, sizeof(CAL32_T)*r->adcChs, CRC_INIT);
    ret += r->ioWrite(r->ioBase+0, (u8*)&computedCRC, 2);
    ret += r->ioWrite(r->ioBase+2, buff, sizeof(CAL32_T)*r->adcChs);
    return ret;
}

static void dpowerSetVPwmDuty(dPower_rsrc_t* r, u16 duty){
    r->vDuty = duty;
    __HAL_TIM_SET_COMPARE(r->VSET_PWM_TIM, r->VSET_PWM_CH, duty);
    log("<%s vPwmDuty:%d >", __func__, r->vDuty);
}

static void dpowerSetIPwmDuty(dPower_rsrc_t* r, u16 duty){
    r->iDuty = duty;
    __HAL_TIM_SET_COMPARE(r->ISET_PWM_TIM, r->ISET_PWM_CH, duty);
    log("<%s vPwmDuty:%d >", __func__, r->vDuty);
}

/**********************************************************
 == THE END ==
**********************************************************/
