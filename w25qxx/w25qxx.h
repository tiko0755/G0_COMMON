#ifndef _W25QXX_H
#define _W25QXX_H

/*
  Author:     Nima Askari
  WebSite:    http://www.github.com/NimaLTD
  Instagram:  http://instagram.com/github.NimaLTD
  Youtube:    https://www.youtube.com/channel/UCUhY7qY1klJm1d2kulr9ckw
  
  Version:    1.1.x	Tiko
  

  
  (1.1.2)
  Fix read ID.
  
  (1.1.1)
  Fix some errors.
  
  (1.1.0)
  Fix some errors.
  
  (1.0.0)
  First release.
*/

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdbool.h>
#include "misc.h"

#define _W25QXX_DEBUG	0
#define W25QXX_PAGE_SZ			256		//1 Page = 256 bytes
#define W25QXX_SECTOR_SZ 		4096	//1 Sector = 4096 bytes

typedef enum
{
	W25Q10=1,
	W25Q20,
	W25Q40,
	W25Q80,
	W25Q16,
	W25Q32,
	W25Q64,
	W25Q128,
	W25Q256,
	W25Q512,
}W25QXX_ID_t;

typedef struct{
	//hard ware
	PIN_T CS;
	SPI_HandleTypeDef* SPI_HANDLE;
	// chip info
	W25QXX_ID_t	ID;
	uint8_t		UniqID[8];
//	uint16_t	PageSize;
	uint32_t	PageCount;
//	uint32_t	SectorSize;
	uint32_t	SectorCount;
	uint32_t	BlockSize;
	uint32_t	BlockCount;
	uint32_t	CapacityInKiloByte;
	uint32_t	UsrCapacityInKiloByte;
	uint8_t		StatusRegister1;
	uint8_t		StatusRegister2;
	uint8_t		StatusRegister3;	
	uint8_t		Lock;
	uint8_t		isInitial;

	// async task
	u8 squ;
	const uint8_t *pBuffer;
	void (*cb)	(void);
	u32 bytes;

// async write bytes ta	sk
	u32 byteAddrStart,byteAddrEnd, byteAddrCur;
	u16 pageAddrStart,pageAddrEnd, pageCur;
	u16 sectorAddrS, sectorAddrE, sectorCur;

	u32 addr,offset;


}w25qxxRsrc_t;

typedef struct{
	w25qxxRsrc_t rsrc;
	// API
	void 	(*ReadBytes)	(w25qxxRsrc_t*, uint32_t ReadAddr, uint8_t *pBuffer, uint32_t NumByteToRead);
	void 	(*WriteBytesAsync)	(w25qxxRsrc_t*, uint32_t WriteAddr, const uint8_t *pBuffer, uint32_t NumByteToWrite, void (*cb)	(void));
	void	(*Polling)		(w25qxxRsrc_t*);

	// lower driver
	bool	(*Init)			(w25qxxRsrc_t*);
	void	(*EraseChip)	(w25qxxRsrc_t*);		// cost about 9sec
	void 	(*EraseSector)	(w25qxxRsrc_t*, uint32_t SectorAddr);	// cost about 90ms
	bool 	(*IsEmptySector)(w25qxxRsrc_t*, uint32_t Sector_Address, uint32_t OffsetInByte, uint32_t NumByteToCheck_up_to_SectorSize);
	void 	(*WritePage)	(w25qxxRsrc_t*, uint32_t Page_Address, uint32_t OffsetInByte, const uint8_t *pBuffer, uint32_t NumByteToWrite_up_to_PageSize);
	void 	(*ReadPage)		(w25qxxRsrc_t*, uint32_t Page_Address, uint32_t OffsetInByte, uint8_t *pBuffer, uint32_t NumByteToRead_up_to_PageSize);
}w25qxxDev_t;

s32 setup_w25qxx(
	w25qxxDev_t* dev, 	
	PIN_T CS, 
	SPI_HandleTypeDef* SPI_HANDLE
);


#ifdef __cplusplus
}
#endif

#endif
