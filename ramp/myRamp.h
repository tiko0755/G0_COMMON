/*
 * myRamp.h
 *
 *  Created on: 09.11.2018
 *      Author: LK
 */


#ifndef _MY_RAMP_H_
#define _MY_RAMP_H_

#include "misc.h"
#include "app_timer.h"

#define RAMP_STATUS_BIT_RUN	0
#define RAMP_STATUS_BIT_DIR	1
#define RAMP_STATUS_BIT_ACC	2
#define RAMP_STATUS_BIT_DEC	3
#define RAMP_STATUS_BIT_REFL	4
#define RAMP_STATUS_BIT_REFR	5

#define	RAMP_DIR_LEFT	0
#define	RAMP_DIR_RIGHT 	1

#define RAMP_RUN_MOD_STOP	0
#define RAMP_RUN_MOD_SPD	1
#define RAMP_RUN_MOD_POS	2

#pragma pack(push,4)        // push current align bytes, and then set 4 bytes align

typedef struct{
	char name[DEV_NAME_LEN];
	TIM_HandleTypeDef *htim;
	u32 tCh;
	const PIN_T* DIR;
	const PIN_T* REFL;
	const PIN_T* REFR;
    
    appTmrDev_t* tmr;
	u16 mStep;

	u16 mul;
	u16 mulNxt;
	u8 comp[4];
	u8 runMod, status;

	u8 dirNxt;

	u16 spdCur;		// current speed
	u16 spdTgt;		// target speed
	u16 spdTgtNxt;	// next target speed
	u16 spdMax;
	u16 spdMin;

	s32 posCur;
	s32 posTgt;
	s32 posTgtNxt;

	u8 accLock;

	u8 squ;

	u8 rampIndx;	// RAMP_TAB index
	u16 pulsePerSpot;	// each freqence keep pulse run
	u32 pulseCount;	// pulses counter
	u32 pulsePerRampCycle;	//speed up to target, need pulses

	u8 posReloadEn;

	u8 isNewMul;
	u8 isHoming;
	u8 en;
	u8 stopImmeditely;
	u8 error;
    u16 tick;
}rampRsrc_t;

typedef struct{
	rampRsrc_t rsrc;
	void (*isr)(rampRsrc_t* rsrc, TIM_HandleTypeDef *htim);
	void (*isrReleasedRefL)(rampRsrc_t* rsrc, u16 GPIO_Pin);
	void (*isrShelteredRefL)(rampRsrc_t* rsrc, u16 GPIO_Pin);
	void (*isrReleasedRefR)(rampRsrc_t* rsrc, u16 GPIO_Pin);
	void (*isrShelteredRefR)(rampRsrc_t* rsrc, u16 GPIO_Pin);

	void (*gohome)(rampRsrc_t* rsrc, u16 maxSpd);
	void (*homing)(rampRsrc_t* rsrc, u16 maxSpd);
	u8	 (*isHoming)(rampRsrc_t* rsrc);

	s32  (*rotateL)(rampRsrc_t* rsrc, u16 targetSpd);
	s32  (*rotateR)(rampRsrc_t* rsrc, u16 targetSpd);
	s32  (*moveTo)(rampRsrc_t* rsrc, s32 targetPos);
	s32  (*moveBy)(rampRsrc_t* rsrc, s32 refPos);
	u8	 (*isRotating)(rampRsrc_t* rsrc);

	void (*stopSoft)(rampRsrc_t* rsrc);
	void (*stop)(rampRsrc_t* rsrc);

	void (*periodJob)(rampRsrc_t* r, u8 tick);
	void (*testMul)(rampRsrc_t* r, u16 delta);
	void (*test)(rampRsrc_t* r, u16 x);

    u8 (*isSheltered)(rampRsrc_t* r);
	u8 (*getRefl)(rampRsrc_t* r);
	u8 (*getDir)(rampRsrc_t* r);

}rampDev_t;
#pragma pack(pop)           //recover align bytes from 4 bytes

s32 rampSetup(
	void*,
	const char* NAME,
	TIM_HandleTypeDef *htim,
	u32 tCh,
	const PIN_T* DIR,
	const PIN_T* REFL,
	const PIN_T* REFR,
	u16 microStep,
    appTmrDev_t* tmr
);

#endif /* _MY_RAMP_H_ */
