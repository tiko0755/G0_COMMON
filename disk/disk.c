#include "disk.h"
#include <string.h>
#include "board.h"

u8 boardAddr = 0;
u8 baudHost = 4;		// BAUD[4]=115200
u8 baud485 = 4;
u8 boardMux = 0;

CONF_T conf = {0};

// storage device
#define EEPROM_SIZE_REG			(16*4)
#define EEPROM_SIZE_CFG			(64)
#define EEPROM_SIZE_USR			(W25QXX_SECTOR_SZ - EEPROM_SIZE_REG - EEPROM_SIZE_CFG)
// define app eeprom base address
#define EEPROM_BASE_USER		(0)
#define EEPROM_BASE_REG			(EEPROM_BASE_USER + EEPROM_SIZE_USR)
#define EEPROM_BASE_CFG			(EEPROM_BASE_REG + EEPROM_SIZE_REG)

s8 configWrite(void){
	u8 buff[16]={0};
	u16 sum = 0,i;
	buff[0] = baudHost;
	buff[1] = baud485;
	buff[2] = boardAddr;
	buff[3] = boardAddr>>8;
	buff[4] = boardMux;
	for(i=0;i<14;i++){	sum += buff[i];	}
	buff[14] = sum;
	buff[15] = sum>>8;
	ioWrite(EEPROM_BASE_CFG, buff, 16);
	return 0;
}

s8 configRead(void){
	u8 buff[16] = {0};
	u16 sum,checkcode,i;

	ioWrite(EEPROM_BASE_CFG, buff, 16);
	for(i=0,sum=0;i<14;i++){	sum += buff[i];	}
	checkcode = buff[15];	checkcode <<= 8;
	checkcode |= buff[14];

	if(sum == checkcode){
		baudHost = buff[0];
		baud485 = buff[1];
		if(baudHost >= 7)	 baudHost = 2;	// 2@115200
		if(baud485 >= 7)	 baud485 = 2;	// 2@115200
		boardAddr = buff[3];	boardAddr <<= 8;
		boardAddr |= buff[2];
		boardMux = buff[4];
	}
	else{
		baudHost = 2;	// 2@115200
		baud485 = 2;	// 2@115200
		boardAddr = 0;
		boardMux = 0;
	}

	return 0;
}

u8 ioReadReg(u16 addr, s32 *val){
	if(addr >= (EEPROM_SIZE_REG/4)){	return 	EEPROM_SIZE_REG/4;	}
	ioRead(EEPROM_BASE_REG + addr*4, (u8*)val, 4);
	return 0;
}

u8 ioWriteReg(u16 addr, s32 val){
	if(addr >= (EEPROM_SIZE_REG/4)){	return 	EEPROM_SIZE_REG/4;	}
	ioWrite(EEPROM_BASE_REG + addr*4, (u8*)&val, 4);
	return 0;
}

u16 usrWrite(u16 addr, const u8 *pDat, u16 nBytes){
	if(addr >= EEPROM_SIZE_USR){	return 	EEPROM_SIZE_USR;	}
	ioWrite(EEPROM_BASE_USER + addr, pDat, nBytes);
	return 0;
}

u16 usrRead(u16 addr, u8 *pDat, u16 nBytes){
	if(addr >= (EEPROM_SIZE_USR/4)){	return 	EEPROM_SIZE_USR;	}
	ioRead(EEPROM_BASE_USER + addr, pDat, nBytes);
	return 0;
}

u16 usrGetRomSize(void){
	return(EEPROM_SIZE_USR);
}
