/******************** (C) COPYRIGHT 2015 INCUBECN *****************************
* File Name          : board.c
* Author             : Tiko Zhong
* Date First Issued  : 11/18/2021
* Description        : 
*                      
********************************************************************************
* History:
* Apr22,2021: V0.2
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include "board.h"
#include "main.h"
#include "at24cxx.h"

#define NOUSED_PIN_INDX 255

/* import handle from main.c variables ---------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
const char ABOUT[] = {"MB030CM02 1.1"};
const char COMMON_HELP[] = {
	"Commands:"
	"\n help()"
	"\n about()"
	"\n restart()"
	"\n reg.write(addr,val)"
	"\n reg.read(addr)"
	"\n baud.set(bHost,bBus)"
	"\n baud.get()"
	"\n"
};
char addrPre[4] = {0};	//addr precode
u8 boardAddr = 0;
u8 initalDone = 0;
u32 boardAge = 0;
u8 baudIndxHost = 2;	//BAUD[2]=115200
u8 baudIndx485 = 2;
u32 errorCode;// = 0;
/**********************************************
*  PINs Define
**********************************************/
const PIN_T RUNNING = {LED0_GPIO_Port, LED0_Pin};

/**********************************************
*  static Devices
**********************************************/
// =============================================
#define RX_POOL_LEN	(MAX_CMD_LEN)
#define TX_POOL_LEN	(MAX_CMD_LEN)
#define	RX_BUF_LEN	(128)
// uart device
static u8 uartRxPool[RX_POOL_LEN] = {0};
static u8 uartRxBuf[2*RX_BUF_LEN] = {0};
static u8 uartTxPool[TX_POOL_LEN] = {0};
UartDev_t console;

// =============================================
AT24CXX_Dev_T erom;
const PIN_T SCL = {SCL_GPIO_Port, SCL_Pin};
const PIN_T SDA = {SDA_GPIO_Port, SDA_Pin};
// define app eeprom size
#define EEPROM_SIZE_USR			(4*1024)
#define EEPROM_SIZE_SECTOR0		(1*512)		// for system config
#define EEPROM_SIZE_SECTOR1		(1*512)
#define EEPROM_SIZE_SECTOR2		(1*512)
#define EEPROM_SIZE_SECTOR3		(1*512)
#define EEPROM_SIZE_SECTOR4		(1*512)
#define EEPROM_SIZE_SECTOR5		(1*512)
#define EEPROM_SIZE_REG			(1*1024)
// define app eeprom base address
#define EEPROM_BASE_USER		0
#define EEPROM_BASE_SECTOR0		(EEPROM_BASE_USER + EEPROM_SIZE_USR)
#define EEPROM_BASE_SECTOR1		(EEPROM_BASE_SECTOR0 + EEPROM_SIZE_SECTOR0)
#define EEPROM_BASE_SECTOR2		(EEPROM_BASE_SECTOR1 + EEPROM_SIZE_SECTOR1)
#define EEPROM_BASE_SECTOR3		(EEPROM_BASE_SECTOR2 + EEPROM_SIZE_SECTOR2)
#define EEPROM_BASE_SECTOR4		(EEPROM_BASE_SECTOR3 + EEPROM_SIZE_SECTOR3)
#define EEPROM_BASE_SECTOR5		(EEPROM_BASE_SECTOR4 + EEPROM_SIZE_SECTOR4)
#define EEPROM_BASE_REG			(EEPROM_BASE_SECTOR5 + EEPROM_SIZE_SECTOR5)
// ===============================================
static s8 configWrite(u16 romBase);
static s8 configRead(u16 romBase);
static u8 getBoardAddr(void);

// =============================================
const PIN_T GPIO_IN[FDR_IN_CNT] = {
	{IN0_GPIO_Port, IN0_Pin},
	{IN1_GPIO_Port, IN1_Pin},
	{IN2_GPIO_Port, IN2_Pin},
	{IN3_GPIO_Port, IN3_Pin},
};
const PIN_T GPIO_OUT[FDR_OUT_CNT] = {
	{OUT0_GPIO_Port, OUT0_Pin},
	{OUT1_GPIO_Port, OUT1_Pin}
};

const PIN_T STPR_CS[2] = {	{CS1_GPIO_Port, CS1_Pin},{CS2_GPIO_Port, CS2_Pin}};
const PIN_T STPR_SW[2] = {{SW1_GPIO_Port, SW1_Pin},{SW2_GPIO_Port, SW2_Pin}};
const PIN_T STPR_FLG[2] = {{FLG1_GPIO_Port, FLG1_Pin},{FLG2_GPIO_Port, FLG2_Pin}};
const PIN_T STPR_BUSY[2] = {{NBSY1_GPIO_Port, NBSY1_Pin},{NBSY2_GPIO_Port, NBSY2_Pin}};
cSPIN_DEV_T stpr[2];

BrdFdrDev_T feeder;
/* Private function prototypes -----------------------------------------------*/
// after GPIO initial, excute this function to enable
void boardPreInit(void){
	AT24CXX_Setup(&erom, SCL, SDA, AT24C64, 0X00);
	configRead(EEPROM_BASE_SECTOR0);
}

void boardInit(void){
	//read board addr
	boardAddr = 0;	//getBoardAddr();
	// setupt console
	setupUartDev(&console, &huart1, uartTxPool, RX_POOL_LEN, uartRxPool, RX_POOL_LEN, uartRxBuf, RX_BUF_LEN);
	memset(addrPre,0,4);
	strFormat(addrPre, 4, "%d.", boardAddr);
	print("%sabout(\"%s\")\r\n", addrPre, ABOUT);

	// setup CSPIN
	cSPIN_Setup(
		&stpr[0], "m0",
		CHIP_L6480,
		&hspi2,
		&STPR_CS[0],

		&STPR_SW[0],
		NULL,

		&STPR_FLG[0],
		&STPR_BUSY[0],
		NULL,

		printLCD,
		ioRead,
		ioWrite,
		EEPROM_BASE_SECTOR1
	);
	cSPIN_Setup(
		&stpr[1], "m1",
		CHIP_L6480,
		&hspi2,
		&STPR_CS[1],

		&STPR_SW[1],
		NULL,

		&STPR_FLG[1],
		&STPR_BUSY[1],
		NULL,

		printLCD,
		ioRead,
		ioWrite,
		EEPROM_BASE_SECTOR2
	);

	BrdFdrSetup(
		&feeder,
		"fdr",
		stpr,		// steppers
		GPIO_IN,
		GPIO_OUT,
		printLCD,
		ioRead,
		ioWrite,
		EEPROM_BASE_SECTOR3
	);

	print("initial complete, type \"%d.help\" for help\n", boardAddr);

	console.StartRcv(&console.rsrc);
	initalDone = 1;
}

void printS(const char* STRING){
	console.Send(&console.rsrc, (const u8*)STRING, strlen(STRING));
}

void print(const char* FORMAT_ORG, ...){
	va_list ap;
	char buf[MAX_CMD_LEN] = {0};
	s16 bytes;
	//take string
	va_start(ap, FORMAT_ORG);
	bytes = vsnprintf(buf, MAX_CMD_LEN, FORMAT_ORG, ap);
	va_end(ap);
	//send out
	if(bytes>0)	console.Send(&console.rsrc, (u8*)buf, bytes);
}

void printLCD(const char* FORMAT_ORG, ...){
	va_list ap;
	char buf[MAX_CMD_LEN] = {0};
	s16 bytes;
	//take string
	va_start(ap, FORMAT_ORG);
	bytes = vsnprintf(buf, MAX_CMD_LEN, FORMAT_ORG, ap);
	va_end(ap);
	if(bytes <= 0)	return;

	buf[bytes++] = 0xff;
	buf[bytes++] = 0xff;
	buf[bytes++] = 0xff;
	//send out
	if(bytes>0)	console.Send(&console.rsrc, (u8*)buf, bytes);
}

s8 ioWrite(u16 addr, const u8 *pDat, u16 nBytes){
	erom.Write(&erom.rsrc, EEPROM_BASE_USER + addr, pDat, nBytes);
	return 0;
}

s8 ioRead(u16 addr, u8 *pDat, u16 nBytes){
	erom.Read(&erom.rsrc, EEPROM_BASE_USER+addr, pDat, nBytes);
  return 0;
}

s8 ioReadReg(u16 addr, s32 *val){
	return(erom.Read(&erom.rsrc, EEPROM_BASE_REG+addr*4, (u8*)val, 4));
}

s8 ioWriteReg(u16 addr, s32 val){
	return(erom.Write(&erom.rsrc, EEPROM_BASE_REG+addr*4, (u8*)&val, 4));
}

static s8 configWrite(u16 romBase){
	u8 buff[32]={0},i;
	buff[0] = boardAge;
	buff[1] = boardAge>>8;
	buff[2] = boardAge>>16;
	buff[3] = boardAge>>24;
	buff[4] = baudIndxHost;
	buff[5] = baudIndx485;
	buff[31] = 0xa5;
	for(i=0;i<31;i++){	buff[31] ^= buff[i];	}
	erom.Write(&erom.rsrc, romBase, buff, 32);
	return 0;
}

static s8 configRead(u16 romBase){
	u8 buff[32] = {0},i,check = 0xa5;
	erom.Read(&erom.rsrc, romBase, buff, 32);
	for(i=0;i<31;i++){	check ^= buff[i];	}
	if(buff[31] == check){
		boardAge = buff[3];		boardAge <<= 8;
		boardAge |= buff[2];	boardAge <<= 8;
		boardAge |= buff[1];	boardAge <<= 8;
		boardAge |= buff[0];
		baudIndxHost = buff[4];
		baudIndx485 = buff[5];
		if(baudIndxHost >= 7)	 baudIndxHost = 2;	// 2@115200
		if(baudIndx485 >= 7)	 baudIndx485 = 2;	// 2@115200
	}
	return 0;
}

const PIN_T addrPin[5] = {
	{A0_GPIO_Port, A0_Pin},
	{A1_GPIO_Port, A1_Pin},
	{A2_GPIO_Port, A2_Pin},
	{A3_GPIO_Port, A3_Pin},
	{A4_GPIO_Port, A4_Pin}
};

static u8 getBoardAddr(void){
	u8 addr = 0, i;
	for(i=0;i<5;i++){
		if(HAL_GPIO_ReadPin(addrPin[i].GPIOx, addrPin[i].GPIO_Pin) == GPIO_PIN_RESET){	addr |= BIT(i);	}
	}
	return addr;
}

u8 brdCmd(const char* CMD, u8 brdAddr, void (*xprintS)(const char* MSG), void (*xprint)(const char* FORMAT_ORG, ...)){
	s32 i,j,k, argv[5];
	// common
	if(strncmp(CMD, "about ", strlen("about ")) == 0){
		xprint("+ok@%d.about(\"%s\")\r\n", brdAddr, ABOUT);
		return 1;
	}
	else if(strncmp(CMD, "help ", strlen("help ")) == 0){
		xprintS(COMMON_HELP);
		xprint("+ok@%d.help()\r\n",brdAddr);
		return 1;
	}
	else if(strncmp(CMD, "restart ", strlen("restart ")) == 0){
		HAL_NVIC_SystemReset();
		return 1;
	}

	else if(sscanf(CMD, "reg.write %d %d ", &i, &j)==2){
		if(i>=EEPROM_SIZE_REG/4)	{
			xprint("+err@%d.reg.write(\"address[0..%d]\")\r\n", brdAddr, EEPROM_SIZE_REG/4);
			return 1;
		}
		if(ioWriteReg(i,j) == 0)	xprint("+ok@%d.reg.write(%d,%d)\r\n", brdAddr, i, j);
		else xprint("+err@%d.reg.write(%d,%d)\r\n", brdAddr, i, j);
		return 1;
	}
	else if(sscanf(CMD, "reg.read %d ", &i)==1){
		if(i>=EEPROM_SIZE_REG/4){
			xprint("+err@%d.reg.read(\"address[0..%d]\")\r\n", brdAddr, EEPROM_SIZE_REG/4);
			return 1;
		}
		ioReadReg(i,&j);
		xprint("+ok@%d.reg.read(%d,%d)\r\n", brdAddr, i, j);
		return 1;
	}

	else if(sscanf(CMD, "baud.set %d %d", &i,&j)==2){
		for(k=0;k<8;k++){
			baudIndxHost = k;
			if(i==BAUD[baudIndxHost])	break;
		}
		for(k=0;k<8;k++){
			baudIndx485 = k;
			if(j==BAUD[baudIndx485])	break;
		}
		configWrite(EEPROM_BASE_SECTOR0);
		xprint("+ok@%d.baud.set(%d,%d)\r\n", brdAddr, BAUD[baudIndxHost], BAUD[baudIndx485]);
		return 1;
	}
	else if(strncmp(CMD, "baud.get ", strlen("baud.get "))==0){
		configRead(EEPROM_BASE_SECTOR0);
		xprint("+ok@%d.baud.get(%d,%d)\r\n", brdAddr, BAUD[baudIndxHost], BAUD[baudIndx485]);
		return 1;
	}

	return 0;
}


/**
  * @brief EXTI line detection callbacks
  * @param GPIO_Pin: Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
}

/**
  * @brief  Conversion complete callback in non blocking mode
  * @param  AdcHandle : ADC handle
  * @note   This example shows a simple way to report end of conversion
  *         and get conversion result. You can add your own implementation.
  * @retval None
  */
// void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *AdcHandle){}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){
	if(initalDone==0)	return;
	if(huart->Instance == console.rsrc.huart->Instance){
		console.rsrc.flag |= BIT(0);
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
}

/**
  * @brief  TxRx Transfer completed callback.
  * @param  hspi: SPI handle
  * @note   This example shows a simple way to report end of DMA TxRx transfer, and
  *         you can add your own implementation.
  * @retval None
  */
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi){
}

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi){
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi){
}

/**
  * @brief  SPI error callbacks.
  * @param  hspi: SPI handle
  * @note   This example shows a simple way to report transfer error, and you can
  *         add your own implementation.
  * @retval None
  */
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi){
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
