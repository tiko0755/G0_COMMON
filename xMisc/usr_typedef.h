/**
 ****************************************************************************************
 *
 * @file usr_typedef.h
 *
 * @brief Header file - User Function
 *
 *****************************************************************************************
 */
#ifndef _USR_TYPE_DEF_H_
#define _USR_TYPE_DEF_H_

/*
 * INCLUDE FILES
 *****************************************************************************************
 */
#include <stdint.h>

typedef void (*CB0)(void);
typedef void (*CB1)(void* argV);
typedef void (*CB2)(int32_t argN, void* argV);
typedef void (*CBS)(const char* FORMAT_ORG, ...);

typedef void (*CB)(void* argp);
typedef void (*CBx)(int32_t, void* argp);
typedef void (*XPrint)(const char* FORMAT_ORG, ...);
typedef void (*Proc)(CB resolve, CB err);

typedef int32_t (*IO_Write)(uint16_t addr, const uint8_t *pDat, uint16_t nBytes);
typedef int32_t (*IO_Read)(uint16_t addr, uint8_t *pDat, uint16_t nBytes);

typedef int32_t (*IO_WriteAsyn)(uint16_t addr, const uint8_t *pDat, uint16_t nBytes, CB2 cmplt);
typedef int32_t (*IO_ReadAsyn)(uint16_t addr, uint8_t *pDat, uint16_t nBytes, CB2 cmplt);

//#pragma pack(push,4)        // push current align bytes, and then set 4 bytes align
//// put user structure here
//#pragma pack(pop)        //recover align bytes from 4 bytes

#endif

