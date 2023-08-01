/******************** (C) COPYRIGHT 2015 INCUBECN *****************************
* File Name          : task.c
* Author             : Tiko Zhong
* Date First Issued  : 11/18/2016
* Description        : 
*                      
********************************************************************************
* History:
* 11/18/2016: V0.0    
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "task.h"
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include "board.h"

/* Public variables ---------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

static void forwardRes(void* rsrc, const char* MSG);
static void forwardReq(const char* MSG);
static u8 doCommand(char* buff, u16 len,
        void (*xprint)(const char* FORMAT_ORG, ...),
        void (*forward)(const char* MSG));
/**********************************************
*  PINs Define
**********************************************/

/* Private function prototypes -----------------------------------------------*/
static u16 tickRT4,tick1,tick4,tick8,tick16,tick32,tick64,tick128;
static void taskRT4(void);
static void task0(void);
static void task1(void);
static void task4(void);
static void task8(void);
static void task16(void);
static void task32(void);
static void task64(void);
static void task128(void);

void taskIrq(void){
    u8 i;
    if((++tickRT4&0x03) == 0)    taskRT4();
    tick1++;
    tick4++;
    tick8++;
    tick16++;
    tick32++;
    tick64++;
    tick128++;
}

/**
  * @brief polling task
  * @param none
  * @retval None
  */
void taskPolling(void){
    if(initalDone == 0)    return;
    task0();
    task1();
    if(tick4>=4)        {task4();tick4 = 0;        }
    if(tick8>=8)        {task8();tick8 = 0;        }
    if(tick16>=16)        {task16();tick16 = 0;    }
    if(tick32>=32)        {task32();tick32 = 0;    }
    if(tick64>=64)        {task64();tick64 = 0;    }
    if(tick128>=128)    {task128();tick128 = 0;    }
}

/**
  * @brief quickly task
  * @param none
  * @retval None
  */
static void task0(void){
    console.TxPolling(&console.rsrc);
    rs485.TxPolling(&rs485.rsrc);
}

static void task1(void){
    s32 i;
    if(tick1 == 0)    return;
    tick1 = 0;
}

/**
  * @brief real time task
  * @param none
  * @retval None
  */
static void taskRT4(void){
}

/**
  * @brief tack per 4ms
  * @param none
  * @retval None
  */
#define MAX_LINE_LEN    MAX_CMD_LEN
static u32 breakTim = 0;
static u32 rbCount = 0;
static void task4(void){
    s32 len, count, bytes;
    char buff[MAX_LINE_LEN+1] = {0};

    console.RxPolling(&console.rsrc);
    rs485.RxPolling(&rs485.rsrc);

    //message from uart
    if(fetchLineFromRingBuffer(&console.rsrc.rxRB, (char*)buff, MAX_LINE_LEN)){
        doCommand(buff, MAX_LINE_LEN, print, forwardReq);
    }

    //message from rs485
    memset(buff,0,MAX_LINE_LEN);
    len = rs485.RxFetchFrame(&rs485.rsrc, (u8*)buff, MAX_LINE_LEN);
    if(len>0){
    }
}

/**
  * @brief tack per 8ms
  * @param none
  * @retval None
  */
static u16 ledTick = 0;
static void task16(void){
    ledTick += 16;
}

/**
  * @brief tack per 16ms
  * @param none
  * @retval None
  */
static void commandFormat(char* buff, u16 len){
    u16 i;
    for(i=0;i<len;i++){
        if(buff[i] == 0)    break;
        if(buff[i] == '(' || buff[i] == ')' || buff[i] == ',')    buff[i] = ' ';
        if(buff[i] >= 'A' && buff[i] <= 'Z')    buff[i] += 32;
    }
}

static u8 doCommand(char* buff, u16 len,
        void (*xprint)(const char* FORMAT_ORG, ...),
        void (*forward)(const char* MSG)){
//    s32 i;
//    char *CMD;
//    if(buff==NULL)    return 0;
//    //message from uart
//    if(sscanf(buff, "%d.", &i) <= 0){
//        if(strncmp(buff, "help", strlen("help")) == 0){    printHelp(boardAddr, xprint);    }
//        else if(strncmp(buff, "about", strlen("about")) == 0){
//            xprint("+ok@about(%d,\"%s\")\r\n", boardAddr, ABOUT);
//            ledFlshTz = 15;
//        }
//        else if(promise[0].Cmd(&promise[0].rsrc, buff)){}
//        else if(promise[1].Cmd(&promise[1].rsrc, buff)){}
//        else if(promise[2].Cmd(&promise[2].rsrc, buff)){}
//        else if(promise[3].Cmd(&promise[3].rsrc, buff)){}
//        else{    xprint("+unknown@%s", buff);    }
//    }
//    else if(sscanf(buff, "%d.", &i)==1){
//        if(i == boardAddr){
//            commandFormat(buff, len);
//            CMD = (char*)buff + strlen(addrPre);
//            if(brdCmd(CMD, boardAddr, xprint)){    }
//            else{        xprint("+unknown@%s", buff);    }
//        }
//        else{
//            if(promise[0].Cmd(&promise[0].rsrc, buff)){}
//            else if(promise[1].Cmd(&promise[1].rsrc, buff)){}
//            else if(promise[2].Cmd(&promise[2].rsrc, buff)){}
//            else if(promise[3].Cmd(&promise[3].rsrc, buff)){}
//            else if(forward){    forward((char*)buff);    }
//        }
//    }
    return 1;
}

static void task8(void)
{
}

/**
  * @brief task per 32ms
  * @param none
  * @retval none
  */
static void task32(void){
}

/**
  * @brief tack per 64ms
  * @param none
  * @retval None
  */
static void task64(void){
//    HAL_IWDG_Refresh(&hiwdg);
}

/**
  * @brief tack per 128ms
  * @param none
  * @retval None
  */
static u16 xDelay = 0;
static void task128(void){
//    xDelay+=128;
//    if(xDelay>200){
//        xDelay = 0;
//        print("%d\t%d\n", stprRamp.rsrc.spdCur, stprRamp.rsrc.spdTgt);
//    }
}

static void forwardRes(void* rsrc, const char* MSG){
    if(strncmp(MSG, "+$timeout@req", strlen("+$timeout@req")) == 0){
        print("host: %s", MSG);
    }
    else{    printS(MSG);        }
}

static void forwardReq(const char* MSG){
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
