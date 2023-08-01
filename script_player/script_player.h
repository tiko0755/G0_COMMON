/******************** (C) COPYRIGHT 2022 INCUBECN *****************************
* File Name          : script_player.h
* Author             : Tiko Zhong
* Date First Issued  : 06/oct,2022
* Description        : This file provides a set of functions needed to manage the
*                      stepper ramp generator
********************************************************************************
* History:e
* 06/oct,2022: V0.0
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _SCRIPT_PLAYER_H
#define _SCRIPT_PLAYER_H

#include "misc.h"

#define APP_ANY			("$$any")
#define APP_START		("$$start:")
#define APP_END			("$$end:")
#define APP_F_DELAY		("$$.delay")

#define APP_CMD_LEN	(36)
/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
typedef void (*cb_prm)(const char*);

typedef struct{
	char req[APP_CMD_LEN];
	u16 delay;
	char res[APP_CMD_LEN];
} reqNRes_t;

#define SCRIPT_NAME_LEN	36
typedef struct{
	const char NAME[SCRIPT_NAME_LEN];
	const reqNRes_t* SCRIPT;
} scriptBind_t;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */


s32 scriptStartX(const char* org, const scriptBind_t*, u16);
void appPrc(u8 tick);
void appTakeResp(const char*);

#endif /* _APLENTAL_QT_H */

/******************* (C) COPYRIGHT 2015 INCUBECN *****END OF FILE****/
