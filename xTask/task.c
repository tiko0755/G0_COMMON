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
#include "cspin_command.h"
#include "boardFeeder.h"

/* Public variables ---------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

//extern UdpDev_t* handler_udp;

/**********************************************
*  PINs Define
**********************************************/

/* Private function prototypes -----------------------------------------------*/
static u16 tickRT4,tick4,tick8,tick16,tick32,tick64,tick128;
void taskRT4(void);
static void task0(void);
void task4(void);
void task8(void);
void task16(void);
void task32(void);
void task64(void);
void task128(void);

void taskIrq(void){
	if((++tickRT4&0x03) == 0)	taskRT4();
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
	if(initalDone == 0)	return;
	task0();
	if(tick4>=4)		{task4();tick4 = 0;}
	if(tick8>=8)		{task8();tick8 = 0;}
	if(tick16>=16)		{task16();tick16 = 0;}
	if(tick32>=32)		{task32();tick32 = 0;}
	if(tick64>=64)		{task64();tick64 = 0;}
	if(tick128>=128)	{task128();tick128 = 0;}
}

/**
  * @brief quickly task
  * @param none
  * @retval None
  */
static void task0(void){
	console.TxPolling(&console.rsrc);
}

/**
  * @brief real time task
  * @param none
  * @retval None
  */
void taskRT4(void){
}

/**
  * @brief tack per 4ms
  * @param none
  * @retval None
  */
void task4(void){
	console.RxPolling(&console.rsrc);
}

/**
  * @brief tack per 8ms
  * @param none
  * @retval None
  */
void task8(void){

}

/**
  * @brief tack per 16ms
  * @param none
  * @retval None
  */

void commandFormat(char* buff, u16 len){
	u16 i;
	for(i=0;i<len;i++){
		if(buff[i] == 0)	break;
		if(buff[i] == '(' || buff[i] == ')' || buff[i] == ',')	buff[i] = ' ';
		if(buff[i] >= 'A' && buff[i] <= 'Z')	buff[i] += 32;
	}
}

u8 doCommand(RINGBUFF_T* rb, char* buff, u16 len, void (*xprintS)(const char* MSG), void (*xprint)(const char* FORMAT_ORG, ...)){
	char *CMD;
	if(rb==NULL || buff==NULL || len==0)	return 0;
	//message from uart
	if(fetchLineFromRingBuffer(rb, (char*)buff, len)){
		if(cmdBrdFdr(&feeder, buff, printLCD)){}
		else if(strncmp((char*)buff, addrPre, strlen(addrPre)) == 0){
			commandFormat(buff, len);
			CMD = (char*)buff+strlen(addrPre);
			if(brdCmd(CMD, boardAddr, xprintS, xprint)){	}
			else if(l6480Cmd(&stpr[0], CMD, boardAddr, xprintS, xprint)){	}
			else if(l6480Cmd(&stpr[1], CMD, boardAddr, xprintS, xprint)){	}
			else{		xprint("+unknown@%s", buff);	}
		}
		return 1;
	}
	return 0;
}

#define MAX_LINE_LEN	MAX_CMD_LEN
void task16(void)
{
	u32 len;
	char buff[MAX_LINE_LEN] = {0};
	//message from uart
	doCommand(&console.rsrc.rxRB, buff, MAX_LINE_LEN, printS, print);
	feeder.Polling(&feeder.rsrc, 16);
}

/**
  * @brief task per 32ms
  * @param none
  * @retval none
  */
void task32(void){
}

/**
  * @brief tack per 64ms
  * @param none
  * @retval None
  */
void task64(void)
{
	HAL_GPIO_TogglePin(RUNNING.GPIOx, RUNNING.GPIO_Pin);
}

/**
  * @brief tack per 128ms
  * @param none
  * @retval None
  */
void task128(void){
	HAL_IWDG_Refresh(&hiwdg);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
