/**********************************************************
filename: tb67s109.h
**********************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _TB67S109_H
#define _TB67S109_H

#include "misc.h"

typedef void (*tb67sSetMod) (u8 mod);

typedef struct {
	const PIN_T *EN;
	const PIN_T *DMOD[3];
	tb67sSetMod setMod;
	// PWM generator
	TIM_HandleTypeDef* htim;
	uint32_t ch;
	// eeprom
	u16 ioBase;
	s8 (*ioWrite)(u16 addr, const u8 *pDat, u16 nBytes);
	s8 (*ioRead)(u16 addr, u8 *pDat, u16 nBytes);
	// local var
	u16 mA;
	u16 mStep;
} TB67S109_RSRC_T;

typedef struct {
	TB67S109_RSRC_T rsrc;
	void (*Enable) (TB67S109_RSRC_T* pRsrc);
	void (*Disable) (TB67S109_RSRC_T* pRsrc);
	void (*SetMicroStep)	(TB67S109_RSRC_T* pRsrc, u16 mSteps);
	void (*SetCurrent)	(TB67S109_RSRC_T* pRsrc, u16 mA);
	void (*Save)	(TB67S109_RSRC_T* pRsrc);
	void (*Default)	(TB67S109_RSRC_T* pRsrc);
}TB67S109_DEV_T;

/* output variables for extern function --------------------------------------*/
void TB67S109_Setup(
	TB67S109_DEV_T *pDev,
	TIM_HandleTypeDef* htim,
	uint32_t ch,
	const PIN_T *EN,
	const PIN_T *DMOD0,
	const PIN_T *DMOD1,
	const PIN_T *DMOD2,
	u16 ioBase,
	s8 (*ioWrite)(u16 addr, const u8 *pDat, u16 nBytes),
	s8 (*ioRead)(u16 addr, u8 *pDat, u16 nBytes)
);

void TB67S109_Setup1(
	TB67S109_DEV_T *pDev,
	TIM_HandleTypeDef* htim,
	uint32_t ch,
	const PIN_T *EN,
	tb67sSetMod setMod,
	u16 ioBase,
	s8 (*ioWrite)(u16 addr, const u8 *pDat, u16 nBytes),
	s8 (*ioRead)(u16 addr, u8 *pDat, u16 nBytes)
);

#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
