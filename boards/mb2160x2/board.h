/**********************************************************
filename: board.h
**********************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _BOARD_H_
#define _BOARD_H_

#include "misc.h"
#include "rs485Dev.h"
#include "uartdev.h"

/* output variables for extern function --------------------------------------*/

#if defined(F_APP_FTP)
#define MAX_HTTPSOCK    4
#else
#define MAX_HTTPSOCK    6
#endif

extern const char ABOUT[];
extern const char COMMON_HELP[];

extern SPI_HandleTypeDef hspi1;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

// gloable var
extern char addrPre[4];
extern u8 boardAddr;
extern u8 baudHost;
extern u8 baud485;
extern u32 errorCode;

extern UartDev_t console;
extern Rs485Dev_t rs485;

extern const PIN_T DE;
extern const PIN_T LAN_CS;
extern const PIN_T LAN_IRQ;
extern const PIN_T RUNNING;
extern u8 initalDone;

// gloable method
void boardPreInit(void);
void boardInit(void);
u8 brdCmd(const char* CMD, u8 brdAddr, void (*xprintS)(const char* MSG), void (*xprint)(const char* FORMAT_ORG, ...));

void print(const char* FORMAT_ORG, ...);
void printS(const char* MSG);
void print485(const char* FORMAT_ORG, ...);
void printS485(const char* STRING);
void printSUDP(const char* STRING);
void printUDP(const char* FORMAT_ORG, ...);

//s8 ioWrite(u16 addr, const u8 *pDat, u16 nBytes);
//s8 ioRead(u16 addr, u8 *pDat, u16 nBytes);
s8 ioWriteReg(u16 addr, s32 val);
s8 ioReadReg(u16 addr, s32 * val);
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
