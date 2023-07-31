/**********************************************************
filename: LY68L6400.h
**********************************************************/
#ifndef _LY68L6400_H_
#define _LY68L6400_H_

#include "misc.h"

/*****************************************************************************
 @ typedefs
****************************************************************************/
typedef struct{
	/* config	*/
	PIN_T CS;
	SPI_HandleTypeDef* hspi;
	u8 flag;
}LY68L6400_Rsrc_T;

typedef struct{
	LY68L6400_Rsrc_T rsrc;
	//basic
	s32 (*Write)(LY68L6400_Rsrc_T* pRsrc, u32 addr, const u8 *pDat, u16 nBytes);
	s32 (*Read)(LY68L6400_Rsrc_T* pRsrc, u32 addr, u8 *pDat, u16 nBytes);
	void (*TxRxCpltCB)(LY68L6400_Rsrc_T* pRsrc, SPI_HandleTypeDef* hspi);
	void (*ErrorCB)(LY68L6400_Rsrc_T* pRsrc, SPI_HandleTypeDef* hspi);
}LY68L6400_Dev_T;

void LY68L6400_Setup(
	LY68L6400_Dev_T *pDev, 
	SPI_HandleTypeDef* hspi, 
	const PIN_T CS
);

#endif
