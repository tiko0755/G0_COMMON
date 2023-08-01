/******************** (C) COPYRIGHT 2022 INCUBECN *****************************
* File Name          : script_demo.h
* Author             : Tiko Zhong
* Date First Issued  : 09/21,2022
* Description        : This file provides a set of functions needed to manage the
*                      stepper ramp generator
********************************************************************************
* History:e
* 09/21,2022: V0.0
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _SCRIPT_DEMO_H
#define _SCRIPT_DEMO_H

/* Includes ------------------------------------------------------------------*/
#include "misc.h"
/* Exported types ------------------------------------------------------------*/
typedef struct {
	const char* SCRIPT;
	void* id;
}SCRIPT_T;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
#define SCRIPT_COUNT	(4)
extern SCRIPT_T STARTS[];
//extern const char* STARTS[SCRIPT_COUNT];
extern const char SCRIPT_MAIN[];
#endif /* _SCRIPT_DEMO_H */

/******************* (C) COPYRIGHT 2015 INCUBECN *****END OF FILE****/
