/**********************************************************
filename: misc.h
**********************************************************/
#ifndef _MISC_H_
#define _MISC_H_

#define USING_RTOS	0
#define USING_F0	1

#if USING_RTOS
#include "cmsis_os.h"
#endif

#if USING_F0
#include "stm32f0xx_hal.h"
#endif

#if USING_F1
#include "stm32f1xx_hal.h"
#endif

#if USING_G0
#include "stm32g0xx_hal.h"
#endif

#define NOP __NOP
#define CALLBACK_MSG_LEN	48
#define DEV_NAME_LEN		16
#define MAX_CMD_LEN			512
#define MAX_TASK			64
#define MAX_INSTANCE		16

#define	BIT(n)	((u32)1<<n)
#define	BIT_LEN(n)	(0XFFFFFFFF>>(32-n))

#if !defined(MAX)
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif
#if !defined(MIN)
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

/****************************************************************************
 @ enums
****************************************************************************/
typedef enum{DEV_ERROR = 0, DEV_SUCCESS = !DEV_ERROR}	DEV_STATUS;
typedef enum{FALSE = 0, TRUE = !FALSE}	Bool;
typedef enum{NONE_BLOCKING = 0, BLOCKING,} TRANSFER_BLOCK_T;
/*****************************************************************************
 @ typedefs
****************************************************************************/
typedef uint8_t	 	u8;
typedef uint16_t 	u16;
typedef uint32_t 	u32;
typedef int8_t 		s8;
typedef int16_t 	s16;
//typedef int32_t 	s32;
typedef int			s32;

typedef struct {
	GPIO_TypeDef* GPIOx;	// Port Register Base addr.
	uint16_t GPIO_Pin;		// Pin
} PIN_T;

typedef struct{
	//GPIO
	PIN_T PA0,PA1,PA2;
	PIN_T PB0,PB1,PB2,PB3;
	PIN_T PC0,PC1,PC2,PC3;
	PIN_T PD0,PD1,PD2,PD3;
	//SPI IIC ADC TIM
	void* SPI_HANDLE;
	void* IIC_HANDLE;
	void* UART_HANDLE;
	void* TIM_HANDLE;
	void (*Setup)(u8 argc, void* argp);
}DEV_PORT;

typedef struct {
	s32 mul;
	u16 div;
} FLOAT_T;

typedef int32_t Float16B;

typedef struct {
	u32 integer;
	u16 decimal;
} DOUBLE;

typedef struct {
	u8 offset;
	u16 gainMul;
	u16 gainDiv;
} CAL_T;

typedef struct {
	u16 offset;
	u32 gainMul;
	u32 gainDiv;
}CAL32_T;

typedef struct {
	const char *KeyStr;
	const char *ReplaceBy;
}CMD_DEF_T;

typedef struct {
	const char* devName;
	const char* funcName;
	const char* format;
	//s32 (*func)	(void* pRsrc, u8 argc, void* argv);
} CMD_FUNC_T;

extern char CMD_END[4];
extern const u32 BAUD[8];
/* delay about 1us*/
void miscDelay(u8 us);
s16 strFormat(char *buf, u16 len, const char* FORMAT_ORG, ...);
void devRename(char* oldName, const char* NEW_NAME);
#endif

