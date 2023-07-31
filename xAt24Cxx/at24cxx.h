/**********************************************************
filename: at24cxx.h
**********************************************************/
#ifndef _AT24C256_H_
#define _AT24C256_H_

#include "misc.h"
#include "IIC_IO.h"

/*****************************************************************************
 @ typedefs
****************************************************************************/
typedef struct{
	u32 capacityBytes;
	u8 pageSizeBytes;
}AT24CXX_CHIP;

typedef struct{
	//char name[DEV_NAME_LEN];
	/* config	*/
	IIC_IO_Dev_T DevIIC;
	IIC_IO_Dev_T *pIIC;
	AT24CXX_CHIP chip;
	u8 slvWrtAddr;	//slave write addr
	//u32 tick;
	u8 writeCycTime;
}AT24CXX_Rsrc_T;

extern const AT24CXX_CHIP HT2201;
extern const AT24CXX_CHIP AT24C02;
extern const AT24CXX_CHIP AT24C04;
extern const AT24CXX_CHIP AT24C08;
extern const AT24CXX_CHIP AT24C16;
extern const AT24CXX_CHIP AT24C32;
extern const AT24CXX_CHIP AT24C64;
extern const AT24CXX_CHIP AT24C128;
extern const AT24CXX_CHIP AT24C256;
extern const AT24CXX_CHIP AT24C512;

typedef struct{
	AT24CXX_Rsrc_T rsrc;
	//basic
	s8 (*Write)(AT24CXX_Rsrc_T* pRsrc, u16 addr, const u8 *pDat, u16 nBytes);
	s8 (*Read)(AT24CXX_Rsrc_T* pRsrc, u16 addr, u8 *pDat, u16 nBytes);	
}AT24CXX_Dev_T;

void AT24CXX_Setup(
	AT24CXX_Dev_T *pDev, 
	const PIN_T scl, 
	const PIN_T sda, 
	const AT24CXX_CHIP chip,
	const u8 cfgAddr	//a0..A2, 0..7
);
	
void AT24CXX_Setup1(
	AT24CXX_Dev_T *pDev,
	IIC_IO_Dev_T *pIIC,
	const AT24CXX_CHIP chip,
	const u8 cfgAddr	//a0..a2, 0..7
);

#endif
