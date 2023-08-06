/*
 * TMC5160.h
 *
 *  Created on: 17.03.2017
 *      Author: ED based on BS
 */

#ifndef TMC_IC_TMC5160_H_
#define TMC_IC_TMC5160_H_

#include "API_Header.h"
#include "TMC5160_Register.h"
#include "TMC5160_Constants.h"
#include "TMC5160_Fields.h"

#define TMC5160_FIELD_READ(motor, address, mask, shift)           FIELD_READ(tmc5160_readInt, motor, address, mask, shift)
#define TMC5160_FIELD_WRITE(motor, address, mask, shift, value)   FIELD_WRITE(tmc5160_writeInt, motor, address, mask, shift, value)
#define TMC5160_FIELD_UPDATE(motor, address, mask, shift, value)  FIELD_UPDATE(tmc5160_readInt, tmc5160_writeInt, motor, address, mask, shift, value)

// Factor between 10ms units and internal units for 16MHz
//#define TPOWERDOWN_FACTOR (4.17792*100.0/255.0)
// TPOWERDOWN_FACTOR = k * 100 / 255 where k = 2^18 * 255 / fClk for fClk = 16000000)

typedef struct
{
	int velocity, oldX;
	uint32_t oldTick;
	int32_t regShadow[TMC5160_REGISTER_COUNT];
	uint8_t registerAccess[TMC5160_REGISTER_COUNT];
//	uint8_t channels[TMC5160_MOTORS];
} TMC5160TypeDef;

typedef struct{
	char name[DEV_NAME_LEN];
	const PIN_T *REFL_UC;
	const PIN_T *REFR_UC;
	const PIN_T *DRV_ENN_CFG6;
	const PIN_T *ENCA_DCIN_CFG5;
	const PIN_T *ENCB_DCEN_CFG4;
	const PIN_T *ENCN_DCO;
	const PIN_T *SD_MODE;
	const PIN_T *SPI_MODE;
	const PIN_T *SWN_DIAG0;
	const PIN_T *SWP_DIAG1;
	const PIN_T *CS;
	SPI_HandleTypeDef* SPI_HANDLE;

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
	bool uart_mode;
	TMC5160TypeDef TMC5160;
} TMC5160_RSRC;

typedef struct{
	TMC5160_RSRC rsrc;
	uint32_t (*right)(TMC5160_RSRC *rsrc,int32_t velocity);
	uint32_t (*left)(TMC5160_RSRC *rsrc,int32_t velocity);
	uint32_t (*rotate)(TMC5160_RSRC *rsrc,int32_t velocity);
	uint32_t (*stop)(TMC5160_RSRC *rsrc);
	uint32_t (*moveTo)(TMC5160_RSRC *rsrc,int32_t position);
	uint32_t (*moveBy)(TMC5160_RSRC *rsrc,int32_t *ticks);
//	uint32_t (*GAP)(uint8_t type, uint8_t motor, int32_t *value);
//	uint32_t (*SAP)(uint8_t type, uint8_t motor, int32_t value);
	void (*readRegister)(TMC5160_RSRC *rsrc,uint8_t address, int32_t *value);
	void (*writeRegister)(TMC5160_RSRC *rsrc,uint8_t address, int32_t value);
	uint32_t (*getMeasuredSpeed)(TMC5160_RSRC *rsrc,int32_t *value);

	void (*periodicJob)(TMC5160_RSRC *rsrc, uint32_t tick);
	void (*init)(TMC5160_RSRC *rsrc);
	void (*deInit)(TMC5160_RSRC *rsrc);
	uint8_t (*reset)(TMC5160_RSRC *rsrc);
} TMC5160_DEV;

// setup a TMC5160 device
void setupDev_tmc5160(
	TMC5160_DEV *dev,
	const char* NAME,
	SPI_HandleTypeDef* SPI_HANDLE,	// spi handle
	const PIN_T *CS,				// spi select
	const PIN_T *DRV_ENN_CFG6,		// driver enable
	const PIN_T *SD_MODE,			//
	const PIN_T *SWN_DIAG0,			//
	const PIN_T *SWP_DIAG1,			//
	const PIN_T *ENCA_DCIN_CFG5,	//
	const PIN_T *ENCB_DCEN_CFG4,	//
	const PIN_T *ENCN_DCO			//
);

#endif /* TMC_IC_TMC5160_H_ */
