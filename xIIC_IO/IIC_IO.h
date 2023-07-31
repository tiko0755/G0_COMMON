/**********************************************************
filename: IIC_IO.h
**********************************************************/
#ifndef _IIC_IO_H_
#define _IIC_IO_H_

#include "misc.h"

/*****************************************************************************
 @ typedefs
****************************************************************************/
typedef struct{
	/* config	*/
	PIN_T SCL;
	PIN_T SDA;
}IIC_IO_Rsrc_T;

typedef struct{
	IIC_IO_Rsrc_T rsrc;
	//op
	s8 (*Write)	(IIC_IO_Rsrc_T* pRsrc, u8 slaveWrtAddr, u8 regAddr, const u8* pDat, u8 datLen);
	s8 (*Read)	(IIC_IO_Rsrc_T* pRsrc, u8 slaveWrtAddr, u8 regAddr, u8* pDat, u8 datLen);
	s8 (*Write16)	(IIC_IO_Rsrc_T* pRsrc, u8 slaveWrtAddr, u16 regAddr, const u8* pDat, u8 datLen);
	s8 (*Read16)	(IIC_IO_Rsrc_T* pRsrc, u8 slaveWrtAddr, u16 regAddr, u8* pDat, u8 datLen);	
}IIC_IO_Dev_T;

void IIC_IO_Setup(IIC_IO_Dev_T *pDev, const PIN_T scl, const PIN_T sda);

#endif
