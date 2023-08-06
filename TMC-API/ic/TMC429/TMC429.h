/*
 * TMC429 library definitions
 *
 * This file contains all macro and function definitions of the TMC429 library.
*/

#ifndef TMC_IC_TMC429_H_
#define TMC_IC_TMC429_H_

#include "API_Header.h"
#include "TMC429_Register.h"
#include "misc.h"

#define TMC429_MOTOR_COUNT 3

typedef unsigned char 	UCHAR;
typedef unsigned int 	UINT;

#define DEFAULT_V_MAX 		2047
#define DEFAULT_A_MAX 		800
#define DEFAULT_PULSE_DIV 	2
#define DEFAULT_RAMP_DIV 	6
#define DEFAULT_PMUL 		198
#define DEFAULT_PDIV 		6

typedef struct
{
	s32 X_TARGET;	// RW
	s32 X_ACTUAL;	// RW, Write when HOLD
	u16 V_MIN;		// RW
	u16 V_MAX;		// RW
	s16 V_TARGET;	// RW
	s16 V_ACTUAL;	// R
	u16 A_MAX;		// RW
	s16 A_ACTUAL;	// R
	u32 IS_AGTAT;	// RW
	TMC429_REG_Pmul_Pdiv_t PMul_PDiv;				// RW
	TMC429_REG_RampMod_RefConf_t RampMod_RefConf;	// RW
	TMC429_REG_IntMsk_IntFlg_t IntMsk_IntFlg;		// RW
	TMC429_REG_RampDiv_PulseDiv_t RampDiv_PulseDiv;	// RW
	u16 DX_REF_TOLERANCE;		// RW
	u32 X_LATCHED;				// R
	u8 USTEP_COUNT_429;			// RW
} TMC429_Axis_Reg_T;

typedef struct
{
	u32 DATAGRAM_LOW_WORD;
	u32 DATAGRAM_HIGH_WORD;

	u16 COVER_POS;
	u16 COVER_LEN;

	u16 COVER_DATAGRAM;
	u16 IF_CONFIGURATION_429;
	u32 POS_COMP_429;
	u8 POS_COMP_INT_429;
} TMC429_Common_Reg_T;

typedef struct{
	char name[DEV_NAME_LEN];
	const PIN_T *CS;
	SPI_HandleTypeDef* SPI_HANDLE;

	TMC429_Axis_Reg_T axisReg[TMC429_MOTOR_COUNT];
	TMC429_Common_Reg_T comReg;

	u16 eepromAddrBase;
	s8 (*EepromWrite)  (u16 addr, const u8 *pDat, u16 nBytes);
	s8 (*EepromRead)   (u16 addr, u8 *pDat, u16 nBytes);

	/* callback */
	void (*callBackRefLTurnOn) 	(char *devName);	//left sw turn on
	void (*callBackRefLRelease) (char *devName);	//left sw release
	void (*callBackRefRTurnOn) 	(char *devName);
	void (*callBackRefRRelease) (char *devName);
	void (*callBackAbsPosChanged) (char *devName, u32 pos);
	void (*callBackStatusChanged) (char *devName, u16 status);
	void (*callBackAdcOutChanged) (char *devName, u8 adc);

	u32 tick;
	bool vMaxModified;
} TMC429_RSRC;

typedef struct{
	TMC429_RSRC rsrc;

	void (*Init)(TMC429_RSRC *rsrc);
	u8 (*HardStop)(TMC429_RSRC *rsrc, u8 Motor);
	u8 (*right)(TMC429_RSRC *rsrc, u8 motor, int32_t velocity);
	u8 (*left)(TMC429_RSRC *rsrc, u8 motor, int32_t velocity);
	u8 (*rotate)(TMC429_RSRC *rsrc, u8 motor, s16 velocity);
	u8 (*moveTo)(TMC429_RSRC *rsrc, u8 motor, int32_t position);
	u8 (*moveBy)(TMC429_RSRC *rsrc, u8 motor, int32_t position);
	uint32_t (*stop)(TMC429_RSRC *rsrc);
	u8 (*isTargetPos)(TMC429_RSRC *rsrc, u8 motor);
	u8 (*isOn_RefSw)(TMC429_RSRC *rsrc, u8 motor);

	void (*motionConf)(TMC429_RSRC *rsrc,
		u8 motor,
		u32 vmax,	// max veloctity
		u32 amax, 	// max accelation
		u8 pulsDiv,	//
		u8 rampDiv,
		u8 pmul,
		u8 pdiv
	);
	void (*motionConf_update)(TMC429_RSRC *rsrc,u8 motor);

	void (*save)(TMC429_RSRC *rsrc, u8 motor);

	void (*readRegS)(TMC429_RSRC *rsrc, const char *REG_NAME, int32_t *val);
	void (*writeRegS)(TMC429_RSRC *rsrc, const char *REG_NAME, int32_t val);
	void (*readRegX)(TMC429_RSRC *rsrc,uint8_t address, int32_t *value);
	void (*writeRegX)(TMC429_RSRC *rsrc,uint8_t address, int32_t value);
	uint32_t (*getMeasuredSpeed)(TMC429_RSRC *rsrc,int32_t *value);

	void (*periodicJob)(TMC429_RSRC *rsrc, uint32_t tick);
	void (*deInit)(TMC429_RSRC *rsrc);

	u8 (*ReadReg)(TMC429_RSRC *p, uint8_t address, u32 *value);
	u8 (*WriteReg)(TMC429_RSRC *p, uint8_t address, u32 value);

	u8 (*regW_X_TARGET)(TMC429_RSRC *rsrc, u8 motor, s32 target);
	u8 (*regR_X_TARGET)(TMC429_RSRC *rsrc, u8 motor, s32* target);

	u8 (*regW_X_ACTUAL)(TMC429_RSRC *rsrc, u8 motor, s32 actual);
	u8 (*regR_X_ACTUAL)(TMC429_RSRC *rsrc, u8 motor, s32* actual);

	u8 (*regW_V_MIN)(TMC429_RSRC *rsrc, u8 motor, u16 min);
	u8 (*regR_V_MIN)(TMC429_RSRC *rsrc, u8 motor, u16* min);

	u8 (*regW_V_MAX)(TMC429_RSRC *rsrc, u8 motor, u16 max);
	u8 (*regR_V_MAX)(TMC429_RSRC *rsrc, u8 motor, u16* max);

	u8 (*regW_V_TARGET)(TMC429_RSRC *rsrc, u8 motor, s16 target);
	u8 (*regR_V_TARGET)(TMC429_RSRC *rsrc, u8 motor, s16* target);

	u8 (*regR_V_ACTUAL)(TMC429_RSRC *rsrc, u8 motor, s16* actual);

	u8 (*regW_A_MAX)(TMC429_RSRC *rsrc, u8 motor, u16 max);
	u8 (*regR_A_MAX)(TMC429_RSRC *rsrc, u8 motor, u16* max);

	u8 (*regR_A_ACTUAL)(TMC429_RSRC *rsrc, u8 motor, s16* actual);

	u8 (*regR_CurrentScaling)(TMC429_RSRC *rsrc, u8 motor,
			u8* IS_AGTAT,
			u8* IS_ALEAT,
			u8* IS_V0,
			u16* A_THRESHOLD
		);

	u8 (*regW_CurrentScaling)(TMC429_RSRC *rsrc, u8 motor,
			u8 IS_AGTAT,
			u8 IS_ALEAT,
			u8 IS_V0,
			u16 A_THRESHOLD
		);

	u8 (*regR_PMUL_PDIV)(TMC429_RSRC *rsrc, u8 motor, TMC429_REG_Pmul_Pdiv_t * reg);
	u8 (*regW_PMUL_PDIV)(TMC429_RSRC *rsrc, u8 motor, TMC429_REG_Pmul_Pdiv_t reg);

	u8 (*regR_RampMode_RefConf_lp)(TMC429_RSRC *rsrc, u8 motor, u8* ramp, u8* refConf, u8* lp);
	u8 (*regW_RampMode_RefConf_lp)(TMC429_RSRC *rsrc, u8 motor, u8 ramp, u8 refConf, u8 lp);

	u8 (*regR_INTERRUPT)(TMC429_RSRC *rsrc, u8 motor, u8* mask, u8* flags);
	u8 (*regW_INTERRUPT)(TMC429_RSRC *rsrc, u8 motor, u8 mask, u8 flags);

	u8 (*regR_RampDiv_PulseDiv_Usrs)(TMC429_RSRC *rsrc, u8 motor, TMC429_REG_RampDiv_PulseDiv_t* reg);
	u8 (*regW_RampDiv_PulseDiv_Usrs)(TMC429_RSRC *rsrc, u8 motor, TMC429_REG_RampDiv_PulseDiv_t reg);

	u8 (*regR_DxRefTolerance)(TMC429_RSRC *rsrc, u8 motor, u16* refTolerance);
	u8 (*regW_DxRefTolerance)(TMC429_RSRC *rsrc, u8 motor, u16 refTolerance);

	u8 (*regR_xLatched)(TMC429_RSRC *rsrc, u8 motor, u32* xlatched);
	u8 (*regW_xLatched)(TMC429_RSRC *rsrc, u8 motor, u32 xlatched);

	u8 (*regR_uStepCount)(TMC429_RSRC *rsrc, u8 motor, u8* uStepCount);
	u8 (*regW_uStepCount)(TMC429_RSRC *rsrc, u8 motor, u8 uStepCount);

	u8 (*regR_IFConf)(TMC429_RSRC *rsrc, u16* conf);
	u8 (*regW_IFConf)(TMC429_RSRC *rsrc, u16 conf);

	u8 (*regR_PosComp)(TMC429_RSRC *rsrc, u32* posComp);
	u8 (*regW_PosComp)(TMC429_RSRC *rsrc, u32 posComp);

	u8 (*regR_PosCompInt)(TMC429_RSRC *rsrc, u8* posCompInt, u8* m, u8* i);
	u8 (*regW_PosCompInt)(TMC429_RSRC *rsrc, u8 posCompInt, u8 m, u8 i);

	u8 (*regR_Version)(TMC429_RSRC *rsrc, u32* ver);
	u8 (*regR_refsw)(TMC429_RSRC *rsrc, u8* ref);

	u8 (*regR_SMGP)(TMC429_RSRC *rsrc, u32* conf);
	u8 (*regW_SMGP)(TMC429_RSRC *rsrc, u32 conf);
} TMC429_DEV;

// setup a TMC429 device
void setupDev_tmc429(
	TMC429_DEV *dev,
	const char* NAME,
	SPI_HandleTypeDef* SPI_HANDLE,	// spi handle
	const PIN_T *CS,				// spi select
	u16 romBase,
	s8 (*EepromRead)   (u16 addr, u8 *pDat, u16 nBytes),
	s8 (*EepromWrite)  (u16 addr, const u8 *pDat, u16 nBytes)
);
extern u8 spdTestEn;
extern u32 tickStart;
#endif /* TMC_IC_TMC429_H_ */
