#include "xtb67s109.h"
#include "gpioDecal.h"
#include "string.h"
/**********************************************************
 private function
**********************************************************/
static void tb67s109_driverEn(TB67S109_RSRC_T* pRsrc);
static void tb67s109_driverDis(TB67S109_RSRC_T* pRsrc);
static void tb67s109_setMicroStep(TB67S109_RSRC_T* pRsrc, u16 mSteps);
static void tb67s109_setCurrent(TB67S109_RSRC_T* pRsrc, u16 mA);
static void tb67s109_save(TB67S109_RSRC_T* pRsrc);
static s32 tb67s109_load(TB67S109_RSRC_T* pRsrc);
static void tb67s109_default(TB67S109_RSRC_T* pRsrc);

#define MA_TO_DUTY(ma)		((ma*51)/215)

/**********************************************************
 public function
**********************************************************/
void TB67S109_SetupComm(
	TB67S109_DEV_T *pDev,
	TIM_HandleTypeDef* htim,
	uint32_t ch,
	const PIN_T *EN,
	u16 ioBase,
	s8 (*ioWrite)(u16 addr, const u8 *pDat, u16 nBytes),
	s8 (*ioRead)(u16 addr, u8 *pDat, u16 nBytes)
){
	pDev->rsrc.htim = htim;
	pDev->rsrc.ch = ch;
	pDev->rsrc.EN = EN;

	pDev->rsrc.ioBase = ioBase;
	pDev->rsrc.ioWrite = ioWrite;
	pDev->rsrc.ioRead = ioRead;

	pDev->Enable = tb67s109_driverEn;
	pDev->Disable = tb67s109_driverDis;
	pDev->SetCurrent = tb67s109_setCurrent;
	pDev->SetMicroStep = tb67s109_setMicroStep;
	pDev->Save = tb67s109_save;
	pDev->Default = tb67s109_default;

	// load parameter from eeprom
	if(tb67s109_load(&pDev->rsrc) < 0){
		pDev->rsrc.mA = 500;
		pDev->rsrc.mStep = 32;
		tb67s109_save(&pDev->rsrc);
	}

	HAL_TIM_PWM_Start(htim, ch);
	__HAL_TIM_SET_COMPARE(htim, ch, MA_TO_DUTY(pDev->rsrc.mA));

	tb67s109_setMicroStep(&pDev->rsrc, pDev->rsrc.mStep);	// set micro step
	tb67s109_driverEn(&pDev->rsrc);
}

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
){
	memset(&pDev->rsrc, 0, sizeof(TB67S109_RSRC_T));
	pDev->rsrc.DMOD[0] = DMOD0;
	pDev->rsrc.DMOD[1] = DMOD1;
	pDev->rsrc.DMOD[2] = DMOD2;

	as_OUTPUT_PP_PULLUP_HIGH(*DMOD0);
	as_OUTPUT_PP_PULLUP_HIGH(*DMOD1);
	as_OUTPUT_PP_PULLUP_HIGH(*DMOD2);

	TB67S109_SetupComm(
		pDev,		//		TB67S109_DEV_T *pDev,
		htim, 		//		TIM_HandleTypeDef* htim,
		ch,			//		uint32_t ch,
		EN,			//		const PIN_T *EN,
		ioBase,		//		u16 ioBase,
		ioWrite,	//		s8 (*ioWrite)(u16 addr, const u8 *pDat, u16 nBytes),
		ioRead		//		s8 (*ioRead)(u16 addr, u8 *pDat, u16 nBytes)
	);
}

void TB67S109_Setup1(
	TB67S109_DEV_T *pDev,
	TIM_HandleTypeDef* htim,
	uint32_t ch,
	const PIN_T *EN,
	const tb67sSetMod setMod,
	u16 ioBase,
	s8 (*ioWrite)(u16 addr, const u8 *pDat, u16 nBytes),
	s8 (*ioRead)(u16 addr, u8 *pDat, u16 nBytes)
){
	memset(&pDev->rsrc,0,sizeof(TB67S109_RSRC_T));
	pDev->rsrc.setMod = setMod;
	setMod(7);

	TB67S109_SetupComm(
		pDev,		//		TB67S109_DEV_T *pDev,
		htim, 		//		TIM_HandleTypeDef* htim,
		ch,			//		uint32_t ch,
		EN,			//		const PIN_T *EN,
		ioBase,		//		u16 ioBase,
		ioWrite,	//		s8 (*ioWrite)(u16 addr, const u8 *pDat, u16 nBytes),
		ioRead		//		s8 (*ioRead)(u16 addr, u8 *pDat, u16 nBytes)
	);
}

static void tb67s109_driverEn(TB67S109_RSRC_T* pRsrc){
	HAL_GPIO_WritePin(pRsrc->EN->GPIOx, pRsrc->EN->GPIO_Pin, GPIO_PIN_SET);
}

static void tb67s109_driverDis(TB67S109_RSRC_T* pRsrc){
	HAL_GPIO_WritePin(pRsrc->EN->GPIOx, pRsrc->EN->GPIO_Pin, GPIO_PIN_RESET);
}

/**
  * @brief  Setup TB67S109
  * mSteps
  * [0]-Standby mode 	[1]-1/1			[2]-1/2 step(A)	[3]-1/4 step
  * [4]-1/2 step(A)		[5]-1/8 step	[6]-1/16 step 	[7]-1/32 step
  * @retval int
  */
static void tb67s109_setMicroStep(TB67S109_RSRC_T* pRsrc, u16 microSteps){
	u8 mSteps = 0xff;
	switch(microSteps){
	case 0:	// standby mode
		mSteps = 0;
		break;
	case 1:	// Full Step
		mSteps = 1;
		break;
	case 2:	// 1/2 step
		mSteps = 2;
		break;
	case 4:	// 1/4 step
		mSteps = 3;
		break;
	case 8:	// 1/8 step
		mSteps = 5;
		break;
	case 16:	// 1/16 step
		mSteps = 6;
		break;
	case 32:	// 1/32 step
		mSteps = 7;
		break;
	}

	if(mSteps == 0xff){
		return;
	}

	pRsrc->mStep = microSteps;

	if(mSteps & BIT(0)){	HAL_GPIO_WritePin(pRsrc->DMOD[0]->GPIOx, pRsrc->DMOD[0]->GPIO_Pin, GPIO_PIN_SET);	}
	else{	HAL_GPIO_WritePin(pRsrc->DMOD[0]->GPIOx, pRsrc->DMOD[0]->GPIO_Pin, GPIO_PIN_RESET);	}

	if(mSteps & BIT(1)){	HAL_GPIO_WritePin(pRsrc->DMOD[1]->GPIOx, pRsrc->DMOD[1]->GPIO_Pin, GPIO_PIN_SET);	}
	else{	HAL_GPIO_WritePin(pRsrc->DMOD[1]->GPIOx, pRsrc->DMOD[1]->GPIO_Pin, GPIO_PIN_RESET);	}

	if(mSteps & BIT(2)){	HAL_GPIO_WritePin(pRsrc->DMOD[2]->GPIOx, pRsrc->DMOD[2]->GPIO_Pin, GPIO_PIN_SET);	}
	else{	HAL_GPIO_WritePin(pRsrc->DMOD[2]->GPIOx, pRsrc->DMOD[2]->GPIO_Pin, GPIO_PIN_RESET);	}

}

static void tb67s109_setCurrent(TB67S109_RSRC_T* pRsrc, u16 mA){
	__HAL_TIM_SET_COMPARE(pRsrc->htim, pRsrc->ch, MA_TO_DUTY(mA));
	pRsrc->mA = mA;
}

static void tb67s109_save(TB67S109_RSRC_T* pRsrc){
	u8 buff[4] = {0},i;

	buff[1] = pRsrc->mA>>8;
	buff[2] = pRsrc->mA;
	buff[3] = pRsrc->mStep;

	buff[0] = 0xa5;		// make check code
	for(i=1;i<4;i++){	buff[0] ^= buff[i];	}

	pRsrc->ioWrite(pRsrc->ioBase, buff, 4);
}

static s32 tb67s109_load(TB67S109_RSRC_T* pRsrc){
	u8 buff[4] = {0}, chck,i;

	pRsrc->ioRead(pRsrc->ioBase, buff, 4);

	chck = 0xa5;		// make check code
	for(i=1;i<4;i++){	chck ^= buff[i];	}

	if(chck != buff[0]){	return -1;	}

	pRsrc->mA = buff[1];	// current byte[1]
	pRsrc->mA <<= 8;
	pRsrc->mA |= buff[2];	// current byte[0]
	pRsrc->mStep = buff[3];	// mStep
	return 0;
}

static void tb67s109_default(TB67S109_RSRC_T* pRsrc){
	pRsrc->mA = 4000;
	pRsrc->mStep = 32;
	tb67s109_save(pRsrc);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

