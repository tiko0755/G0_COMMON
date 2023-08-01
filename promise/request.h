/******************** (C) COPYRIGHT 2022 INCUBECN *****************************
* File Name          : request.h
* Author             : Tiko Zhong
* Date First Issued  : 26/oct,2022
* Description        : This file provides a set of functions needed to manage the
*                      stepper ramp generator
********************************************************************************
* History:e
* 06/oct,2022: V0.0
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _REQUEST_H
#define _REQUEST_H

#include "misc.h"
#include "ring_buffer.h"

#define REQUEST_POOL_LEN	(256)
#define REQUEST_DEPTH		(16)	// for most commands have 24 bytes maximum len

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
typedef void (*cb_resp)(void* rsrc, const char*);

typedef struct{
	u32 timeout;		// wait response timeout
	cb_resp cbHasSend;	// callback when receive a line
	cb_resp cbHasRcv;	// callback when receive a line
	u16 reqLen;			// send string len, this string in reqRB
	void* id;
}request_resp_t;

typedef struct{
	request_resp_t curResp;
	RINGBUFF_T reqRB;	// command line ring buffer
	RINGBUFF_T respRB;	// callback ring buffer
	u8 cmdPool[REQUEST_POOL_LEN];
	request_resp_t respPool[16];
	void (*xPrintS)(const char* ORG);
	u8 squ;
	u32 tick;
} requestRsrc_t;

typedef struct{
	requestRsrc_t rsrc;
	s32 (*Add)(requestRsrc_t* r, const char* req, u32 timeout, void* id, cb_resp cbHasSend, cb_resp cbRcv);
	void (*PeriodJob)(requestRsrc_t*, u8 tick);
	void (*AsyncJob)(requestRsrc_t*, const char* RECV_LINE);
} requestDev_t;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

void setup_request(requestDev_t* dev, void (*xPrintS)(const char* ORG));

#endif /* _REQUEST_H */

/******************* (C) COPYRIGHT 2015 INCUBECN *****END OF FILE****/
