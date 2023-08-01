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

extern u8 boardAddr;
extern u8 baudHost;
extern u8 baud485;
extern u8 boardMux;

extern CONF_T conf;

s8 configRead(void);
s8 configWrite(void);

// return: [0] success; [others] address overfloat, max address
u8 ioWriteReg(u16 addr, s32 val);
u8 ioReadReg(u16 addr, s32* val);

// return: [0] success; [others] address overfloat, max address
u16 usrWrite(u16 addr, const u8 *pDat, u16 nBytes);
u16 usrRead(u16 addr, u8 *pDat, u16 nBytes);
u16 usrGetRomSize(void);

#ifdef __cplusplus
}
#endif

#endif
