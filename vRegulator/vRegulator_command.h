/******************** (C) COPYRIGHT 2015 INCUBECN *****************************
* File Name          : vRegulator_command.h
* Author             : Tiko Zhong
* Date First Issued  : Apr/16/2022
* Description        : This file provides a set of functions needed to manage the
*                      digital DC regulator
********************************************************************************
* History:
* 12/01/2015: V0.0
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _VREGULATOR_COMMAND_H
#define _VREGULATOR_COMMAND_H

/* Includes ------------------------------------------------------------------*/
#include "misc.h"

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
u8 vRegulatorCmd(
	void *dev, 	
	const char* CMD, 
	u8 brdAddr, 
	void (*xprintS)(const char* MSG), 
	void (*xprint)(const char* FORMAT_ORG, ...)
);

#endif /* _CSPIN_COMMAND_H */

/******************* (C) COPYRIGHT 2015 INCUBECN *****END OF FILE****/
