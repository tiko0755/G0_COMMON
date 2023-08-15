/******************** (C) COPYRIGHT 2015 INCUBECN *****************************
* File Name          : inputCmd.h
* Author             : Tiko Zhong
* Date First Issued  : 12/01/2015
* Description        : This file provides a set of functions needed to manage the
*                      communication using HAL_UARTxxx
********************************************************************************
* History:
* 12/01/2015: V0.0
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _INPUT_CMD_H
#define _INPUT_CMD_H

/* Includes ------------------------------------------------------------------*/
#include "misc.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
extern const char INPUT_HELP[];
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
u8 inputCmdU8(void *dev, u8* CMD, u8 len, void (*xprint)(const char* FORMAT_ORG, ...));
u8 inputCmd(void *dev, char* CMD, u8 brdAddr, void (*xprint)(const char* FORMAT_ORG, ...));

#endif /* _INPUT_CMD_H */

/******************* (C) COPYRIGHT 2015 INCUBECN *****END OF FILE****/
