/******************** (C) COPYRIGHT 2022 INCUBECN *****************************
* File Name          : promise.h
* Author             : Tiko Zhong
* Date First Issued  : 26/oct,2022
* Description        : This file provides a set of functions needed to manage the
*                      stepper ramp generator
********************************************************************************
* History:e
* 06/oct,2022: V0.0
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _PROMISE_H
#define _PROMISE_H

#include "misc.h"
#include "request.h"
#include "script_demo.h"

#define PROMISE_CMD_LEN	(64)
#define PROMISE_REG_SZ	(8)

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
typedef struct{
	u16 squ,squCpy;
	u32 tick;
	u32 tDelay;
	u32 tTimeout;

	SCRIPT_T* curRun;
	SCRIPT_T* STARTS;

	s32 r[PROMISE_REG_SZ];

	const char* pos[8];
	const char* nextPos;
	u8 posIndx;
	u8 debugEn;
	u8 retryTz;
	requestDev_t* req;
	char busOrg[PROMISE_CMD_LEN];	// line from scirpt
	char busResp[PROMISE_CMD_LEN];	// msg from bus
	char cmdHost[PROMISE_CMD_LEN];	// msg from host
	void (*xPrint)(const char* FORMAT_ORG, ...);

	const char *isr_cmd;
	const char *isr_t0;
	const char *isr_t1;
	const char *isr_50ms;

} promiseRsrc_t;

typedef struct{
	promiseRsrc_t rsrc;
	s32 (*Start)(promiseRsrc_t*, SCRIPT_T* SCRIPT);
	void (*PeriodJOb)(promiseRsrc_t*,u8);
	//void (*Resp)(promiseRsrc_t*, const char* );
	s32 (*Cmd)(promiseRsrc_t*, const char* ORG);
} promiseDev_t;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

const char* promiseReadLine(char* buff, u16 len, const char* ORG);
void promiseSetup(promiseDev_t* dev, requestDev_t* req, void (*xPrint)(const char* FORMAT_ORG, ...));

#endif /* _APLENTAL_QT_H */

/******************* (C) COPYRIGHT 2015 INCUBECN *****END OF FILE****/
