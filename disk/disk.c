#include "disk.h"
#include <string.h>
#include "user_log.h"

u8 boardAddr = 0;
u8 baudHost = 4;        // BAUD[4]=115200
u8 baud485 = 4;
u8 boardMux = 0;

CONF_T conf = {0};


// =============================================
// define app eeprom size
#define EEPROM_SIZE_USR            (6*1024)
#define EEPROM_SIZE_REG            (1*1024)
#define EEPROM_SIZE_NET            (1*1024)
// define app eeprom base address
#define EEPROM_BASE_USER        0
#define EEPROM_BASE_REG            (EEPROM_BASE_USER + EEPROM_SIZE_USR)
#define EEPROM_BASE_NET            (EEPROM_BASE_REG + EEPROM_SIZE_NET)

// storage device
#define ROM_SZ_BYTES_REG     (32*4)
#define ROM_SZ_BYTES_CFG     (128)
#define ROM_SZ_BYTES_USR     (TOTAL_SZ_BYTES - ROM_SZ_BYTES_REG - ROM_SZ_BYTES_CFG)
// define app eeprom base address
#define ROM_BASE_USER        (0)
#define ROM_BASE_REG         (ROM_BASE_USER + ROM_SZ_BYTES_USR)
#define ROM_BASE_CFG         (ROM_BASE_REG + ROM_SZ_BYTES_REG)

static IO_Write disk_ioWrite = NULL;
static IO_Read disk_ioRead = NULL;

void dist_setup(IO_Read rdMethod, IO_Write wrtMethod){
    disk_ioWrite = wrtMethod;
    disk_ioRead = rdMethod;
}

//static s8 configWrite(void){
////    u8 buff[32]={0};
////    buff[14] = g_baudHost;
////    buff[15] = g_baud485;
////    buff[16] = HAL_GetTick()&0xff;            // mac[3]
////    buff[17] = (HAL_GetTick()>>8)&0xff;        // mac[4]
////    buff[18] = (HAL_GetTick()>>16)&0xff;    // mac[5]
////    buff[31] = 0xaa;
////    erom.Write(&erom.rsrc, EEPROM_BASE_NET, buff, 32);
//    return 0;
//}

//static s8 configRead(void){
////    u8 buff[32] = {0};
////    erom.Read(&erom.rsrc, EEPROM_BASE_NET, buff, 32);
////    if(buff[31] == 0xaa){
////        g_baudHost = buff[14];
////        g_baud485 = buff[15];

////        if(g_baudHost >= 7)    g_baudHost = 4;    // 4@115200
////        if(g_baud485 >= 7)     g_baud485 = 4;    // 4@115200
////    }
//    return 0;
//}




s32 configWrite(void){
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
	disk_ioWrite(ROM_BASE_CFG, buff, 16);
	return 0;
}

s32 configRead(void){
	u8 buff[16] = {0};
	u16 sum,checkcode,i;

	disk_ioWrite(ROM_BASE_CFG, buff, 16);
	for(i=0,sum=0;i<14;i++){	sum += buff[i];	}
	checkcode = buff[15];	checkcode <<= 8;
	checkcode |= buff[14];


    if(sum == checkcode){
        baudHost = buff[0];
        baud485 = buff[1];
        if(baudHost >= 7)     baudHost = 2;    // 2@115200
        if(baud485 >= 7)     baud485 = 2;    // 2@115200
        boardAddr = buff[3];    boardAddr <<= 8;
        boardAddr |= buff[2];
        boardMux = buff[4];
    }
    else{
        baudHost = 2;    // 2@115200
        baud485 = 2;    // 2@115200
        boardAddr = 0;
        boardMux = 0;
    }

    return 0;
}

s32 ioReadReg(u16 addr, s32 *val){
	if(addr >= (ROM_SZ_BYTES_REG/4)){	return 	ROM_SZ_BYTES_REG/4;	}
	disk_ioRead(ROM_BASE_REG + addr*4, (u8*)val, 4);
	return 0;
}

s32 ioWriteReg(u16 addr, s32 val){
	if(addr >= (ROM_SZ_BYTES_REG/4)){	return 	ROM_SZ_BYTES_REG/4;	}
	disk_ioWrite(ROM_BASE_REG + addr*4, (u8*)&val, 4);
	return 0;
}

s32 usrWrite(u16 addr, const u8 *pDat, u16 nBytes){
	if(addr >= ROM_SZ_BYTES_USR){	return 	ROM_SZ_BYTES_USR;	}
	disk_ioWrite(ROM_BASE_USER + addr, pDat, nBytes);
	return 0;
}

s32 usrRead(u16 addr, u8 *pDat, u16 nBytes){
	if(addr >= (ROM_SZ_BYTES_USR/4)){	return 	ROM_SZ_BYTES_USR;	}
	disk_ioRead(ROM_BASE_USER + addr, pDat, nBytes);
	return 0;
}

s32 usrGetRomSize(void){
	return(ROM_SZ_BYTES_USR);
}


