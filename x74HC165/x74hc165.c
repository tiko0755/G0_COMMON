/**********************************************************
filename: x74hc165.c
**********************************************************/

/************************����ͷ�ļ�***************************************************/
#include "x74hc165.h"
#include "misc.h"
#include <string.h>
#include "gpioDecal.h"

/**********************************************************
 Private function
**********************************************************/
static void hc165Polling	(x74hc165Rsrc_T* pRsrc);
static u8 hc165Readpin	(x74hc165Rsrc_T *pRsrc, u8 indx);
static u8 hc165Read		(x74hc165Rsrc_T *pRsrc);
/**********************************************************
 Public function
**********************************************************/
void x74hc165_Setup(
		hc165_Dev_T *pDev,
	const PIN_T sck,
	const PIN_T lck,
	const PIN_T dat
){
	x74hc165Rsrc_T *p = &pDev->rsrc;
	memset(p, 0, sizeof(x74hc165Rsrc_T));
	p->SCK = sck;
	p->LCK = lck;
	p->DAT = dat;
	pDev->Polling = hc165Polling;
	pDev->read = hc165Read;
	pDev->readpin = hc165Readpin;
	writePin(sck, GPIO_PIN_RESET);
}

static void hc165Delay(u8 delay){
	u8 i;
	for(i=0;i<delay;i++)	NOP();
}
/**********************************************************
 hc165Polling
**********************************************************/
static void hc165Polling(x74hc165Rsrc_T* pRsrc){
	u8 i;
	GPIO_PinState staBck = readPin(pRsrc->LCK);
	writePin(pRsrc->LCK, GPIO_PIN_RESET);
	hc165Delay(50);
	writePin(pRsrc->SCK, GPIO_PIN_RESET);
	writePin(pRsrc->LCK, GPIO_PIN_SET);
	hc165Delay(50);
	// MSB FIRST
	for(i=0;i<8;i++){
		pRsrc->serial[i] <<= 1;
		if(readPin(pRsrc->DAT) == GPIO_PIN_SET)	pRsrc->serial[i] |= BIT(0);
		writePin(pRsrc->SCK, GPIO_PIN_SET);
		hc165Delay(50);
		writePin(pRsrc->SCK, GPIO_PIN_RESET);
		hc165Delay(50);
	}
	writePin(pRsrc->LCK, staBck);	// recover
	// toggle event
	for(i=0;i<8;i++){
		if(pRsrc->cbFalling && (pRsrc->serial[i] & ((BIT(7)|BIT(0)) == BIT(7))) )
			pRsrc->cbFalling(i);
		if(pRsrc->cbRaising && (pRsrc->serial[i] & ((BIT(7)|BIT(0)) == BIT(0))) )
			pRsrc->cbRaising(i);
	}
}

/**********************************************************
 hc165Readpin
**********************************************************/
static u8 hc165Readpin	(x74hc165Rsrc_T *pRsrc, u8 indx){
	return(pRsrc->serial[indx] & BIT(0));
}

/**********************************************************
 hc165Readpin
**********************************************************/
static u8 hc165Read		(x74hc165Rsrc_T *pRsrc){
	u8 sta=0,i;
	for(i=0;i<8;i++){
		sta >>= 1;
		if(pRsrc->serial[i] & BIT(0))	sta |= BIT(7);
	}
	return sta;
}
/**********************************************************
 == THE END ==
**********************************************************/
