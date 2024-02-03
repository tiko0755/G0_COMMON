#include "disk.h"
#include <string.h>
#include "user_log.h"
#include "CRC16.h"
#include "thread_delay.h"

//u8 boardAddr = 0;
//u8 baudHost = 4;        // BAUD[4]=115200
//u8 baud485 = 4;
//u8 boardMux = 0;

CONF_T conf = {0};

// =============================================
// storage device
#define ROM_SZ_BYTES_REG     (64*4)
#define ROM_SZ_BYTES_CFG     (256)
#define ROM_SZ_BYTES_USR     (TOTAL_SZ_BYTES - ROM_SZ_BYTES_REG - ROM_SZ_BYTES_CFG)
// define app eeprom base address
#define ROM_BASE_USER        (0)
#define ROM_BASE_REG         (ROM_BASE_USER + ROM_SZ_BYTES_USR)
#define ROM_BASE_CFG         (ROM_BASE_REG + ROM_SZ_BYTES_REG)

static IO_Write disk_ioWrite = NULL;
static IO_Read disk_ioRead = NULL;

void disk_setup(IO_Read rdMethod, IO_Write wrtMethod){
    disk_ioWrite = wrtMethod;
    disk_ioRead = rdMethod;
}

s32 configWrite(void){
	u16 chk;
    chk = CRC16((u8*)&conf, sizeof(CONF_T), CRC_INIT);
    disk_ioWrite(ROM_BASE_CFG+0, (u8*)&chk, 2);
    thread_delay(5);
	disk_ioWrite(ROM_BASE_CFG+2, (u8*)&conf, sizeof(CONF_T));
	return 0;
}

s32 configRead(void){
	u8 buff[16] = {0};
	u16 sum,chk0,chk1,i;

    disk_ioRead(ROM_BASE_CFG+0, (u8*)&chk0, 2);
    disk_ioRead(ROM_BASE_CFG+2, (u8*)&conf, sizeof(CONF_T));
    chk1 = CRC16((u8*)&conf, sizeof(CONF_T), CRC_INIT);
    
    if(chk0 != chk1){
        conf.baud485 = 115200;
        conf.baudHost = 115200;
        conf.brdAddr = 0;
        conf.brddMux = 0;
        conf.gw = 0;
        conf.ip = 0;
        conf.mac = 0;
        configWrite();
    }

    return 0;
}

s32 ioReadReg(u16 addr, s32 *val){
	if(addr >= (ROM_SZ_BYTES_REG/4)){	return 	-1;	}
	if(disk_ioRead!=NULL){
        disk_ioRead(ROM_BASE_REG + addr*4, (u8*)val, 4);
        return 0;
    }
    return -2;
}

s32 ioWriteReg(u16 addr, s32 val){
	if(addr >= (ROM_SZ_BYTES_REG/4)){	return 	ROM_SZ_BYTES_REG/4;	}
	disk_ioWrite(ROM_BASE_REG + addr*4, (u8*)&val, 4);
	return 0;
}

s32 usrWrite(u16 addr, const u8 *pDat, u16 nBytes){
	if(addr >= ROM_SZ_BYTES_USR){	
        log("<%s err:addr_overflow >", __func__);
        return 	-1;	
    }
	disk_ioWrite(ROM_BASE_USER + addr, pDat, nBytes);
	return 0;
}

s32 usrRead(u16 addr, u8 *pDat, u16 nBytes){
	if(addr >= ROM_SZ_BYTES_USR){	
        log("<%s err:addr_overflow >", __func__);
        return 	-1;	
    }
	disk_ioRead(ROM_BASE_USER + addr, pDat, nBytes);
	return 0;
}

//s32 usrGetRomSize(void){
//	return(ROM_SZ_BYTES_USR);
//}
