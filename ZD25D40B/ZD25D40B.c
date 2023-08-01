#include "ZD25D40B.h"
#include <string.h>

#if (_W25QXX_DEBUG==1)
#include <stdio.h>
#include "board.h"		// using print
#endif

#define W25QXX_DUMMY_BYTE         0xA5

#define W25QXX_ASYNC_SQU_ERASE			1
#define W25QXX_ASYNC_SQU_WRITE_BYTES	20

static bool W25qxx_Init(w25qxxRsrc_t*);

static u8 W25qxx_isWriteEnd(w25qxxRsrc_t* pRsrc);
static s32 W25qxx_WritePage(w25qxxRsrc_t*, uint32_t Page_Address, uint32_t OffsetInByte, const uint8_t *pBuffer, uint32_t NumByteToWrite_up_to_PageSize);
s32 W25qxx_WriteBytesSync(w25qxxRsrc_t*, uint32_t WriteAddr, const uint8_t *pBuffer, uint32_t NumByteToWrite);
static void W25qxx_ReadBytesSync(w25qxxRsrc_t*, uint32_t ReadAddr, uint8_t *pBuffer, uint32_t NumByteToRead);
static void W25qxx_Polling(w25qxxRsrc_t* pRsrc );

static u8 W25qxx_IsAllTaskFree(w25qxxRsrc_t* r);

static s32 W25qxx_StartChipErase(w25qxxRsrc_t* r, cbNorFlash eraseCmpltCB, void* rsrcX);
static void W25qxx_ChipEraseTask(void* r);

static s32 W25qxx_StartBlockErase(w25qxxRsrc_t* r, uint32_t BlockAddr, u16 count, cbNorFlash eraseCmpltCB, void* rsrcX);
static void W25qxx_BlockEraseTask(void* r);
static void W25qxx_BlockErase_64k(w25qxxRsrc_t* r, uint32_t blockAddr);

static s32 W25qxx_StartSectorErase(w25qxxRsrc_t* r, uint32_t SectorAddr, u16 count, cbNorFlash eraseCmpltCB, void* rsrcX);
static void W25qxx_SectorEraseTask(void* r);
static void W25qxx_SectorErase_4k(w25qxxRsrc_t* r, uint32_t SectorAddr);

static void W25qxx_PageErase(w25qxxRsrc_t* r, uint32_t PageAddr);

static s32 W25qxx_StartWriteBytes(w25qxxRsrc_t*, uint32_t WriteAddr, const uint8_t *pBuffer, uint32_t NumByteToWrite, cbNorFlash eraseCmpltCB, void* rsrcX);
static void W25qxx_WriteBytesTask(void* r);


#if (_W25QXX_DEBUG==1)
static u32 StartTime;
#endif

//###################################################################################################################
s32 setup_w25qxx(
	w25qxxDev_t* dev, 	
	PIN_T CS, 
	SPI_HandleTypeDef* SPI_HANDLE
){
	w25qxxRsrc_t* pRsrc = &dev->rsrc;
	memset(pRsrc, 0, sizeof(w25qxxRsrc_t));
	pRsrc->CS = CS;
	pRsrc->SPI_HANDLE = SPI_HANDLE;

	W25qxx_Init(pRsrc);

	pRsrc->burstPrgAddr = 0xffffffff;
	// register API function
	dev->Init = W25qxx_Init;
	
	dev->IsWriteBusy = W25qxx_isWriteEnd;

	dev->WriteBytesSync = W25qxx_WriteBytesSync;
	dev->ReadBytesSync = W25qxx_ReadBytesSync;
	dev->Polling = W25qxx_Polling;

	dev->StartChipErase = W25qxx_StartChipErase;
	dev->StartBlockErase = W25qxx_StartBlockErase;
	dev->StartSectorErase = W25qxx_StartSectorErase;
	dev->StartWriteBytes = W25qxx_StartWriteBytes;

	return 0;
}

static uint8_t	W25qxx_Spi(w25qxxRsrc_t* pRsrc, uint8_t	Data)
{
	uint8_t	ret;
	HAL_SPI_TransmitReceive(pRsrc->SPI_HANDLE, &Data, &ret, 1, 5);
	return ret;	
}
//###################################################################################################################
static uint32_t W25qxx_ReadID(w25qxxRsrc_t* pRsrc)
{
  uint32_t Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;
  HAL_GPIO_WritePin(pRsrc->CS.GPIOx,pRsrc->CS.GPIO_Pin,GPIO_PIN_RESET);
  W25qxx_Spi(pRsrc, 0x9f);	// 0x9f
  Temp0 = W25qxx_Spi(pRsrc, W25QXX_DUMMY_BYTE);	//0x05
  Temp1 = W25qxx_Spi(pRsrc, W25QXX_DUMMY_BYTE);
  Temp2 = W25qxx_Spi(pRsrc, W25QXX_DUMMY_BYTE);
  HAL_GPIO_WritePin(pRsrc->CS.GPIOx, pRsrc->CS.GPIO_Pin, GPIO_PIN_SET);
  Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;
  return Temp;
}
//###################################################################################################################
static void W25qxx_ReadUniqID(w25qxxRsrc_t* pRsrc)
{
  HAL_GPIO_WritePin(pRsrc->CS.GPIOx,pRsrc->CS.GPIO_Pin,GPIO_PIN_RESET);
  W25qxx_Spi(pRsrc, 0x4B);
	for(uint8_t	i=0;i<4;i++)
		W25qxx_Spi(pRsrc, W25QXX_DUMMY_BYTE);
	for(uint8_t	i=0;i<8;i++)
		pRsrc->UniqID[i] = W25qxx_Spi(pRsrc, W25QXX_DUMMY_BYTE);
  HAL_GPIO_WritePin(pRsrc->CS.GPIOx,pRsrc->CS.GPIO_Pin,GPIO_PIN_SET);
}
//###################################################################################################################
static void W25qxx_WriteEnable(w25qxxRsrc_t* pRsrc)
{
	u8 i;
	HAL_GPIO_WritePin(pRsrc->CS.GPIOx,pRsrc->CS.GPIO_Pin,GPIO_PIN_RESET);
	W25qxx_Spi(pRsrc, 0x06);
	HAL_GPIO_WritePin(pRsrc->CS.GPIOx,pRsrc->CS.GPIO_Pin,GPIO_PIN_SET);
	for(i=0;i<200;i++){	NOP();	}
}

//###################################################################################################################
static uint8_t W25qxx_ReadStatusRegister(w25qxxRsrc_t* pRsrc, uint8_t	SelectStatusRegister_1_2_3)
{
	uint8_t	status=0;
	HAL_GPIO_WritePin(pRsrc->CS.GPIOx,pRsrc->CS.GPIO_Pin,GPIO_PIN_RESET);
	if(SelectStatusRegister_1_2_3==1)
	{
		W25qxx_Spi(pRsrc, 0x05);
		status=W25qxx_Spi(pRsrc, W25QXX_DUMMY_BYTE);	
		pRsrc->StatusRegister1 = status;
	}
	else if(SelectStatusRegister_1_2_3==2)
	{
		W25qxx_Spi(pRsrc, 0x35);
		status=W25qxx_Spi(pRsrc, W25QXX_DUMMY_BYTE);	
		pRsrc->StatusRegister2 = status;
	}
	else
	{
		W25qxx_Spi(pRsrc, 0x15);
		status=W25qxx_Spi(pRsrc, W25QXX_DUMMY_BYTE);
		pRsrc->StatusRegister3 = status;
	}	
	HAL_GPIO_WritePin(pRsrc->CS.GPIOx,pRsrc->CS.GPIO_Pin,GPIO_PIN_SET);
	return status;
}

//###################################################################################################################
static u8 W25qxx_isWriteEnd(w25qxxRsrc_t* pRsrc)
{
	HAL_GPIO_WritePin(pRsrc->CS.GPIOx,pRsrc->CS.GPIO_Pin,GPIO_PIN_RESET);
	W25qxx_Spi(pRsrc, 0x05);
	pRsrc->StatusRegister1 = W25qxx_Spi(pRsrc, W25QXX_DUMMY_BYTE);
	HAL_GPIO_WritePin(pRsrc->CS.GPIOx,pRsrc->CS.GPIO_Pin,GPIO_PIN_SET);
	return (pRsrc->StatusRegister1 & BIT(0));
}

//###################################################################################################################
static bool W25qxx_Init(w25qxxRsrc_t* pRsrc){
  pRsrc->Lock=1;
  uint32_t	id;

  HAL_GPIO_WritePin(pRsrc->CS.GPIOx,pRsrc->CS.GPIO_Pin,GPIO_PIN_SET);
  HAL_Delay(1);

  HAL_GPIO_WritePin(pRsrc->CS.GPIOx,pRsrc->CS.GPIO_Pin,GPIO_PIN_RESET);
  W25qxx_Spi(pRsrc, 0x05);
  HAL_GPIO_WritePin(pRsrc->CS.GPIOx, pRsrc->CS.GPIO_Pin, GPIO_PIN_SET);
  HAL_Delay(1);

  #if (_W25QXX_DEBUG==1)
  print("w25qxx Init Begin...\r\n");
  #endif
  id=W25qxx_ReadID(pRsrc);
	
  #if (_W25QXX_DEBUG==1)
  print("w25qxx ID:0x%X\r\n", id);
  #endif
  switch(id & 0x000000FF)
  {
		case 0x20:	// 	w25q512
			pRsrc->ID=W25Q512;
			pRsrc->BlockCount=1024;
			#if (_W25QXX_DEBUG==1)
			print("w25qxx Chip: w25q512\r\n");
			#endif
		break;
		case 0x19:	// 	w25q256
			pRsrc->ID=W25Q256;
			pRsrc->BlockCount=512;
			#if (_W25QXX_DEBUG==1)
			print("w25qxx Chip: w25q256\r\n");
			#endif
		break;
		case 0x18:	// 	w25q128
			pRsrc->ID=W25Q128;
			pRsrc->BlockCount=256;
			#if (_W25QXX_DEBUG==1)
			print("w25qxx Chip: w25q128\r\n");
			#endif
		break;
		case 0x17:	//	w25q64
			pRsrc->ID=W25Q64;
			pRsrc->BlockCount=128;
			#if (_W25QXX_DEBUG==1)
			print("w25qxx Chip: w25q64\r\n");
			#endif
		break;
		case 0x16:	//	w25q32
			pRsrc->ID=W25Q32;
			pRsrc->BlockCount=64;
			#if (_W25QXX_DEBUG==1)
			print("w25qxx Chip: w25q32\r\n");
			#endif
		break;
		case 0x15:	//	w25q16
			pRsrc->ID=W25Q16;
			pRsrc->BlockCount=32;
			#if (_W25QXX_DEBUG==1)
			print("w25qxx Chip: w25q16\r\n");
			#endif
		break;
		case 0x14:	//	w25q80
			pRsrc->ID=W25Q80;
			pRsrc->BlockCount=16;
			#if (_W25QXX_DEBUG==1)
			print("w25qxx Chip: w25q80\r\n");
			#endif
		break;
		case 0x13:	//	w25q40
			pRsrc->ID=W25Q40;
			pRsrc->BlockCount=8;
			#if (_W25QXX_DEBUG==1)
			print("w25qxx Chip: w25q40\r\n");
			#endif
		break;
		case 0x12:	//	w25q20
			pRsrc->ID=W25Q20;
			pRsrc->BlockCount=4;
			#if (_W25QXX_DEBUG==1)
			print("w25qxx Chip: w25q20\r\n");
			#endif
		break;
		case 0x11:	//	w25q10
			pRsrc->ID=W25Q10;
			pRsrc->BlockCount=2;
			#if (_W25QXX_DEBUG==1)
			print("w25qxx Chip: w25q10\r\n");
			#endif
		break;
		default:
				#if (_W25QXX_DEBUG==1)
				print("w25qxx Unknown ID\r\n");
				#endif
			pRsrc->Lock=0;	
			return false;
				
	}		

	pRsrc->SectorCount=pRsrc->BlockCount*16;
	pRsrc->PageCount = (pRsrc->SectorCount*W25QXX_SECTOR_SZ)/W25QXX_PAGE_SZ;
	pRsrc->BlockSize = W25QXX_SECTOR_SZ*16;
	pRsrc->CapacityInKiloByte = (pRsrc->SectorCount*W25QXX_SECTOR_SZ)/1024;

	#if (_W25QXX_DEBUG==1)
	print("BlockCount:\t%d\r\n", pRsrc->BlockCount);
	print("SectorCount:\t%d\r\n", pRsrc->SectorCount);
	print("PageCount:\t%d\r\n", pRsrc->PageCount);
	print("BlockSize:\t%d\r\n", pRsrc->BlockSize);
	print("SectorSize:\t%d\r\n", W25QXX_SECTOR_SZ);
	print("PageSize:\t%d\r\n", W25QXX_PAGE_SZ);
	print("CapacityInKiloByte:\t%d\r\n", pRsrc->CapacityInKiloByte);
	#endif

	pRsrc->UsrCapacityInKiloByte = ((pRsrc->SectorCount-1)*W25QXX_SECTOR_SZ)/1024;	//last sector for swap
	W25qxx_ReadUniqID(pRsrc);
	W25qxx_ReadStatusRegister(pRsrc, 1);
	W25qxx_ReadStatusRegister(pRsrc, 2);
	W25qxx_ReadStatusRegister(pRsrc, 3);

	pRsrc->Lock=0;
	pRsrc->isInitial = 1;
	return true;
}	

//###################################################################################################################
static s32 W25qxx_WritePage(w25qxxRsrc_t* pRsrc, uint32_t Page_Address, uint32_t OffsetInByte, const uint8_t *pBuffer, uint32_t NumByteToWrite_up_to_PageSize){
	u32 bytes, address;

	if(W25qxx_isWriteEnd(pRsrc) > 0){	return -1;	}
	pRsrc->Lock=1;
	bytes = NumByteToWrite_up_to_PageSize;
	if((NumByteToWrite_up_to_PageSize + OffsetInByte) > W25QXX_PAGE_SZ)
		bytes = W25QXX_PAGE_SZ - OffsetInByte;
	else if(NumByteToWrite_up_to_PageSize==0)
		bytes = W25QXX_PAGE_SZ;
	#if (_W25QXX_DEBUG==1)
	print("w25qxx WritePage:%d, Offset:%d ,Writes %d Bytes, begin...\r\n",Page_Address,OffsetInByte,bytes);
	StartTime=HAL_GetTick();
	#endif	
	W25qxx_WriteEnable(pRsrc);
	HAL_GPIO_WritePin(pRsrc->CS.GPIOx,pRsrc->CS.GPIO_Pin,GPIO_PIN_RESET);
	W25qxx_Spi(pRsrc, 0x02);
	address = (Page_Address*W25QXX_PAGE_SZ)+OffsetInByte;
	if(pRsrc->ID>=W25Q256){	W25qxx_Spi(pRsrc, (address & 0xFF000000) >> 24);	}
	W25qxx_Spi(pRsrc, (address & 0xFF0000) >> 16);
	W25qxx_Spi(pRsrc, (address & 0xFF00) >> 8);
	W25qxx_Spi(pRsrc, address&0xFF);
	HAL_SPI_Transmit(pRsrc->SPI_HANDLE, (uint8_t*)pBuffer, bytes, 10);
	HAL_GPIO_WritePin(pRsrc->CS.GPIOx,pRsrc->CS.GPIO_Pin,GPIO_PIN_SET);
	pRsrc->Lock=0;
	return 0;
}

//###################################################################################################################
static void W25qxx_ReadBytesSync(w25qxxRsrc_t* pRsrc, uint32_t ReadAddr, uint8_t *pBuffer, uint32_t NumByteToRead){
	pRsrc->Lock=1;
	#if (_W25QXX_DEBUG==1)
	uint32_t	StartTime=HAL_GetTick();
	print("w25qxx ReadBytes at Address:%d, %d Bytes  begin...\r\n",ReadAddr, NumByteToRead);
	#endif	
	HAL_GPIO_WritePin(pRsrc->CS.GPIOx,pRsrc->CS.GPIO_Pin,GPIO_PIN_RESET);
	W25qxx_Spi(pRsrc, 0x0B);
	if(pRsrc->ID>=W25Q256)
		W25qxx_Spi(pRsrc, (ReadAddr & 0xFF000000) >> 24);
	W25qxx_Spi(pRsrc, (ReadAddr & 0xFF0000) >> 16);
	W25qxx_Spi(pRsrc, (ReadAddr& 0xFF00) >> 8);
	W25qxx_Spi(pRsrc, ReadAddr & 0xFF);
	W25qxx_Spi(pRsrc, 0);
	HAL_SPI_Receive(pRsrc->SPI_HANDLE,pBuffer,NumByteToRead, 100);
	HAL_GPIO_WritePin(pRsrc->CS.GPIOx,pRsrc->CS.GPIO_Pin,GPIO_PIN_SET);
	#if (_W25QXX_DEBUG==1)
	print("w25qxx ReadBytes done after %d ms\r\n", HAL_GetTick()-StartTime);
	for(uint32_t i=0;i<NumByteToRead ; i++)
	{
		if((i%8==0)&&(i>2))
		{
			print("\r\n");
			W25qxx_Delay(10);
		}
		print("0x%02X,",pBuffer[i]);		
	}
	print("\r\n");
	#endif	
//	W25qxx_Delay(1);
	pRsrc->Lock=0;
}

// polling to write bytes
static void W25qxx_Polling(w25qxxRsrc_t* r){
	if(r->asyncTask){	r->asyncTask(r);	}
}

static u8 W25qxx_IsAllTaskFree(w25qxxRsrc_t* r){
	if(r->chipEraseSqu | r->blockEraseSqu | r->sectorEraseSqu | r->writeBytesSqu){	return 0;	}
	return 1;
}

static s32 W25qxx_StartChipErase(w25qxxRsrc_t* r, cbNorFlash eraseCmpltCB, void* rsrcX){
	if(W25qxx_IsAllTaskFree(r) == 0){	return -1;	}
	r->cmpltCB = eraseCmpltCB;
	r->obj = rsrcX;
	r->chipEraseSqu = 1;
	r->asyncTask = W25qxx_ChipEraseTask;
	return 0;
}
static void W25qxx_ChipEraseTask(void* rx){
	w25qxxRsrc_t* r = rx;
	if((r->chipEraseSqu == 0) || (W25qxx_isWriteEnd(r)>0)){	return;	}
	switch(r->chipEraseSqu){
	case 1:
		r->Lock=1;
		#if (_W25QXX_DEBUG==1)
		StartTime=HAL_GetTick();
		print("w25qxx EraseChip Begin...\r\n");
		#endif
		W25qxx_WriteEnable(r);
		HAL_GPIO_WritePin(r->CS.GPIOx,r->CS.GPIO_Pin,GPIO_PIN_RESET);
		W25qxx_Spi(r, 0xC7);
		HAL_GPIO_WritePin(r->CS.GPIOx,r->CS.GPIO_Pin,GPIO_PIN_SET);
		r->Lock=0;
		r->chipEraseSqu++;
		break;
	case 2:
		#if (_W25QXX_DEBUG==1)
		print("w25qxx EraseChip done after %d ms!\r\n",HAL_GetTick()-StartTime);
		#endif
		r->chipEraseSqu = 0;
		if(r->cmpltCB){	r->cmpltCB(r->obj);	}
		break;
	}
}

static s32 W25qxx_StartBlockErase(w25qxxRsrc_t* r, uint32_t BlockAddr, u16 count, cbNorFlash eraseCmpltCB, void* rsrcX){
	if((W25qxx_IsAllTaskFree(r)==0) || (BlockAddr>=r->BlockCount)){	return -1;	}
	r->blockEraseIndx = BlockAddr;
	r->blockCountErase = count;
	r->cmpltCB = eraseCmpltCB;
	r->obj = rsrcX;
	r->blockEraseSqu = 1;
	r->asyncTask = W25qxx_BlockEraseTask;
	return 0;
}
static void W25qxx_BlockEraseTask(void* rx){
	w25qxxRsrc_t* r = rx;
	if((r->blockEraseSqu==0) || (W25qxx_isWriteEnd(r)>0)){	return;	}

	if(r->blockCountErase > 0){
		if(r->blockEraseIndx >= r->BlockCount){
			r->blockEraseSqu = 0;
			#if (_W25QXX_DEBUG==1)
			print("w25qxx EraseBlock stop after %d ms!\r\n",HAL_GetTick()-StartTime);
			#endif
			if(r->cmpltCB){	r->cmpltCB(r->obj);	}
		}
		else{
			W25qxx_BlockErase_64k(r, r->blockEraseIndx++);
			r->blockCountErase --;
		}
	}
	else{
		r->blockEraseSqu = 0;
		#if (_W25QXX_DEBUG==1)
		print("w25qxx EraseBlock done after %d ms!\r\n", HAL_GetTick()-StartTime);
		#endif
		if(r->cmpltCB){	r->cmpltCB(r->obj);	}
	}
}

static void W25qxx_BlockErase_64k(w25qxxRsrc_t* r, uint32_t blockAddr){
	u32 blckAddr;
	r->Lock=1;
	#if (_W25QXX_DEBUG==1)
	StartTime = HAL_GetTick();
	print("w25qxx EraseBlock %d Begin @%d...\r\n", blockAddr, StartTime);
	#endif
	blckAddr = blockAddr * 64 * 1024;
	W25qxx_WriteEnable(r);
	HAL_GPIO_WritePin(r->CS.GPIOx,r->CS.GPIO_Pin,GPIO_PIN_RESET);
	W25qxx_Spi(r, 0xd8);
	if(r->ID>=W25Q256)
	W25qxx_Spi(r, (blckAddr & 0xFF000000) >> 24);
	W25qxx_Spi(r, (blckAddr & 0xFF0000) >> 16);
	W25qxx_Spi(r, (blckAddr & 0xFF00) >> 8);
	W25qxx_Spi(r, blckAddr & 0xFF);
	HAL_GPIO_WritePin(r->CS.GPIOx, r->CS.GPIO_Pin,GPIO_PIN_SET);
	r->Lock=0;
}

static s32 W25qxx_StartSectorErase(w25qxxRsrc_t* r, uint32_t SectorAddr, u16 count, cbNorFlash eraseCmpltCB, void* rsrcX){
	if((W25qxx_IsAllTaskFree(r)==0) || (SectorAddr>=r->SectorCount)){	return -1;	}
	r->sectorEraseIndx = SectorAddr;
	r->sectorCountErase = count;
	r->cmpltCB = eraseCmpltCB;
	r->obj = rsrcX;
	r->sectorEraseSqu = 1;
	r->asyncTask = W25qxx_SectorEraseTask;
	return 0;
}
static void W25qxx_SectorEraseTask(void* rx){
	w25qxxRsrc_t* r = rx;
	if((r->sectorEraseSqu == 0) || (W25qxx_isWriteEnd(r)>0)){	return;	}

	if(r->sectorCountErase > 0){
		if(r->sectorEraseIndx >= r->SectorCount){
			r->sectorEraseSqu = 0;
			#if (_W25QXX_DEBUG==1)
			print("w25qxx EraseSector stop after %d ms!\r\n", HAL_GetTick()-StartTime);
			#endif
			if(r->cmpltCB){	r->cmpltCB(r->obj);	}
		}
		else{
			W25qxx_SectorErase_4k(r, r->sectorEraseIndx++);
			r->sectorCountErase --;
		}
	}
	else{
		r->sectorEraseSqu = 0;
		#if (_W25QXX_DEBUG==1)
		print("w25qxx EraseSector done after %d ms!\r\n", HAL_GetTick()-StartTime);
		#endif
		if(r->cmpltCB){	r->cmpltCB(r->obj);	}
	}
}
static void W25qxx_SectorErase_4k(w25qxxRsrc_t* r, uint32_t SectorAddr){
	r->Lock=1;
	#if (_W25QXX_DEBUG==1)
	StartTime=HAL_GetTick();
	print("w25qxx EraseSector %d Begin...\r\n",SectorAddr);
	#endif
	SectorAddr = SectorAddr * W25QXX_SECTOR_SZ;
	W25qxx_WriteEnable(r);
	HAL_GPIO_WritePin(r->CS.GPIOx, r->CS.GPIO_Pin, GPIO_PIN_RESET);
	W25qxx_Spi(r, 0x20);
	if(r->ID >= W25Q256)
	W25qxx_Spi(r, (SectorAddr & 0xFF000000) >> 24);
	W25qxx_Spi(r, (SectorAddr & 0xFF0000) >> 16);
	W25qxx_Spi(r, (SectorAddr & 0xFF00) >> 8);
	W25qxx_Spi(r, SectorAddr & 0xFF);
	HAL_GPIO_WritePin(r->CS.GPIOx, r->CS.GPIO_Pin, GPIO_PIN_SET);
	r->Lock=0;
}

static void W25qxx_PageErase(w25qxxRsrc_t* r, uint32_t PageAddr){
	u32 addr;
	r->Lock=1;
	#if (_W25QXX_DEBUG==1)
	StartTime=HAL_GetTick();
	print("w25qxx ErasePage %d Begin...\r\n", PageAddr);
	#endif
	addr = PageAddr * W25QXX_PAGE_SZ;
	W25qxx_WriteEnable(r);
	HAL_GPIO_WritePin(r->CS.GPIOx, r->CS.GPIO_Pin, GPIO_PIN_RESET);
	W25qxx_Spi(r, 0x81);
	if(r->ID >= W25Q256)
	W25qxx_Spi(r, (addr & 0xFF000000) >> 24);
	W25qxx_Spi(r, (addr & 0xFF0000) >> 16);
	W25qxx_Spi(r, (addr & 0xFF00) >> 8);
	W25qxx_Spi(r, addr & 0xFF);
	HAL_GPIO_WritePin(r->CS.GPIOx, r->CS.GPIO_Pin, GPIO_PIN_SET);
	r->Lock=0;
}
s32 W25qxx_WriteBytesSync(w25qxxRsrc_t *r, uint32_t WriteAddr, const uint8_t *pBuffer, uint32_t NumByteToWrite){
	u32 addrStart = WriteAddr;
	s32 bytesRest = NumByteToWrite;
	u16 addrPg;
	u8 buf[W25QXX_PAGE_SZ];
	u8 offset = 0;
	const u8* p = pBuffer;
	u16 bytes;
	s32 i,j;

	while(bytesRest > 0){
		addrPg = addrStart>>8;
		offset = addrStart & 0x000000ff;
		bytes = W25QXX_PAGE_SZ - offset;	// maximum can be program once
		bytes = (bytesRest >= bytes ? bytes : bytesRest);
		// read from rom
		W25qxx_ReadBytesSync(r, addrStart & 0xffffff00, buf, W25QXX_PAGE_SZ);

		// check if empty
		j = 0;
		for(i=0; i<bytes; i++){
			if(buf[offset+i] != 0xff){
				j = 1;
				break;
			}
		}

		if(j>0){
			// without blank page, erase this page first
			while(W25qxx_isWriteEnd(r) > 0);
			W25qxx_PageErase(r, addrPg);
			memset(&buf[offset], 0xff, bytes);
			while(W25qxx_WritePage(r, addrPg, 0, buf, W25QXX_PAGE_SZ)<0);
		}

		while(W25qxx_WritePage(r, addrPg, offset, p, bytes)<0);
		addrStart += bytes;
		p += bytes;
		bytesRest -= bytes;
		while(W25qxx_isWriteEnd(r) > 0);

		#if (_W25QXX_DEBUG==1)
		print("W25qxx_WritePage done %d ms\r\n", HAL_GetTick()-StartTime);
		#endif
	}
	return 0;
}

static s32 W25qxx_StartWriteBytes(w25qxxRsrc_t *r, uint32_t WriteAddr, const uint8_t *pBuffer, uint32_t NumByteToWrite, cbNorFlash eraseCmpltCB, void* rsrcX){
	if((W25qxx_IsAllTaskFree(r)==0) || (WriteAddr >= (r->CapacityInKiloByte*1024-1))){	return -1;	}
	r->cmpltCB = eraseCmpltCB;
	r->obj = rsrcX;
	r->writeBytesSqu = 1;
	r->asyncTask = W25qxx_WriteBytesTask;
	return 0;
}

static void W25qxx_WriteBytesTask(void* rx){
	w25qxxRsrc_t* r = rx;
	if((r->writeBytesSqu == 0) || (W25qxx_isWriteEnd(r)>0)){	return;	}

}


//###################################################################################################################
