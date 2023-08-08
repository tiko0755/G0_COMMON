/******************** (C) COPYRIGHT 2022 INCUBECN *****************************
* File Name          : rampCmd.h
* Author             : Tiko Zhong
* Date First Issued  : 09/21,2022
* Description        : This file provides a set of functions needed to manage the
*                      stepper ramp generator
********************************************************************************
* History:e
* 09/21,2022: V0.0
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _RAMP_CMD_H
#define _RAMP_CMD_H

#include "misc.h"

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
extern const char RAMP_HELP[];
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
u8 rampCmdU8(void *dev, u8* cmd, u8 len, void (*xprint)(const char* FORMAT_ORG, ...));
u8 rampCmd(void *dev, char* CMD, u8 brdAddr, void (*xprint)(const char* FORMAT_ORG, ...));

#endif /* _OUTPUT_CMD_H */

/******************* (C) COPYRIGHT 2015 INCUBECN *****END OF FILE****/
