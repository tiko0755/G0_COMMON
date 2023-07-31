/**********************************************************
filename: boardFeeder.h
**********************************************************/
#ifndef _BOARD_FDR_H
#define _BOARD_FDR_H

#include "misc.h"
#include "cspin.h"

/*****************************************************************************
 @ typedefs
****************************************************************************/
#define FDR_IN_CNT	4
#define FDR_OUT_CNT	2

// out define
#define FDR_OUT_SEND	0	//[1->0]:send completed
#define FDR_OUT_	1	// label sense event
#define FDR_OUT2	2	// TBD
#define FDR_OUT3	3	// Loop test

#define FER_IN_SEND 1		//[0->1]start sending
#define FER_IN_TAG_SNS	0	// [0->1]lable sense
#define FDR_IN1	1	// recycle 
#define FDR_IN2	2	// recycle
#define FDR_IN3	3	// strip toggle
#define FDR_IN4	4	// TBD

typedef struct{
	// motion
	__IO u8 motionMod;	// 0: run mode, 1: position
	__IO u32 spd;		// speed, in step/s
	__IO u32 pos;		// position, in step
	__IO u8 isHoldStop;	// 1: stop with hold
	__IO u8 motionSta;	// 0: stop, 1: rotate+, 2:rotate-
	// speed
	__IO u32 acc;		// accelate, in step/(s*s)
	__IO u32 dec;		// accelate, in step/(s*s)
	__IO u32 maxSpd;		// max speed, in step/s
	__IO u16 minSpd;		// min speed, in step/s
	__IO u16 fullSpdTh;	// speed throd changed into full step
	__IO u8 microStepMode;	// 0: full, 1: 1/2, 7: 1/128
	// phase current
	__IO u8 kvalAcc,kvalDec,kValRun,kValHold;
	__IO u16 intSpd;	//insertect speed
	__IO u8 stSlop;		// start slop
	__IO u8 accSlop;	// acc slop
	__IO u8 decSlop;	// dec slop
	__IO u8 isVsComp, isTempComp;
}UI_motionInfo_t;

typedef struct{
	u32 constSpd;	// in micro-step/second
	u32 position;	// if position mode, position in microstep
	u8 runMod;		// 0> constant speed, 1> position mode
					// 2> rotateL under constant speed;	3> rotateL under position mode
					// 4> rotateR under constant speed;	5> rotateR under position mode
	u8 stopMod;		// 0: without hold stop, 1: with hold stop
} fdrMotion_t;

typedef struct{
	char name[DEV_NAME_LEN];
	// gpio
	const PIN_T* IN;
	const PIN_T* OUT;
	u16 tick;
	u16 tmr;
	u8 squ;
	u8 stopLock;
	u32 errorCode;	
	
	u8 squStack;
	
	cSPIN_DEV_T* stprR;	// recycle
	cSPIN_DEV_T* stprX;	// send
	
	// stepper local setting
	fdrMotion_t stprCfg[2];	// 0: send, 1: stripp, 2:recycle
	
//	u32 constSpdR,constSpdX,constSpdY;		// in micro-step/second
//	u32 positionR, positionX, positionY;	// if position mode, position in microstep
//	u8 runModR, runModX, runModY;			// 0> constant speed, 1> position mode
//											// 2> rotateL under constant speed;	3> rotateL under position mode
//											// 4> rotateR under constant speed;	5> rotateR under position mode
//	u8 stopModR, stopModX, stopModY;		// 0: without hold stop, 1: with hold stop

	// setting
	u32 counter;
	s32 songliaobuchang;	// in 0.1mm
	u32 songliaoxianwei;	// in mm
	u32 songliaoshudu;		// in mm/s
	u32 shouliaoshudu;		// in mm/s
	u32 boliaochidu;		// in mm
	u32 psw;	// password
	u8 mode;	// 0: auto, 1: mannual
	u8 lock;	// 0: unlocked, 1: locked
	
	u8 dbgEn;

	u8 tagSnsSeries;
	u8 inSeries[FDR_IN_CNT];
	
	u32 event;	// event

	// ui
	UI_motionInfo_t rInfo, xInfo, yInfo;
	
	s8 (*RegRead)   (u16 addr, u8 *pDat, u16 nBytes);
	s8 (*RegWrite)  (u16 addr, const u8 *pDat, u16 nBytes);
	u16 romBase;
	void (*print)	(const char* FORMAT_ORG, ...);

	u8 threadID;	//[0] main, [1]sending [2]
	// thread send
	u8 tSendSqu;
	u16 tSendTick, tSendTmr;

	//cb
	void (*CbInputFalling)	(u8 indx);
	void (*CbInputRaising)	(u8 indx);
	
}BrdFdrRsrc_T;

typedef struct{
	BrdFdrRsrc_T rsrc;
	//basic
	s32 (*Polling)(BrdFdrRsrc_T*, u16 tick);
	void (*Stop)(BrdFdrRsrc_T*);
	void (*StopRecover)(BrdFdrRsrc_T*);
	void (*writePin)(BrdFdrRsrc_T*, u8 indx, u8 sta);
	void (*togglePin)(BrdFdrRsrc_T*, u8 indx);
	GPIO_PinState (*readPin)(BrdFdrRsrc_T*, u8 indx);
	void (*allStop)(BrdFdrRsrc_T*);
	s32 (*startThreadSend)(BrdFdrRsrc_T*);

	void (*TjcUIReset)	(BrdFdrRsrc_T* p);
	void (*TjcEepromWriteAll)(BrdFdrRsrc_T* pRsrc);
	s8 (*TjcEepromReadAll)(BrdFdrRsrc_T* pRsrc);
	void (*TjcCounterInc)(BrdFdrRsrc_T* pRsrc);
	void (*updateStatus)(BrdFdrRsrc_T* p, const char* INFO, u8 tmr);
//	s8 UI_tagFlash(BrdFdrRsrc_T* pRsrc, u8 en)
//	s8 UI_sendingFlash(BrdFdrRsrc_T* pRsrc, u8 en)
//	s8 UI_strippFlash(BrdFdrRsrc_T* pRsrc, u8 en)
//	s8 UI_eStopFlash(BrdFdrRsrc_T* pRsrc, u8 en)



}BrdFdrDev_T;

void BrdFdrSetup(
	void *pDev,
	const char* NAME,
	cSPIN_DEV_T* stprs,		// for Y axis
	const PIN_T* pinIn,
	const PIN_T* pinOut,
	void (*printLCD)(const char* FORMAT_ORG, ...),
	s8 (*EepromRead)   (u16 addr, u8 *pDat, u16 nBytes),
	s8 (*EepromWrite)  (u16 addr, const u8 *pDat, u16 nBytes),
	u16 romBase
);

u32 boliaochiduToStep(u16 len);
u32 songliaoxianweiToStep(u16 len);
u32 songliaobuchangToStep(u16 len);
#endif
