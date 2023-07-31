/**********************************************************
filename: at24cxx.c
**********************************************************/

/************************包含头文件***************************************************/
#include "at24cxx.h"
#include "misc.h"

#define DEVICE_ADDR 0xa0
#define TW_MS 5

const AT24CXX_CHIP HT2201 = {128,16};		//
const AT24CXX_CHIP AT24C01 = {128,16};	//
const AT24CXX_CHIP AT24C02 = {256,16};	//
const AT24CXX_CHIP AT24C04 = {512,16};	//
const AT24CXX_CHIP AT24C08 = {1024*1,16};	//
const AT24CXX_CHIP AT24C16 = {1024*2,16};	//
const AT24CXX_CHIP AT24C32 = {1024*4,32};	//
const AT24CXX_CHIP AT24C64 = {1024*8,32};	//
const AT24CXX_CHIP AT24C128 = {1024*16,64};	//
const AT24CXX_CHIP AT24C256 = {1024*32,64};	//
const AT24CXX_CHIP AT24C512 = {1024*64,128};	//

/**********************************************************
 Private function
**********************************************************/
static s8 readBuf(AT24CXX_Rsrc_T* pRsrc, u16 regAddr, u8 *pBuf, u16 nBytes);
static s8 writeBuf(AT24CXX_Rsrc_T* pRsrc, u16 regAddr, const u8 *pBuf, u16 nBytes);
static s8 writePage(AT24CXX_Rsrc_T* pRsrc, u16 regAddr, const u8 *pBuf, u16 nBytes);

/**********************************************************
 Public function
**********************************************************/
void AT24CXX_Setup(
	AT24CXX_Dev_T *pDev,
	const PIN_T scl, 
	const PIN_T sda, 
	const AT24CXX_CHIP chip,
	const u8 cfgAddr	//a0..a2, 0..7
){
	IIC_IO_Setup(&pDev->rsrc.DevIIC, scl, sda);
	AT24CXX_Setup1(pDev, &pDev->rsrc.DevIIC, chip, cfgAddr);
}

void AT24CXX_Setup1(
	AT24CXX_Dev_T *pDev,
	IIC_IO_Dev_T *pIIC,
	const AT24CXX_CHIP chip,
	const u8 cfgAddr	//a0..a2, 0..7
){
	pDev->rsrc.pIIC = pIIC;
	pDev->rsrc.slvWrtAddr = DEVICE_ADDR|(cfgAddr<<1);
	pDev->rsrc.chip = chip;
	//pDev->rsrc.tick = HAL_GetTick();
	//basic op
	pDev->Write = writeBuf;
	pDev->Read = readBuf;
}

/**********************************************************
 read data
**********************************************************/
s8 readBuf(AT24CXX_Rsrc_T* pRsrc, u16 regAddr, u8 *pBuf, u16 nBytes){
	IIC_IO_Dev_T* iic = pRsrc->pIIC;
	if(nBytes == 0 )	return -1;
	if(regAddr+nBytes-1 >= pRsrc->chip.capacityBytes)	return -2;
	if(pRsrc->chip.capacityBytes >= AT24C32.capacityBytes)
		return(iic->Read16(&iic->rsrc, pRsrc->slvWrtAddr, regAddr, pBuf, nBytes));
	else
		return(iic->Read(&iic->rsrc, pRsrc->slvWrtAddr, regAddr, pBuf, nBytes));
}

/**********************************************************
 write data
**********************************************************/
s8 writePage(AT24CXX_Rsrc_T* pRsrc, u16 regAddr, const u8 *pBuf, u16 nBytes){
	IIC_IO_Dev_T* iic = pRsrc->pIIC;
	if((nBytes > pRsrc->chip.pageSizeBytes) || nBytes==0 )	return -3;
	if(pRsrc->chip.capacityBytes >= AT24C32.capacityBytes)
		return(iic->Write16(&iic->rsrc, pRsrc->slvWrtAddr, regAddr, pBuf, nBytes));
	else
		return(iic->Write(&iic->rsrc, pRsrc->slvWrtAddr, regAddr, pBuf, nBytes));
}

s8 writeBuf(AT24CXX_Rsrc_T* pRsrc, u16 regAddr, const u8 *pBuf, u16 nBytes){
	u16 thsAddr = regAddr, count;
	u8 tmpU8;
	s8 rtn;

	//if more than capacity
	if(nBytes == 0 )	return -4;
	if(thsAddr+nBytes-1 >= pRsrc->chip.capacityBytes)	return -5;

	for(count=0; (nBytes-count)>0; ){
		tmpU8 = thsAddr^0xffff;
		tmpU8 &= pRsrc->chip.pageSizeBytes-1;
		tmpU8 += 1;	//bytes can be write it this page from regAddr
		if(nBytes-count < tmpU8)	tmpU8 = nBytes-count;
		rtn = writePage(pRsrc, thsAddr, &pBuf[count], tmpU8);
		if( rtn<0)	return rtn;
		HAL_Delay(TW_MS);
		thsAddr += tmpU8;
		count += tmpU8;
	}
	return 0;
}

/**********************************************************
 == THE END ==
**********************************************************/
