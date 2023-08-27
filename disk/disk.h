#ifndef _DISK_H
#define _DISK_H

/*
  Author:    Tiko
  (1.0.0)
  First release.
*/

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdbool.h>
#include "misc.h"
#include "usr_typedef.h"
#include "stm_flash.h"

// total storage size(bytes)
#define TOTAL_SZ_BYTES          (STM_FLASH_TOTAL_SZ_BYTES)
     
// config IO 
extern CONF_T conf;
s32 configRead(void);
s32 configWrite(void);

s32 ioWriteReg(u16 addr, s32 val);
s32 ioReadReg(u16 addr, s32* val);

s32 usrWrite(u16 addr, const u8 *pDat, u16 nBytes);
s32 usrRead(u16 addr, u8 *pDat, u16 nBytes);
// user IO ROM defines
#define USR_ADC_BYTES   ((4*8)+2)

#define USR_ADC_BASE    0








void disk_setup(IO_Read rdMethod, IO_Write wrtMethod);

#ifdef __cplusplus
}
#endif

#endif
