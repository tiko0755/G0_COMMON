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
const char ABOUT[] = {"MB030F6W5500-1.0.0"};
const char COMMON_HELP[] = {
    "Commands:"
    "\n help()"
    "\n about()"
    "\n restart()"
    "\n reg.write(addr,val)"
    "\n reg.read(addr)"
    "\n baud.set(bHost,bBus)"
    "\n baud.get()"
    "\n ipconf.setip(ip0,..,ip3)"
    "\n   |-ipconf.setip(12.34.56.78:port)"
    "\n ipconf.setmask(msk0,..,msk3)"
    "\n ipconf.setgw(gw0,..,gw3)"
    "\n ipconf.info()"
    "\n"
};
char addrPre[4] = {0};    //addr precode
u8 boardAddr = 0;
u8 initalDone = 0;
u8 baudHost = 4;    //BAUD[4]=115200
u8 baud485 = 4;
u32 errorCode;// = 0;
/**********************************************
*  PINs Define
**********************************************/
const PIN_T RUNNING = {LED0_GPIO_Port, LED0_Pin};

/**********************************************
*  static Devices
**********************************************/
// =============================================
#define RX_POOL_LEN    (MAX_CMD_LEN)
#define TX_POOL_LEN    (MAX_CMD_LEN)
#define    RX_BUF_LEN    (128)
// uart device
static u8 uartRxPool[RX_POOL_LEN] = {0};
static u8 uartRxBuf[2*RX_BUF_LEN] = {0};
static u8 uartTxPool[TX_POOL_LEN] = {0};
UartDev_t console;
// rs485 device
const PIN_T DE = {DE_GPIO_Port, DE_Pin};
const PIN_T DET = {DET_GPIO_Port, DET_Pin};
static u8 rs485RxPool[RX_POOL_LEN] = {0};
static u8 rs485RxBuf[2*RX_BUF_LEN] = {0};
static u8 rs485TxPool[TX_POOL_LEN] = {0};
static s8 rs485AfterSend_1(UART_HandleTypeDef *huart);
static s8 rs485BeforeSend_1(void);
Rs485Dev_t rs485;

// =============================================
AT24CXX_Dev_T erom;
const PIN_T SCL = {OUT2_GPIO_Port, LED0_Pin};
const PIN_T SDA = {OUT2_GPIO_Port, LED0_Pin};
// define app eeprom size
#define EEPROM_SIZE_USR            (6*1024)
#define EEPROM_SIZE_REG            (1*1024)
#define EEPROM_SIZE_NET            (1*1024)
// define app eeprom base address
#define EEPROM_BASE_USER        0
#define EEPROM_BASE_REG            (EEPROM_BASE_USER + EEPROM_SIZE_USR)
#define EEPROM_BASE_NET            (EEPROM_BASE_REG + EEPROM_SIZE_NET)

static s8 configWrite(void);
static s8 configRead(void);

/* Private function prototypes -----------------------------------------------*/
// after GPIO initial, excute this function to enable
void boardPreInit(void){
    AT24CXX_Setup(&erom, SCL, SDA, AT24C64, 0X00);
    configRead();
}

void boardInit(void){
    //read board addr
    setupUartDev(&console, &huart2, uartTxPool, RX_POOL_LEN, uartRxPool, RX_POOL_LEN, uartRxBuf, RX_BUF_LEN);
    memset(addrPre,0,4);
    strFormat(addrPre, 4, "%d.", boardAddr);
    print("%sabout(\"%s\")\r\n", addrPre, ABOUT);

    //printS("setup rs485_1...");
    setupRs485Dev(&rs485, &huart1, rs485TxPool, RX_POOL_LEN, rs485RxPool, RX_POOL_LEN, rs485RxBuf, RX_BUF_LEN, DE, DET,
        rs485BeforeSend_1,
        rs485AfterSend_1
    );
    //printS("ok\r\n");

    HAL_GPIO_TogglePin(RUNNING.GPIOx, RUNNING.GPIO_Pin);
    HAL_Delay(30);
    HAL_GPIO_TogglePin(RUNNING.GPIOx, RUNNING.GPIO_Pin);
    HAL_Delay(30);
    HAL_GPIO_TogglePin(RUNNING.GPIOx, RUNNING.GPIO_Pin);
    HAL_Delay(30);
    HAL_GPIO_TogglePin(RUNNING.GPIOx, RUNNING.GPIO_Pin);
    HAL_Delay(30);

    printS("initial complete, type \"help\" for help\n");

    console.StartRcv(&console.rsrc);
    rs485.rsrc.uartdev.StartRcv(&rs485.rsrc.uartdev.rsrc);
    HAL_GPIO_WritePin(rs485.rsrc.DE.GPIOx, rs485.rsrc.DE.GPIO_Pin, GPIO_PIN_RESET);

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
    if(bytes>0)    console.Send(&console.rsrc, (u8*)buf, bytes);
}

void printS485(const char* STRING){
    rs485.Send(&rs485.rsrc, (const u8*)STRING, strlen(STRING));
}

void print485(const char* FORMAT_ORG, ...){
    va_list ap;
    char buf[MAX_CMD_LEN] = {0};
    s16 bytes;
    //take string
    va_start(ap, FORMAT_ORG);
    bytes = vsnprintf(buf, MAX_CMD_LEN, FORMAT_ORG, ap);
    va_end(ap);
    //send out
    if(bytes>0)    rs485.Send(&rs485.rsrc, (u8*)buf, bytes);
}

void printSUDP(const char* STRING){
//    handler_udp->send(&handler_udp->rsrc, (u8*)STRING, strlen(STRING));
}

void printUDP(const char* FORMAT_ORG, ...){
//    va_list ap;
//    char buf[MAX_CMD_LEN] = {0};
//    s16 bytes;
//    //take string
//    va_start(ap, FORMAT_ORG);
//    bytes = vsnprintf(buf, MAX_CMD_LEN, FORMAT_ORG, ap);
//    va_end(ap);
//    //send out
//    if(bytes>0)    handler_udp->send(&handler_udp->rsrc, (u8*)buf, bytes);
}

//s8 ioWrite(u16 addr, const u8 *pDat, u16 nBytes){
//    erom.Write(&erom.rsrc, EEPROM_BASE_USER + addr, pDat, nBytes);
//    return 0;
//}
//
//s8 ioRead(u16 addr, u8 *pDat, u16 nBytes){
//    erom.Read(&erom.rsrc, EEPROM_BASE_USER+addr, pDat, nBytes);
//  return 0;
//}

s8 ioReadReg(u16 addr, s32 *val){
    return(erom.Read(&erom.rsrc, EEPROM_BASE_REG+addr*4, (u8*)val, 4));
}

s8 ioWriteReg(u16 addr, s32 val){
    return(erom.Write(&erom.rsrc, EEPROM_BASE_REG+addr*4, (u8*)&val, 4));
}

static s8 configWrite(void){
    u8 buff[32]={0},i;
    buff[14] = baudHost;
    buff[15] = baud485;
    buff[16] = HAL_GetTick()&0xff;            // mac[3]
    buff[17] = (HAL_GetTick()>>8)&0xff;        // mac[4]
    buff[18] = (HAL_GetTick()>>16)&0xff;    // mac[5]
    buff[31] = 0xaa;
    erom.Write(&erom.rsrc, EEPROM_BASE_NET, buff, 32);
    return 0;
}

static s8 configRead(void){
    u8 buff[32] = {0},i;
    erom.Read(&erom.rsrc, EEPROM_BASE_NET, buff, 32);
    if(buff[31] == 0xaa){
        baudHost = buff[14];
        baud485 = buff[15];

        if(baudHost >= 7)     baudHost = 4;    // 4@115200
        if(baud485 >= 7)     baud485 = 4;    // 4@115200
    }
    return 0;
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
        if(i>=EEPROM_SIZE_REG/4)    {
            xprint("+err@%d.reg.write(\"address[0..%d]\")\r\n", brdAddr, EEPROM_SIZE_REG/4);
            return 1;
        }
        if(ioWriteReg(i,j) == 0)    xprint("+ok@%d.reg.write(%d,%d)\r\n", brdAddr, i, j);
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
//        if(ioReadReg(i,&j) == 0)
//            xprint("+ok@%d.reg.read(%d,%d)\r\n", brdAddr, i, j);
//        else xprint("+err@%d.reg.read(%d,%d)\r\n", brdAddr, i, j);
        return 1;
    }

    else if(sscanf(CMD, "baud.set %d %d", &i,&j)==2){
        for(k=0;k<7;k++){
            baudHost = k;
            if(i==BAUD[baudHost])    break;
        }
        for(k=0;k<7;k++){
            baud485 = k;
            if(j==BAUD[baud485])    break;
        }
        configWrite();
        xprint("+ok@%d.baud.set(%d,%d)\r\n", brdAddr, BAUD[baudHost], BAUD[baud485]);
        return 1;
    }
    else if(strncmp(CMD, "baud.get ", strlen("baud.get "))==0){
        configRead();
        xprint("+ok@%d.baud.get(%d,%d)\r\n", brdAddr, BAUD[baudHost], BAUD[baud485]);
        return 1;
    }

    return 0;
}

//// TCP Serve new receive callback
//static void cb_newRcvTcps(u16 rcvBytes){    // callback while there are receive data
//    s32 i,j,k;
//    s32 t0,t1;
//    u8 buff[MAX_CMD_LEN] = {0};
//    char* p;
//    s32 *tmpBuf;
//
//    handler_tcps->take_rcv(&handler_tcps->rsrc, buff, (rcvBytes>800?800:rcvBytes));
//    // print("tcps rcv:%s", buff);
//    if(rcvBytes > MAX_CMD_LEN){
//        handler_tcps->printS(&handler_tcps->rsrc, "+err@NET_PAYLOAD_LEN_OVERFLOAT\n");
//        printS("+err@NET_PAYLOAD_LEN_OVERFLOAT\n");
//        return;
//    }
//
//    for(i=0;i<MAX_CMD_LEN;i++){
//        if(buff[i] == 0)    break;
//        if(buff[i] == '(' || buff[i] == ')' || buff[i] == ',')    buff[i] = ' ';
//        if(buff[i] >= 'A' && buff[i] <= 'Z')    buff[i] += 32;
//    }
//    // common command
//    if(brdCmd((char*)buff, boardAddr, printS, print)){
//        handler_tcps->send(&handler_tcps->rsrc, buff, strlen((char*)buff));
//    }
//    else{    //forward to rs485
//        printS485(buff);
//    }
//
//}
//
//// TCP Serve connected callback
//static void cb_connectedTcps(u8* ip, u16 port){
//    print("sever connected to %d.%d.%d.%d, port[%d]\n", ip[0], ip[1], ip[2], ip[3], port);
//}
//
//static void cb_closedTcps(void){
//    printS("sever closed\n");
//}
//
//static void cb_listenTcps(u16 port){
//    print("server listen on port[%d]\n", port);
//}

//static void cb_newRcvClient(u16 rcvBytes){    // callback while there are receive data
//    u8 rcvBuf[MAX_CMD_LEN] = {0};
//    s32 rtn;
//    memset(rcvBuf,0,MAX_CMD_LEN);
//    memcpy(rcvBuf, "+ok@", 4);
//    rtn = handler_tcpc->take_rcv(&handler_tcpc->rsrc, &rcvBuf[4], rcvBytes);
//    print("client receive %d bytes:%s", rtn, &rcvBuf[4]);
//    //handler_tcpc->closeSession(&handler_tcpc->rsrc);
//    handler_tcpc->send(&handler_tcpc->rsrc, rcvBuf, rcvBytes+4);
//}
//
//static void cb_connectedClient(u8* ip, u16 port){
//    print("client connected to %d.%d.%d.%d, port[%d]\n", ip[0], ip[1], ip[2], ip[3], port);
//}
//
//static void cb_closedClient(){
//    printS("client closed\n");
//}

// rcvBytes, include 8 bytes, ip/port/payloadlen
static void cb_newRcvUdp(u16 rcvBytes){
//    u16 i;
//    u8 buff[MAX_CMD_LEN] = {0};
//    s32 count;
//    // poll for net data and insert ringbuffer

//    count = RingBuffer_GetFree(&handler_udp->rsrc.rxRB);
//    if(count==0)    return;
//    i = handler_udp->take_rcv(&handler_udp->rsrc, buff, MIN(MAX_CMD_LEN,count));
//    if(i>0)    RingBuffer_InsertMult(&handler_udp->rsrc.rxRB, buff, strlen((char*)buff));
//    else    return;
}

static void cb_closedUdp(){
    printS("UDP closed\n");
}

static s8 rs485BeforeSend_1(void){
    if(initalDone == 0)    return 0;
    if(HAL_GPIO_ReadPin(rs485.rsrc.DET.GPIOx, rs485.rsrc.DET.GPIO_Pin)==GPIO_PIN_SET){
        return -1;
    }
    HAL_GPIO_WritePin(rs485.rsrc.DE.GPIOx, rs485.rsrc.DE.GPIO_Pin, GPIO_PIN_SET);
    while(1){
        if(HAL_GPIO_ReadPin(rs485.rsrc.DET.GPIOx, rs485.rsrc.DET.GPIO_Pin)==GPIO_PIN_SET){
            break;
        }
    }
    return 0;
}

static s8 rs485AfterSend_1(UART_HandleTypeDef *huart){
    if(initalDone == 0)    return 0;
    if(huart->Instance == rs485.rsrc.uartdev.rsrc.huart->Instance){
        HAL_GPIO_WritePin(rs485.rsrc.DE.GPIOx, rs485.rsrc.DE.GPIO_Pin, GPIO_PIN_RESET);
        rs485.rsrc.uartdev.rsrc.flag |= BIT(0);
//        while(1){
//            if(HAL_GPIO_ReadPin(rs485.rsrc.DET.GPIOx, rs485.rsrc.DET.GPIO_Pin)==GPIO_PIN_RESET){
//                break;
//            }
//        }
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
    if(initalDone==0)    return;
    rs485.rsrc.uartdev.rsrc.afterSend(huart);
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
