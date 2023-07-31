/**********************************************************
filename: LY68L6400.c
**********************************************************/

/************************����ͷ�ļ�***************************************************/
#include "LY68L6400.h"
#include "misc.h"
#include <string.h>
/**********************************************************
 Private function
**********************************************************/
static s32 SRamReadBuf(LY68L6400_Rsrc_T* pRsrc, u32 regAddr, u8 *pBuf, u16 nBytes);
static s32 SRamWriteBuf(LY68L6400_Rsrc_T* pRsrc, u32 regAddr, const u8 *pBuf, u16 nBytes);
static void SRamIRQ_TxRxCpltCallback(LY68L6400_Rsrc_T* pRsrc,SPI_HandleTypeDef* hspi);
static void SRamIRQ_ErrorCallback(LY68L6400_Rsrc_T* pRsrc,SPI_HandleTypeDef* hspi);
/**********************************************************
 Public function
**********************************************************/
void LY68L6400_Setup(
	LY68L6400_Dev_T *pDev, 
	SPI_HandleTypeDef* hspi, 
	const PIN_T CS
){
	pDev->rsrc.CS = CS;
	pDev->rsrc.hspi = hspi;
	pDev->Read = SRamReadBuf;
	pDev->Write = SRamWriteBuf;
	pDev->TxRxCpltCB = SRamIRQ_TxRxCpltCallback;
	pDev->ErrorCB = SRamIRQ_ErrorCallback;
	pDev->rsrc.flag = 0;
	HAL_GPIO_WritePin(pDev->rsrc.CS.GPIOx, pDev->rsrc.CS.GPIO_Pin, GPIO_PIN_SET);
}

/**********************************************************
 read data
**********************************************************/
static s32 SRamReadBuf(LY68L6400_Rsrc_T* pRsrc, u32 regAddr, u8 *pBuf, u16 nBytes){
	u8 pre[4];
	
	pre[0] = 0x03;
	pre[1] = 0xff&(regAddr>>16);
	pre[2] = 0xff&(regAddr>>8);
	pre[3] = 0xff&regAddr;
	
	HAL_GPIO_WritePin(pRsrc->CS.GPIOx, pRsrc->CS.GPIO_Pin, GPIO_PIN_RESET);
	while(HAL_SPI_Transmit(pRsrc->hspi, pre, 4, 1) != HAL_OK){	};
	if(HAL_SPI_Receive_IT(pRsrc->hspi, pBuf, nBytes) != HAL_OK){	};
	HAL_GPIO_WritePin(pRsrc->CS.GPIOx, pRsrc->CS.GPIO_Pin, GPIO_PIN_SET);
	return 0;
}
/**********************************************************
 write data
**********************************************************/
static s32 SRamWriteBuf(LY68L6400_Rsrc_T* pRsrc, u32 regAddr, const u8 *pBuf, u16 nBytes){
	u8 pre[4];
	
	pre[0] = 0x02;
	pre[1] = 0xff&(regAddr>>16);
	pre[2] = 0xff&(regAddr>>8);
	pre[3] = 0xff&regAddr;
	
	HAL_GPIO_WritePin(pRsrc->CS.GPIOx, pRsrc->CS.GPIO_Pin, GPIO_PIN_RESET);
	while(HAL_SPI_Transmit(pRsrc->hspi, pre, 4, 1) != HAL_OK){};
	if(HAL_SPI_Transmit_IT(pRsrc->hspi, (u8*)pBuf, nBytes) != HAL_OK){	};
	HAL_GPIO_WritePin(pRsrc->CS.GPIOx, pRsrc->CS.GPIO_Pin, GPIO_PIN_SET);
	return 0;
}

static void SRamIRQ_TxRxCpltCallback(LY68L6400_Rsrc_T* pRsrc, SPI_HandleTypeDef* hspi){
	if(pRsrc->hspi->Instance != hspi->Instance)	return;
	pRsrc->flag &= (0xff^BIT(0));
}

static void SRamIRQ_ErrorCallback(LY68L6400_Rsrc_T* pRsrc, SPI_HandleTypeDef* hspi){
	if(pRsrc->hspi->Instance != hspi->Instance)	return;
	pRsrc->flag &= (0xff^BIT(1));
}
/**********************************************************
 == THE END ==
**********************************************************/

