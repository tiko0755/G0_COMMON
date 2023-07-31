#include "w25qxx.h"
#include <string.h>

#include "board.h"		// using print

#if (_W25QXX_DEBUG==1)
#include <stdio.h>
#include "board.h"		// using print
#endif

#define W25QXX_DUMMY_BYTE         0xA5

#define W25QXX_ASYNC_SQU_ERASE			1
#define W25QXX_ASYNC_SQU_WRITE_BYTES	20

static bool W25qxx_Init(w25qxxRsrc_t*);
static void W25qxx_EraseChip(w25qxxRsrc_t*);
static void W25qxx_EraseSector(w25qxxRsrc_t*, uint32_t SectorAddr);

static bool W25qxx_IsEmptyPage(w25qxxRsrc_t*, uint32_t Page_Address, uint32_t OffsetInByte, uint32_t NumByteToCheck_up_to_PageSize);
static bool W25qxx_IsEmptySector(w25qxxRsrc_t*, uint32_t Sector_Address, uint32_t OffsetInByte, uint32_t NumByteToCheck_up_to_SectorSize);

static void W25qxx_WritePage(w25qxxRsrc_t*, uint32_t Page_Address, uint32_t OffsetInByte, const uint8_t *pBuffer, uint32_t NumByteToWrite_up_to_PageSize);
static void W25qxx_ReadPage(w25qxxRsrc_t*, uint32_t Page_Address, uint32_t OffsetInByte, uint8_t *pBuffer, uint32_t NumByteToRead_up_to_PageSize);

static void W25qxx_ReadBytes(w25qxxRsrc_t*, uint32_t ReadAddr, uint8_t *pBuffer, uint32_t NumByteToRead);
static void W25qxx_WriteBytesAsync(w25qxxRsrc_t*, uint32_t WriteAddr, const uint8_t *pBuffer, uint32_t NumByteToWrite, void (*cb) (void));
static void W25qxx_Polling(w25qxxRsrc_t* pRsrc );
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
	// register API function
	dev->Init = W25qxx_Init;
	dev->EraseChip = W25qxx_EraseChip;
	dev->EraseSector = W25qxx_EraseSector;
	
	dev->IsEmptySector = W25qxx_IsEmptySector;
	dev->ReadPage = W25qxx_ReadPage;
	dev->WritePage = W25qxx_WritePage;

	dev->ReadBytes = W25qxx_ReadBytes;
	dev->WriteBytesAsync = W25qxx_WriteBytesAsync;
	dev->Polling = W25qxx_Polling;

	return 0;
}

static uint8_t	W25qxx_Spi(w25qxxRsrc_t* pRsrc, uint8_t	Data)
{
	uint8_t	ret;
	HAL_SPI_TransmitReceive(pRsrc->SPI_HANDLE, &Data,&ret,1,100);
	return ret;	
}
//###################################################################################################################
static uint32_t W25qxx_ReadID(w25qxxRsrc_t* pRsrc)
{
  uint32_t Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;
  HAL_GPIO_WritePin(pRsrc->CS.GPIOx,pRsrc->CS.GPIO_Pin,GPIO_PIN_RESET);
  W25qxx_Spi(pRsrc, 0x9F);
  Temp0 = W25qxx_Spi(pRsrc, W25QXX_DUMMY_BYTE);
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
static void W25qxx_WaitForWriteEnd(w25qxxRsrc_t* pRsrc)
{
	u8 i;
	HAL_GPIO_WritePin(pRsrc->CS.GPIOx,pRsrc->CS.GPIO_Pin,GPIO_PIN_RESET);
	W25qxx_Spi(pRsrc, 0x05);
  do
  {
    pRsrc->StatusRegister1 = W25qxx_Spi(pRsrc, W25QXX_DUMMY_BYTE);
	  for(i=0;i<250;i++){	NOP();	}
  }
  while ((pRsrc->StatusRegister1 & 0x01) == 0x01);
 HAL_GPIO_WritePin(pRsrc->CS.GPIOx,pRsrc->CS.GPIO_Pin,GPIO_PIN_SET);
}

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
//  W25qxx_Delay(1);
  HAL_GPIO_WritePin(pRsrc->CS.GPIOx,pRsrc->CS.GPIO_Pin,GPIO_PIN_SET);
//  W25qxx_Delay(100);
  uint32_t	id;
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
	pRsrc->PageCount=(pRsrc->SectorCount*W25QXX_SECTOR_SZ)/W25QXX_PAGE_SZ;
	pRsrc->BlockSize=W25QXX_SECTOR_SZ*16;
	pRsrc->CapacityInKiloByte=(pRsrc->SectorCount*W25QXX_SECTOR_SZ)/1024;
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
static void	W25qxx_EraseChip(w25qxxRsrc_t* pRsrc){
	pRsrc->Lock=1;	
	#if (_W25QXX_DEBUG==1)
	uint32_t	StartTime=HAL_GetTick();	
	print("w25qxx EraseChip Begin...\r\n");
	#endif
	W25qxx_WriteEnable(pRsrc);
	HAL_GPIO_WritePin(pRsrc->CS.GPIOx,pRsrc->CS.GPIO_Pin,GPIO_PIN_RESET);
	W25qxx_Spi(pRsrc, 0xC7);
	HAL_GPIO_WritePin(pRsrc->CS.GPIOx,pRsrc->CS.GPIO_Pin,GPIO_PIN_SET);
	#if (_W25QXX_DEBUG==1)
	print("w25qxx EraseBlock done after %d ms!\r\n",HAL_GetTick()-StartTime);
	#endif
	pRsrc->Lock=0;	
	pRsrc->squ = W25QXX_ASYNC_SQU_ERASE;
}
//###################################################################################################################
static void W25qxx_EraseSector(w25qxxRsrc_t* pRsrc, uint32_t SectorAddr){
	pRsrc->Lock=1;
	#if (_W25QXX_DEBUG==1)
	uint32_t	StartTime=HAL_GetTick();
	print("w25qxx EraseSector %d Begin...\r\n",SectorAddr);
	#endif
	W25qxx_WaitForWriteEnd(pRsrc);
	  SectorAddr = SectorAddr * W25QXX_SECTOR_SZ;
	  W25qxx_WriteEnable(pRsrc);
	  HAL_GPIO_WritePin(pRsrc->CS.GPIOx,pRsrc->CS.GPIO_Pin,GPIO_PIN_RESET);
	  W25qxx_Spi(pRsrc, 0x20);
	  if(pRsrc->ID>=W25Q256)
		W25qxx_Spi(pRsrc, (SectorAddr & 0xFF000000) >> 24);
	  W25qxx_Spi(pRsrc, (SectorAddr & 0xFF0000) >> 16);
	  W25qxx_Spi(pRsrc, (SectorAddr & 0xFF00) >> 8);
	  W25qxx_Spi(pRsrc, SectorAddr & 0xFF);
	  HAL_GPIO_WritePin(pRsrc->CS.GPIOx,pRsrc->CS.GPIO_Pin,GPIO_PIN_SET);
	  //W25qxx_WaitForWriteEnd(pRsrc);
	#if (_W25QXX_DEBUG==1)
	print("w25qxx EraseSector done after %d ms\r\n",HAL_GetTick()-StartTime);
	#endif
//	W25qxx_Delay(1);
	pRsrc->Lock=0;
}

//###################################################################################################################
static bool W25qxx_IsEmptyPage(w25qxxRsrc_t* pRsrc, uint32_t Page_Address, uint32_t OffsetInByte, uint32_t NumByteToCheck_up_to_PageSize){
	pRsrc->Lock=1;	
	if(((NumByteToCheck_up_to_PageSize+OffsetInByte)>W25QXX_PAGE_SZ)||(NumByteToCheck_up_to_PageSize==0))
		NumByteToCheck_up_to_PageSize=W25QXX_PAGE_SZ-OffsetInByte;
	#if (_W25QXX_DEBUG==1)
	print("w25qxx CheckPage:%d, Offset:%d, Bytes:%d begin...\r\n",Page_Address,OffsetInByte,NumByteToCheck_up_to_PageSize);
	W25qxx_Delay(100);
	uint32_t	StartTime=HAL_GetTick();
	#endif		
	uint8_t	pBuffer[32];
	uint32_t	WorkAddress;
	uint32_t	i;
	for(i=OffsetInByte; i<W25QXX_PAGE_SZ; i+=sizeof(pBuffer))
	{
		HAL_GPIO_WritePin(pRsrc->CS.GPIOx,pRsrc->CS.GPIO_Pin,GPIO_PIN_RESET);
		WorkAddress=(i+Page_Address*W25QXX_PAGE_SZ);
		W25qxx_Spi(pRsrc, 0x0B);
		if(pRsrc->ID>=W25Q256)
			W25qxx_Spi(pRsrc, (WorkAddress & 0xFF000000) >> 24);
		W25qxx_Spi(pRsrc, (WorkAddress & 0xFF0000) >> 16);
		W25qxx_Spi(pRsrc, (WorkAddress & 0xFF00) >> 8);
		W25qxx_Spi(pRsrc, WorkAddress & 0xFF);
		W25qxx_Spi(pRsrc, 0);
		HAL_SPI_Receive(pRsrc->SPI_HANDLE,pBuffer,sizeof(pBuffer),100);	
		HAL_GPIO_WritePin(pRsrc->CS.GPIOx,pRsrc->CS.GPIO_Pin,GPIO_PIN_SET);	
		for(uint8_t x=0;x<sizeof(pBuffer);x++)
		{
			if(pBuffer[x]!=0xFF)
				goto NOT_EMPTY;		
		}			
	}	
	if((W25QXX_PAGE_SZ+OffsetInByte)%sizeof(pBuffer)!=0)
	{
		i-=sizeof(pBuffer);
		for( ; i<W25QXX_PAGE_SZ; i++)
		{
			HAL_GPIO_WritePin(pRsrc->CS.GPIOx,pRsrc->CS.GPIO_Pin,GPIO_PIN_RESET);
			WorkAddress=(i+Page_Address*W25QXX_PAGE_SZ);
			W25qxx_Spi(pRsrc, 0x0B);
			if(pRsrc->ID>=W25Q256)
				W25qxx_Spi(pRsrc, (WorkAddress & 0xFF000000) >> 24);
			W25qxx_Spi(pRsrc, (WorkAddress & 0xFF0000) >> 16);
			W25qxx_Spi(pRsrc, (WorkAddress & 0xFF00) >> 8);
			W25qxx_Spi(pRsrc, WorkAddress & 0xFF);
			W25qxx_Spi(pRsrc, 0);
			HAL_SPI_Receive(pRsrc->SPI_HANDLE,pBuffer,1,100);	
			HAL_GPIO_WritePin(pRsrc->CS.GPIOx,pRsrc->CS.GPIO_Pin,GPIO_PIN_SET);	
			if(pBuffer[0]!=0xFF)
				goto NOT_EMPTY;
		}
	}	
	#if (_W25QXX_DEBUG==1)
	print("w25qxx CheckPage is Empty in %d ms\r\n",HAL_GetTick()-StartTime);
	W25qxx_Delay(100);
	#endif	
	pRsrc->Lock=0;
	return true;	
	NOT_EMPTY:
	#if (_W25QXX_DEBUG==1)
	print("w25qxx CheckPage is Not Empty in %d ms\r\n",HAL_GetTick()-StartTime);
	W25qxx_Delay(100);
	#endif	
	pRsrc->Lock=0;
	return false;
}
//###################################################################################################################
static bool W25qxx_IsEmptySector(w25qxxRsrc_t* pRsrc, uint32_t Sector_Address, uint32_t OffsetInByte, uint32_t NumByteToCheck_up_to_SectorSize){
	pRsrc->Lock=1;	
	if((NumByteToCheck_up_to_SectorSize>W25QXX_SECTOR_SZ)||(NumByteToCheck_up_to_SectorSize==0))
		NumByteToCheck_up_to_SectorSize=W25QXX_SECTOR_SZ;
	#if (_W25QXX_DEBUG==1)
	print("w25qxx CheckSector:%d, Offset:%d, Bytes:%d begin...\r\n",Sector_Address,OffsetInByte,NumByteToCheck_up_to_SectorSize);
	uint32_t	StartTime=HAL_GetTick();
	#endif		
	uint8_t	pBuffer[32];
	uint32_t	WorkAddress;
	uint32_t	i;
	for(i=OffsetInByte; i<W25QXX_SECTOR_SZ; i+=sizeof(pBuffer))
	{
		HAL_GPIO_WritePin(pRsrc->CS.GPIOx,pRsrc->CS.GPIO_Pin,GPIO_PIN_RESET);
		WorkAddress=(i+Sector_Address*W25QXX_SECTOR_SZ);
		W25qxx_Spi(pRsrc, 0x0B);
		if(pRsrc->ID>=W25Q256)
			W25qxx_Spi(pRsrc, (WorkAddress & 0xFF000000) >> 24);
		W25qxx_Spi(pRsrc, (WorkAddress & 0xFF0000) >> 16);
		W25qxx_Spi(pRsrc, (WorkAddress & 0xFF00) >> 8);
		W25qxx_Spi(pRsrc, WorkAddress & 0xFF);
		W25qxx_Spi(pRsrc, 0);
		HAL_SPI_Receive(pRsrc->SPI_HANDLE,pBuffer,sizeof(pBuffer),100);	
		HAL_GPIO_WritePin(pRsrc->CS.GPIOx,pRsrc->CS.GPIO_Pin,GPIO_PIN_SET);	
		for(uint8_t x=0;x<sizeof(pBuffer);x++)
		{
			if(pBuffer[x]!=0xFF)
				goto NOT_EMPTY;		
		}			
	}	
	if((W25QXX_SECTOR_SZ+OffsetInByte)%sizeof(pBuffer)!=0)
	{
		i-=sizeof(pBuffer);
		for( ; i<W25QXX_SECTOR_SZ; i++)
		{
			HAL_GPIO_WritePin(pRsrc->CS.GPIOx,pRsrc->CS.GPIO_Pin,GPIO_PIN_RESET);
			WorkAddress=(i+Sector_Address*W25QXX_SECTOR_SZ);
			W25qxx_Spi(pRsrc, 0x0B);
			if(pRsrc->ID>=W25Q256)
				W25qxx_Spi(pRsrc, (WorkAddress & 0xFF000000) >> 24);
			W25qxx_Spi(pRsrc, (WorkAddress & 0xFF0000) >> 16);
			W25qxx_Spi(pRsrc, (WorkAddress & 0xFF00) >> 8);
			W25qxx_Spi(pRsrc, WorkAddress & 0xFF);
			W25qxx_Spi(pRsrc, 0);
			HAL_SPI_Receive(pRsrc->SPI_HANDLE,pBuffer,1,100);	
			HAL_GPIO_WritePin(pRsrc->CS.GPIOx,pRsrc->CS.GPIO_Pin,GPIO_PIN_SET);	
			if(pBuffer[0]!=0xFF)
				goto NOT_EMPTY;
		}
	}	
	#if (_W25QXX_DEBUG==1)
	print("w25qxx CheckSector is Empty in %d ms\r\n",HAL_GetTick()-StartTime);
	#endif	
	pRsrc->Lock=0;
	return true;	
	NOT_EMPTY:
	#if (_W25QXX_DEBUG==1)
	print("w25qxx CheckSector is Not Empty in %d ms\r\n",HAL_GetTick()-StartTime);
	#endif	
	pRsrc->Lock=0;
	return false;
}

//###################################################################################################################
static void W25qxx_WritePage(w25qxxRsrc_t* pRsrc, uint32_t Page_Address, uint32_t OffsetInByte, const uint8_t *pBuffer, uint32_t NumByteToWrite_up_to_PageSize){
	pRsrc->Lock=1;
	if(((NumByteToWrite_up_to_PageSize+OffsetInByte)>W25QXX_PAGE_SZ)||(NumByteToWrite_up_to_PageSize==0))
		NumByteToWrite_up_to_PageSize=W25QXX_PAGE_SZ-OffsetInByte;
	if((OffsetInByte+NumByteToWrite_up_to_PageSize) > W25QXX_PAGE_SZ)
		NumByteToWrite_up_to_PageSize = W25QXX_PAGE_SZ-OffsetInByte;
	#if (_W25QXX_DEBUG==1)
	print("w25qxx WritePage:%d, Offset:%d ,Writes %d Bytes, begin...\r\n",Page_Address,OffsetInByte,NumByteToWrite_up_to_PageSize);
	uint32_t	StartTime=HAL_GetTick();
	#endif	
	W25qxx_WaitForWriteEnd(pRsrc);
  W25qxx_WriteEnable(pRsrc);
  HAL_GPIO_WritePin(pRsrc->CS.GPIOx,pRsrc->CS.GPIO_Pin,GPIO_PIN_RESET);
  W25qxx_Spi(pRsrc, 0x02);
	Page_Address = (Page_Address*W25QXX_PAGE_SZ)+OffsetInByte;	
	if(pRsrc->ID>=W25Q256)
		W25qxx_Spi(pRsrc, (Page_Address & 0xFF000000) >> 24);
  W25qxx_Spi(pRsrc, (Page_Address & 0xFF0000) >> 16);
  W25qxx_Spi(pRsrc, (Page_Address & 0xFF00) >> 8);
  W25qxx_Spi(pRsrc, Page_Address&0xFF);
//	memcpy(pRsrc->pageBuf, pBuffer, NumByteToWrite_up_to_PageSize);
//	HAL_SPI_Transmit(pRsrc->SPI_HANDLE, pRsrc->pageBuf, NumByteToWrite_up_to_PageSize, 100);
	HAL_SPI_Transmit(pRsrc->SPI_HANDLE, (uint8_t*)pBuffer, NumByteToWrite_up_to_PageSize, 100);	
	HAL_GPIO_WritePin(pRsrc->CS.GPIOx,pRsrc->CS.GPIO_Pin,GPIO_PIN_SET);
  W25qxx_WaitForWriteEnd(pRsrc);
	#if (_W25QXX_DEBUG==1)
	StartTime = HAL_GetTick()-StartTime; 
	for(uint32_t i=0;i<NumByteToWrite_up_to_PageSize ; i++)
	{
		if((i%8==0)&&(i>2))
		{
			print("\r\n");
			W25qxx_Delay(10);			
		}
		print("0x%02X,",pBuffer[i]);
	}	
	print("\r\n");
	print("w25qxx WritePage done after %d ms\r\n",StartTime);
	#endif	
	pRsrc->Lock=0;
}

//###################################################################################################################
static void W25qxx_ReadBytes(w25qxxRsrc_t* pRsrc, uint32_t ReadAddr, uint8_t *pBuffer, uint32_t NumByteToRead){
	pRsrc->Lock=1;
	#if (_W25QXX_DEBUG==1)
	uint32_t	StartTime=HAL_GetTick();
	print("w25qxx ReadBytes at Address:%d, %d Bytes  begin...\r\n",ReadAddr,NumByteToRead);
	#endif	
	HAL_GPIO_WritePin(pRsrc->CS.GPIOx,pRsrc->CS.GPIO_Pin,GPIO_PIN_RESET);
	W25qxx_Spi(pRsrc, 0x0B);
	if(pRsrc->ID>=W25Q256)
		W25qxx_Spi(pRsrc, (ReadAddr & 0xFF000000) >> 24);
  W25qxx_Spi(pRsrc, (ReadAddr & 0xFF0000) >> 16);
  W25qxx_Spi(pRsrc, (ReadAddr& 0xFF00) >> 8);
  W25qxx_Spi(pRsrc, ReadAddr & 0xFF);
	W25qxx_Spi(pRsrc, 0);
	HAL_SPI_Receive(pRsrc->SPI_HANDLE,pBuffer,NumByteToRead,2000);	
	HAL_GPIO_WritePin(pRsrc->CS.GPIOx,pRsrc->CS.GPIO_Pin,GPIO_PIN_SET);
	#if (_W25QXX_DEBUG==1)
	StartTime = HAL_GetTick()-StartTime; 
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
	print("w25qxx ReadBytes done after %d ms\r\n",StartTime);
	#endif	
//	W25qxx_Delay(1);
	pRsrc->Lock=0;
}
//###################################################################################################################
static void W25qxx_ReadPage(w25qxxRsrc_t* pRsrc, uint32_t Page_Address, uint32_t OffsetInByte, uint8_t *pBuffer, uint32_t NumByteToRead_up_to_PageSize){
	pRsrc->Lock=1;
	if((NumByteToRead_up_to_PageSize>W25QXX_PAGE_SZ)||(NumByteToRead_up_to_PageSize==0))
		NumByteToRead_up_to_PageSize=W25QXX_PAGE_SZ;
	if((OffsetInByte+NumByteToRead_up_to_PageSize) > W25QXX_PAGE_SZ)
		NumByteToRead_up_to_PageSize = W25QXX_PAGE_SZ-OffsetInByte;
	#if (_W25QXX_DEBUG==1)
	print("w25qxx ReadPage:%d, Offset:%d ,Read %d Bytes, begin...\r\n",Page_Address,OffsetInByte,NumByteToRead_up_to_PageSize);
	uint32_t	StartTime=HAL_GetTick();
	#endif	
	Page_Address = Page_Address*W25QXX_PAGE_SZ+OffsetInByte;
	HAL_GPIO_WritePin(pRsrc->CS.GPIOx,pRsrc->CS.GPIO_Pin,GPIO_PIN_RESET);
	W25qxx_Spi(pRsrc, 0x0B);
	if(pRsrc->ID>=W25Q256)
		W25qxx_Spi(pRsrc, (Page_Address & 0xFF000000) >> 24);
  W25qxx_Spi(pRsrc, (Page_Address & 0xFF0000) >> 16);
  W25qxx_Spi(pRsrc, (Page_Address& 0xFF00) >> 8);
  W25qxx_Spi(pRsrc, Page_Address & 0xFF);
	W25qxx_Spi(pRsrc, 0);
	HAL_SPI_Receive(pRsrc->SPI_HANDLE,pBuffer,NumByteToRead_up_to_PageSize,100);	
	HAL_GPIO_WritePin(pRsrc->CS.GPIOx,pRsrc->CS.GPIO_Pin,GPIO_PIN_SET);
	#if (_W25QXX_DEBUG==1)
	StartTime = HAL_GetTick()-StartTime; 
	for(uint32_t i=0;i<NumByteToRead_up_to_PageSize ; i++)
	{
		if((i%8==0)&&(i>2))
		{
			print("\r\n");
			W25qxx_Delay(10);
		}
		print("0x%02X,",pBuffer[i]);		
	}	
	print("\r\n");
	print("w25qxx ReadPage done after %d ms\r\n",StartTime);
	#endif	
	pRsrc->Lock=0;
}

static void W25qxx_WriteBytesAsync(
	w25qxxRsrc_t* pRsrc,
	uint32_t WriteAddr,
	const uint8_t *pBuffer,
	uint32_t NumByteToWrite,
	void (*cb)	(void)){

	if(NumByteToWrite==0)	return;

	pRsrc->pBuffer = pBuffer;
	pRsrc->byteAddrStart = WriteAddr;
	pRsrc->bytes = NumByteToWrite;
	pRsrc->cb = cb;

	pRsrc->byteAddrEnd = WriteAddr + NumByteToWrite - 1;
	pRsrc->pageAddrStart = pRsrc->byteAddrStart / W25QXX_PAGE_SZ;
	pRsrc->pageAddrEnd = pRsrc->byteAddrEnd / W25QXX_PAGE_SZ;
	pRsrc->pageCur = pRsrc->pageAddrStart;
	pRsrc->sectorAddrS = pRsrc->byteAddrStart / W25QXX_SECTOR_SZ;
	pRsrc->sectorAddrE = pRsrc->byteAddrEnd / W25QXX_SECTOR_SZ;
	pRsrc->sectorCur = pRsrc->sectorAddrS;

	pRsrc->squ = W25QXX_ASYNC_SQU_WRITE_BYTES;
}

// polling to write bytes
static void W25qxx_Polling(w25qxxRsrc_t* pRsrc){
	u8 isEmpty,i,tmp[W25QXX_PAGE_SZ];
	const uint8_t *p;
	u32 offsetBytes,bytes,bytesNeed;
	u16 sIndxSwap;	// destination swap sector index
	u16 pageIndx0,pageIndx1;

	switch(pRsrc->squ){
	// async erase entire chip start here
	case W25QXX_ASYNC_SQU_ERASE + 0:
		if(W25qxx_isWriteEnd(pRsrc) == 0){	pRsrc->squ = 0;	}
		break;
	// async write bytes start here
	case W25QXX_ASYNC_SQU_WRITE_BYTES + 0:
		// check if all the destination page is empty, just write it
		isEmpty = 1;
		for(i=pRsrc->pageAddrStart;i<=pRsrc->pageAddrEnd && isEmpty;i++){
			if(W25qxx_IsEmptyPage(pRsrc, i, 0, W25QXX_PAGE_SZ) == false)	isEmpty = 0;
		}
		if(isEmpty > 0){
			// just write it and return
			p = pRsrc->pBuffer;
			bytesNeed = pRsrc->byteAddrEnd-pRsrc->byteAddrStart+1;
			pRsrc->byteAddrCur = pRsrc->byteAddrStart;
			for(i=pRsrc->pageAddrStart;i<=pRsrc->pageAddrEnd;i++){
				if((p - pRsrc->pBuffer) >= bytesNeed)	break;
				offsetBytes = pRsrc->byteAddrCur%W25QXX_PAGE_SZ;
				bytes =( (W25QXX_PAGE_SZ-offsetBytes) > (bytesNeed-(p - pRsrc->pBuffer)) ? (bytesNeed-(p - pRsrc->pBuffer)) : (W25QXX_PAGE_SZ-offsetBytes));
				W25qxx_WritePage(pRsrc, i, offsetBytes, p, bytes);
				pRsrc->byteAddrCur += bytes;
				p += bytes;
			}
			if(pRsrc->cb)	pRsrc->cb();
			pRsrc->squ = 0;
		}
		else{
			pRsrc->sectorCur = pRsrc->sectorAddrS;
			pRsrc->squ++;
		}
		break;
	// loop sector to write all
	case W25QXX_ASYNC_SQU_WRITE_BYTES + 1:
		sIndxSwap = pRsrc->SectorCount - 1;
		if(pRsrc->sectorCur <= pRsrc->sectorAddrE){
			//if current sector is not empty, erase last sector(swap sector)
			if(W25qxx_IsEmptySector(pRsrc, pRsrc->sectorCur, 0, W25QXX_SECTOR_SZ) == false){
				W25qxx_EraseSector(pRsrc, sIndxSwap);
				pRsrc->squ++;
			}
			else{	pRsrc->squ += 2;	}
		}
		else{
			if(pRsrc->cb)	pRsrc->cb();
			pRsrc->squ = 0;
		}
		break;
	// wait for erase swap sector complete
	case W25QXX_ASYNC_SQU_WRITE_BYTES + 2:
		if(W25qxx_isWriteEnd(pRsrc) == 0){	pRsrc->squ++;	}
		break;
	// copy current sector to swap sector
	case W25QXX_ASYNC_SQU_WRITE_BYTES + 3:
		pageIndx0 = (pRsrc->sectorCur * W25QXX_SECTOR_SZ)/W25QXX_PAGE_SZ;		// source
		pageIndx1 = ((pRsrc->SectorCount - 1) * W25QXX_SECTOR_SZ)/W25QXX_PAGE_SZ;
		for(i=0;i<W25QXX_SECTOR_SZ/W25QXX_PAGE_SZ;i++){
			W25qxx_ReadPage(pRsrc, pageIndx0+i, 0, tmp, W25QXX_PAGE_SZ);
			W25qxx_WritePage(pRsrc, pageIndx1+i, 0, tmp, W25QXX_PAGE_SZ);
		}
		W25qxx_EraseSector(pRsrc, pRsrc->sectorCur);	// cost about 9ms
		pRsrc->squ++;
		break;
	// wait for erase current sector complete
	case W25QXX_ASYNC_SQU_WRITE_BYTES + 4:
		if(W25qxx_isWriteEnd(pRsrc) == 0){	pRsrc->squ++;	}
		break;
	// rewrite current sector
	case W25QXX_ASYNC_SQU_WRITE_BYTES + 5:
		p = pRsrc->pBuffer;
		pageIndx0 = (pRsrc->sectorCur * W25QXX_SECTOR_SZ)/W25QXX_PAGE_SZ;		// source
		pageIndx1 = ((pRsrc->SectorCount - 1) * W25QXX_SECTOR_SZ)/W25QXX_PAGE_SZ;
		for(i=0;i<W25QXX_SECTOR_SZ/W25QXX_PAGE_SZ;i++){
			// read from swap
			W25qxx_ReadPage(pRsrc, pageIndx1+i, 0, tmp, W25QXX_PAGE_SZ);
			// need to fill tmp buffer
			if((pageIndx0+i) >= pRsrc->pageAddrStart && (pageIndx0+i) <= pRsrc->pageAddrEnd){
				offsetBytes = pRsrc->byteAddrCur%W25QXX_PAGE_SZ;
				bytes =( (W25QXX_PAGE_SZ-offsetBytes) > (pRsrc->bytes-(p - pRsrc->pBuffer)) ? (pRsrc->bytes-(p - pRsrc->pBuffer)) : (W25QXX_PAGE_SZ-offsetBytes));
				memcpy(tmp+offsetBytes, p, bytes);
				pRsrc->byteAddrCur += bytes;
				p += bytes;
			}
			W25qxx_WritePage(pRsrc, pageIndx0+i, 0, tmp, W25QXX_PAGE_SZ);
		}
		pRsrc->sectorCur ++;
		pRsrc->squ = W25QXX_ASYNC_SQU_WRITE_BYTES + 1;
		break;
	}
}

//###################################################################################################################
