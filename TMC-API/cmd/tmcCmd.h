/******************** (C) COPYRIGHT 2015 INCUBECN *****************************
* File Name          : tmcCmd.h
* Author             : Tiko Zhong
* Date First Issued  : 08/01, 2022
* Description        : This file provides a set of functions needed to manage the
*                      communication using HAL_UARTxxx
********************************************************************************
* History:
* Aug01,2022: V0.0
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _TMC_CMD_H
#define _TMC_CMD_H

/* Includes ------------------------------------------------------------------*/
#include "misc.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
extern const char TMC_HELP[];
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
u8 tmcCmd(void *dev, char* CMD, u8 brdAddr, void (*xprint)(const char* FORMAT_ORG, ...));

#endif /* _TMC_CMD_H */

/******************* (C) COPYRIGHT 2015 INCUBECN *****END OF FILE****/
