/******************** (C) COPYRIGHT 2022 INCUBECN *****************************
* File Name          : script_player.c
* Author             : Tiko Zhong
* Date First Issued  : 09/21,2022
* Description        : This file provides a set of functions needed to manage the
*                      stepper ramp generator
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "script_rom.h"
#include <string.h>
#include <stdio.h>
#include "board.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
u32 scriptRomAddr = 0xffffffff;
/* Private function prototypes -----------------------------------------------*/
static void scriptRomPrc(scriptRomRsrc_t*, u8 tick);
static s32 scriptEraseAsync(scriptRomRsrc_t*, void (*CB)(void*), void*);
static s32 scriptRomAppend(scriptRomRsrc_t*);
static s32 scriptRomRead(scriptRomRsrc_t*, uint32_t ReadAddr, uint8_t *pBuffer, uint32_t NumByteToRead);
static void script_blckEraseCmpltCB(void*);

s32 setup_scriptRom(
	scriptRomDev_t *dev,
	u32 blckSize,
	u16 pageSize,
	u16 blckIndxStart,
	u8 blckCount,
	void (*blockErase)	(u16 blckIndx, u16 count, void (*CB)(void*), void*),
	void (*pagePrg)		(uint32_t Page_Address, uint32_t OffsetInByte, const uint8_t *pBuffer, uint32_t NumByteToWrite_up_to_PageSize),
	void (*ReadBytes)	(uint32_t ReadAddr, uint8_t *pBuffer, uint32_t NumByteToRead)
){
	scriptRomRsrc_t *r = (scriptRomRsrc_t*)&dev->rsrc;
	r->blockSize = blckSize;
	r->pageSize = pageSize;
	r->blockIndxStart = blckIndxStart;
	r->blockIndxEnd = blckIndxStart + blckCount - 1;
	r->blockErase = blockErase;
	r->pagePrg = pagePrg;
	r->ReadBytes = ReadBytes;

	dev->EraseAsync = scriptEraseAsync;
	dev->Read = scriptRomRead;
	dev->Append = NULL;
	dev->Prc = scriptRomPrc;
	return 0;
}

u8 scriptCmd(void *devX, const char* CMD, void (*xprint)(const char* FORMAT_ORG, ...)){
	s32 i,j;
	scriptRomDev_t* dev = (scriptRomDev_t*)devX;
	UNUSED(dev);

	if(sscanf(CMD, "script.erase %d %d", &i, &j)==2){
		scriptEraseAsync(&dev->rsrc, (cbSrciptRom)i, (void*)j);
//		xprint("+ok@%d.script.erase %d %d\r\n", brdAddr, i, j);
		scriptRomAddr = 0;
		return 1;
	}

	else if(strncmp(CMD, "script.append", strlen("script.append"))==0){
//		xprint("+ok@%d.script.append\r\n", brdAddr);
		return 1;
	}

	else if(sscanf(CMD, "read %d %d", &i, &j)==2){
//		flshRom.ReadBytes(&flshRom.rsrc, i, buff, j);
//		xprint((char*)ccc);
		return 1;
	}
	return 0;
}


/*******************************************************************************
* Function Name  : scriptRomPrc
* Description    :
* Input          : - CMD, command line
				 : - brdAddr, board address
				 : - print function
* Output         : None
* Return         : None
*******************************************************************************/
static void scriptRomPrc(scriptRomRsrc_t* r, u8 tick){
	UNUSED(tick);
}

static s32 scriptEraseAsync(scriptRomRsrc_t* r, void (*CB)(void*), void* x){
	r->CB = CB;
	r->obj = x;
	r->blockErase(r->blockIndxStart, (r->blockIndxEnd - r->blockIndxStart) + 1, script_blckEraseCmpltCB, r);
	return 0;
}

static s32 scriptRomAppend(scriptRomRsrc_t* r){
	return 0;
}

static s32 scriptRomRead(scriptRomRsrc_t* r, uint32_t ReadAddr, uint8_t *pBuffer, uint32_t NumByteToRead){
	s32 addr = r->blockIndxStart*r->blockSize + ReadAddr;
	r->ReadBytes(addr, pBuffer, NumByteToRead);
	return 0;
}

// flasch
static void script_blckEraseCmpltCB(void* x){
	scriptRomRsrc_t* r = (scriptRomRsrc_t*)x;
	printS("erase done\n");
	if(r->CB){	r->CB(r->obj);	}
}


/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
