/******************** (C) COPYRIGHT 2015 INCUBECN *****************************
* File Name          : outputCmd.h
* Author             : Tiko Zhong
* Date First Issued  : 12/01/2015
* Description        : This file provides a set of functions needed to manage the
*                      communication using HAL_UARTxxx
********************************************************************************
* History:e
* 12/01/2015: V0.0
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _OUTPUT_CMD_H
#define _OUTPUT_CMD_H

#include "misc.h"

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
extern const char OUTPUT_HELP[];
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
u8 outputCmd(void *dev, char* CMD, u8 brdAddr, void (*xprint)(const char* FORMAT_ORG, ...));

#endif /* _OUTPUT_CMD_H */

/******************* (C) COPYRIGHT 2015 INCUBECN *****END OF FILE****/
