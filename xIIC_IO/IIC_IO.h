/**********************************************************
filename: IIC_IO.h
**********************************************************/
#ifndef _IIC_IO_H_
#define _IIC_IO_H_

#include "misc.h"

/*****************************************************************************
 @ typedefs
****************************************************************************/
#pragma pack(push,4)        // push current align bytes, and then set 4 bytes align
typedef struct{
    /* config    */
    const PIN_T* SCL;
    const PIN_T* SDA;
}IIC_IO_Rsrc_T;

typedef struct{
    IIC_IO_Rsrc_T rsrc;
    //op
    s8 (*Write)    (IIC_IO_Rsrc_T* pRsrc, u8 slaveWrtAddr, u8 regAddr, const u8* pDat, u8 datLen);
    s8 (*Read)    (IIC_IO_Rsrc_T* pRsrc, u8 slaveWrtAddr, u8 regAddr, u8* pDat, u8 datLen);
    s8 (*Write16)    (IIC_IO_Rsrc_T* pRsrc, u8 slaveWrtAddr, u16 regAddr, const u8* pDat, u8 datLen);
    s8 (*Read16)    (IIC_IO_Rsrc_T* pRsrc, u8 slaveWrtAddr, u16 regAddr, u8* pDat, u8 datLen);    
}IIC_IO_Dev_T;
#pragma pack(pop)           //recover align bytes from 4 bytes

void IIC_IO_Setup(IIC_IO_Dev_T *pDev, const PIN_T* SCL, const PIN_T* SDA);

#endif
