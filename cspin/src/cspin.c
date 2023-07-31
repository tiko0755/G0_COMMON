/**
  ******************************************************************************
  * @file    cspin.c 
  * @author  IPC Rennes
  * @version V2.1
  * @date    October 15, 2013
  * @brief   cSPIN (motor and L6482) product related routines
  * @note    (C) COPYRIGHT 2013 STMicroelectronics
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  */ 

/* Includes ------------------------------------------------------------------*/
#include "misc.h"
#include "gpioDecal.h"
#include "cspin.h"
#include "cspin_config.h"
#include "string.h"
#include "board.h"

/** @addtogroup cSPIN FW library interface
  * @{
  */
//#define L6480	1
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void cSPIN_Delay(__IO u32 nCount);
void cSPIN_Reset_And_Standby(cSPIN_Rsrc_T* pRsrc);
void cSPIN_Regs_Struct_Reset(cSPIN_Rsrc_T* pRsrc);
void cSPIN_Registers_SetAll(cSPIN_Rsrc_T* pRsrc, cSPIN_RegsStruct_TypeDef *reg);
void cSPIN_Registers_GetAll(cSPIN_Rsrc_T* pRsrc, cSPIN_RegsStruct_TypeDef *reg);
void cSPIN_Reset_Device(cSPIN_Rsrc_T* pRsrc);
u8 cSPIN_Write_Byte(cSPIN_Rsrc_T* pRsrc, u8 byte);

/* commands(19) --------------------------------------------------------------*/
void cSPIN_Set_Param			(cSPIN_Rsrc_T* pRsrc, cSPIN_Regs_T param, uint32_t value);
uint32_t cSPIN_Get_Param	(cSPIN_Rsrc_T* pRsrc, cSPIN_Regs_T param);

u8 isCmdErr						(cSPIN_Rsrc_T* pRsrc);
u8 cSPIN_SetAbsPos		(cSPIN_Rsrc_T* pRsrc, u32 value);
u8 cSPIN_GetAbsPos		(cSPIN_Rsrc_T* pRsrc, u32 *value);
u8 cSPIN_SetAbsPos		(cSPIN_Rsrc_T* pRsrc, u32 param);
u8 cSPIN_GetAbsPos		(cSPIN_Rsrc_T* pRsrc, u32 *param);
u8 cSPIN_SetElPos			(cSPIN_Rsrc_T* pRsrc, u16 param);
u8 cSPIN_GetElPos			(cSPIN_Rsrc_T* pRsrc, u16 *param);
u8 cSPIN_SetMarkPos		(cSPIN_Rsrc_T* pRsrc, u32 param);
u8 cSPIN_GetMarkPos		(cSPIN_Rsrc_T* pRsrc, u32 *param);
u8 cSPIN_GetSpd_stepPerS	(cSPIN_Rsrc_T* pRsrc, u32 *stepPerSec);

u8 cSPIN_SetAcc				(cSPIN_Rsrc_T* pRsrc, u16 param);
u8 cSPIN_GetAcc				(cSPIN_Rsrc_T* pRsrc, u16 *param);
u8 cSPIN_SetDec				(cSPIN_Rsrc_T* pRsrc, u16 param);
u8 cSPIN_GetDec				(cSPIN_Rsrc_T* pRsrc, u16 *param);
u8 cSPIN_SetMaxSpd		(cSPIN_Rsrc_T* pRsrc, u16 param);
u8 cSPIN_GetMaxSpd		(cSPIN_Rsrc_T* pRsrc, u16 *param);
//static u8 cSPIN_RegSetMaxSpd_stepPerSec(cSPIN_Rsrc_T* pRsrc, u16 stepPerSecond);
u8 cSPIN_SetMinSpd		(cSPIN_Rsrc_T* pRsrc, u16 param);
u8 cSPIN_GetMinSpd		(cSPIN_Rsrc_T* pRsrc, u16 *param);
u8 cSPIN_SetFSSpd			(cSPIN_Rsrc_T* pRsrc, u16 param);
u8 cSPIN_GetFSSpd			(cSPIN_Rsrc_T* pRsrc, u16 *param);
u8 cSPIN_SetStepSel		(cSPIN_Rsrc_T* pRsrc, u8 param);
u8 cSPIN_GetStepSel		(cSPIN_Rsrc_T* pRsrc, u8 *param);

u8 cSPIN_SetLowSpdOpt	(cSPIN_Rsrc_T* pRsrc, u8 param);
u8 cSPIN_GetLowSpdOpt	(cSPIN_Rsrc_T* pRsrc, u8 *param);
u8 cSPIN_SetBoostMode	(cSPIN_Rsrc_T* pRsrc, u8 param);
u8 cSPIN_GetBoostMode	(cSPIN_Rsrc_T* pRsrc, u8 *param);

//u8 cSPIN_SetKValHold	(cSPIN_Rsrc_T* pRsrc, u8 param);
//u8 cSPIN_GetKValHold	(cSPIN_Rsrc_T* pRsrc, u8 *param);
//u8 cSPIN_SetKValRun		(cSPIN_Rsrc_T* pRsrc, u8 param);
//u8 cSPIN_GetKValRun		(cSPIN_Rsrc_T* pRsrc, u8 *param);
//u8 cSPIN_SetKValAcc		(cSPIN_Rsrc_T* pRsrc, u8 param);
//u8 cSPIN_GetKValAcc		(cSPIN_Rsrc_T* pRsrc, u8 *param);
//u8 cSPIN_SetKValDec		(cSPIN_Rsrc_T* pRsrc, u8 param);
//u8 cSPIN_GetKValDec		(cSPIN_Rsrc_T* pRsrc, u8 *param);
u8 cSPIN_SetKVal		(cSPIN_Rsrc_T* pRsrc, u8 kHold, u8 kRun, u8 kAcc, u8 kDec);
u8 cSPIN_GetKVal		(cSPIN_Rsrc_T* pRsrc, u8* kHold, u8* kRun, u8* kAcc, u8* kDec);


u8 cSPIN_SetIntSpd		(cSPIN_Rsrc_T* pRsrc, u16 param);
u8 cSPIN_GetIntSpd		(cSPIN_Rsrc_T* pRsrc, u16 *param);
u8 cSPIN_SetStartSlp	(cSPIN_Rsrc_T* pRsrc, u8 param);
u8 cSPIN_GetStartSlp	(cSPIN_Rsrc_T* pRsrc, u8 *param);
u8 cSPIN_SetFnSlpAcc	(cSPIN_Rsrc_T* pRsrc, u8 param);
u8 cSPIN_GetFnSlpAcc	(cSPIN_Rsrc_T* pRsrc, u8 *param);
u8 cSPIN_SetFnSlpDec	(cSPIN_Rsrc_T* pRsrc, u8 param);
u8 cSPIN_GetFnSlpDec	(cSPIN_Rsrc_T* pRsrc, u8 *param);
u8 cSPIN_SetKTherm		(cSPIN_Rsrc_T* pRsrc, u8 param);
u8 cSPIN_GetKTherm		(cSPIN_Rsrc_T* pRsrc, u8 *param);
u8 cSPIN_GetAdcOut		(cSPIN_Rsrc_T* pRsrc, u8 *param);
u8 cSPIN_SetOCDTh			(cSPIN_Rsrc_T* pRsrc, u8 param);
u8 cSPIN_GetOCDTh			(cSPIN_Rsrc_T* pRsrc, u8 *param);
u8 cSPIN_SetStallTh		(cSPIN_Rsrc_T* pRsrc, u8 param);
u8 cSPIN_GetStallTh		(cSPIN_Rsrc_T* pRsrc, u8 *param);

u8 cSPIN_SetSyncSel		(cSPIN_Rsrc_T* pRsrc, u8 param);
u8 cSPIN_GetSyncSel		(cSPIN_Rsrc_T* pRsrc, u8 *param);
u8 cSPIN_SetSyncEn		(cSPIN_Rsrc_T* pRsrc, u8 param);
u8 cSPIN_GetSyncEn		(cSPIN_Rsrc_T* pRsrc, u8 *param);

// use for L6480
u8 cSPIN_SetGateTcc		(cSPIN_Rsrc_T* pRsrc, u8 param);
u8 cSPIN_GetGateTcc		(cSPIN_Rsrc_T* pRsrc, u8 *param);
u8 cSPIN_SetGateCurrent		(cSPIN_Rsrc_T* pRsrc, u8 param);
u8 cSPIN_GetGateCurrent		(cSPIN_Rsrc_T* pRsrc, u8 *param);
u8 cSPIN_SetGateTBoost		(cSPIN_Rsrc_T* pRsrc, u8 param);
u8 cSPIN_GetGateTBoost		(cSPIN_Rsrc_T* pRsrc, u8 *param);
u8 cSPIN_SetGateTDeadTime	(cSPIN_Rsrc_T* pRsrc, u8 param);
u8 cSPIN_GetGateTDeadTime	(cSPIN_Rsrc_T* pRsrc, u8 *param);
u8 cSPIN_SetGateTBlank		(cSPIN_Rsrc_T* pRsrc, u8 param);
u8 cSPIN_GetGateTBlank		(cSPIN_Rsrc_T* pRsrc, u8 *param);

//u8 cSPIN_SetConfig (cSPIN_Rsrc_T* pRsrc, u32 param);
//u8 cSPIN_GetConfig (cSPIN_Rsrc_T* pRsrc, u32 *param);

static u8 cSPIN_SetConfig (cSPIN_Rsrc_T* pRsrc, u16 param);
static u8 cSPIN_GetConfig (cSPIN_Rsrc_T* pRsrc, u16* param);

u8 cSPIN_SetSwMode (cSPIN_Rsrc_T* pRsrc, u8 param);
u8 cSPIN_GetSwMode (cSPIN_Rsrc_T* pRsrc, u8 *param);
u8 cSPIN_SetVsCompEn (cSPIN_Rsrc_T* pRsrc, u8 param);
u8 cSPIN_GetVsCompEn (cSPIN_Rsrc_T* pRsrc, u8 *param);
u8 cSPIN_SetOCSDEn (cSPIN_Rsrc_T* pRsrc, u8 param);
u8 cSPIN_GetOCSDEn (cSPIN_Rsrc_T* pRsrc, u8 *param);
u8 cSPIN_SetUVLOVal (cSPIN_Rsrc_T* pRsrc, u8 param);
u8 cSPIN_GetUVLOVal (cSPIN_Rsrc_T* pRsrc, u8 *param);
u8 cSPIN_SetVccVal (cSPIN_Rsrc_T* pRsrc, u8 param);
u8 cSPIN_GetVccVal (cSPIN_Rsrc_T* pRsrc, u8 *param);
u8 cSPIN_SetPwmFrq (cSPIN_Rsrc_T* pRsrc, u8 param);
u8 cSPIN_GetPwmFrq (cSPIN_Rsrc_T* pRsrc, u8 *param);
u8 cSPIN_GetStatus (cSPIN_Rsrc_T* pRsrc, u16 *param);

static void regDefault(cSPIN_Rsrc_T* pRsrc, cSPIN_RegsStruct_TypeDef *reg);

void cSPIN_Run						(cSPIN_Rsrc_T* pRsrc, cSPIN_Direction_T direction, uint32_t speed);
void cSPIN_Step_Clock			(cSPIN_Rsrc_T* pRsrc, cSPIN_Direction_T direction);
void cSPIN_Move						(cSPIN_Rsrc_T* pRsrc, cSPIN_Direction_T direction, uint32_t n_step);
void cSPIN_MoveStep					(cSPIN_Rsrc_T* pRsrc, cSPIN_Direction_T direction, uint32_t n_step);
void cSPIN_Go_To					(cSPIN_Rsrc_T* pRsrc, uint32_t abs_pos);
void cSPIN_Go_To_Dir			(cSPIN_Rsrc_T* pRsrc, cSPIN_Direction_T direction, uint32_t abs_pos);
void cSPIN_Go_Until				(cSPIN_Rsrc_T* pRsrc, cSPIN_Action_T action, cSPIN_Direction_T direction, uint32_t speed);
void cSPIN_Release_SW			(cSPIN_Rsrc_T* pRsrc, cSPIN_Action_T action, cSPIN_Direction_T direction);
void cSPIN_Go_Home				(cSPIN_Rsrc_T* pRsrc);
void cSPIN_Go_Mark				(cSPIN_Rsrc_T* pRsrc);
void cSPIN_Reset_Pos			(cSPIN_Rsrc_T* pRsrc);
void cSPIN_Soft_Stop			(cSPIN_Rsrc_T* pRsrc);
void cSPIN_Hard_Stop			(cSPIN_Rsrc_T* pRsrc);
void cSPIN_Soft_HiZ				(cSPIN_Rsrc_T* pRsrc);
void cSPIN_Hard_HiZ				(cSPIN_Rsrc_T* pRsrc);

void cSPIN_tickExe(cSPIN_Rsrc_T* pRsrc, u8 tick);

uint16_t cSPIN_Get_Status	(cSPIN_Rsrc_T* pRsrc);
uint8_t cSPIN_Busy_HW(cSPIN_Rsrc_T* pRsrc);
uint8_t cSPIN_Busy_SW(cSPIN_Rsrc_T* pRsrc);
uint8_t cSPIN_Flag(cSPIN_Rsrc_T* pRsrc);

static s8 cspinEepromWriteAllReg(cSPIN_Rsrc_T* pRsrc, cSPIN_RegsStruct_TypeDef *regs);
static s8 cspinEepromReadAllReg(cSPIN_Rsrc_T* pRsrc, cSPIN_RegsStruct_TypeDef *regs);
static void cspinDefault(cSPIN_Rsrc_T* pRsrc);
static s8 cspinSaveToEEPROM(cSPIN_Rsrc_T* pRsrc);

static void cspinPrintAll(cSPIN_Rsrc_T* pRsrc);
/**
  * @brief  Setup a motor to be ready to run
  * @param  None
  * @retval None
  */
void cSPIN_Setup(
	cSPIN_DEV_T *pDev, 
	const char* NAME, 
	CSPIN_CHIP CHIP,
	SPI_HandleTypeDef* SPI_HANDLE,
	const PIN_T* CS,
	
	const PIN_T* SW_L,
	const PIN_T* SW_R,
	
	const PIN_T* FLAG,
	const PIN_T* SYNC,
	const PIN_T* STCK,
	
	void (*print)			 (const char* FORMAT_ORG, ...),
	s8 (*EepromRead)   (u16 addr, u8 *pDat, u16 nBytes),
	s8 (*EepromWrite)  (u16 addr, const u8 *pDat, u16 nBytes),
	u16 romBase
){
	cSPIN_RegsStruct_TypeDef regs = {0};
	cSPIN_Rsrc_T* pRsrc = &pDev->rsrc;
	pRsrc->SPI_HANDLE = SPI_HANDLE;
	pRsrc->CS = CS;		
	pRsrc->SW_L = SW_L;
	pRsrc->SW_R = SW_R;
	pRsrc->FLG = FLAG;
	pRsrc->SYNC = SYNC;
	pRsrc->STCK = STCK;
	
	if(CS)			as_OUTPUT_PP_NOPULL_HIGH(*CS);
	if(SW_L)		as_INPUT_PULLUP(*SW_L);
	if(SW_R)		as_INPUT_PULLUP(*SW_R);
	if(SYNC)		as_INPUT_NOPULL(*SYNC);
	if(STCK)		as_OUTPUT_PP_NOPULL_HIGH(*STCK);
	
	pRsrc->chip = CHIP;
	
//	if(spiConfig(
//		SPI_HANDLE,
//		SPI_FIRSTBIT_MSB,
//		SPI_POLARITY_HIGH,
//		SPI_PHASE_2EDGE,
//		SPI_BAUDRATEPRESCALER_32
//	) < 0) {
//		pDev->rsrc.errorCode |= BIT(1);
//	}
	
	memset(pRsrc->name,0, DEV_NAME_LEN);
	strcpy(pRsrc->name, NAME);

	pRsrc->refLSeries = 0xff;
	pRsrc->refRSeries = 0xff;
	pRsrc->runTmr = 0;

	/* register functions */
	pDev->RegSetAbsPos = 	cSPIN_SetAbsPos;
	pDev->RegSetElPos = 	cSPIN_SetElPos;
	pDev->RegSetMarkPos = 	cSPIN_SetMarkPos;

	pDev->RegGetAbsPos = 	cSPIN_GetAbsPos;
	pDev->RegGetElPos = 	cSPIN_GetElPos;
	pDev->RegGetMarkPos = 	cSPIN_GetMarkPos;
	pDev->RegGetSpd_StepPerS = 	cSPIN_GetSpd_stepPerS;

	pDev->RegSetAcc_stepPerSS = 	cSPIN_SetAcc;
	pDev->RegSetDec_stepPerSS = 	cSPIN_SetDec;
	pDev->RegSetMaxSpd_stepPerS = 	cSPIN_SetMaxSpd;
	pDev->RegSetMinSpd_stepPerS = 	cSPIN_SetMinSpd;
	pDev->RegSetLowSpdOpt = 	cSPIN_SetLowSpdOpt;
	pDev->RegSetFSSpd_stepPerS = 	cSPIN_SetFSSpd;

	pDev->RegSetBoostMode = 	cSPIN_SetBoostMode;

	pDev->RegSetKVal = cSPIN_SetKVal;
	pDev->RegGetKVal = cSPIN_GetKVal;
//	pDev->RegSetKValHold = 	cSPIN_SetKValHold;
//	pDev->RegSetKValRun = 	cSPIN_SetKValRun;
//	pDev->RegSetKValAcc = 	cSPIN_SetKValAcc;
//	pDev->RegSetKValDec = 	cSPIN_SetKValDec;
//	pDev->RegGetKValHold = 	cSPIN_GetKValHold;
//	pDev->RegGetKValRun = 	cSPIN_GetKValRun;
//	pDev->RegGetKValAcc = 	cSPIN_GetKValAcc;
//	pDev->RegGetKValDec = 	cSPIN_GetKValDec;

	pDev->RegGetAcc_stepPerSS = 	cSPIN_GetAcc;
	pDev->RegGetDec_stepPerSS = 	cSPIN_GetDec;
	pDev->RegGetMaxSpd_stepPerS = 	cSPIN_GetMaxSpd;
	pDev->RegGetMinSpd_stepPerS = 	cSPIN_GetMinSpd;
	pDev->RegGetLowSpdOpt = 	cSPIN_GetLowSpdOpt;
	pDev->RegGetFSSpd_stepPerS = 	cSPIN_GetFSSpd;
	pDev->RegGetBoostMode = 	cSPIN_GetBoostMode;

	pDev->RegSetIntSpd_stepPerS = 	cSPIN_SetIntSpd;
	pDev->RegGetIntSpd_stepPerS = 	cSPIN_GetIntSpd;
	pDev->RegSetStartSlp = 	cSPIN_SetStartSlp;
	pDev->RegGetStartSlp = 	cSPIN_GetStartSlp;
	pDev->RegSetFnSlpAcc = 	cSPIN_SetFnSlpAcc;
	pDev->RegGetFnSlpAcc = 	cSPIN_GetFnSlpAcc;
	pDev->RegSetFnSlpDec = 	cSPIN_SetFnSlpDec;
	pDev->RegGetFnSlpDec = 	cSPIN_GetFnSlpDec;
	pDev->RegSetKTherm = 	cSPIN_SetKTherm;
	pDev->RegGetKTherm = 	cSPIN_GetKTherm;
	pDev->RegGetAdcOut = 	cSPIN_GetAdcOut;
	pDev->RegSetOCDTh = 	cSPIN_SetOCDTh;
	pDev->RegGetOCDTh = 	cSPIN_GetOCDTh;
	pDev->RegSetStallTh = 	cSPIN_SetStallTh;
	pDev->RegGetStallTh = 	cSPIN_GetStallTh;
	pDev->RegSetStepSel = 	cSPIN_SetStepSel;
	pDev->RegGetStepSel = 	cSPIN_GetStepSel;
	pDev->RegSetSyncSel = 	cSPIN_SetSyncSel;
	pDev->RegGetSyncSel = 	cSPIN_GetSyncSel;
	pDev->RegSetSyncEn = 	cSPIN_SetSyncEn;
	pDev->RegGetSyncEn = 	cSPIN_GetSyncEn;
	if(pDev->rsrc.chip == CHIP_L6480){
		pDev->RegSetGateTcc = 	cSPIN_SetGateTcc;
		pDev->RegGetGateTcc = 	cSPIN_GetGateTcc;
		pDev->RegSetGateCurrent = 	cSPIN_SetGateCurrent;
		pDev->RegGetGateCurrent = 	cSPIN_GetGateCurrent;
		pDev->RegSetGateTBoost = 	cSPIN_SetGateTBoost;
		pDev->RegGetGateTBoost = 	cSPIN_GetGateTBoost;
		pDev->RegSetGateTDeadTime = 	cSPIN_SetGateTDeadTime;
		pDev->RegGetGateTDeadTime = 	cSPIN_GetGateTDeadTime;
		pDev->RegSetGateTBlank = 	cSPIN_SetGateTBlank;
		pDev->RegGetGateTBlank = 	cSPIN_GetGateTBlank;
	}
	
	pDev->SaveToROM = cspinSaveToEEPROM;
	pDev->RegSetCofig = cSPIN_SetConfig;
	pDev->RegGetCofig = cSPIN_GetConfig;
	pDev->RegSetSwMode = cSPIN_SetSwMode;
	pDev->RegGetSwMode = cSPIN_GetSwMode;
	pDev->RegSetVsCompEn = cSPIN_SetVsCompEn;
	pDev->RegGetVsCompEn = cSPIN_GetVsCompEn;
	pDev->RegSetOCSDEn = cSPIN_SetOCSDEn; 
	pDev->RegGetOCSDEn = cSPIN_GetOCSDEn;
	pDev->RegSetUVLOVal =cSPIN_SetUVLOVal; 
	pDev->RegGetUVLOVal =cSPIN_GetUVLOVal; 
	pDev->RegSetVccVal = cSPIN_SetVccVal;
	pDev->RegGetVccVal = cSPIN_GetVccVal; 
	pDev->RegSetPwmFrq = cSPIN_SetPwmFrq;
	pDev->RegGetPwmFrq = cSPIN_GetPwmFrq;

	pDev->Set_Param 	= cSPIN_Set_Param;
	pDev->Get_Param		=	cSPIN_Get_Param;
	
	pDev->Run			=	cSPIN_Run;
	pDev->Step_Clock	=	cSPIN_Step_Clock;
	pDev->Move_microStep	=	cSPIN_Move;
	pDev->Move_Step 	= cSPIN_MoveStep;
	
	pDev->Go_To			=	cSPIN_Go_To;
	pDev->Go_To_Dir		=	cSPIN_Go_To_Dir;
	pDev->Go_Until		=	cSPIN_Go_Until;
	pDev->Release_SW	=	cSPIN_Release_SW;
	pDev->Go_Home		=	cSPIN_Go_Home;
	pDev->Go_Mark		=	cSPIN_Go_Mark;
	pDev->Reset_Pos		=	cSPIN_Reset_Pos;
	pDev->Soft_Stop		=	cSPIN_Soft_Stop;
	pDev->Hard_Stop		=	cSPIN_Hard_Stop;
	pDev->Soft_HiZ		=	cSPIN_Soft_HiZ;
	pDev->Hard_HiZ		=	cSPIN_Hard_HiZ;
	pDev->Get_Status	=	cSPIN_Get_Status;
	pDev->ResetDev		= 	cSPIN_Reset_Device;
	pDev->Default 		= cspinDefault;
	pDev->PrintRegs 	= cspinPrintAll;
	
	pDev->Polling 		= cSPIN_tickExe;
		
	pDev->Busy_HW		=	cSPIN_Busy_HW;
	pDev->Busy_SW		=	cSPIN_Busy_SW;
	pDev->Flag			=	cSPIN_Flag;
	
	pDev->SaveToROM 	= cspinSaveToEEPROM;
	pDev->Default 	= cspinDefault;
	
	pDev->rsrc.print = print;
	pDev->rsrc.EepromRead = EepromRead;
	pDev->rsrc.EepromWrite = EepromWrite;
	pDev->rsrc.eepromAddrBase = romBase;
	
	/* Resets and puts cSPIN into standby mode */
	cSPIN_Reset_Device(pRsrc);	//software reset

	regDefault(pRsrc, &regs);
	if(cspinEepromReadAllReg(pRsrc, &regs) < 0){
		/* Load default to reg */
		regDefault(pRsrc, &regs);
		cspinEepromWriteAllReg(pRsrc, &regs);
		pRsrc->flags |= 1<<MOTOR_EEPROM_ERR_BIT;
	}
	cSPIN_Registers_SetAll(pRsrc, &regs);

	/* Clear Flag pin */
	// reg.STATUS = cSPIN_Get_Status(pRsrc);
}

static void regDefault(cSPIN_Rsrc_T* pRsrc, cSPIN_RegsStruct_TypeDef *reg){
	/***********************************************
	@ Load register val from cspin_config.h
	***********************************************/
	/* Acceleration rate settings to cSPIN_CONF_PARAM_ACC in steps/s2, range 14.55 to 59590 steps/s2 */
	reg->ACC 		= AccDec_Steps_to_Par(cSPIN_CONF_PARAM_ACC);
	/* Deceleration rate settings to cSPIN_CONF_PARAM_DEC in steps/s2, range 14.55 to 59590 steps/s2 */
	reg->DEC 		= AccDec_Steps_to_Par(cSPIN_CONF_PARAM_DEC); 
	/* Maximum speed settings to cSPIN_CONF_PARAM_MAX_SPEED in steps/s, range 15.25 to 15610 steps/s */
	reg->MAX_SPEED 	= MaxSpd_Steps_to_Par(cSPIN_CONF_PARAM_MAX_SPEED);
	/* Full step speed settings cSPIN_CONF_PARAM_FS_SPD in steps/s, range 7.63 to 15625 steps/s */
	reg->FS_SPD 	= FSSpd_Steps_to_Par(cSPIN_CONF_PARAM_FS_SPD);
	if(pRsrc->chip==CHIP_L6480 || pRsrc->chip==CHIP_L6470){
		/* Minimum speed settings to cSPIN_CONF_PARAM_MIN_SPEED in steps/s, range 0 to 976.3 steps/s */
		reg->MIN_SPEED	= cSPIN_CONF_PARAM_LSPD_BIT|MinSpd_Steps_to_Par(cSPIN_CONF_PARAM_MIN_SPEED);
		/* Acceleration duty cycle (torque) settings to cSPIN_CONF_PARAM_KVAL_ACC in %, range 0 to 99.6% */
		reg->KVAL_ACC 	= Kval_Perc_to_Par(cSPIN_CONF_PARAM_KVAL_ACC);
		/* Deceleration duty cycle (torque) settings to cSPIN_CONF_PARAM_KVAL_DEC in %, range 0 to 99.6% */
		reg->KVAL_DEC 	= Kval_Perc_to_Par(cSPIN_CONF_PARAM_KVAL_DEC);		
		/* Run duty cycle (torque) settings to cSPIN_CONF_PARAM_KVAL_RUN in %, range 0 to 99.6% */
		reg->KVAL_RUN 	= Kval_Perc_to_Par(cSPIN_CONF_PARAM_KVAL_RUN);
		/* Hold duty cycle (torque) settings to cSPIN_CONF_PARAM_KVAL_HOLD in %, range 0 to 99.6% */
		reg->KVAL_HOLD 	= Kval_Perc_to_Par(cSPIN_CONF_PARAM_KVAL_HOLD);
		/* Thermal compensation param settings to cSPIN_CONF_PARAM_K_THERM, range 1 to 1.46875 */
		reg->K_THERM 	= KTherm_to_Par(cSPIN_CONF_PARAM_K_THERM);
		/* Intersect speed settings for BEMF compensation to cSPIN_CONF_PARAM_INT_SPD in steps/s, range 0 to 3906 steps/s */
		reg->INT_SPD 	= IntSpd_Steps_to_Par(cSPIN_CONF_PARAM_INT_SPD);
		/* BEMF start slope settings for BEMF compensation to cSPIN_CONF_PARAM_ST_SLP in % step/s, range 0 to 0.4% s/step */
		reg->ST_SLP 	= BEMF_Slope_Perc_to_Par(cSPIN_CONF_PARAM_ST_SLP);
		/* BEMF final acc slope settings for BEMF compensation to cSPIN_CONF_PARAM_FN_SLP_ACC in% step/s, range 0 to 0.4% s/step */
		reg->FN_SLP_ACC = BEMF_Slope_Perc_to_Par(cSPIN_CONF_PARAM_FN_SLP_ACC);
		/* BEMF final dec slope settings for BEMF compensation to cSPIN_CONF_PARAM_FN_SLP_DEC in% step/s, range 0 to 0.4% s/step */
		reg->FN_SLP_DEC = BEMF_Slope_Perc_to_Par(cSPIN_CONF_PARAM_FN_SLP_DEC);
		/* Stall threshold settings to cSPIN_CONF_PARAM_STALL_TH in mV, range 31.25 to 1000mV */
		reg->STALL_TH 	= StallTh_to_Par(cSPIN_CONF_PARAM_STALL_TH);
	}
//	if(pRsrc->chip==6482){
//        /* Minimum speed settings to cSPIN_CONF_PARAM_MIN_SPEED in steps/s, range 0 to 976.3 steps/s */
//	reg->MIN_SPEED	= MinSpd_Steps_to_Par(cSPIN_CONF_PARAM_MIN_SPEED);
//        /* Reference voltage assigned to the torque regulation DAC during motor acceleration, range 7.8mV to 1000mV */
//	reg->TVAL_ACC 	= Tval_Current_to_Par(cSPIN_CONF_PARAM_TVAL_ACC);
//        /* Reference voltage assigned to the torque regulation DAC during motor deceleration, range 7.8mV to 1000mV */
//	reg->TVAL_DEC 	= Tval_Current_to_Par(cSPIN_CONF_PARAM_TVAL_DEC);		
//        /* Reference voltage assigned to the torque regulation DAC when motor is running, range 7.8mV to 1000mV */
//	reg->TVAL_RUN 	= Tval_Current_to_Par(cSPIN_CONF_PARAM_TVAL_RUN);
//        /* Reference voltage assigned to the torque regulation DAC when motor is stopped, range 7.8mV to 1000mV */
//	reg->TVAL_HOLD 	= Tval_Current_to_Par(cSPIN_CONF_PARAM_TVAL_HOLD);
//				/* Maximum fast decay and fall step times used by the current control system, range 2us to 32us */
//	reg->T_FAST 	= (u8)cSPIN_CONF_PARAM_TOFF_FAST | (u8)cSPIN_CONF_PARAM_FAST_STEP;
//				/* Minimum ON time value used by the current control system, range 0.5us to 64us */
//	reg->TON_MIN 	= Tmin_Time_to_Par(cSPIN_CONF_PARAM_TON_MIN);
//				/* Minimum OFF time value used by the current control system, range 0.5us to 64us */
//	reg->TOFF_MIN	= Tmin_Time_to_Par(cSPIN_CONF_PARAM_TOFF_MIN);
//				/* Set Config register according to config parameters */
//				/* clock setting, switch hard stop interrupt mode, */
//				/*  supply voltage compensation, overcurrent shutdown */
//        /* UVLO threshold, VCC reg output voltage , target switching period, predictive current control */
//	reg->CONFIG 	= (u16)cSPIN_CONF_PARAM_CLOCK_SETTING | \
//                                          (u16)cSPIN_CONF_PARAM_SW_MODE	   | \
//                                          (u16)cSPIN_CONF_PARAM_TQ_REG     | \
//                                          (u16)cSPIN_CONF_PARAM_OC_SD      | \
//                                          (u16)cSPIN_CONF_PARAM_UVLOVAL    | \
//                                          (u16)cSPIN_CONF_PARAM_VCCVAL	   | \
//                                          (u16)cSPIN_CONF_PARAM_TSW        | \
//                                          (u16)cSPIN_CONF_PARAM_PRED;
//	}
	/* Overcurrent threshold settings to cSPIN_CONF_PARAM_OCD_TH, range 31.25 to 1000mV */
	reg->OCD_TH 	= cSPIN_CONF_PARAM_OCD_TH;        
  /* Alarm settings to cSPIN_CONF_PARAM_ALARM_EN */
	reg->ALARM_EN 	= cSPIN_CONF_PARAM_ALARM_EN;
  /* Step mode and sycn mode settings via cSPIN_CONF_PARAM_SYNC_MODE and cSPIN_CONF_PARAM_STEP_MODE */
	reg->STEP_MODE 	= (u8)cSPIN_CONF_PARAM_SYNC_MODE | \
                          (u8)cSPIN_CONF_PARAM_STEP_MODE;
	if(pRsrc->chip==CHIP_L6480){
		/* Sink/source current, duration of constant current phases, duration of overboost phase settings */
		reg->GATECFG1   = (u16)cSPIN_CONF_PARAM_IGATE | \
														(u16)cSPIN_CONF_PARAM_TCC   | \
														(u16)cSPIN_CONF_PARAM_TBOOST;
		/* Blank time, Dead time stiings */
		 reg->GATECFG2  = (u16)cSPIN_CONF_PARAM_TBLANK | \
														(u16)cSPIN_CONF_PARAM_TDT;
		/* Set Config register according to config parameters */
		/* clock setting, switch hard stop interrupt mode, */
		/*  supply voltage compensation, overcurrent shutdown */
		/* UVLO threshold, VCC reg output voltage , PWM frequency */
		reg->CONFIG 	= (u16)cSPIN_CONF_PARAM_CLOCK_SETTING | \
													(u16)cSPIN_CONF_PARAM_SW_MODE	  | \
													(u16)cSPIN_CONF_PARAM_VS_COMP		| \
													(u16)cSPIN_CONF_PARAM_OC_SD      | \
													(u16)cSPIN_CONF_PARAM_UVLOVAL    | \
													(u16)cSPIN_CONF_PARAM_VCCVAL	   	| \
													(u16)cSPIN_CONF_PARAM_PWM_DIV    | \
													(u16)cSPIN_CONF_PARAM_PWM_MUL;
	}
	else if(pRsrc->chip==CHIP_L6470){
		/* Set Config register according to config parameters */
		/* clock setting, switch hard stop interrupt mode, */
		/*  supply voltage compensation, overcurrent shutdown */
		/* UVLO threshold, VCC reg output voltage , PWM frequency */
		reg->CONFIG 	= (u16)cSPIN_CONF_PARAM_CLOCK_SETTING | \
													(u16)cSPIN_CONF_PARAM_SW_MODE	  | \
													(u16)cSPIN_CONF_PARAM_VS_COMP		| \
													(u16)cSPIN_CONF_PARAM_OC_SD      | \
													(u16)cSPIN_CONF_PARAM_PWM_DIV    | \
													(u16)cSPIN_CONF_PARAM_PWM_MUL;
	}
}


/**
  * @brief  Issues cSPIN NOP command.
  * @param  None
  * @retval None
  */
//void cSPIN_Nop(cSPIN_Rsrc_T* pRsrc)
//{
//	/* Send NOP operation code to cSPIN */
//	cSPIN_Write_Byte(pRsrc,cSPIN_NOP);
//}

/**
  * @brief  Issues cSPIN Set Param command.
  * @param  param cSPIN register address
  * @param  value to be set
  * @retval None
  */
void cSPIN_Set_Param(cSPIN_Rsrc_T* pRsrc, cSPIN_Regs_T param, u32 value)
{
	/* Send SetParam operation code to cSPIN */
	cSPIN_Write_Byte(pRsrc, (u8)cSPIN_SET_PARAM | (u8)param);
	if(pRsrc->chip == CHIP_L6470){
		switch (param)
		{
			/* These following REG need 3 bytes */
			case cSPIN_ABS_POS: ;
			case cSPIN_MARK: ;
				/* Send parameter - byte 2 to cSPIN */
				cSPIN_Write_Byte(pRsrc, (u8)(value >> 16));	
			/* These following REG need 2 bytes */
			case cSPIN_EL_POS: ;
			case cSPIN_ACC: ;
			case cSPIN_DEC: ;
			case cSPIN_MAX_SPEED: ;
			case cSPIN_MIN_SPEED: ;
			case cSPIN_FS_SPD: ;
			case cSPIN_INT_SPD: ;				
			case cSPIN6470_CONFIG:
			case cSPIN6470_STATUS:
				/* Send parameter - byte 1 to cSPIN */
					cSPIN_Write_Byte(pRsrc, (u8)(value >> 8));
			/* These following REG need 1 bytes */
			default:
				/* Send parameter - byte 0 to cSPIN */
					cSPIN_Write_Byte(pRsrc, (u8)(value));
		}
	}
	else if(pRsrc->chip == CHIP_L6480){
		switch (param)
		{
			/* These following REG need 3 bytes */
			case cSPIN_ABS_POS: ;
			case cSPIN_MARK: ;
				/* Send parameter - byte 2 to cSPIN */
				cSPIN_Write_Byte(pRsrc, (u8)(value >> 16));	
			/* These following REG need 2 bytes */
			case cSPIN_EL_POS: ;
			case cSPIN_ACC: ;
			case cSPIN_DEC: ;
			case cSPIN_MAX_SPEED: ;
			case cSPIN_MIN_SPEED: ;
			case cSPIN_FS_SPD: ;
			case cSPIN_INT_SPD: ;
			case cSPIN6480_GATECFG1: 
			case cSPIN6480_CONFIG:
			case cSPIN6480_STATUS:
				/* Send parameter - byte 1 to cSPIN */
					cSPIN_Write_Byte(pRsrc, (u8)(value >> 8));
			/* These following REG need 1 bytes */
			default:
				/* Send parameter - byte 0 to cSPIN */
					cSPIN_Write_Byte(pRsrc, (u8)(value));
		}
	}
}

/**
  * @brief  Issues cSPIN Get Param command.
  * @param  param cSPIN register address
  * @retval Register value - 1 to 3 bytes (depends on register)
  */
u32 cSPIN_Get_Param(cSPIN_Rsrc_T* pRsrc, cSPIN_Regs_T param)
{
	u32 rx = 0;
	/* Send GetParam operation code to cSPIN */
	cSPIN_Write_Byte(pRsrc, (u8)cSPIN_GET_PARAM | (u8)param);
	/* read back data */
	if(pRsrc->chip == CHIP_L6470){
		switch (param)
		{
			case cSPIN_ABS_POS: 
			case cSPIN_MARK: 
			case cSPIN_SPEED:
				rx = cSPIN_Write_Byte(pRsrc, 0x00);		rx <<= 8;
			case cSPIN_EL_POS: 
			case cSPIN_ACC: 
			case cSPIN_DEC: 
			case cSPIN_MAX_SPEED: 
			case cSPIN_MIN_SPEED: 
			case cSPIN_FS_SPD: 
			case cSPIN_INT_SPD: 
			case cSPIN6470_CONFIG: 
			case cSPIN6470_STATUS:
				rx |= cSPIN_Write_Byte(pRsrc, 0x00);		rx <<= 8;
			default:
				rx |= cSPIN_Write_Byte(pRsrc, 0x00);		
		}
	}
	else if(pRsrc->chip == CHIP_L6480){
		switch (param)
		{
			case cSPIN_ABS_POS: 
			case cSPIN_MARK: 
			case cSPIN_SPEED:
				rx = cSPIN_Write_Byte(pRsrc, 0x00);		rx <<= 8;
			case cSPIN_EL_POS: 
			case cSPIN_ACC: 
			case cSPIN_DEC: 
			case cSPIN_MAX_SPEED: 
			case cSPIN_MIN_SPEED: 
			case cSPIN_FS_SPD: 
			case cSPIN_INT_SPD: 
			case cSPIN6480_GATECFG1: 
			case cSPIN6480_CONFIG: 
			case cSPIN6480_STATUS:
				rx |= cSPIN_Write_Byte(pRsrc, 0x00);		rx <<= 8;
			default:
				rx |= cSPIN_Write_Byte(pRsrc, 0x00);		
		}
	}
	return rx;
}

/*****************************************
  * @brief  Issues cSPIN registers ops
  * @param  value, 22bit
  * @param  
  * @retval None
*****************************************/
u8 isCmdErr(cSPIN_Rsrc_T* pRsrc){
	cSPIN_Delay(48);	//about 1us @ 48MHz	
	if(cSPIN_Get_Status(pRsrc) & BIT(7) )	return TRUE;
	return FALSE;		
}

/*****************************************
  * @brief  Issues cSPIN registers ops
  * @param  true value, 22bit
  * @param  
  * @retval None
*****************************************/
u8 cSPIN_SetAbsPos			(cSPIN_Rsrc_T* pRsrc, u32 param){
	cSPIN_Set_Param(pRsrc, cSPIN_ABS_POS, param);
	return isCmdErr(pRsrc);
}
u8 cSPIN_GetAbsPos			(cSPIN_Rsrc_T* pRsrc, u32 *param){
	*param = cSPIN_Get_Param(pRsrc, cSPIN_ABS_POS);
	return isCmdErr(pRsrc);
}

u8 cSPIN_SetElPos			(cSPIN_Rsrc_T* pRsrc, u16 param){
	cSPIN_Set_Param(pRsrc, cSPIN_EL_POS, param);
	return isCmdErr(pRsrc);
}
u8 cSPIN_GetElPos			(cSPIN_Rsrc_T* pRsrc, u16 *param){
	*param = (u16)cSPIN_Get_Param(pRsrc, cSPIN_EL_POS);
	return isCmdErr(pRsrc);
}

u8 cSPIN_SetMarkPos		(cSPIN_Rsrc_T* pRsrc, u32 param){
	cSPIN_Set_Param(pRsrc, cSPIN_MARK, param);
	return isCmdErr(pRsrc);
}
u8 cSPIN_GetMarkPos		(cSPIN_Rsrc_T* pRsrc, u32 *param){
	*param = cSPIN_Get_Param(pRsrc, cSPIN_MARK);
	return isCmdErr(pRsrc);
}

u8 cSPIN_GetSpd_stepPerS	(cSPIN_Rsrc_T* pRsrc, u32 *param){
	*param = cSPIN_Get_Param(pRsrc, cSPIN_SPEED);
	// convert to step/sec
	*param *= 3;
	*param /=200;
	return isCmdErr(pRsrc);
}
// in step/(s*s)
u8 cSPIN_SetAcc	(cSPIN_Rsrc_T* pRsrc, u16 param){
	u32 x = param*100;
	x /= 1455;
	cSPIN_Set_Param(pRsrc, cSPIN_ACC, AccDec_Steps_to_Par(param));
	return isCmdErr(pRsrc);
}
u8 cSPIN_GetAcc	(cSPIN_Rsrc_T* pRsrc, u16 *param){
	u32 x=0;
	x = (u16)cSPIN_Get_Param(pRsrc, cSPIN_ACC);
	*param = Par_toAccDec_Steps(x);
	return isCmdErr(pRsrc);
}

u8 cSPIN_SetDec	(cSPIN_Rsrc_T* pRsrc, u16 param){
	u32 x = param*100;
	x /= 1455;
	cSPIN_Set_Param(pRsrc, cSPIN_DEC, AccDec_Steps_to_Par(param));
	return isCmdErr(pRsrc);
}
u8 cSPIN_GetDec	(cSPIN_Rsrc_T* pRsrc, u16 *param){
	u32 x=0;
	x = (u16)cSPIN_Get_Param(pRsrc, cSPIN_DEC);
	x *= 1455;
	*param = x/100 + (x%100 >50?1:0);
	return isCmdErr(pRsrc);
}

// in 15.25 step/s
u8 cSPIN_SetMaxSpd	(cSPIN_Rsrc_T* pRsrc, u16 param){
	u32 x = param;
	x <<= 4;
	x = x/244 + (x%244>=122?1:0);
	cSPIN_Set_Param(pRsrc, cSPIN_MAX_SPEED, x);
	return isCmdErr(pRsrc);
}

u8 cSPIN_GetMaxSpd		(cSPIN_Rsrc_T* pRsrc, u16 *param){
	u32 x;
	// x*15.25 = x*(15.25*16)/16 = x*244/16
	x = (u16)cSPIN_Get_Param(pRsrc, cSPIN_MAX_SPEED);
	x *= 244;
	*param = x/16;
	
	x %= 16;
	*param += x*953/1000;

	return isCmdErr(pRsrc);
}
// in 0.238 step/s
u8 cSPIN_SetMinSpd			(cSPIN_Rsrc_T* pRsrc, u16 param){
	u32 x;
	u16 reg = 0;
	
	// x/0.238 = x*1000/238
	x = param*1000;
	x = x/238;

	reg = (u16)cSPIN_Get_Param(pRsrc, cSPIN_MIN_SPEED);
	if(isCmdErr(pRsrc)==TRUE)	return TRUE;
	reg &= (1<<12);	reg |= (x&0x0fff);
	cSPIN_Set_Param(pRsrc, cSPIN_MIN_SPEED, reg);
	return isCmdErr(pRsrc);
}

u8 cSPIN_GetMinSpd			(cSPIN_Rsrc_T* pRsrc, u16 *param){
	u32 x;
	x = (u16)cSPIN_Get_Param(pRsrc, cSPIN_MIN_SPEED)&0x0fff;
	x *= 238;
	*param = x/1000;// + (x%1000>500?1:0);
	return isCmdErr(pRsrc);
}

u8 cSPIN_SetLowSpdOpt		(cSPIN_Rsrc_T* pRsrc, u8 param){
	u16 reg = 0;
	reg = (u16)cSPIN_Get_Param(pRsrc, cSPIN_MIN_SPEED)&0x0fff;
	if(isCmdErr(pRsrc)==TRUE)	return TRUE;
	if(param == FALSE)
		cSPIN_Set_Param(pRsrc, cSPIN_MIN_SPEED, reg);
	else
		cSPIN_Set_Param(pRsrc, cSPIN_MIN_SPEED, reg|(1<<12));
	return isCmdErr(pRsrc);
}
u8 cSPIN_GetLowSpdOpt			(cSPIN_Rsrc_T* pRsrc, u8 *param){
	if(cSPIN_Get_Param(pRsrc, cSPIN_MIN_SPEED)&(1<<12))
		*param = TRUE;
	else
		*param = FALSE;
	return isCmdErr(pRsrc);
}

u8 cSPIN_SetFSSpd			(cSPIN_Rsrc_T* pRsrc, u16 param){
	u16 reg = 0;
	u32 x = param;
	reg = (u16)cSPIN_Get_Param(pRsrc, cSPIN_FS_SPD);
	if(isCmdErr(pRsrc)==TRUE)	return TRUE;
	reg &= (1<<10);	
	// x = param/15.25 = param*16/(15.25*16) = param * 16 / 244
	x = (x<<4)/244;
	reg |= (x&(0xffff>>(16-10)));
	cSPIN_Set_Param(pRsrc, cSPIN_FS_SPD, reg);
	return isCmdErr(pRsrc);
}
u8 cSPIN_GetFSSpd			(cSPIN_Rsrc_T* pRsrc, u16 *param){
	u32 x = (u16)cSPIN_Get_Param(pRsrc, cSPIN_FS_SPD)&(0xffff>>(16-10));
	// x*15.25 = x*(15.25*16)/16=x*244/16
	*param = (x*244)>>4;
	return isCmdErr(pRsrc);
}

u8 cSPIN_SetBoostMode		(cSPIN_Rsrc_T* pRsrc, u8 param){
	u16 reg = 0;
	reg = (u16)cSPIN_Get_Param(pRsrc, cSPIN_FS_SPD)&(0xffff>>(16-10));
	if(isCmdErr(pRsrc)==TRUE)	return TRUE;
	if(param == FALSE)
		cSPIN_Set_Param(pRsrc, cSPIN_FS_SPD, reg);
	else
		cSPIN_Set_Param(pRsrc, cSPIN_FS_SPD, reg|(1<<10));
	return isCmdErr(pRsrc);
}
u8 cSPIN_GetBoostMode			(cSPIN_Rsrc_T* pRsrc, u8 *param){
	if(cSPIN_Get_Param(pRsrc, cSPIN_FS_SPD)&(1<<10))
		*param = TRUE;
	else
		*param = FALSE;
	return isCmdErr(pRsrc);
}

u8 cSPIN_SetKVal		(cSPIN_Rsrc_T* pRsrc, u8 kHold, u8 kRun, u8 kAcc, u8 kDec){
	cSPIN_Set_Param(pRsrc, cSPIN_KVAL_HOLD, kHold);	cSPIN_Delay(50);
	cSPIN_Set_Param(pRsrc, cSPIN_KVAL_RUN, kRun);	cSPIN_Delay(50);
	cSPIN_Set_Param(pRsrc, cSPIN_KVAL_ACC, kAcc);	cSPIN_Delay(50);
	cSPIN_Set_Param(pRsrc, cSPIN_KVAL_DEC, kDec);
	return isCmdErr(pRsrc);
}

u8 cSPIN_GetKVal		(cSPIN_Rsrc_T* pRsrc, u8* kHold, u8* kRun, u8* kAcc, u8* kDec){
	*kHold = (u8)cSPIN_Get_Param(pRsrc, cSPIN_KVAL_HOLD);	cSPIN_Delay(50);
	*kRun = (u8)cSPIN_Get_Param(pRsrc, cSPIN_KVAL_RUN);	cSPIN_Delay(50);
	*kAcc = (u8)cSPIN_Get_Param(pRsrc, cSPIN_KVAL_ACC);	cSPIN_Delay(50);
	*kDec = (u8)cSPIN_Get_Param(pRsrc, cSPIN_KVAL_DEC);
	return isCmdErr(pRsrc);
}

//u8 cSPIN_SetKValHold (cSPIN_Rsrc_T* pRsrc, u8 param){
//	cSPIN_Set_Param(pRsrc, cSPIN_KVAL_HOLD, param);
//	return isCmdErr(pRsrc);
//}
//u8 cSPIN_GetKValHold (cSPIN_Rsrc_T* pRsrc, u8 *param){
//	*param = (u8)cSPIN_Get_Param(pRsrc, cSPIN_KVAL_HOLD);
//	return isCmdErr(pRsrc);
//}
//u8 cSPIN_SetKValRun (cSPIN_Rsrc_T* pRsrc, u8 param){
//	cSPIN_Set_Param(pRsrc, cSPIN_KVAL_RUN, param);
//	return isCmdErr(pRsrc);
//}
//u8 cSPIN_GetKValRun (cSPIN_Rsrc_T* pRsrc, u8 *param){
//	*param = (u8)cSPIN_Get_Param(pRsrc, cSPIN_KVAL_RUN);
//	return isCmdErr(pRsrc);
//}
//u8 cSPIN_SetKValAcc (cSPIN_Rsrc_T* pRsrc, u8 param){
//	cSPIN_Set_Param(pRsrc, cSPIN_KVAL_ACC, param);
//	return isCmdErr(pRsrc);
//}
//u8 cSPIN_GetKValAcc (cSPIN_Rsrc_T* pRsrc, u8 *param){
//	*param = (u8)cSPIN_Get_Param(pRsrc, cSPIN_KVAL_ACC);
//	return isCmdErr(pRsrc);
//}
//u8 cSPIN_SetKValDec (cSPIN_Rsrc_T* pRsrc, u8 param){
//	cSPIN_Set_Param(pRsrc, cSPIN_KVAL_DEC, param);
//	return isCmdErr(pRsrc);
//}
//u8 cSPIN_GetKValDec (cSPIN_Rsrc_T* pRsrc, u8 *param){
//	*param = (u8)cSPIN_Get_Param(pRsrc, cSPIN_KVAL_DEC);
//	return isCmdErr(pRsrc);
//}

u8 cSPIN_SetIntSpd (cSPIN_Rsrc_T* pRsrc, u16 param){
	u32 x;
	
	// x = param/0.0596 = param*10000/(0.0596*10000)
	x = param;
	x = x*10000/596;
	
	cSPIN_Set_Param(pRsrc, cSPIN_INT_SPD, x&0xffff);
	return isCmdErr(pRsrc);
}
u8 cSPIN_GetIntSpd (cSPIN_Rsrc_T* pRsrc, u16 *param){
	u32 x;
	x = (u16)cSPIN_Get_Param(pRsrc, cSPIN_INT_SPD);
	
	// *param = x*0.0596 = x*596/10000
	*param = x*596/10000;
	
	return isCmdErr(pRsrc);
}

u8 cSPIN_SetStartSlp (cSPIN_Rsrc_T* pRsrc, u8 param){
	cSPIN_Set_Param(pRsrc, cSPIN_ST_SLP, param);
	return isCmdErr(pRsrc);
}
u8 cSPIN_GetStartSlp (cSPIN_Rsrc_T* pRsrc, u8 *param){
	*param = (u8)cSPIN_Get_Param(pRsrc, cSPIN_ST_SLP);
	return isCmdErr(pRsrc);
}

u8 cSPIN_SetFnSlpAcc (cSPIN_Rsrc_T* pRsrc, u8 param){
	cSPIN_Set_Param(pRsrc, cSPIN_FN_SLP_ACC, param);
	return isCmdErr(pRsrc);
}
u8 cSPIN_GetFnSlpAcc (cSPIN_Rsrc_T* pRsrc, u8 *param){
	*param = (u8)cSPIN_Get_Param(pRsrc, cSPIN_FN_SLP_ACC);
	return isCmdErr(pRsrc);
}

u8 cSPIN_SetFnSlpDec (cSPIN_Rsrc_T* pRsrc, u8 param){
	cSPIN_Set_Param(pRsrc, cSPIN_FN_SLP_DEC, param);
	return isCmdErr(pRsrc);
}
u8 cSPIN_GetFnSlpDec (cSPIN_Rsrc_T* pRsrc, u8 *param){
	*param = (u8)cSPIN_Get_Param(pRsrc, cSPIN_FN_SLP_DEC);
	return isCmdErr(pRsrc);
}

u8 cSPIN_SetKTherm (cSPIN_Rsrc_T* pRsrc, u8 param){
	cSPIN_Set_Param(pRsrc, cSPIN_K_THERM, param);
	return isCmdErr(pRsrc);
}
u8 cSPIN_GetKTherm (cSPIN_Rsrc_T* pRsrc, u8 *param){
	*param = (u8)cSPIN_Get_Param(pRsrc, cSPIN_K_THERM);
	return isCmdErr(pRsrc);
}

u8 cSPIN_GetAdcOut (cSPIN_Rsrc_T* pRsrc, u8 *param){
	*param = (u8)cSPIN_Get_Param(pRsrc, cSPIN_ADC_OUT);
	return isCmdErr(pRsrc);
}

u8 cSPIN_SetOCDTh (cSPIN_Rsrc_T* pRsrc, u8 param){
	cSPIN_Set_Param(pRsrc, cSPIN_OCD_TH, param);
	return isCmdErr(pRsrc);
}
u8 cSPIN_GetOCDTh (cSPIN_Rsrc_T* pRsrc, u8 *param){
	*param = (u8)cSPIN_Get_Param(pRsrc, cSPIN_OCD_TH);
	return isCmdErr(pRsrc);
}

u8 cSPIN_SetStallTh (cSPIN_Rsrc_T* pRsrc, u8 param){
	cSPIN_Set_Param(pRsrc, cSPIN_STALL_TH, param);
	return isCmdErr(pRsrc);
}
u8 cSPIN_GetStallTh (cSPIN_Rsrc_T* pRsrc, u8 *param){
	*param = (u8)cSPIN_Get_Param(pRsrc, cSPIN_STALL_TH);
	return isCmdErr(pRsrc);
}

u8 cSPIN_SetStepSel (cSPIN_Rsrc_T* pRsrc, u8 param){
	u8 reg = 0;
	reg = (u8)cSPIN_Get_Param(pRsrc, cSPIN_STEP_MODE);
	if(isCmdErr(pRsrc)==TRUE)	return TRUE;
	reg &= (0xff<<3);	reg |= param;
	cSPIN_Set_Param(pRsrc, cSPIN_STEP_MODE, reg);
	return isCmdErr(pRsrc);
}
u8 cSPIN_GetStepSel (cSPIN_Rsrc_T* pRsrc, u8 *param){
	*param = (u8)cSPIN_Get_Param(pRsrc, cSPIN_STEP_MODE)&0x07;
	return isCmdErr(pRsrc);
}

u8 cSPIN_SetSyncSel (cSPIN_Rsrc_T* pRsrc, u8 param){
	u8 reg = 0;
	reg = (u8)cSPIN_Get_Param(pRsrc, cSPIN_STEP_MODE);
	if(isCmdErr(pRsrc)==TRUE)	return TRUE;
	reg &= (0xff^(0x07<<4));	reg |= (param&0x07)<<4;
	cSPIN_Set_Param(pRsrc, cSPIN_STEP_MODE, reg);
	return isCmdErr(pRsrc);
}
u8 cSPIN_GetSyncSel (cSPIN_Rsrc_T* pRsrc, u8 *param){
	*param = ((u8)cSPIN_Get_Param(pRsrc, cSPIN_STEP_MODE)>>4)&0x07;
	return isCmdErr(pRsrc);
}

u8 cSPIN_SetSyncEn (cSPIN_Rsrc_T* pRsrc, u8 param){
	u8 reg = 0;
	reg = (u8)cSPIN_Get_Param(pRsrc, cSPIN_STEP_MODE);
	if(isCmdErr(pRsrc)==TRUE)	return TRUE;
	reg &= 0x7f;	reg |= (param&0x01)<<7;
	cSPIN_Set_Param(pRsrc, cSPIN_STEP_MODE, reg);
	return isCmdErr(pRsrc);
}
u8 cSPIN_GetSyncEn (cSPIN_Rsrc_T* pRsrc, u8 *param){
	*param = ((u8)cSPIN_Get_Param(pRsrc, cSPIN_STEP_MODE)>>7)&0x01;
	return isCmdErr(pRsrc);
}

// only for L6480 start
u8 cSPIN_SetGateTcc (cSPIN_Rsrc_T* pRsrc, u8 param){
	u16 reg = 0;
	reg = (u16)cSPIN_Get_Param(pRsrc, cSPIN6480_GATECFG1);
	if(isCmdErr(pRsrc)==TRUE)	return TRUE;
	reg &= (0xffff<<5);	reg |= param&0x1f;
	cSPIN_Set_Param(pRsrc, cSPIN6480_GATECFG1, reg);
	return isCmdErr(pRsrc);
}
u8 cSPIN_GetGateTcc (cSPIN_Rsrc_T* pRsrc, u8 *param){
	*param = (u16)cSPIN_Get_Param(pRsrc, cSPIN6480_GATECFG1)&0x1f;
	return isCmdErr(pRsrc);
}

u8 cSPIN_SetGateCurrent (cSPIN_Rsrc_T* pRsrc, u8 param){
	u16 reg = 0;
	reg = (u16)cSPIN_Get_Param(pRsrc, cSPIN6480_GATECFG1);
	if(isCmdErr(pRsrc)==TRUE)	return TRUE;
	reg &= 0xff1f;	reg |= (param&0x07)<<5;
	cSPIN_Set_Param(pRsrc, cSPIN6480_GATECFG1, reg);
	return isCmdErr(pRsrc);
}
u8 cSPIN_GetGateCurrent (cSPIN_Rsrc_T* pRsrc, u8 *param){
	*param = (cSPIN_Get_Param(pRsrc, cSPIN6480_GATECFG1)>>5)&0x07;
	return isCmdErr(pRsrc);
}

u8 cSPIN_SetGateTBoost (cSPIN_Rsrc_T* pRsrc, u8 param){
	u16 reg = 0;
	reg = (u16)cSPIN_Get_Param(pRsrc, cSPIN6480_GATECFG1);
	if(isCmdErr(pRsrc)==TRUE)	return TRUE;
	reg &= 0xf8ff;	reg |= (u16)(param&0x07)<<8;
	cSPIN_Set_Param(pRsrc, cSPIN6480_GATECFG1, reg);
	return isCmdErr(pRsrc);
}
u8 cSPIN_GetGateTBoost (cSPIN_Rsrc_T* pRsrc, u8 *param){
	*param = (u8)(cSPIN_Get_Param(pRsrc, cSPIN6480_GATECFG1)>>8)&0x07;
	return isCmdErr(pRsrc);
}

u8 cSPIN_SetGateTDeadTime (cSPIN_Rsrc_T* pRsrc, u8 param){
	u8 reg = 0;
	reg = (u8)cSPIN_Get_Param(pRsrc, cSPIN6480_GATECFG2);
	if(isCmdErr(pRsrc)==TRUE)	return TRUE;
	reg &= 0xff<<5;	reg |= (param&0x1f);
	cSPIN_Set_Param(pRsrc, cSPIN6480_GATECFG2, reg);
	return isCmdErr(pRsrc);
}
u8 cSPIN_GetGateTDeadTime (cSPIN_Rsrc_T* pRsrc, u8 *param){
	*param = (u8)cSPIN_Get_Param(pRsrc, cSPIN6480_GATECFG2)&0x1f;
	return isCmdErr(pRsrc);
}

u8 cSPIN_SetGateTBlank (cSPIN_Rsrc_T* pRsrc, u8 param){
	u8 reg = 0;
	reg = (u8)cSPIN_Get_Param(pRsrc, cSPIN6480_GATECFG2);
	if(isCmdErr(pRsrc)==TRUE)	return TRUE;
	reg &= 0xff>>3;	reg |= (param&0x07)<<5;
	cSPIN_Set_Param(pRsrc, cSPIN6480_GATECFG2, reg);
	return isCmdErr(pRsrc);
}
u8 cSPIN_GetGateTBlank (cSPIN_Rsrc_T* pRsrc, u8 *param){
	*param = (cSPIN_Get_Param(pRsrc, cSPIN6480_GATECFG2)>>5)&0x07;
	return isCmdErr(pRsrc);
}

// for L6480 end

/************************
	*config
************************/
//osc is fixed @int16MHz,8MHz output
u8 cSPIN_SetSwMode (cSPIN_Rsrc_T* pRsrc, u8 param){
	u16 reg = 0;
	u32 conf = cSPIN6470_CONFIG;
	
	if(pRsrc->chip == 6470)	conf = cSPIN6470_CONFIG;
	else if(pRsrc->chip == 6480)	conf = cSPIN6480_CONFIG;

	reg = (u16)cSPIN_Get_Param(pRsrc, conf);
	if(isCmdErr(pRsrc)==TRUE)	return TRUE;
	reg &= (1<<4)^0xff;	reg |= (param&0x01)<<4;
	cSPIN_Set_Param(pRsrc, conf, reg);
	return isCmdErr(pRsrc);
}
u8 cSPIN_GetSwMode (cSPIN_Rsrc_T* pRsrc, u8 *param){
	u32 conf = cSPIN6470_CONFIG;
	
	if(pRsrc->chip == 6470)	conf = cSPIN6470_CONFIG;
	else if(pRsrc->chip == 6480)	conf = cSPIN6480_CONFIG;
	
	*param = (u8)((cSPIN_Get_Param(pRsrc, conf)>>4)&0x01);
	return isCmdErr(pRsrc);
}

u8 cSPIN_SetVsCompEn (cSPIN_Rsrc_T* pRsrc, u8 param){
	u16 reg = 0;
	u32 conf = cSPIN6470_CONFIG;
	
	if(pRsrc->chip == CHIP_L6470)	conf = cSPIN6470_CONFIG;
	else if(pRsrc->chip == CHIP_L6480)	conf = cSPIN6480_CONFIG;
	
	reg = (u16)cSPIN_Get_Param(pRsrc, conf);
	if(isCmdErr(pRsrc)==TRUE)	return TRUE;
	reg &= (1<<5)^0xff;	reg |= (param&0x01)<<5;
	cSPIN_Set_Param(pRsrc, conf, reg);
	return isCmdErr(pRsrc);
}
u8 cSPIN_GetVsCompEn (cSPIN_Rsrc_T* pRsrc, u8 *param){
	u32 conf = cSPIN6470_CONFIG;
	
	if(pRsrc->chip == CHIP_L6470)	conf = cSPIN6470_CONFIG;
	else if(pRsrc->chip == CHIP_L6480)	conf = cSPIN6480_CONFIG;
	
	*param = (u8)((cSPIN_Get_Param(pRsrc, conf)>>5)&0x01);
	return isCmdErr(pRsrc);
}

u8 cSPIN_SetOCSDEn (cSPIN_Rsrc_T* pRsrc, u8 param){
	u16 reg = 0;
	u32 conf = cSPIN6470_CONFIG;
	
	if(pRsrc->chip == CHIP_L6470)	conf = cSPIN6470_CONFIG;
	else if(pRsrc->chip == CHIP_L6480)	conf = cSPIN6480_CONFIG;
	
	reg = (u16)cSPIN_Get_Param(pRsrc, conf);
	if(isCmdErr(pRsrc)==TRUE)	return TRUE;
	reg &= (1<<7)^0xff;	reg |= (param&0x01)<<7;
	cSPIN_Set_Param(pRsrc, conf, reg);
	return isCmdErr(pRsrc);
}
u8 cSPIN_GetOCSDEn (cSPIN_Rsrc_T* pRsrc, u8 *param){
	u32 conf = cSPIN6470_CONFIG;
	
	if(pRsrc->chip == CHIP_L6470)	conf = cSPIN6470_CONFIG;
	else if(pRsrc->chip == CHIP_L6480)	conf = cSPIN6480_CONFIG;
	
	*param = (u8)((cSPIN_Get_Param(pRsrc, conf)>>7)&0x01);
	return isCmdErr(pRsrc);
}

u8 cSPIN_SetUVLOVal (cSPIN_Rsrc_T* pRsrc, u8 param){
	u16 reg = 0;
	u32 conf = cSPIN6470_CONFIG;
	
	if(pRsrc->chip == CHIP_L6470)	conf = cSPIN6470_CONFIG;
	else if(pRsrc->chip == CHIP_L6480)	conf = cSPIN6480_CONFIG;
	
	reg = (u16)cSPIN_Get_Param(pRsrc, conf);
	if(isCmdErr(pRsrc)==TRUE)	return TRUE;
	reg &= 0xfeff;	reg |= (param&0x01)<<8;
	cSPIN_Set_Param(pRsrc, conf, reg);
	return isCmdErr(pRsrc);
}
u8 cSPIN_GetUVLOVal (cSPIN_Rsrc_T* pRsrc, u8 *param){
	u32 conf = cSPIN6470_CONFIG;
	
	if(pRsrc->chip == CHIP_L6470)	conf = cSPIN6470_CONFIG;
	else if(pRsrc->chip == CHIP_L6480)	conf = cSPIN6480_CONFIG;
	
	*param = (u8)((cSPIN_Get_Param(pRsrc, conf)>>8)&0x01);
	return isCmdErr(pRsrc);
}

u8 cSPIN_SetVccVal (cSPIN_Rsrc_T* pRsrc, u8 param){
	u16 reg = 0;
	u32 conf = cSPIN6470_CONFIG;
	
	if(pRsrc->chip == CHIP_L6470)	conf = cSPIN6470_CONFIG;
	else if(pRsrc->chip == CHIP_L6480)	conf = cSPIN6480_CONFIG;
	
	reg = (u16)cSPIN_Get_Param(pRsrc, conf);
	if(isCmdErr(pRsrc)==TRUE)	return TRUE;
	reg &= 0xfdff;	reg |= (param&0x01)<<9;
	cSPIN_Set_Param(pRsrc, conf, reg);
	return isCmdErr(pRsrc);
}
u8 cSPIN_GetVccVal (cSPIN_Rsrc_T* pRsrc, u8 *param){
	u32 conf = cSPIN6470_CONFIG;
	
	if(pRsrc->chip == CHIP_L6470)	conf = cSPIN6470_CONFIG;
	else if(pRsrc->chip == CHIP_L6480)	conf = cSPIN6480_CONFIG;
	
	*param = (u8)((cSPIN_Get_Param(pRsrc, conf)>>9)&0x01);
	return isCmdErr(pRsrc);
}

u8 cSPIN_SetPwmFrq (cSPIN_Rsrc_T* pRsrc, u8 param){
	u16 reg = 0;
	u32 conf = cSPIN6470_CONFIG;
	
	if(pRsrc->chip == CHIP_L6470)	conf = cSPIN6470_CONFIG;
	else if(pRsrc->chip == CHIP_L6480)	conf = cSPIN6480_CONFIG;
	
	reg = (u16)cSPIN_Get_Param(pRsrc, conf);
	if(isCmdErr(pRsrc)==TRUE)	return TRUE;
	reg &= (0xffff>>6);	reg |= (u16)(param&0x3f)<<10;
	cSPIN_Set_Param(pRsrc, conf, reg);
	return isCmdErr(pRsrc);
}

u8 cSPIN_GetPwmFrq (cSPIN_Rsrc_T* pRsrc, u8 *param){
	u16 reg = 0;
	u32 conf = cSPIN6470_CONFIG;
	
	if(pRsrc->chip == CHIP_L6470)	conf = cSPIN6470_CONFIG;
	else if(pRsrc->chip == CHIP_L6480)	conf = cSPIN6480_CONFIG;
	
	*param = (u8)((cSPIN_Get_Param(pRsrc, conf)>>10)&0x3f);
	return isCmdErr(pRsrc);
}

//u8 cSPIN_SetPwmDec (cSPIN_Rsrc_T* pRsrc, u8 param){
//	u16 reg = 0;
//	reg = (u16)cSPIN_Get_Param(pRsrc, cSPIN_CONFIG);
//	if(isCmdErr(pRsrc)==CMD_FAIL)	return CMD_FAIL;
//	reg &= (0xffff^(0x07<<10));	reg |= (u16)(param&0x07)<<10;
//	cSPIN_Set_Param(pRsrc, cSPIN_CONFIG, reg);
//	return isCmdErr(pRsrc);
//}
//u8 cSPIN_GetPwmDec (cSPIN_Rsrc_T* pRsrc, u8 *param){
//	*param = (u8)((cSPIN_Get_Param(pRsrc, cSPIN_CONFIG)>>10)&0x07);
//	return isCmdErr(pRsrc);
//}

//u8 cSPIN_SetPwmInt (cSPIN_Rsrc_T* pRsrc, u8 param){
//	u16 reg = 0;
//	reg = (u16)cSPIN_Get_Param(pRsrc, cSPIN_CONFIG);
//	if(isCmdErr(pRsrc)==CMD_FAIL)	return CMD_FAIL;
//	reg &= (0xffff^(0x07<<13));	reg |= (u16)(param&0x07)<<13;
//	cSPIN_Set_Param(pRsrc, cSPIN_CONFIG, reg);
//	return isCmdErr(pRsrc);
//}
//u8 cSPIN_GetPwmInt (cSPIN_Rsrc_T* pRsrc, u8 *param){
//	*param = (u8)((cSPIN_Get_Param(pRsrc, cSPIN_CONFIG)>>13)&0x07);
//	return isCmdErr(pRsrc);
//}

//u8 cSPIN_GetStatus (cSPIN_Rsrc_T* pRsrc, u16 *param){
//	*param = (u16)(cSPIN_Get_Param(pRsrc, cSPIN_STATUS));
//	return isCmdErr(pRsrc);
//}


/**
  * @brief  Issues cSPIN Run command.
  * @param  direction Movement direction (FWD, REV)
  * @param  speed over 3 bytes
  * @retval None
  */
void cSPIN_Run(cSPIN_Rsrc_T* pRsrc, cSPIN_Direction_T direction, u32 stpPerS)
{
	u32 speed = stpPerS;
	// speed = stpPerS / 0.015 = stpPerS*200/(0.015*200) = stpPerS*200/3
	speed = speed*200/3;

	/* Send RUN operation code to cSPIN */
	cSPIN_Write_Byte(pRsrc, (u8)cSPIN_RUN | (u8)direction);
	/* Send speed - byte 2 data cSPIN */
	cSPIN_Write_Byte(pRsrc, (u8)(speed >> 16));
	/* Send speed - byte 1 data cSPIN */
	cSPIN_Write_Byte(pRsrc, (u8)(speed >> 8));
	/* Send speed - byte 0 data cSPIN */
	cSPIN_Write_Byte(pRsrc, (u8)(speed));
}

/**
  * @brief  Issues cSPIN Step Clock command.
  * @param  direction Movement direction (FWD, REV)
  * @retval None
  */
void cSPIN_Step_Clock(cSPIN_Rsrc_T* pRsrc, cSPIN_Direction_T direction)
{
	/* Send StepClock operation code to cSPIN */
	cSPIN_Write_Byte(pRsrc, (u8)cSPIN_STEP_CLOCK | (u8)direction);
}

/**
  * @brief  Issues cSPIN Move command.
  * @param  direction Movement direction
  * @param  n_step number of steps
  * @retval None
  */
void cSPIN_Move(cSPIN_Rsrc_T* pRsrc, cSPIN_Direction_T direction, u32 n_step)
{
	/* Send Move operation code to cSPIN */
	cSPIN_Write_Byte(pRsrc, (u8)cSPIN_MOVE | (u8)direction);
	/* Send n_step - byte 2 data cSPIN */
	cSPIN_Write_Byte(pRsrc, (u8)(n_step >> 16));
	/* Send n_step - byte 1 data cSPIN */
	cSPIN_Write_Byte(pRsrc, (u8)(n_step >> 8));
	/* Send n_step - byte 0 data cSPIN */
	cSPIN_Write_Byte(pRsrc, (u8)(n_step));
}

// in step
void cSPIN_MoveStep(cSPIN_Rsrc_T* pRsrc, cSPIN_Direction_T direction, u32 step)
{
	u8 mStep = 0;
	cSPIN_GetStepSel(pRsrc, &mStep);
	cSPIN_Move(pRsrc, direction, step<<mStep);
}

/**
  * @brief  Issues cSPIN Go To command.
  * @param  abs_pos absolute position where requested to move
  * @retval None
  */
void cSPIN_Go_To(cSPIN_Rsrc_T* pRsrc, u32 abs_pos)
{
	/* Send GoTo operation code to cSPIN */
	cSPIN_Write_Byte(pRsrc, cSPIN_GO_TO);
	/* Send absolute position parameter - byte 2 data to cSPIN */
	cSPIN_Write_Byte(pRsrc, (u8)(abs_pos >> 16));
	/* Send absolute position parameter - byte 1 data to cSPIN */
	cSPIN_Write_Byte(pRsrc, (u8)(abs_pos >> 8));
	/* Send absolute position parameter - byte 0 data to cSPIN */
	cSPIN_Write_Byte(pRsrc, (u8)(abs_pos));
}

/**
  * @brief  Issues cSPIN Go To Dir command.
  * @param  direction movement direction
  * @param  abs_pos absolute position where requested to move
  * @retval None
  */
void cSPIN_Go_To_Dir(cSPIN_Rsrc_T* pRsrc, cSPIN_Direction_T direction, u32 abs_pos)
{
	/* Send GoTo_DIR operation code to cSPIN */
	cSPIN_Write_Byte(pRsrc, (u8)cSPIN_GO_TO_DIR | (u8)direction);
	/* Send absolute position parameter - byte 2 data to cSPIN */
	cSPIN_Write_Byte(pRsrc, (u8)(abs_pos >> 16));
	/* Send absolute position parameter - byte 1 data to cSPIN */
	cSPIN_Write_Byte(pRsrc, (u8)(abs_pos >> 8));
	/* Send absolute position parameter - byte 0 data to cSPIN */
	cSPIN_Write_Byte(pRsrc, (u8)(abs_pos));
}

/**
  * @brief  Issues cSPIN Go Until command.
  * @param  action
  * @param  direction movement direction
  * @param  speed
  * @retval None
  */
void cSPIN_Go_Until(cSPIN_Rsrc_T* pRsrc, cSPIN_Action_T action, cSPIN_Direction_T direction, u32 speed)
{
	/* Send GoUntil operation code to cSPIN */
	cSPIN_Write_Byte(pRsrc, (u8)cSPIN_GO_UNTIL | (u8)action | (u8)direction);
	/* Send speed parameter - byte 2 data to cSPIN */
	cSPIN_Write_Byte(pRsrc, (u8)(speed >> 16));
	/* Send speed parameter - byte 1 data to cSPIN */
	cSPIN_Write_Byte(pRsrc, (u8)(speed >> 8));
	/* Send speed parameter - byte 0 data to cSPIN */
	cSPIN_Write_Byte(pRsrc, (u8)(speed));
}

/**
  * @brief  Issues cSPIN Release SW command.
  * @param  action
  * @param  direction movement direction
  * @retval None
  */
void cSPIN_Release_SW(cSPIN_Rsrc_T* pRsrc, cSPIN_Action_T action, cSPIN_Direction_T direction)
{
	/* Send ReleaseSW operation code to cSPIN */
	cSPIN_Write_Byte(pRsrc, (u8)cSPIN_RELEASE_SW | (u8)action | (u8)direction);
}

/**
  * @brief  Issues cSPIN Go Home command. (Shorted path to zero position)
  * @param  None
  * @retval None
  */
void cSPIN_Go_Home(cSPIN_Rsrc_T* pRsrc)
{
	/* Send GoHome operation code to cSPIN */
	cSPIN_Write_Byte(pRsrc, cSPIN_GO_HOME);
}

/**
  * @brief  Issues cSPIN Go Mark command.
  * @param  None
  * @retval None
  */
void cSPIN_Go_Mark(cSPIN_Rsrc_T* pRsrc)
{
	/* Send GoMark operation code to cSPIN */
	cSPIN_Write_Byte(pRsrc, cSPIN_GO_MARK);
}

/**
  * @brief  Issues cSPIN Reset Pos command.
  * @param  None
  * @retval None
  */
void cSPIN_Reset_Pos(cSPIN_Rsrc_T* pRsrc)
{
	/* Send ResetPos operation code to cSPIN */
	cSPIN_Write_Byte(pRsrc, cSPIN_RESET_POS);
}

/**
  * @brief  Issues cSPIN Reset Device command.
  * @param  None
  * @retval None
  */
void cSPIN_Reset_Device(cSPIN_Rsrc_T* pRsrc)
{
	/* Send ResetDevice operation code to cSPIN */
	cSPIN_Write_Byte(pRsrc, cSPIN_RESET_DEVICE);
}

/**
  * @brief  Issues cSPIN Soft Stop command.
  * @param  None
  * @retval None
  */
void cSPIN_Soft_Stop(cSPIN_Rsrc_T* pRsrc)
{
	/* Send SoftStop operation code to cSPIN */
	cSPIN_Write_Byte(pRsrc, cSPIN_SOFT_STOP);
}

/**
  * @brief  Issues cSPIN Hard Stop command.
  * @param  None
  * @retval None
  */
void cSPIN_Hard_Stop(cSPIN_Rsrc_T* pRsrc)
{
	/* Send HardStop operation code to cSPIN */
	cSPIN_Write_Byte(pRsrc, cSPIN_HARD_STOP);
}

/**
  * @brief  Issues cSPIN Soft HiZ command.
  * @param  None
  * @retval None
  */
void cSPIN_Soft_HiZ(cSPIN_Rsrc_T* pRsrc)
{
	/* Send SoftHiZ operation code to cSPIN */
	cSPIN_Write_Byte(pRsrc, cSPIN_SOFT_HIZ);
}

/**
  * @brief  Issues cSPIN Hard HiZ command.
  * @param  None
  * @retval None
  */
void cSPIN_Hard_HiZ(cSPIN_Rsrc_T* pRsrc)
{
	/* Send HardHiZ operation code to cSPIN */
	cSPIN_Write_Byte(pRsrc, cSPIN_HARD_HIZ);
}

/**
  * @brief  Issues cSPIN Get Status command.
  * @param  None
  * @retval Status Register content
  */
u16 cSPIN_Get_Status(cSPIN_Rsrc_T* pRsrc)
{
	u16 temp = 0;
	u16 rx = 0;
	/* Send GetStatus operation code to cSPIN */
	cSPIN_Write_Byte(pRsrc, cSPIN_GET_STATUS);
	/* Send zero byte / receive MSByte from cSPIN */
	temp = cSPIN_Write_Byte(pRsrc, (u8)(0x00));
	temp = temp << 8;
	rx |= temp;
	/* Send zero byte / receive LSByte from cSPIN */
	temp = cSPIN_Write_Byte(pRsrc, (u8)(0x00));
	rx |= temp;
	return rx;
}

/**
  * @brief  Checks if the cSPIN is Busy by hardware - active Busy signal.
  * @param  None
  * @retval one if chip is busy, otherwise zero
  */
u8 cSPIN_Busy_HW(cSPIN_Rsrc_T* pRsrc)
{
	if(HAL_GPIO_ReadPin(pRsrc->SYNC->GPIOx, pRsrc->SYNC->GPIO_Pin) == GPIO_PIN_RESET) return 0x01;
	else return 0x00;
}

/**
  * @brief  Checks if the cSPIN is Busy by SPI - Busy flag bit in Status Register.
  * @param  None
  * @retval one if chip is busy, otherwise zero
  */
u8 cSPIN_Busy_SW(cSPIN_Rsrc_T* pRsrc)
{
	if(!(cSPIN_Get_Status(pRsrc) & cSPIN_STATUS_BUSY)) return 0x01;
	else return 0x00;
}

/**
  * @brief  Checks cSPIN Flag signal.
  * @param  None
  * @retval one if Flag signal is active, otherwise zero
  */
u8 cSPIN_Flag(cSPIN_Rsrc_T* pRsrc)
{
	if(HAL_GPIO_ReadPin(pRsrc->nFlag->GPIOx, pRsrc->nFlag->GPIO_Pin) == GPIO_PIN_RESET)	return 0x01;
	else return 0x00;
}


/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Inserts a delay time.
  * @param  nCount specifies the delay time length.
  * @retval None
  */
void cSPIN_Delay(__IO u32 nCount)
{
  for(; nCount!= 0;nCount--);
}

/**
  * @brief  Transmits/Receives one byte to/from cSPIN over SPI.
  * @param  byte Transmited byte
  * @retval Received byte
  */
u8 cSPIN_Write_Byte(cSPIN_Rsrc_T* pRsrc, u8 byte)
{
	u8 rtn=0;
	while (HAL_SPI_GetState(pRsrc->SPI_HANDLE) != HAL_SPI_STATE_READY);
	/* nSS signal activation - low */
	HAL_GPIO_WritePin(pRsrc->CS->GPIOx, pRsrc->CS->GPIO_Pin, GPIO_PIN_RESET);
	/* SPI byte send */
	HAL_SPI_TransmitReceive(pRsrc->SPI_HANDLE, &byte, &rtn, 1, HAL_MAX_DELAY);
	/* nSS signal deactivation - high */
	HAL_GPIO_WritePin(pRsrc->CS->GPIOx, pRsrc->CS->GPIO_Pin,GPIO_PIN_SET);
	return rtn;
}

/**
  * @brief  Resets CSPIN and puts it into standby mode
  * @param  None
  * @retval None
  */
void cSPIN_Reset_And_Standby(cSPIN_Rsrc_T* pRsrc)
{
	HAL_GPIO_WritePin(pRsrc->nStby_nReset->GPIOx, pRsrc->nStby_nReset->GPIO_Pin,GPIO_PIN_RESET);
  cSPIN_Delay(2000);
  HAL_GPIO_WritePin(pRsrc->nStby_nReset->GPIOx, pRsrc->nStby_nReset->GPIO_Pin,GPIO_PIN_SET);
}

///**
//  * @brief  Fills-in cSPIN configuration structure with default values.
//  * @param  cSPIN_RegsStruct structure address (pointer to struct)
//  * @retval None
//  */
//void cSPIN_Regs_Struct_Reset(cSPIN_Rsrc_T* pRsrc, cSPIN_RegsStruct_TypeDef *reg)
//{
//	reg.ABS_POS		= 0;
//	reg.EL_POS 		= 0;
//	reg.MARK 			= 0;
//	reg.ACC 				= 0x08A;
//	reg.DEC 				= 0x08A;
//	reg.MAX_SPEED 	= 0x041;
//	reg.MIN_SPEED 	= 0;
//	reg.FS_SPD 		= 0x027;

//	reg.KVAL_HOLD 	= 0x29;
//	reg.KVAL_RUN 	= 0x29;
//	reg.KVAL_ACC 	= 0x29;
//	reg.KVAL_DEC 	= 0x29;
//	reg.INT_SPD 		= 0x0408;
//	reg.ST_SLP 		= 0x19;
//	reg.FN_SLP_ACC = 0x29;
//	reg.FN_SLP_DEC = 0x29;
//	reg.K_THERM 		= 0;
//	reg.STALL_TH 	= 0x10;

////#if defined(L6482)
////	reg.TVAL_HOLD 	= 0x29;
////	reg.TVAL_RUN 	= 0x29;
////	reg.TVAL_ACC 	= 0x29;
////	reg.TVAL_DEC 	= 0x29;
////	reg.T_FAST 		= 0x19;
////	reg.TON_MIN 		= 0x29;
////	reg.TOFF_MIN 	= 0x29;
////#endif /* defined(L6482) */

//	reg.OCD_TH 		= 0x8;
//	reg.STEP_MODE 	= 0x7;
//	reg.ALARM_EN 	= 0xFF;

//	reg.GATECFG1 	= 0;	// only for L6480
//	reg.GATECFG2 	= 0;	// only for L6480
//	reg.CONFIG 		= 0x2C88;
//}
 
/**
  * @brief  Configures cSPIN internal registers with values in the config structure.
  * @param  cSPIN_RegsStruct Configuration structure address (pointer to configuration structure)
  * @retval None
  */
void cSPIN_Registers_SetAll(cSPIN_Rsrc_T* pRsrc, cSPIN_RegsStruct_TypeDef *reg)
{
	cSPIN_Set_Param(pRsrc, cSPIN_ABS_POS, reg->ABS_POS);	HAL_Delay(1);
	cSPIN_Set_Param(pRsrc, cSPIN_EL_POS, reg->EL_POS);	HAL_Delay(1);
	cSPIN_Set_Param(pRsrc, cSPIN_MARK, reg->MARK);	HAL_Delay(1);
	cSPIN_Set_Param(pRsrc, cSPIN_ACC, reg->ACC);	HAL_Delay(1);
	cSPIN_Set_Param(pRsrc, cSPIN_DEC, reg->DEC);	HAL_Delay(1);
	cSPIN_Set_Param(pRsrc, cSPIN_MAX_SPEED, reg->MAX_SPEED);	HAL_Delay(1);
	cSPIN_Set_Param(pRsrc, cSPIN_MIN_SPEED, reg->MIN_SPEED);	HAL_Delay(1);
	cSPIN_Set_Param(pRsrc, cSPIN_FS_SPD, reg->FS_SPD);	HAL_Delay(1);
	if(pRsrc->chip == CHIP_L6470 || pRsrc->chip == CHIP_L6480){
		cSPIN_Set_Param(pRsrc, cSPIN_KVAL_HOLD, reg->KVAL_HOLD);	HAL_Delay(1);
		cSPIN_Set_Param(pRsrc, cSPIN_KVAL_RUN, reg->KVAL_RUN);	HAL_Delay(1);
		cSPIN_Set_Param(pRsrc, cSPIN_KVAL_ACC, reg->KVAL_ACC);	HAL_Delay(1);
		cSPIN_Set_Param(pRsrc, cSPIN_KVAL_DEC, reg->KVAL_DEC);	HAL_Delay(1);
		cSPIN_Set_Param(pRsrc, cSPIN_INT_SPD, reg->INT_SPD);	HAL_Delay(1);
		cSPIN_Set_Param(pRsrc, cSPIN_ST_SLP, reg->ST_SLP);	HAL_Delay(1);
		cSPIN_Set_Param(pRsrc, cSPIN_FN_SLP_ACC, reg->FN_SLP_ACC);	HAL_Delay(1);
		cSPIN_Set_Param(pRsrc, cSPIN_FN_SLP_DEC, reg->FN_SLP_DEC);	HAL_Delay(1);
		cSPIN_Set_Param(pRsrc, cSPIN_K_THERM, reg->K_THERM);	HAL_Delay(1);
		cSPIN_Set_Param(pRsrc, cSPIN_STALL_TH, reg->STALL_TH);	HAL_Delay(1);
	}
//#if defined(L6482)
//	cSPIN_Set_Param(pRsrc, cSPIN_TVAL_HOLD, reg->TVAL_HOLD);	HAL_Delay(1);
//	cSPIN_Set_Param(pRsrc, cSPIN_TVAL_RUN, reg->TVAL_RUN);	HAL_Delay(1);
//	cSPIN_Set_Param(pRsrc, cSPIN_TVAL_ACC, reg->TVAL_ACC);	HAL_Delay(1);
//	cSPIN_Set_Param(pRsrc, cSPIN_TVAL_DEC, reg->TVAL_DEC);	HAL_Delay(1);
//	cSPIN_Set_Param(pRsrc, cSPIN_T_FAST, reg->T_FAST);	HAL_Delay(1);
//	cSPIN_Set_Param(cSPIN_TON_MIN, reg->TON_MIN);	HAL_Delay(1);
//	cSPIN_Set_Param(pRsrc, cpRsrc, SPIN_TOFF_MIN, reg->TOFF_MIN);	HAL_Delay(1);
//#endif /* defined(L6482) */
	cSPIN_Set_Param(pRsrc, cSPIN_OCD_TH, reg->OCD_TH);	HAL_Delay(1);
	cSPIN_Set_Param(pRsrc, cSPIN_STEP_MODE, reg->STEP_MODE);	HAL_Delay(1);
	cSPIN_Set_Param(pRsrc, cSPIN_ALARM_EN, reg->ALARM_EN);	HAL_Delay(1);
	if(pRsrc->chip==CHIP_L6480){
		cSPIN_Set_Param(pRsrc, cSPIN6480_GATECFG1, reg->GATECFG1);	HAL_Delay(1);
		cSPIN_Set_Param(pRsrc, cSPIN6480_GATECFG2, reg->GATECFG2);	HAL_Delay(1);
		cSPIN_Set_Param(pRsrc, cSPIN6480_CONFIG, reg->CONFIG);	HAL_Delay(1);
	}
	else if(pRsrc->chip==CHIP_L6470){
		cSPIN_Set_Param(pRsrc, cSPIN6470_CONFIG, reg->CONFIG);	HAL_Delay(1);
	}
}

/**
  * @brief  Configures cSPIN internal registers with values in the config structure.
  * @param  cSPIN_RegsStruct Configuration structure address (pointer to configuration structure)
  * @retval None
  */
void cSPIN_Registers_GetAll(cSPIN_Rsrc_T* pRsrc, cSPIN_RegsStruct_TypeDef *reg)
{	//
	reg->ABS_POS = cSPIN_Get_Param(pRsrc, cSPIN_ABS_POS);
	reg->SPEED = cSPIN_Get_Param(pRsrc, cSPIN_SPEED);
	reg->ADC_OUT = cSPIN_Get_Param(pRsrc, cSPIN_ADC_OUT);
	reg->EL_POS = cSPIN_Get_Param(pRsrc, cSPIN_EL_POS);
	reg->MARK = cSPIN_Get_Param(pRsrc, cSPIN_MARK);
	reg->ACC = cSPIN_Get_Param(pRsrc, cSPIN_ACC);
	reg->DEC = cSPIN_Get_Param(pRsrc, cSPIN_DEC);
	reg->MAX_SPEED = cSPIN_Get_Param(pRsrc, cSPIN_MAX_SPEED);
	reg->MIN_SPEED = cSPIN_Get_Param(pRsrc, cSPIN_MIN_SPEED);
	reg->FS_SPD = cSPIN_Get_Param(pRsrc, cSPIN_FS_SPD);
	reg->KVAL_HOLD = cSPIN_Get_Param(pRsrc, cSPIN_KVAL_HOLD);
	reg->KVAL_RUN = cSPIN_Get_Param(pRsrc, cSPIN_KVAL_RUN);
	reg->KVAL_ACC = cSPIN_Get_Param(pRsrc, cSPIN_KVAL_ACC);
	reg->KVAL_DEC = cSPIN_Get_Param(pRsrc, cSPIN_KVAL_DEC);
	reg->INT_SPD = cSPIN_Get_Param(pRsrc, cSPIN_INT_SPD);
	reg->ST_SLP = cSPIN_Get_Param(pRsrc, cSPIN_ST_SLP);
	reg->FN_SLP_ACC = cSPIN_Get_Param(pRsrc, cSPIN_FN_SLP_ACC);
	reg->FN_SLP_DEC = cSPIN_Get_Param(pRsrc, cSPIN_FN_SLP_DEC);
	reg->K_THERM = cSPIN_Get_Param(pRsrc, cSPIN_K_THERM);
	reg->STALL_TH = cSPIN_Get_Param(pRsrc, cSPIN_STALL_TH);
//#if defined(L6482)
//	reg->TVAL_HOLD = cSPIN_Get_Param(pRsrc, cSPIN_TVAL_HOLD);
//	reg->TVAL_RUN = cSPIN_Get_Param(pRsrc, cSPIN_TVAL_RUN);
//	reg->TVAL_ACC = cSPIN_Get_Param(pRsrc, cSPIN_TVAL_ACC);
//	reg->TVAL_DEC = cSPIN_Get_Param(pRsrc, cSPIN_TVAL_DEC);
//	reg->T_FAST = cSPIN_Get_Param(pRsrc, cSPIN_T_FAST);
//	reg->TON_MIN = cSPIN_Get_Param(cSPIN_TON_MIN, reg->TON_MIN);
//	reg->TOFF_MIN = cSPIN_Get_Param(pRsrc, cpRsrc, SPIN_TOFF_MIN);
//#endif /* defined(L6482) */
	reg->OCD_TH = cSPIN_Get_Param(pRsrc, cSPIN_OCD_TH);
	reg->STEP_MODE = cSPIN_Get_Param(pRsrc, cSPIN_STEP_MODE);
	reg->ALARM_EN = cSPIN_Get_Param(pRsrc, cSPIN_ALARM_EN);
	if(pRsrc->chip==CHIP_L6480){
		reg->GATECFG1 = cSPIN_Get_Param(pRsrc, cSPIN6480_GATECFG1);
		reg->GATECFG2 = cSPIN_Get_Param(pRsrc, cSPIN6480_GATECFG2);
		reg->CONFIG = cSPIN_Get_Param(pRsrc, cSPIN6480_CONFIG);
		reg->STATUS = cSPIN_Get_Param(pRsrc, cSPIN6480_STATUS);
	}
	else if(pRsrc->chip==CHIP_L6470){
		reg->CONFIG = cSPIN_Get_Param(pRsrc, cSPIN6470_CONFIG);
		reg->STATUS = cSPIN_Get_Param(pRsrc, cSPIN6470_STATUS);
	}
}

/*******************************************************************************
* Function Name  : cSPIN_tickExe
* Description    : per
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void cSPIN_tickExe(cSPIN_Rsrc_T* pRsrc, u8 tick){
	pRsrc->tick+=tick;
	if(pRsrc->tick >= 16){
		pRsrc->tick = 0;
		//push reference sw
		pRsrc->refLSeries <<= 1;
		pRsrc->refRSeries <<= 1;
		if(HAL_GPIO_ReadPin(pRsrc->SW_L->GPIOx, pRsrc->SW_L->GPIO_Pin) == GPIO_PIN_SET)
			pRsrc->refLSeries |= 1;
		if(HAL_GPIO_ReadPin(pRsrc->SW_R->GPIOx, pRsrc->SW_R->GPIO_Pin) == GPIO_PIN_SET)
			pRsrc->refRSeries |= 1;
		//see if there is a rising/falling edge
//		if((pRsrc->refLSeries&0x03) == 0x02)	pRsrc->callBackRefLTurnOn();
//		if((pRsrc->refLSeries&0x03) == 0x01)	pRsrc->callBackRefLRelease();
//		if((pRsrc->refRSeries&0x03) == 0x02)	pRsrc->callBackRefRTurnOn();
//		if((pRsrc->refRSeries&0x03) == 0x01)	pRsrc->callBackRefRRelease();
		//see if absPos changed
//		if(pRsrc->flags & 1<<MOTOR_RPT_ABSPOS_BIT){
//			pRsrc->regPrv.ABS_POS = reg.ABS_POS;
//			reg.ABS_POS = cSPIN_Get_Param(pRsrc, cSPIN_ABS_POS);
////			if(pRsrc->regPrv.ABS_POS ^  reg.ABS_POS)	pRsrc->callBackAbsPosChanged(reg.ABS_POS);		
//		}
//		//see if ADC_OUT changed
//		if(pRsrc->flags & 1<<MOTOR_RPT_ADCOUT_BIT){
//			pRsrc->regPrv.ADC_OUT = reg.ADC_OUT;
//			reg.ADC_OUT = cSPIN_Get_Param(pRsrc, cSPIN_ADC_OUT);
////			if(pRsrc->regPrv.ADC_OUT ^  reg.ADC_OUT)	pRsrc->callBackAdcOutChanged(reg.ADC_OUT);
//		}
//		//see if STATUS changed
//		if(pRsrc->flags & 1<<MOTOR_RPT_STATUS_BIT){
//			pRsrc->regPrv.STATUS = reg.STATUS;
//			reg.STATUS = cSPIN_Get_Status(pRsrc);
////			if(pRsrc->regPrv.STATUS ^  reg.STATUS)	pRsrc->callBackStatusChanged(reg.STATUS);
//		}
	}
}

/*******************************************************************************
* Function Name  : cmd
* Description    : command
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void cSPIN_EnRptStatus(cSPIN_Rsrc_T* pRsrc){
	pRsrc->flags |= 1<<MOTOR_RPT_STATUS_BIT;
}
void cSPIN_DisRptStatus(cSPIN_Rsrc_T* pRsrc){
	pRsrc->flags &= (1<<MOTOR_RPT_STATUS_BIT)^0xffff;
}
void cSPIN_EnRptAbsPos(cSPIN_Rsrc_T* pRsrc){
	pRsrc->flags |= 1<<MOTOR_RPT_ABSPOS_BIT;
}
void cSPIN_DisRptAbsPos(cSPIN_Rsrc_T* pRsrc){
	pRsrc->flags &= (1<<MOTOR_RPT_ABSPOS_BIT)^0xffff;
}
void cSPIN_EnRptAdcOut(cSPIN_Rsrc_T* pRsrc){
	pRsrc->flags |= 1<<MOTOR_RPT_ADCOUT_BIT;
}
void cSPIN_DisRptAdcOut(cSPIN_Rsrc_T* pRsrc){
	pRsrc->flags &= (1<<MOTOR_RPT_ADCOUT_BIT)^0xffff;
}

static s8 cspinEepromWriteAllReg(cSPIN_Rsrc_T* pRsrc, cSPIN_RegsStruct_TypeDef *reg){
	u32 sum = 0;
	u16 dat[22] = {0}, i;

	if(pRsrc->EepromWrite==NULL)	return -1;
	//generate checksum
	i = 0;
	dat[i++] = reg->ACC;
	dat[i++] = reg->DEC;
	dat[i++] = reg->MIN_SPEED;
	dat[i++] = reg->MAX_SPEED;
	dat[i++] = reg->FS_SPD;
	dat[i++] = reg->KVAL_HOLD;
	dat[i++] = reg->KVAL_RUN;
	dat[i++] = reg->KVAL_ACC;
	dat[i++] = reg->KVAL_DEC;
	dat[i++] = reg->INT_SPD;
	dat[i++] = reg->ST_SLP;
	dat[i++] = reg->FN_SLP_ACC;
	dat[i++] = reg->FN_SLP_DEC;
	dat[i++] = reg->K_THERM;
	dat[i++] = reg->ADC_OUT;
	dat[i++] = reg->OCD_TH;
	dat[i++] = reg->STALL_TH;
	dat[i++] = reg->STEP_MODE;
	dat[i++] = reg->ALARM_EN;
	dat[i++] = reg->GATECFG1;
	dat[i++] = reg->GATECFG2;
	dat[i++] = reg->CONFIG;
	for(i=0;i<22;i++){		sum += dat[i];	}
	//print("write.checksum:%d\n",sum);
	pRsrc->EepromWrite(pRsrc->eepromAddrBase, (u8*)&sum, 4);
	pRsrc->EepromWrite(pRsrc->eepromAddrBase + 4, (u8*)dat, 23*sizeof(u16));

	return 0;
}

static s8 cspinEepromReadAllReg(cSPIN_Rsrc_T* pRsrc, cSPIN_RegsStruct_TypeDef *regs){
	u32 checkSum=1, sum=0;
	u16 buf[22] = {0}, i;
	if(pRsrc->EepromRead == NULL)	return -1;

	pRsrc->EepromRead(pRsrc->eepromAddrBase, (u8*)&checkSum, 4);
	pRsrc->EepromRead(pRsrc->eepromAddrBase + 4, (u8*)buf, 23*sizeof(u16));

	for(i=0;i<22;i++){	sum += buf[i];	}
	if(sum != checkSum){	return -2;	}
	i = 0;
	regs->ACC = buf[i++];
	regs->DEC = buf[i++];
	regs->MIN_SPEED = buf[i++];
	regs->MAX_SPEED = buf[i++];
	regs->FS_SPD = buf[i++];
	regs->KVAL_HOLD = buf[i++];
	regs->KVAL_RUN = buf[i++];
	regs->KVAL_ACC = buf[i++];
	regs->KVAL_DEC = buf[i++];
	regs->INT_SPD = buf[i++];
	regs->ST_SLP = buf[i++];
	regs->FN_SLP_ACC = buf[i++];
	regs->FN_SLP_DEC = buf[i++];
	regs->K_THERM = buf[i++];
	regs->ADC_OUT = buf[i++];
	regs->OCD_TH = buf[i++];
	regs->STALL_TH = buf[i++];
	regs->STEP_MODE = buf[i++];
	regs->ALARM_EN = buf[i++];
	regs->GATECFG1 = buf[i++];
	regs->GATECFG2 = buf[i++];
	regs->CONFIG = buf[i++];
	return 0;
}

static void cspinDefault(cSPIN_Rsrc_T* pRsrc){
	cSPIN_RegsStruct_TypeDef regs = {0};
	regDefault(pRsrc, &regs);		
	cspinEepromWriteAllReg(pRsrc, &regs);
	cSPIN_Registers_SetAll(pRsrc, &regs);
}

static s8 cspinSaveToEEPROM(cSPIN_Rsrc_T* pRsrc){
	cSPIN_RegsStruct_TypeDef regs = {0};
	cSPIN_Registers_GetAll(pRsrc, &regs);
	return(cspinEepromWriteAllReg(pRsrc, &regs));
}

static void cspinPrintAll(cSPIN_Rsrc_T* pRsrc){
	cSPIN_RegsStruct_TypeDef regs = {0};
	if(pRsrc->print == NULL)	return;
	cSPIN_Registers_GetAll(pRsrc, &regs);
	// u32 ABS_POS;
	pRsrc->print("ABS_POS:\t0x%x[%d]\n", regs.ABS_POS, regs.ABS_POS);
	//  u16 EL_POS;
	pRsrc->print("EL_POS:\t0x%x[%d]\n", regs.EL_POS, regs.EL_POS);
	//  u32 MARK;
	pRsrc->print("MARK:\t0x%x[%d]\n", regs.MARK, regs.MARK);
	//  u32 SPEED;
	pRsrc->print("SPEED:\t0x%x[%d]\n", regs.SPEED, regs.SPEED);
	//  u16 ACC;
	pRsrc->print("ACC:\t0x%x[%d]\n", regs.ACC, regs.ACC);
	//  u16 DEC;
	pRsrc->print("DEC:\t0x%x[%d]\n", regs.DEC, regs.DEC);
	//  u16 MAX_SPEED;
	pRsrc->print("MAX_SPEED:\t0x%x[%d]\n", regs.MAX_SPEED, regs.MAX_SPEED);
	//  u16 MIN_SPEED;
	pRsrc->print("MIN_SPEED:\t0x%x[%d]\n", regs.MIN_SPEED, regs.MIN_SPEED);
	//  u16 FS_SPD;
	pRsrc->print("FS_SPD:\t0x%x[%d]\n", regs.FS_SPD, regs.FS_SPD);
	//  u8  KVAL_HOLD;
	pRsrc->print("KVAL_HOLD:\t0x%x[%d]\n", regs.KVAL_HOLD, regs.KVAL_HOLD);
	//  u8  KVAL_RUN;
	pRsrc->print("KVAL_RUN:\t0x%x[%d]\n", regs.KVAL_RUN, regs.KVAL_RUN);
	//  u8  KVAL_ACC;
	pRsrc->print("KVAL_ACC:\t0x%x[%d]\n", regs.KVAL_ACC, regs.KVAL_ACC);
	//  u8  KVAL_DEC;
	pRsrc->print("KVAL_DEC:\t0x%x[%d]\n", regs.KVAL_DEC, regs.KVAL_DEC);
	//  u16 INT_SPD;
	pRsrc->print("INT_SPD:\t0x%x[%d]\n", regs.INT_SPD, regs.INT_SPD);
	//  u8  ST_SLP;
	pRsrc->print("ST_SLP:\t0x%x[%d]\n", regs.ST_SLP, regs.ST_SLP);
	//  u8  FN_SLP_ACC;
	pRsrc->print("FN_SLP_ACC:\t0x%x[%d]\n", regs.FN_SLP_ACC, regs.FN_SLP_ACC);
	//  u8  FN_SLP_DEC;
	pRsrc->print("FN_SLP_DEC:\t0x%x[%d]\n", regs.FN_SLP_DEC, regs.FN_SLP_DEC);
	//  u8  K_THERM;
	pRsrc->print("K_THERM:\t0x%x[%d]\n", regs.K_THERM, regs.K_THERM);
	//  u8  ADC_OUT;
	pRsrc->print("ADC_OUT:\t0x%x[%d]\n", regs.ADC_OUT, regs.ADC_OUT);
	//  u8  OCD_TH;
	pRsrc->print("OCD_TH:\t0x%x[%d]\n", regs.OCD_TH, regs.OCD_TH);
	//  u8  STALL_TH;
	pRsrc->print("STALL_TH:\t0x%x[%d]\n", regs.STALL_TH, regs.STALL_TH);
	//  u8  STEP_MODE;
	pRsrc->print("STEP_MODE:\t0x%x[%d]\n", regs.STEP_MODE, regs.STEP_MODE);
	//  u8  ALARM_EN;
	pRsrc->print("ALARM_EN:\t0x%x[%d]\n", regs.ALARM_EN, regs.ALARM_EN);
	//  u16 GATECFG1;		// for L6480 only
	pRsrc->print("GATECFG1:\t0x%x[%d]\n", regs.GATECFG1, regs.GATECFG1);
	//  u8  GATECFG2;  	// for L6480 only
	pRsrc->print("GATECFG2:\t0x%x[%d]\n", regs.GATECFG2, regs.GATECFG2);
	//  u16 CONFIG;
	pRsrc->print("CONFIG:\t0x%x[%d]\n", regs.CONFIG, regs.CONFIG);
	//  u16 STATUS;
	pRsrc->print("STATUS:\t0x%x[%d]\n", regs.STATUS, regs.STATUS);
}

static u8 cSPIN_SetConfig (cSPIN_Rsrc_T* pRsrc, u16 param){
	if(pRsrc->chip==CHIP_L6480){
		cSPIN_Set_Param(pRsrc, cSPIN6480_CONFIG, param);
	}
	else if(pRsrc->chip==CHIP_L6470){
		cSPIN_Set_Param(pRsrc, cSPIN6470_CONFIG, param);
	}
	return isCmdErr(pRsrc);
}

static u8 cSPIN_GetConfig (cSPIN_Rsrc_T* pRsrc, u16* param){
	u32 conf = 0;
	if(pRsrc->chip==CHIP_L6480){
		*param = cSPIN_Get_Param(pRsrc, cSPIN6480_CONFIG);
	}
	else if(pRsrc->chip==CHIP_L6470){
		*param = cSPIN_Get_Param(pRsrc, cSPIN6470_CONFIG);
	}
	return isCmdErr(pRsrc);
}

/** @} */  
/******************* (C) COPYRIGHT 2013 STMicroelectronics *****END OF FILE****/
