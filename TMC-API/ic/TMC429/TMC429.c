/*
	This file provides all functions needed for easy
	access to the TMC429 motion control IC.

	Please not that functions for communication over SPI must be added by the user,
	because this is specific to the MCU that is to be used.

	The ReadWriteSPI function with the following parameters and functionality:
	First parameter: indentifies the SPI device
	Second parameter: byte to be sent to the SPI device
	Third parameter: FALSE means that more bytes will follow, so do not relase the
	  chip select line. TRUE means that this was the last byte, so release the chip
	  select line after this byte has been sent and the answer has been fully received.

	The function shall return the byte that has been received via SPI.
*/

#include "TMC429.h"
#include "string.h"
#include "board.h"

static uint32_t tmc429_getMeasuredSpeed(TMC429_RSRC *rsrc,int32_t *value);
static void tmc429_periodicJob(TMC429_RSRC *rsrc, uint32_t tick);

static void tmc429_deInit(TMC429_RSRC *rsrc);

	// TMC429 library functions
static void Set429SwitchMode(TMC429_RSRC* p, uint8_t Axis, uint8_t SwitchMode);
static uint8_t SetAMax(TMC429_RSRC* p, uint8_t Motor, uint32_t AMax);

static void Init429(TMC429_RSRC* p);
static u8 tmc429_hardStop(TMC429_RSRC *rsrc, u8 Motor);
//static u8 tmc429_rotate(TMC429_RSRC *rsrc,int32_t velocity);
static u8 tmc429_left(TMC429_RSRC *rsrc, u8 motor, int32_t velocity);
static u8 tmc429_right(TMC429_RSRC *rsrc, u8 motor, int32_t velocity);
static u8 tmc429_moveto(TMC429_RSRC *rsrc, u8 motor, int32_t position);
static u8 tmc429_moveby(TMC429_RSRC *rsrc, u8 motor, int32_t position);
static u8 tmc429_rotate(TMC429_RSRC *rsrc, u8 motor, s16 velocity);
static u8 tmc429_isTargetPos(TMC429_RSRC *rsrc, u8 motor);
static u8 tmc429_set429RampMode(TMC429_RSRC *rsrc, u8 motor, u8 rampMode);

static void tmc429_motionConf(TMC429_RSRC *rsrc,
	u8 motor,
	u32 vmax,	// max veloctity
	u32 amax, 	// max accelation
	u8 pulsDiv,	//
	u8 rampDiv,
	u8 pmul,
	u8 pdiv
);
static void tmc429_motionConf_update(TMC429_RSRC *rsrc,u8 motor);

static u8 tmc429_regW_X_TARGET(TMC429_RSRC *rsrc, u8 motor, s32 target);
static u8 tmc429_regR_X_TARGET(TMC429_RSRC *rsrc, u8 motor, s32* target);
static u8 tmc429_regW_X_ACTUAL(TMC429_RSRC *rsrc, u8 motor, s32 actual);
static u8 tmc429_regR_X_ACTUAL(TMC429_RSRC *rsrc, u8 motor, s32* actual);
static u8 tmc429_regW_V_MIN(TMC429_RSRC *rsrc, u8 motor, u16 min);
static u8 tmc429_regR_V_MIN(TMC429_RSRC *rsrc, u8 motor, u16* min);
static u8 tmc429_regW_V_MAX(TMC429_RSRC *rsrc, u8 motor, u16 max);
static u8 tmc429_regR_V_MAX(TMC429_RSRC *rsrc, u8 motor, u16* max);
static u8 tmc429_regW_V_TARGET(TMC429_RSRC *rsrc, u8 motor, s16 target);
static u8 tmc429_regR_V_TARGET(TMC429_RSRC *rsrc, u8 motor, s16* target);
static u8 tmc429_regR_V_ACTUAL(TMC429_RSRC *rsrc, u8 motor, s16* actual);
static u8 tmc429_regW_A_MAX(TMC429_RSRC *rsrc, u8 motor, u16 max);
static u8 tmc429_regR_A_MAX(TMC429_RSRC *rsrc, u8 motor, u16* max);
static u8 tmc429_regR_A_ACTUAL(TMC429_RSRC *rsrc, u8 motor, s16* actual);

static u8 tmc429_regR_CurrentScaling(TMC429_RSRC *rsrc, u8 motor,
		u8* IS_AGTAT,
		u8* IS_ALEAT,
		u8* IS_V0,
		u16* A_THRESHOLD
	);

static u8 tmc429_regW_CurrentScaling(TMC429_RSRC *rsrc, u8 motor,
		u8 IS_AGTAT,
		u8 IS_ALEAT,
		u8 IS_V0,
		u16 A_THRESHOLD
	);

static u8 tmc429_regR_PMUL_PDIV(TMC429_RSRC *rsrc, u8 motor, TMC429_REG_Pmul_Pdiv_t * reg);
static u8 tmc429_regW_PMUL_PDIV(TMC429_RSRC *rsrc, u8 motor, TMC429_REG_Pmul_Pdiv_t reg);

static u8 tmc429_regR_RampMode_RefConf_lp(TMC429_RSRC *rsrc, u8 motor, u8* ramp, u8* refConf, u8* lp);
static u8 tmc429_regW_RampMode_RefConf_lp(TMC429_RSRC *rsrc, u8 motor, u8 ramp, u8 refConf, u8 lp);

static u8 tmc429_regR_INTERRUPT(TMC429_RSRC *rsrc, u8 motor, u8* mask, u8* flags);
static u8 tmc429_regW_INTERRUPT(TMC429_RSRC *rsrc, u8 motor, u8 mask, u8 flags);

static u8 tmc429_regR_RampDiv_PulseDiv_Usrs(TMC429_RSRC *rsrc, u8 motor, TMC429_REG_RampDiv_PulseDiv_t * reg);
static u8 tmc429_regW_RampDiv_PulseDiv_Usrs(TMC429_RSRC *rsrc, u8 motor, TMC429_REG_RampDiv_PulseDiv_t reg);

static u8 tmc429_regR_DxRefTolerance(TMC429_RSRC *rsrc, u8 motor, u16* refTolerance);
static u8 tmc429_regW_DxRefTolerance(TMC429_RSRC *rsrc, u8 motor, u16 refTolerance);

static u8 tmc429_regR_xLatched(TMC429_RSRC *rsrc, u8 motor, u32* xlatched);
static u8 tmc429_regW_xLatched(TMC429_RSRC *rsrc, u8 motor, u32 xlatched);

static u8 tmc429_regR_uStepCount(TMC429_RSRC *rsrc, u8 motor, u8* uStepCount);
static u8 tmc429_regW_uStepCount(TMC429_RSRC *rsrc, u8 motor, u8 uStepCount);

static u8 tmc429_regR_IFConf(TMC429_RSRC *rsrc, u16* conf);
static u8 tmc429_regW_IFConf(TMC429_RSRC *rsrc, u16 conf);

static u8 tmc429_regR_PosComp(TMC429_RSRC *rsrc, u32* posComp);
static u8 tmc429_regW_PosComp(TMC429_RSRC *rsrc, u32 posComp);

static u8 tmc429_regR_PosCompInt(TMC429_RSRC *rsrc, u8* posCompInt, u8* m, u8* i);
static u8 tmc429_regW_PosCompInt(TMC429_RSRC *rsrc, u8 posCompInt, u8 m, u8 i);

static u8 tmc429_regR_Version(TMC429_RSRC *rsrc, u32 *ver);
static u8 tmc429_regR_refsw(TMC429_RSRC *rsrc, u8 *ref);

static u8 tmc429_regR_SMGP(TMC429_RSRC *rsrc, u32* conf);
static u8 tmc429_regW_SMGP(TMC429_RSRC *rsrc, u32 conf);

// new world
static void ReadWrite429(TMC429_RSRC* p, uint8_t *Read, uint8_t *Write);
static u8 Write429Zero(TMC429_RSRC* p, uint8_t Address);

static u8 tmc429_readRegister(TMC429_RSRC *rsrc, uint8_t address, u32 *value);
static u8 tmc429_writeRegister(TMC429_RSRC *rsrc, uint8_t address, u32 value);

static s32 tmc429_loadFromROM(TMC429_RSRC *rsrc, u8 motor);
static void tmc429_saveToROM(TMC429_RSRC *rsrc, u8 motor);

void setupDev_tmc429(
	TMC429_DEV *dev,
	const char* NAME,
	SPI_HandleTypeDef* SPI_HANDLE,	// spi handle
	const PIN_T *CS,				// spi select
	u16 romBase,
	s8 (*EepromRead)   (u16 addr, u8 *pDat, u16 nBytes),
	s8 (*EepromWrite)  (u16 addr, const u8 *pDat, u16 nBytes)
){
	s32 i;
	memset(dev,0,sizeof(TMC429_DEV));
	TMC429_RSRC *rsrc = &dev->rsrc;

	rsrc->SPI_HANDLE = SPI_HANDLE;
	rsrc->CS = CS;
	rsrc->EepromRead = EepromRead;
	rsrc->EepromWrite = EepromWrite;
	rsrc->eepromAddrBase = romBase;

	strcpy(rsrc->name, NAME);

	dev->HardStop = tmc429_hardStop;
	dev->right = tmc429_right;
	dev->left = tmc429_left;
	dev->moveTo = tmc429_moveto;
	dev->moveBy = tmc429_moveby;
	dev->rotate = tmc429_rotate;
	dev->motionConf = tmc429_motionConf;
	dev->motionConf_update = tmc429_motionConf_update;
	dev->isTargetPos = tmc429_isTargetPos;

	dev->getMeasuredSpeed = tmc429_getMeasuredSpeed;
	dev->periodicJob = tmc429_periodicJob;
	dev->Init = Init429;
	dev->deInit = tmc429_deInit;
	dev->save = tmc429_saveToROM;

	dev->regW_X_TARGET = tmc429_regW_X_TARGET;
	dev->regR_X_TARGET = tmc429_regR_X_TARGET;
	dev->regW_X_ACTUAL = tmc429_regW_X_ACTUAL;
	dev->regR_X_ACTUAL = tmc429_regR_X_ACTUAL;
	dev->regW_V_MIN = tmc429_regW_V_MIN;
	dev->regR_V_MIN = tmc429_regR_V_MIN;
	dev->regW_V_MAX = tmc429_regW_V_MAX;
	dev->regR_V_MAX = tmc429_regR_V_MAX;
	dev->regW_V_TARGET = tmc429_regW_V_TARGET;
	dev->regR_V_TARGET = tmc429_regR_V_TARGET;
	dev->regR_V_ACTUAL = tmc429_regR_V_ACTUAL;
	dev->regW_A_MAX = tmc429_regW_A_MAX;
	dev->regR_A_MAX = tmc429_regR_A_MAX;
	dev->regR_A_ACTUAL = tmc429_regR_A_ACTUAL;
	dev->regR_CurrentScaling = tmc429_regR_CurrentScaling;
	dev->regW_CurrentScaling = tmc429_regW_CurrentScaling;
	dev->regR_PMUL_PDIV = tmc429_regR_PMUL_PDIV;
	dev->regW_PMUL_PDIV = tmc429_regW_PMUL_PDIV;
	dev->regR_RampMode_RefConf_lp = tmc429_regR_RampMode_RefConf_lp;
	dev->regW_RampMode_RefConf_lp = tmc429_regW_RampMode_RefConf_lp;
	dev->regR_INTERRUPT = tmc429_regR_INTERRUPT;
	dev->regW_INTERRUPT = tmc429_regW_INTERRUPT;
	dev->regR_RampDiv_PulseDiv_Usrs = tmc429_regR_RampDiv_PulseDiv_Usrs;
	dev->regW_RampDiv_PulseDiv_Usrs = tmc429_regW_RampDiv_PulseDiv_Usrs;
	dev->regR_DxRefTolerance = tmc429_regR_DxRefTolerance;
	dev->regW_DxRefTolerance = tmc429_regW_DxRefTolerance;
	dev->regR_xLatched = tmc429_regR_xLatched;
	dev->regW_xLatched = tmc429_regW_xLatched;
	dev->regR_uStepCount = tmc429_regR_uStepCount;
	dev->regW_uStepCount = tmc429_regW_uStepCount;
	dev->regR_IFConf = tmc429_regR_IFConf;
	dev->regW_IFConf = tmc429_regW_IFConf;
	dev->regR_PosComp = tmc429_regR_PosComp;
	dev->regW_PosComp = tmc429_regW_PosComp;
	dev->regR_PosCompInt = tmc429_regR_PosCompInt;
	dev->regW_PosCompInt = tmc429_regW_PosCompInt;
	dev->regR_Version = tmc429_regR_Version;
	dev->regR_refsw = tmc429_regR_refsw;
	dev->regR_SMGP = tmc429_regR_SMGP;
	dev->regW_SMGP = tmc429_regW_SMGP;

	dev->ReadReg = tmc429_readRegister;
	dev->WriteReg = tmc429_writeRegister;

	//
	for(i=0;i<TMC429_MOTOR_COUNT;i++){
		memset(&rsrc->axisReg[i], 0, sizeof(TMC429_Axis_Reg_T));
		if(tmc429_loadFromROM(rsrc,i) < 0){
			rsrc->axisReg[i].V_MIN = 1;
			rsrc->axisReg[i].V_MAX = DEFAULT_V_MAX;
			rsrc->axisReg[i].A_MAX = DEFAULT_A_MAX;
			rsrc->axisReg[i].RampDiv_PulseDiv.PULSE_DIV = DEFAULT_PULSE_DIV;
			rsrc->axisReg[i].RampDiv_PulseDiv.RAMP_DIV = DEFAULT_RAMP_DIV;
			rsrc->axisReg[i].PMul_PDiv.PMUL = DEFAULT_PMUL;
			rsrc->axisReg[i].PMul_PDiv.PDIV = DEFAULT_PDIV;
			tmc429_saveToROM(rsrc, i);
		}
	}
	Init429(rsrc);
}

static void tmc429_motionConf(TMC429_RSRC *rsrc,
	u8 motor,
	u32 vmax,	// max veloctity
	u32 amax, 	// max accelation
	u8 pulsDiv,	//
	u8 rampDiv,
	u8 pmul,
	u8 pdiv
){
	rsrc->axisReg[motor].V_MAX = vmax;
	tmc429_writeRegister(rsrc, TMC429_IDX_VMAX(motor), vmax);

	rsrc->axisReg[motor].A_MAX = amax;
	tmc429_writeRegister(rsrc, TMC429_IDX_AMAX(motor), amax);

	rsrc->axisReg[motor].RampDiv_PulseDiv.PULSE_DIV = pulsDiv;
	rsrc->axisReg[motor].RampDiv_PulseDiv.RAMP_DIV = rampDiv;
	tmc429_regW_RampDiv_PulseDiv_Usrs(rsrc, motor, rsrc->axisReg[motor].RampDiv_PulseDiv);

	rsrc->axisReg[motor].PMul_PDiv.PMUL = pmul;
	rsrc->axisReg[motor].PMul_PDiv.PDIV = pdiv;
	tmc429_writeRegister(rsrc, TMC429_IDX_PMUL_PDIV(motor), rsrc->axisReg[motor].PMul_PDiv.value);
}

static void tmc429_motionConf_update(TMC429_RSRC *rsrc, u8 motor){
	tmc429_regR_V_MAX(rsrc, motor, &rsrc->axisReg[motor].V_MAX);
	tmc429_regR_A_MAX(rsrc, motor, &rsrc->axisReg[motor].A_MAX);
	tmc429_regR_RampDiv_PulseDiv_Usrs(rsrc, motor, &rsrc->axisReg[motor].RampDiv_PulseDiv);
	tmc429_regR_PMUL_PDIV(rsrc, motor, &rsrc->axisReg[motor].PMul_PDiv);
}

static u8 tmc429_regW_X_TARGET(TMC429_RSRC *rsrc, u8 motor, s32 target){
	if(motor >= TMC429_MOTOR_COUNT)	return 0;
	rsrc->axisReg[motor].X_TARGET = target;
	return(tmc429_writeRegister(rsrc, TMC429_IDX_XTARGET(motor), (u32)target));
}

static u8 tmc429_regR_X_TARGET(TMC429_RSRC *rsrc, u8 motor, s32* target){
	u32 x;
	u8 stat;
	stat = tmc429_readRegister(rsrc, TMC429_IDX_XTARGET(motor), &x);
	// 24 bit register and sign-extends the register value to 32 bit.
	if(x & 0x00800000) *target = (s32)(x|0xff000000);
	else *target = x;
	return(stat);
}

static u8 tmc429_regW_X_ACTUAL(TMC429_RSRC *rsrc, u8 motor, s32 actual){
	return(tmc429_writeRegister(rsrc, TMC429_IDX_XACTUAL(motor), (u32)actual));
}
static u8 tmc429_regR_X_ACTUAL(TMC429_RSRC *rsrc, u8 motor, s32* actual){
	u32 x;
	u8 stat;
	stat = tmc429_readRegister(rsrc, TMC429_IDX_XACTUAL(motor), &x);
	// 24 bit register and sign-extends the register value to 32 bit.
	if(x & 0x00800000) *actual = (s32)(x|0xff000000);
	else *actual = x;
	return(stat);
}

static u8 tmc429_regW_V_MIN(TMC429_RSRC *rsrc, u8 motor, u16 min){
	if(motor >= TMC429_MOTOR_COUNT)	return 0;
	rsrc->axisReg[motor].V_MIN = min;
	return(tmc429_writeRegister(rsrc, TMC429_IDX_VMIN(motor), (u32)min));
}
static u8 tmc429_regR_V_MIN(TMC429_RSRC *rsrc, u8 motor, u16* min){
	u32 x;
	u8 stat;
	stat = tmc429_readRegister(rsrc, TMC429_IDX_VMIN(motor), &x);
	*min = x & 0x000007ff;
	return(stat);
}
static u8 tmc429_regW_V_MAX(TMC429_RSRC *rsrc, u8 motor, u16 max){
	if(motor >= TMC429_MOTOR_COUNT)	return 0;
	rsrc->axisReg[motor].V_MAX = max;
	return(tmc429_writeRegister(rsrc, TMC429_IDX_VMAX(motor), (u32)max));
}
static u8 tmc429_regR_V_MAX(TMC429_RSRC *rsrc, u8 motor, u16* max){
	u32 x;
	u8 stat;
	stat = tmc429_readRegister(rsrc, TMC429_IDX_VMAX(motor), &x);
	*max = x & 0x000007ff;
	return(stat);
}
static u8 tmc429_regW_V_TARGET(TMC429_RSRC *rsrc, u8 motor, s16 target){
	return(tmc429_writeRegister(rsrc, TMC429_IDX_VTARGET(motor), (u32)target));
}
static u8 tmc429_regR_V_TARGET(TMC429_RSRC *rsrc, u8 motor, s16* target){
	u32 x;
	u8 stat;
	stat = tmc429_readRegister(rsrc, TMC429_IDX_VTARGET(motor), &x);
	// 12 bit register and sign-extends the register value to 32 bit.
	if(x & 0x00000800) *target = (s16)(x|0xfffff000);
	else *target = x;
	return(stat);
}

static u8 tmc429_regR_V_ACTUAL(TMC429_RSRC *rsrc, u8 motor, s16* actual){
	u32 x;
	u8 stat;
	stat = tmc429_readRegister(rsrc, TMC429_IDX_VACTUAL(motor), &x);
	// 12 bit register and sign-extends the register value to 32 bit.
	if(x & 0x00000800) *actual = (s16)(x|0xffffff000);
	else *actual = x;
	return(stat);
}

static u8 tmc429_regW_A_MAX(TMC429_RSRC *rsrc, u8 motor, u16 max){
	if(motor >= TMC429_MOTOR_COUNT)	return 0;
	rsrc->axisReg[motor].A_MAX = max;
	return(tmc429_writeRegister(rsrc, TMC429_IDX_AMAX(motor), max));
}
static u8 tmc429_regR_A_MAX(TMC429_RSRC *rsrc, u8 motor, u16* max){
	u32 x;
	u8 stat;
	stat = tmc429_readRegister(rsrc, TMC429_IDX_AMAX(motor), &x);
	*max = x&0x000007ff;
	return(stat);
}

static u8 tmc429_regR_A_ACTUAL(TMC429_RSRC *rsrc, u8 motor, s16* actual){
	u32 x;
	u8 stat;
	stat = tmc429_readRegister(rsrc, TMC429_IDX_AACTUAL(motor), &x);
	if(x & 0x00000800) *actual = (s16)(x|0xfffff000);
	else *actual = x;
	return(stat);
}

static u8 tmc429_regR_CurrentScaling(TMC429_RSRC *rsrc, u8 motor,
		u8* IS_AGTAT,
		u8* IS_ALEAT,
		u8* IS_V0,
		u16* A_THRESHOLD
){
	u32 x;
	u8 stat;
	stat = tmc429_readRegister(rsrc, TMC429_IDX_AGTAT_ALEAT(motor), &x);
	*IS_AGTAT = (x>>20)&0x07;
	*IS_ALEAT = (x>>16)&0x07;
	*IS_V0 = (x>>12)&0x07;
	*A_THRESHOLD = x&0x000007ff;
	return(stat);
}

static u8 tmc429_regW_CurrentScaling(TMC429_RSRC *rsrc, u8 motor,
		u8 IS_AGTAT,
		u8 IS_ALEAT,
		u8 IS_V0,
		u16 A_THRESHOLD
){
	u32 x = 0;
	u8 stat;
	x = (IS_AGTAT<<20)|(IS_ALEAT<<16)|(IS_V0<<12)|A_THRESHOLD;
	stat = tmc429_writeRegister(rsrc, TMC429_IDX_AGTAT_ALEAT(motor), x);
	return(stat);
}

static u8 tmc429_regR_PMUL_PDIV(TMC429_RSRC *rsrc, u8 motor, TMC429_REG_Pmul_Pdiv_t * reg){
	u32 x;
	u8 stat;
	stat = tmc429_readRegister(rsrc, TMC429_IDX_PMUL_PDIV(motor), &x);
	reg->PMUL = (x>>8)&0xff;
	reg->PDIV = x&0x0f;
	return(stat);
}
static u8 tmc429_regW_PMUL_PDIV(TMC429_RSRC *rsrc, u8 motor, TMC429_REG_Pmul_Pdiv_t reg){
	u32 x;
	u8 stat;
	x = reg.value;
	stat = tmc429_writeRegister(rsrc, TMC429_IDX_PMUL_PDIV(motor), x);
	return(stat);
}

static u8 tmc429_regR_RampMode_RefConf_lp(TMC429_RSRC *rsrc, u8 motor, u8* ramp, u8* refConf, u8* lp){
	u32 x;
	u8 stat;
	stat = tmc429_readRegister(rsrc, TMC429_IDX_REFCONF_RM(motor), &x);
	*ramp = x&0x03;
	*refConf = (x>>8)&0x0f;
	*lp = (x>>16)&0x01;
	return(stat);
}

static u8 tmc429_regW_RampMode_RefConf_lp(TMC429_RSRC *rsrc, u8 motor, u8 ramp, u8 refConf, u8 lp){
	u32 x;
	u8 stat;
	x = ((lp&0x01)<<16) | ((refConf&0x0f)<<8) | (0x03&ramp);
	stat = tmc429_writeRegister(rsrc, TMC429_IDX_REFCONF_RM(motor), x);
	return(stat);
}

static u8 tmc429_regR_INTERRUPT(TMC429_RSRC *rsrc, u8 motor, u8* mask, u8* flags){
	u32 x;
	u8 stat;
	stat = tmc429_readRegister(rsrc, TMC429_IDX_IMASK_IFLAGS(motor), &x);
	*mask = (x>>8)&0xff;
	*flags = x&0x0ff;
	return(stat);
}
static u8 tmc429_regW_INTERRUPT(TMC429_RSRC *rsrc, u8 motor, u8 mask, u8 flags){
	u32 x;
	u8 stat;
	x = (mask<<8)|flags;
	stat = tmc429_writeRegister(rsrc, TMC429_IDX_IMASK_IFLAGS(motor), x);
	return(stat);
}

static u8 tmc429_regR_RampDiv_PulseDiv_Usrs(TMC429_RSRC *rsrc, u8 motor, TMC429_REG_RampDiv_PulseDiv_t * reg){
	u32 x;
	u8 stat;
	stat = tmc429_readRegister(rsrc, TMC429_IDX_PULSEDIV_RAMPDIV(motor), &x);
	reg->RAMP_DIV = (x>>8)&0x0f;
	reg->PULSE_DIV = (x>>12)&0x0f;
	reg->USRS = (x)&0x03;
	return(stat);
}
static u8 tmc429_regW_RampDiv_PulseDiv_Usrs(TMC429_RSRC *rsrc, u8 motor, TMC429_REG_RampDiv_PulseDiv_t reg){
	u32 x;
	u8 stat;
	x = reg.value;
	stat = tmc429_writeRegister(rsrc, TMC429_IDX_PULSEDIV_RAMPDIV(motor), x);
	return(stat);
}

static u8 tmc429_regR_DxRefTolerance(TMC429_RSRC *rsrc, u8 motor, u16* refTolerance){
	u32 x;
	u8 stat;
	stat = tmc429_readRegister(rsrc, TMC429_IDX_DX_REFTOLERANCE(motor), &x);
	*refTolerance = (x>>0)&0x07ff;
	return(stat);
}
static u8 tmc429_regW_DxRefTolerance(TMC429_RSRC *rsrc, u8 motor, u16 refTolerance){
	u32 x;
	u8 stat;
	x = refTolerance&0x07ff;
	stat = tmc429_writeRegister(rsrc, TMC429_IDX_DX_REFTOLERANCE(motor), x);
	return(stat);
}

static u8 tmc429_regR_xLatched(TMC429_RSRC *rsrc, u8 motor, u32* xlatched){
	u32 x;
	u8 stat;
	stat = tmc429_readRegister(rsrc, TMC429_IDX_XLATCHED(motor), &x);
	*xlatched = x&0x00ffffff;
	return(stat);
}
static u8 tmc429_regW_xLatched(TMC429_RSRC *rsrc, u8 motor, u32 xlatched){
	u32 x;
	u8 stat;
	x = xlatched&0x00ffffff;
	stat = tmc429_writeRegister(rsrc, TMC429_IDX_XLATCHED(motor), x);
	return(stat);
}

static u8 tmc429_regR_uStepCount(TMC429_RSRC *rsrc, u8 motor, u8* uStepCount){
	u32 x;
	u8 stat;
	stat = tmc429_readRegister(rsrc, TMC429_IDX_USTEP_COUNT_429(motor), &x);
	*uStepCount = x&0x000000ff;
	return(stat);
}
static u8 tmc429_regW_uStepCount(TMC429_RSRC *rsrc, u8 motor, u8 uStepCount){
	u32 x;
	u8 stat;
	x = uStepCount;
	stat = tmc429_writeRegister(rsrc, TMC429_IDX_USTEP_COUNT_429(motor), x);
	return(stat);
}

static u8 tmc429_regR_IFConf(TMC429_RSRC *rsrc, u16* conf){
	u32 x;
	u8 stat;
	stat = tmc429_readRegister(rsrc, TMC429_IDX_IF_CONFIG_429, &x);
	*conf = x&0x000001ff;
	return(stat);
}
static u8 tmc429_regW_IFConf(TMC429_RSRC *rsrc, u16 conf){
	u32 x;
	u8 stat;
	x = conf&0x000001ff;
	stat = tmc429_writeRegister(rsrc, TMC429_IDX_IF_CONFIG_429, x);
	return(stat);
}

static u8 tmc429_regR_PosComp(TMC429_RSRC *rsrc, u32* posComp){
	u32 x;
	u8 stat;
	stat = tmc429_readRegister(rsrc, TMC429_IDX_POS_COMP_429, &x);
	*posComp = x;
	return(stat);
}
static u8 tmc429_regW_PosComp(TMC429_RSRC *rsrc, u32 posComp){
	u32 x;
	u8 stat;
	x = posComp;
	stat = tmc429_writeRegister(rsrc, TMC429_IDX_POS_COMP_429, x);
	return(stat);
}

static u8 tmc429_regR_PosCompInt(TMC429_RSRC *rsrc, u8* posCompInt, u8* m, u8* i){
	u32 x;
	u8 stat;
	stat = tmc429_readRegister(rsrc, TMC429_IDX_POS_COMP_INT_429, &x);
	*posCompInt = (x>>16)&0xff;
	*m = (x>>8)&0x01;
	*i = (x>>0)&0x01;
	return(stat);
}
static u8 tmc429_regW_PosCompInt(TMC429_RSRC *rsrc, u8 posCompInt, u8 m, u8 i){
	u32 x;
	u8 stat;
	x = (posCompInt<<16)|(m<<8)|i;
	stat = tmc429_writeRegister(rsrc, TMC429_IDX_POS_COMP_INT_429, x);
	return(stat);
}

static u8 tmc429_regR_Version(TMC429_RSRC *rsrc, u32 *ver){
	u32 x;
	u8 stat;
	stat = tmc429_readRegister(rsrc, TMC429_IDX_TYPE_VERSION_429, &x);
	*ver = x;
	return(stat);
}

static u8 tmc429_regR_refsw(TMC429_RSRC *rsrc, u8 *ref){
	u32 x;
	u8 stat;
	stat = tmc429_readRegister(rsrc, TMC429_IDX_REF_SWITCHES, &x);
	*ref = x&0x3f;
	return(stat);
}

static u8 tmc429_regR_SMGP(TMC429_RSRC *rsrc, u32* conf){
	u32 x;
	u8 stat;
	stat = tmc429_readRegister(rsrc, TMC429_IDX_SMGP, &x);
	*conf = x;
	return(stat);
}

static u8 tmc429_regW_SMGP(TMC429_RSRC *rsrc, u32 conf){
	u32 x;
	u8 stat;
	x = conf;
	stat = tmc429_writeRegister(rsrc, TMC429_IDX_SMGP, x);
	return(stat);
}

static u8 tmc429_set429RampMode(TMC429_RSRC *rsrc, u8 motor, u8 rampMode){
	u8 ramp, refConf, lp;
	tmc429_regR_RampMode_RefConf_lp(rsrc, motor, &ramp, &refConf, &lp);
	ramp = rampMode;
	return(tmc429_regW_RampMode_RefConf_lp(rsrc, motor, ramp, refConf, lp));
}

static u8 tmc429_rotate(TMC429_RSRC *rsrc, u8 motor, s16 velocity){
	s16 vel = 0;

	if(velocity >= 0 ){
		if(velocity > 2047) vel = 2047;
		else{
			vel = velocity;
		}
	}
	else{
		if(velocity < -2047) vel = -2047;
		else{
			vel = 0x00ffffff - abs(velocity) + 1;
		}
	}
	tmc429_set429RampMode(rsrc, motor, TMC429_RM_VELOCITY);
	return(tmc429_regW_V_TARGET(rsrc, motor, vel));
}

static u8 tmc429_right(TMC429_RSRC *rsrc, u8 motor, int32_t velocity){
	tmc429_set429RampMode(rsrc, motor, TMC429_RM_VELOCITY);
	return(tmc429_regW_V_TARGET(rsrc, motor, velocity));
}

static u8 tmc429_left(TMC429_RSRC *rsrc, u8 motor, int32_t velocity){
	tmc429_set429RampMode(rsrc, motor, TMC429_RM_VELOCITY);
	return(tmc429_regW_V_TARGET(rsrc, motor, -velocity));
}

static u8 tmc429_hardStop(TMC429_RSRC *rsrc, u8 Motor){
	tmc429_set429RampMode(rsrc, Motor, TMC429_RM_VELOCITY);
	tmc429_regW_V_TARGET(rsrc, Motor, 0);
//	return(tmc429_regR_V_ACTUAL(rsrc, Motor, 0));
	return 0;
}

static u8 tmc429_moveto(TMC429_RSRC *rsrc, u8 motor, int32_t position){
	tmc429_set429RampMode(rsrc, motor, TMC429_RM_HOLD);
	tmc429_regW_X_TARGET(rsrc, motor, position);
	return(tmc429_set429RampMode(rsrc, motor, TMC429_RM_RAMP));
}

static u8 tmc429_moveby(TMC429_RSRC *rsrc, u8 motor, int32_t position){
	s32 actualPos;
	tmc429_set429RampMode(rsrc, motor, TMC429_RM_HOLD);
	tmc429_regR_X_ACTUAL(rsrc, motor, &actualPos);
	tmc429_regW_X_TARGET(rsrc, motor, actualPos + position);
	return(tmc429_set429RampMode(rsrc, motor, TMC429_RM_RAMP));
}

static u8 tmc429_readRegister(TMC429_RSRC *p, uint8_t address, u32 *value){
	uint8_t Read[4], Write[4]={0};
	Write[0] = address | TMC429_READ;
	ReadWrite429(p, Read, Write);
	*value = (Read[1]<<16) | (Read[2]<<8) | (Read[3]);
	return Read[0];
}

static u8 tmc429_writeRegister(TMC429_RSRC *p, uint8_t address, u32 value){
	uint8_t Read[4], Write[4];
	Write[0] = address & 0xfe;
	Write[1] = value>>16;
	Write[2] = value>>8;
	Write[3] = 0xff&value;
	ReadWrite429(p, Read, Write);
	return Read[0];
}

static uint32_t tmc429_getMeasuredSpeed(TMC429_RSRC *rsrc,int32_t *value){
	return 0;
}

static void tmc429_periodicJob(TMC429_RSRC *rsrc, uint32_t tick){

}

static void tmc429_deInit(TMC429_RSRC *rsrc){

}

static u8 tmc429_isTargetPos(TMC429_RSRC *rsrc, u8 motor){
	s32 target,actual;
	tmc429_regR_X_TARGET(rsrc, motor, &target);
	tmc429_regR_X_ACTUAL(rsrc, motor, &actual);
	//print("target:%d\tactual:%d\n", target, actual);
	if(abs(target-actual) < 20){	return 1;	}
	else {	return 0;	}
}


/***************************************************************//**
	 \fn ReadWrite429(uint8_t *Read, uint8_t *Write)
	 \brief 32 bit SPI communication with TMC429
	 \param Read   four byte array holding the data read from the TMC428
	 \param Write  four byte array holding the data to write to the TMC428

	 This is the low-level function that does all SPI communication with
	 the TMC429. It sends a 32 bit SPI telegramme to the TMC429 and
	 receives the 32 bit answer telegramme from the TMC429.
********************************************************************/
static void ReadWrite429(TMC429_RSRC* p, uint8_t *rxBuf, uint8_t *txBuf)
{
	while (HAL_SPI_GetState(p->SPI_HANDLE) != HAL_SPI_STATE_READY){}
	/* nSS signal activation - low */
	HAL_GPIO_WritePin(p->CS->GPIOx, p->CS->GPIO_Pin, GPIO_PIN_RESET);
	/* SPI byte send */
	HAL_SPI_TransmitReceive(p->SPI_HANDLE, txBuf, rxBuf, 4, 2);
	/* nSS signal deactivation - high */
	HAL_GPIO_WritePin(p->CS->GPIOx, p->CS->GPIO_Pin, GPIO_PIN_SET);
}


/***************************************************************//**
	 \fn Write429Datagram(uint8_t Address, uint8_t HighByte, uint8_t MidByte, uint8_t LowByte)
	 \brief TMC429 write access
	 \param Address   TMC429 register address
	 \param HighByte  MSB of the TMC429 register
	 \param MidByte   mid byte of the TMC429 register
	 \param LowByte   LSB of the TMC429 register

	 This function write three bytes to a TMC429 register.
********************************************************************/
//static u8 Write429Datagram(TMC429_RSRC* p, uint8_t Address, uint8_t HighByte, uint8_t MidByte, uint8_t LowByte)
//{
//	uint8_t Write[4], Read[4];
//	Write[0] = Address;
//	Write[1] = HighByte;
//	Write[2] = MidByte;
//	Write[3] = LowByte;
//	ReadWrite429(p,Read, Write);
//	return (Read[0]);
//}

/***************************************************************//**
	 \fn Write429Zero(uint8_t Address)
	 \brief Write zero to a TMC429 register
	 \param Address  TMC429 register address

	 This function sets a TMC429 register to zero. This can be useful
	 e.g. to stop a motor quickly.
********************************************************************/
static u8 Write429Zero(TMC429_RSRC* p, uint8_t Address)
{
	uint8_t Write[4], Read[4];

	Write[0] = Address;
	Write[1] = 0;
	Write[2] = 0;
	Write[3] = 0;
	ReadWrite429(p, Read, Write);
	return (Read[0]);
}


/***************************************************************//**
	 \fn Read429Status
	 \brief Read TMC429 status byte

	 \return TMC429 status byte

	 This functions reads just the status byte of the TMC429 using
	 a single byte SPI access which makes this a little bit faster.
********************************************************************/
static u8 Read429Status(TMC429_RSRC* p)
{
	u8 tx=0,rx=0;
	while (HAL_SPI_GetState(p->SPI_HANDLE) != HAL_SPI_STATE_READY){}
	/* nSS signal activation - low */
	HAL_GPIO_WritePin(p->CS->GPIOx, p->CS->GPIO_Pin, GPIO_PIN_RESET);
	/* SPI byte send */
	HAL_SPI_TransmitReceive(p->SPI_HANDLE, &tx, &rx, 1, 1);
	/* nSS signal deactivation - high */
	HAL_GPIO_WritePin(p->CS->GPIOx, p->CS->GPIO_Pin, GPIO_PIN_SET);
	return rx;
}

/***************************************************************//**
	 \fn Set429RampMode(uint8_t Axis, uint8_t RampMode)
	 \brief Set the ramping mode of an axis
	 \param  Axis  Motor number (0, 1 or 2)
	 \param  RampMode  ramping mode (RM_RAMP/RM_SOFT/RM_VELOCITY/RM_HOLD)

	 This functions changes the ramping mode of a motor in the TMC429.
	 It is some TMC429 register bit twiddling.
********************************************************************/
static void Set429RampMode(TMC429_RSRC* p, uint8_t Axis, uint8_t RampMode)
{
	uint8_t Write[4], Read[4];

	Write[0] = TMC429_IDX_REFCONF_RM(Axis)|TMC429_READ;
	ReadWrite429(p, Read, Write);

	Write[0] = TMC429_IDX_REFCONF_RM(Axis);
	Write[1] = Read[1];
	Write[2] = Read[2];
	Write[3] = RampMode;
	ReadWrite429(p, Read, Write);
}

/***************************************************************//**
	 \fn SetAMax(uint8_t Motor, uint32_t AMax)
	 \brief Set the maximum acceleration
	 \param Motor  motor number (0, 1, 2)
	 \param AMax: maximum acceleration (1..2047)

	 This function sets the maximum acceleration and also calculates
	 the PMUL and PDIV value according to all other parameters
	 (please see the TMC429 data sheet for more info about PMUL and PDIV
	 values).
********************************************************************/
static uint8_t SetAMax(TMC429_RSRC* rsrc, u8 Motor, uint32_t AMax)
{
	int32_t pm, pd;
	float p, p_reduced;
	int32_t ramp_div;
	int32_t pulse_div;
	uint8_t PulseRampDiv;
	uint8_t Data[3];
	u8 usrs;
	TMC429_REG_RampDiv_PulseDiv_t regRmpdivPulsediv;
	TMC429_REG_Pmul_Pdiv_t regPmulPdiv;
//	Read429Bytes(rsrc, TMC429_IDX_PULSEDIV_RAMPDIV(Motor), Data);
//	PulseRampDiv = Data[1];
	regRmpdivPulsediv.value = 0;
	regPmulPdiv.value = 0;
	tmc429_regR_RampDiv_PulseDiv_Usrs(rsrc, Motor, &regRmpdivPulsediv);
	pulse_div = regRmpdivPulsediv.PULSE_DIV;
	ramp_div = regRmpdivPulsediv.RAMP_DIV;

	// -1 indicates : no valid pair found
	pm = -1;
	pd = -1;

	AMax &= 0x000007FF;

	if(ramp_div >= pulse_div)
		p = AMax / ( 128.0 * (1<<ramp_div-pulse_div));  // Exponent positive or 0
	else
		p = AMax / ( 128.0 / (1<<pulse_div-ramp_div));  // Exponent negative

	p_reduced = p*0.988;

	int32_t pdiv;
	for(pdiv = 0; pdiv <= 13; pdiv++)
	{
		int32_t pmul = (int32_t) (p_reduced * 8.0 * (1<<pdiv)) - 128;

		if((0 <= pmul) && (pmul <= 127))
		{
			pm = pmul + 128;
			pd = pdiv;
		}
	}

//	Data[0] = 0;
//	Data[1] = (uint8_t) pm;
//	Data[2] = (uint8_t) pd;
//	Write429Bytes(rsrc, TMC429_IDX_PMUL_PDIV(Motor), Data);
	regPmulPdiv.PMUL = pm;
	regPmulPdiv.PDIV = pd;
	tmc429_regW_PMUL_PDIV(rsrc, Motor, regPmulPdiv);

	rsrc->axisReg[Motor].A_MAX = AMax;
	tmc429_regW_A_MAX(rsrc, Motor, rsrc->axisReg[Motor].A_MAX);

	return 0;
}

static void Init429(TMC429_RSRC* p)
{
	uint8_t motor;

	for(motor = 0; motor < 3; motor++)
	{
		Write429Zero(p, TMC429_IDX_XTARGET(motor));
		Write429Zero(p, TMC429_IDX_XACTUAL(motor));
		Write429Zero(p, TMC429_IDX_VMIN(motor));
		Write429Zero(p, TMC429_IDX_VMAX(motor));
		Write429Zero(p, TMC429_IDX_VTARGET(motor));
		Write429Zero(p, TMC429_IDX_AMAX(motor));
		Write429Zero(p, TMC429_IDX_AGTAT_ALEAT(motor));
		Write429Zero(p, TMC429_IDX_PMUL_PDIV(motor));
		Write429Zero(p, TMC429_IDX_REFCONF_RM(motor));
		Write429Zero(p, TMC429_IDX_IMASK_IFLAGS(motor));
		Write429Zero(p, TMC429_IDX_PULSEDIV_RAMPDIV(motor));
		Write429Zero(p, TMC429_IDX_DX_REFTOLERANCE(motor));
		Write429Zero(p, TMC429_IDX_XLATCHED(motor));
		Write429Zero(p, TMC429_IDX_USTEP_COUNT_429(motor));

		tmc429_motionConf(p,
			motor,
			p->axisReg[motor].V_MAX,	// max veloctity
			p->axisReg[motor].A_MAX, 	// max accelation
			p->axisReg[motor].RampDiv_PulseDiv.PULSE_DIV,	//pulse_div
			p->axisReg[motor].RampDiv_PulseDiv.RAMP_DIV,	// rampDiv
			p->axisReg[motor].PMul_PDiv.PMUL,
			p->axisReg[motor].PMul_PDiv.PDIV
		);

		p->axisReg[motor].IntMsk_IntFlg.MASK = 0xff;
		p->axisReg[motor].IntMsk_IntFlg.FLAGS = 0xff;
		tmc429_writeRegister(p, TMC429_IDX_IMASK_IFLAGS(motor), p->axisReg[motor].IntMsk_IntFlg.value);

//		tmc429_regW_V_MIN(p, motor, p->axisReg[motor].V_MIN);
//		SetAMax(p, motor, p->axisReg[motor].A_MAX);
//		tmc429_regW_V_MAX(p, motor, p->axisReg[motor].V_MAX);
	}

	// global config
	tmc429_regW_SMGP(p, (0x00<<16)|(0x04<<8)|0x02);
	// common config
	tmc429_regW_IFConf(p,
			TMC429_IFCONF_INV_REF	| 		// 所有参考开关反相
			TMC429_IFCONF_SDO_INT 	| 		// 中断信号映射到SDO引脚
			TMC429_IFCONF_EN_SD		| 		//　Step/direct模式
			TMC429_IFCONF_POS_COMP_OFF	 	// 没有电机位置需要比较
	);
}

static s32 tmc429_loadFromROM(TMC429_RSRC *rsrc, u8 motor){
	u8 buff[16] = {0}, chck,i;
	TMC429_REG_Pmul_Pdiv_t PMul_PDiv;				// RW
	TMC429_REG_RampMod_RefConf_t RampMod_RefConf;	// RW
	TMC429_REG_IntMsk_IntFlg_t IntMsk_IntFlg;		// RW
	TMC429_REG_RampDiv_PulseDiv_t RampDiv_PulseDiv;	// RW

	rsrc->EepromRead(rsrc->eepromAddrBase+motor*16, buff, 16);
	chck = 0xa5;		// make check code
	for(i=1;i<16;i++){	chck ^= buff[i];	}

	if(chck != buff[0])	return -1;

	rsrc->axisReg[motor].V_MAX = buff[1];	// VMAX, BYTE1
	rsrc->axisReg[motor].V_MAX <<= 8;
	rsrc->axisReg[motor].V_MAX |= buff[2];	// VMAX, BYTE0
	rsrc->axisReg[motor].A_MAX = buff[3];	// VMAX, BYTE1
	rsrc->axisReg[motor].A_MAX <<= 8;
	rsrc->axisReg[motor].A_MAX |= buff[4];	// VMAX, BYTE0
	rsrc->axisReg[motor].PMul_PDiv.PMUL = buff[5];	// PMUL
	rsrc->axisReg[motor].PMul_PDiv.PDIV = buff[6];	// PDIV
	rsrc->axisReg[motor].RampDiv_PulseDiv.RAMP_DIV = buff[7];	// RampDiv
	rsrc->axisReg[motor].RampDiv_PulseDiv.PULSE_DIV= buff[8];	// PulseDiv
	rsrc->axisReg[motor].RampMod_RefConf.RAMP_MOD = buff[9];	// RampMod
	rsrc->axisReg[motor].RampMod_RefConf.REF_CONF = buff[10];	// RefConf
	rsrc->axisReg[motor].V_MIN = buff[11];	// VMIN, BYTE1
	rsrc->axisReg[motor].V_MIN <<= 8;
	rsrc->axisReg[motor].V_MIN |= buff[12];	// VMIN, BYTE0

	return 0;
}

static void tmc429_saveToROM(TMC429_RSRC *rsrc, u8 motor){
	u8 buff[16] = {0}, i;

	buff[1] = rsrc->axisReg[motor].V_MAX>>8;	// VMAX, BYTE1
	buff[2] = rsrc->axisReg[motor].V_MAX;	// VMAX, BYTE0
	buff[3] = rsrc->axisReg[motor].A_MAX>>8;	// VMAX, BYTE1
	buff[4] = rsrc->axisReg[motor].A_MAX;	// VMAX, BYTE0
	buff[5] = rsrc->axisReg[motor].PMul_PDiv.PMUL;	// PMUL
	buff[6] = rsrc->axisReg[motor].PMul_PDiv.PDIV;	// PDIV
	buff[7] = rsrc->axisReg[motor].RampDiv_PulseDiv.RAMP_DIV;	// RampDiv
	buff[8] = rsrc->axisReg[motor].RampDiv_PulseDiv.PULSE_DIV;	// PulseDiv
	buff[9] = rsrc->axisReg[motor].RampMod_RefConf.RAMP_MOD;	// RampMod
	buff[10] = rsrc->axisReg[motor].RampMod_RefConf.REF_CONF;	// RefConf
	buff[11] = rsrc->axisReg[motor].V_MIN>>8;	// VMIN, BYTE1
	buff[12] = rsrc->axisReg[motor].V_MIN;	// VMIN, BYTE0

	buff[0] = 0xa5;		// make check code
	for(i=1;i<16;i++){	buff[0] ^= buff[i];	}

	rsrc->EepromWrite(rsrc->eepromAddrBase+motor*16, buff, 16);
}
