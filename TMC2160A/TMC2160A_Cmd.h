/******************** (C) COPYRIGHT 2022 tiko *****************************
* File Name          : rampCmd.h
* Author             : Tiko Zhong
* Description        : This file provides a set of functions needed to manage the
*                      stepper ramp generator
********************************************************************************
* History:e
* 09/21,2022: V0.0
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TMC2160A_CMD_H__
#define __TMC2160A_CMD_H__

#include "misc.h"

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
extern const char TMC2160A_HELP[];
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
u8 tmc2160aCmdU8(void *dev, u8* cmd, u8 len, void (*xprint)(const char* FORMAT_ORG, ...));
u8 tmc2160aCmd(void *dev, char* CMD, u8 brdAddr, void (*xprint)(const char* FORMAT_ORG, ...));

#endif /* _OUTPUT_CMD_H */

/******************* (C) COPYRIGHT 2015 INCUBECN *****END OF FILE****/
