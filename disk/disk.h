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

// total storage size(bytes)
#define TOTAL_SZ_BYTES          (4*1024)

extern u8 boardAddr;
extern u8 baudHost;
extern u8 baud485;
extern u8 boardMux;

extern CONF_T conf;

s32 configRead(void);
s32 configWrite(void);

// return: [0] success; [others] address overfloat, max address
s32 ioWriteReg(u16 addr, s32 val);
s32 ioReadReg(u16 addr, s32* val);

// return: [0] success; [others] address overfloat, max address
s32 usrWrite(u16 addr, const u8 *pDat, u16 nBytes);
s32 usrRead(u16 addr, u8 *pDat, u16 nBytes);
s32 usrGetRomSize(void);
s32 usrGetRomFreeSize(void);
<<<<<<< HEAD

void dist_setup(IO_Read rdMethod, IO_Write wrtMethod);

=======
>>>>>>> 8e78bad99e5fb8e3566d992f419c8b0d4d480ca2

#ifdef __cplusplus
}
#endif

#endif
