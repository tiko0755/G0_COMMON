/**********************************************************
filename: vRegulator.h
**********************************************************/
#ifndef _DC2DC_H_
#define _DC2DC_H_

#include "misc.h"
#include "usr_typedef.h"

/* Attention: PWM timer should set its counter to 4096, LOW means off.
 *
 *
 *
 */

/*****************************************************************************
 @ typedefs
****************************************************************************/
#pragma pack(push,4)        // push current align bytes, and then set 4 bytes align
typedef enum{
    DPWR_CH_VOLT = 0,
    DPWR_CH_CURR,
}DPOWER_CH;

typedef struct{
	char name[DEV_NAME_LEN];
    
	TIM_HandleTypeDef* vTimHandle;
	u32 vTimChannel;
    TIM_HandleTypeDef* iTimHandle;
	u32 iTimChannel;
	ADC_HandleTypeDef* vAdcHandle;
	u32 vAdcChannel;
    ADC_HandleTypeDef* iAdcHandle;
	u32 iAdcChannel;

	u16 mv_adc_rate;
	u16 pwmDuty;
	u32 curVolt, prvVolt;

	u32 calVolt_uv;
	u16 calAdc, calDuty;

	u16 limit_mv, powerOn_mv;
	u8 isPowerOn;

	u32 errorCode;

	//eeprom io
    IO_Read ioRead;
    IO_Write ioWrite;
	u16 ioBase;
    CAL32_T cal[2][4];
}dcvRsrc_t;

typedef struct{
	dcvRsrc_t rsrc;
	//op
	void (*defaultx)(dcvRsrc_t*);
	u16 (*getVolt)(dcvRsrc_t*);
	u16 (*getADC)(dcvRsrc_t*);
	s32 (*turnOn)(dcvRsrc_t*);
	s32 (*turnOff)(dcvRsrc_t*);
	s32 (*setVolt)(dcvRsrc_t*, u16 mv);
	s32 (*incVoltBy)(dcvRsrc_t*, u16 inc_mv);
	s32 (*decVoltBy)(dcvRsrc_t*, u16 dec_mv);
	void (*callibrate_uv)(dcvRsrc_t*, u32 true_uv);
	void (*config)(dcvRsrc_t* pRsrc, u16 limit, u16 poweron, u8 ispoweron);
}dcvDev_t;
#pragma pack(pop)           //recover align bytes from 4 bytes

void dcvDevSetup(dcvDev_t* pDev,
	const char* NAME,
	TIM_HandleTypeDef* vTimHandle,
	u32 vTimChannel,
	ADC_HandleTypeDef* vAdcHandle,
	u32 vAdcChannel,
    TIM_HandleTypeDef* iTimHandle,
	u32 iTimChannel,
    ADC_HandleTypeDef* iAdcHandle,
	u32 iAdcChannel,
	u16 ioBase,
    s32 (*ioWrite)(u16 addr, const u8 *pDat, u16 nBytes),
	s32 (*ioRead)(u16 addr, u8 *pDat, u16 nBytes)
);

#endif
