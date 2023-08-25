/******************** (C) COPYRIGHT 2015 INCUBECN *****************************
* File Name          : stm_flash_cmd.h
* Author             : Tiko Zhong
* Description        : This file provides a set of functions needed to manage the
*                      communication using HAL_UARTxxx
********************************************************************************
* History:
* 12/01/2015: V0.0
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _STM_FLASH_CMD_H_
#define _STM_FLASH_CMD_H_

/* Includes ------------------------------------------------------------------*/
#include "misc.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
extern const char STM_FLSH_HELP[];
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
u8 stmFlsh_Cmd(void *dev, u8* CMD, u8 len, void (*xprint)(const char* FORMAT_ORG, ...));

#endif /* _INPUT_CMD_H */

/******************* (C) COPYRIGHT 2015 INCUBECN *****END OF FILE****/
