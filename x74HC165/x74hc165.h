/**********************************************************
filename: x74hc165.h
**********************************************************/
#ifndef _74HC165_H_
#define _74HC165_H_

#include "misc.h"

#define X74HC165_TW	50	// pulse width time, tw

/*****************************************************************************
 @ typedefs
****************************************************************************/
typedef struct{
	/* config	*/
	PIN_T SCK, LCK, DAT;	// SCK,LCK config as output, DAT config as input
	u8 serial[8];
	void (*cbFalling)(u8 pinIndx);
	void (*cbRaising)(u8 pinIndx);
}x74hc165Rsrc_T;

typedef struct{
	x74hc165Rsrc_T rsrc;
	void (*Polling)	(x74hc165Rsrc_T* pRsrc);
	u8 (*readpin)	(x74hc165Rsrc_T *pRsrc, u8 indx);
	u8 (*read)		(x74hc165Rsrc_T *pRsrc);
}hc165_Dev_T;

void x74hc165_Setup(
	hc165_Dev_T *pDev,
	const PIN_T sck,
	const PIN_T lck,
	const PIN_T dat
);
	
#endif
