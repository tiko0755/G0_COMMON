/******************** (C) COPYRIGHT 2015 INCUBECN *****************************
* File Name          : cspin_command.h
* Author             : Tiko Zhong
* Date First Issued  : 12/01/2015
* Description        : This file provides a set of functions needed to manage the
*                      communication using HAL_UARTxxx
********************************************************************************
* History:
* 12/01/2015: V0.0
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _CSPIN_COMMAND_H
#define _CSPIN_COMMAND_H

/* Includes ------------------------------------------------------------------*/
#include "misc.h"

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
u8 l6480Cmd(
	void *dev, 	
	const char* CMD, 
	u8 brdAddr, 
	void (*xprintS)(const char* MSG), 
	void (*xprint)(const char* FORMAT_ORG, ...)
);

#endif /* _CSPIN_COMMAND_H */

/******************* (C) COPYRIGHT 2015 INCUBECN *****END OF FILE****/
