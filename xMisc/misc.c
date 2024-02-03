/******************** (C) COPYRIGHT 2015 INCUBECN *****************************
* File Name          : misc.c
* Author             : Tiko Zhong
* Date First Issued  : 07/03/201
* Description        : 
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "misc.h"
#include "stdarg.h"
#include "string.h"
#include "stdio.h"
/* Private define ------------------------------------------------------------*/
#define DEV_MAX 64
/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
char CMD_END[4] = "\r\n";

/* Private function prototypes -----------------------------------------------*/
/*******************************************************************************
* Function Name  : delay
* Description    : delay for n us. COUNT_OF_1US should be tuned in different plat
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
#define COUNT_OF_1US 12
void miscDelay(u8 us){
    u8 usX1;
    u8 delayUs = us;
    while(delayUs){
        for(usX1=0; usX1<COUNT_OF_1US; usX1++){}
        delayUs --;
    }
}

s16 strFormat(char *buf, u16 len, const char* FORMAT_ORG, ...){
    va_list ap;
    s16 bytes;
    //take string
    if(FORMAT_ORG == NULL)    return -1;
    va_start(ap, FORMAT_ORG);
    bytes = vsnprintf(buf, len, FORMAT_ORG, ap);
    va_end(ap);
    return bytes;
}

void devRename(char* devName, const char* NEW_NAME){
    memset(devName,0, DEV_NAME_LEN);
    strcpy(devName, NEW_NAME);
}

/* get complement code function, unsigned short must be U16 */
#define COMPLEMENT_CODE_U16     0x8000
long getComplementCode_u16(unsigned short raw_code)
{
	long complement_code;
	int dir;

	if (0 != (raw_code & COMPLEMENT_CODE_U16)){
		dir = -1;
		raw_code =  (0xFFFF - raw_code) + 1;
	} else {
		dir = 1;
	}
	complement_code = (long)raw_code * dir;

	return complement_code;
}


/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
