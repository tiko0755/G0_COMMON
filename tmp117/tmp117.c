/**********************************************************
filename: tmp117.c
**********************************************************/

/************************����ͷ�ļ�***************************************************/
#include "tmp117.h"
#include "string.h"
#include "misc.h"

#define DEVICE_ADDR 0x90
#define TW_MS 5

/**********************************************************
 Private function
**********************************************************/
static s32 tmp117_temperature(TMP117_Rsrc_T* pRsrc);
static void tmp117_polling(TMP117_Rsrc_T* pRsrc, u8 tick);
static s8 tmp117_writeReg(TMP117_Rsrc_T* pRsrc, u8 regAddr, u16 val);
static u16 tmp117_readReg(TMP117_Rsrc_T* pRsrc, u8 regAddr);
static u16 tmp117_getID(TMP117_Rsrc_T* pRsrc);
static s8 tmp117_offset(TMP117_Rsrc_T* pRsrc, s16 offset);
static s16 tmp117_offget(TMP117_Rsrc_T* pRsrc);
/**********************************************************
 Public function
**********************************************************/
void TMP117_Setup(
	TMP117_Dev_T *pDev, 
	const PIN_T SCL, 
	const PIN_T SDA, 
	const u8 cfgAddr,	//A0, 0..3
	const char* NAME
){
	IIC_IO_Setup(&pDev->rsrc.DevIIC, SCL, SDA);
	TMP117_Setup1(pDev, &pDev->rsrc.DevIIC, cfgAddr, NAME);
}

void TMP117_Setup1(
	TMP117_Dev_T *pDev,
	IIC_IO_Dev_T *pIIC,
	const u8 cfgAddr,	//A0, 0..3
	const char* NAME
){
	pDev->rsrc.pIIC = pIIC;
	pDev->rsrc.slvWrtAddr = DEVICE_ADDR|(cfgAddr<<1);
#if TMP117_USING_NAME
	memset(pDev->rsrc.name,0,DEV_NAME_LEN);
	strcpy(pDev->rsrc.name, NAME);
#endif
	memset(pDev->rsrc.tempSerial,0,sizeof(s32)*4);
	pDev->rsrc.tmr = 200;
	
	//basic op
	pDev->polling = tmp117_polling;
	pDev->temperature = tmp117_temperature;
	pDev->regRead = tmp117_readReg;
	pDev->regWrite = tmp117_writeReg;
	pDev->getID = tmp117_getID;
	pDev->offget = tmp117_offget;
	pDev->offset = tmp117_offset;
}

/**********************************************************
 get temperatur
**********************************************************/
static s32 tmp117_temperature(TMP117_Rsrc_T* pRsrc){
	s32 temp = 0;
	temp = tmp117_readReg(pRsrc, 0x00);
	temp *= 125;
	temp >>= 4;
	return temp;
}

/**********************************************************
 write data
**********************************************************/
static void tmp117_polling(TMP117_Rsrc_T* pRsrc, u8 tick){
	u16 conf;
	pRsrc->tick += tick;
	if(pRsrc->tick > pRsrc->tmr){
		pRsrc->tick = 0;
		conf = tmp117_readReg(pRsrc, 0x01);	//read config first
		pRsrc->tempSerial[1] = pRsrc->tempSerial[0];
		pRsrc->tempSerial[0] = tmp117_temperature(pRsrc);
//		if(conf & BIT(13)){
//			pRsrc->tempSerial[1] = pRsrc->tempSerial[0];
//			pRsrc->tempSerial[0] = tmp117_temperature(pRsrc);
//			if(pRsrc->cbNewData){
//				pRsrc->cbNewData(pRsrc->name, pRsrc->tempSerial[0]);
//			}
//		}
	}
}

/**********************************************************
 write register
**********************************************************/
static s8 tmp117_writeReg(TMP117_Rsrc_T* pRsrc, u8 regAddr, u16 val){
	u8 buf[2] = {0};
	u16 rtn = 0;
	IIC_IO_Dev_T* iic = pRsrc->pIIC;
	buf[0] = (val>>8)&0xff;
	buf[1] = val&0xff;
	iic->Write(&iic->rsrc, pRsrc->slvWrtAddr, regAddr, buf, 2);
	return 0;
}

/**********************************************************
 read register
**********************************************************/
static u16 tmp117_readReg(TMP117_Rsrc_T* pRsrc, u8 regAddr)
{
	u8 buf[2] = {0};
	u16 rtn = 0;
	IIC_IO_Dev_T* iic = pRsrc->pIIC;
	iic->Read(&iic->rsrc, pRsrc->slvWrtAddr, regAddr, buf, 2);
	rtn = buf[0];
	rtn <<= 8;
	rtn |= buf[1];
	return rtn;
}

static s8 tmp117_offset(TMP117_Rsrc_T* pRsrc, s16 offset){
	tmp117_writeReg(pRsrc, 0x07, offset);
}

static s16 tmp117_offget(TMP117_Rsrc_T* pRsrc){
	return (tmp117_readReg(pRsrc, 0x07));
}

static u16 tmp117_getID(TMP117_Rsrc_T* pRsrc){
	return(tmp117_readReg(pRsrc, 0x0f));
}

/**********************************************************
 == THE END ==
**********************************************************/
