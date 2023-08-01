/******************** (C) COPYRIGHT 2022 INCUBECN *****************************
* File Name          : script_rom.h
* Author             : Tiko Zhong
* Date First Issued  : 16/oct,2022
* Description        :
********************************************************************************
* History:e
* 06/oct,2022: V0.0
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _SCRIPT_ROM_H
#define _SCRIPT_ROM_H

#include "misc.h"

typedef void (*cbSrciptRom)(void* rsrc);

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
typedef struct{
	u16 blockIndxStart, blockIndxEnd;
	u32 blockSize;
	u16 pageSize;
	void (*blockErase)	(u16 blckIndx, u16 count, void (*CB)(void*), void* rsrcX);
	void (*pagePrg)		(uint32_t Page_Address, uint32_t OffsetInByte, const uint8_t *pBuffer, uint32_t NumByteToWrite_up_to_PageSize);
	void (*ReadBytes)	(uint32_t ReadAddr, uint8_t *pBuffer, uint32_t NumByteToRead);
	cbSrciptRom CB;
	void* obj;
	u8 squ;
} scriptRomRsrc_t;

typedef struct{
	scriptRomRsrc_t rsrc;
	s32 (*EraseAsync)(scriptRomRsrc_t*, void (*CB)(void*), void*);
	s32 (*Append)(scriptRomRsrc_t*);
	s32 (*Read)(scriptRomRsrc_t*, uint32_t ReadAddr, uint8_t *pBuffer, uint32_t NumByteToRead);
	void (*Prc)(scriptRomRsrc_t*, u8 tick);
} scriptRomDev_t;
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
u8 scriptCmd(void *dev, const char* CMD, void (*xprint)(const char* FORMAT_ORG, ...));
s32 setup_scriptRom(
	scriptRomDev_t *dev,
	u32 blckSize,
	u16 pageSize,
	u16 blckIndxStart,
	u8 blckCount,
	void (*blockErase)	(u16 blckIndx, u16 count, void (*CB)(void*), void*),
	void (*pagePrg)		(uint32_t Page_Address, uint32_t OffsetInByte, const uint8_t *pBuffer, uint32_t NumByteToWrite_up_to_PageSize),
	void (*ReadBytes)	(uint32_t ReadAddr, uint8_t *pBuffer, uint32_t NumByteToRead)
);

#endif /* _APLENTAL_QT_H */

/******************* (C) COPYRIGHT 2015 INCUBECN *****END OF FILE****/
