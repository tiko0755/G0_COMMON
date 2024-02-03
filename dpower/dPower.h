/**********************************************************
filename: dpower.h
**********************************************************/
#ifndef __DPOWER_H__
#define __DPOWER_H__

#include "misc.h"
#include "usr_typedef.h"

/*****************************************************************************
 @ typedefs
****************************************************************************/
#pragma pack(push,4)        // push current align bytes, and then set 4 bytes align

typedef enum{
    DPWR_CH_VOLT = 0,
    DPWR_CH_CURR,
}DPOWER_CH;

typedef struct{
    const PIN_T* EN;        // dpowerery enable pin
    
    TIM_HandleTypeDef* VSET_PWM_TIM; // set voltage
    u32 VSET_PWM_CH;
    u16 vDuty;
    
    TIM_HandleTypeDef*  ISET_PWM_TIM; // set load current
    u32 ISET_PWM_CH;
    u16 iDuty;
    
    // methods
    IO_Read ioRead;
    IO_Write ioWrite;
    s32 (*readADC)(DPOWER_CH ch, s32* val, u8 tz);
    // variables
	u32 errorCode;
    CAL32_T cal[3];
    u16 ioBase;
    u8 adcChs;
    u8 currPos;
}dPower_rsrc_t;

typedef struct{
	dPower_rsrc_t rsrc;
    // turn off output, to hiZ
    s32 (*Off)(dPower_rsrc_t* r);
    // set output voltage, under max_ma current
    s32 (*SetDischarged)(dPower_rsrc_t* r, u16 mv, s32 max_ua);
    s32 (*SetChargedCurr)(dPower_rsrc_t* r, s32 ua);
    s32 (*Info)(dPower_rsrc_t* r, u16* mv, s32* ua);
    
    // set power up setting
    s32 (*PowerUp)(dPower_rsrc_t* r, u16 mv_dischg, s32 max_ua_dischg, s32 ua_chg);
    
    // Calibration
    s32 (*Cal_offset_blocking)(dPower_rsrc_t* r, u8 ch, u16 timeout);
    s32 (*Cal_gain_blocking)(dPower_rsrc_t* r, u8 ch, s32 scaleTo, u16 timeout);
    void (*Cal_reset)(dPower_rsrc_t* r, u8 ch, s16 offset, s32 mul, s32 div);
//    s32 (*Cal_save)(dPower_rsrc_t* r);
    
    // lower
	s32 (*ReadRaw_blocking)(dPower_rsrc_t* r, u8 ch, s32* rslt, u16 timeout);
    void (*SetPwmDuty_reg)(dPower_rsrc_t* r, u16 duty);
    void (*SetPwmDuty_load)(dPower_rsrc_t* r, u16 duty);
}dPower_dev_t;
#pragma pack(pop)           //recover align bytes from 4 bytes

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
);

#endif
