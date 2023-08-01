/******************** (C) COPYRIGHT 2022 INCUBECN *****************************
* File Name          : script_player.c
* Author             : Tiko Zhong
* Date First Issued  : 09/21,2022
* Description        : This file provides a set of functions needed to manage the
*                      stepper ramp generator
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "script_player.h"
#include <string.h>
#include <stdio.h>
#include "board.h"

static s32 app_r[8] = {0};

/* Private typedef -----------------------------------------------------------*/

const reqNRes_t* SCRIPT;

static u8 appDebugEn = 0;

static cb_prm promiseTakeResp = NULL;

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
char appCmdResp[APP_CMD_LEN] = {0};
char appCmdOrg[APP_CMD_LEN] = {0};
/* Private function prototypes -----------------------------------------------*/

/*******************************************************************************
* Function Name  : aplentalQT_periodJob
* Description    : rampCmd function commands
* Input          : - CMD, command line
				 : - brdAddr, board address
				 : - print function
* Output         : None
* Return         : None
*******************************************************************************/
static u8 appPrcSqu = 0;
static u16 appTick = 0;
static s32 appDelay = 0;
static u8 appResend;
static u8 appIndx = 0;
static s32 scriptStart(scriptBind_t* scrptBinding);

s32 scriptStartX(const char* org, const scriptBind_t* SCRIPT_BIND, u16 len){
	s32 i;
	if(strncmp(org, "debug.en", strlen("debug.en")) == 0){
		appDebugEn = 1;
		print("+ok@debug.en\r\n");
		return 0;
	}
	else if(strncmp(org, "debug.dis", strlen("debug.dis")) == 0){
		appDebugEn = 0;
		print("+ok@debug.dis\r\n");
		return 0;
	}
	for(i=0;i<len;i++){
		if(strncmp(org, SCRIPT_BIND[i].NAME, strlen(SCRIPT_BIND[i].NAME)) == 0){
			if(scriptStart(&SCRIPT_BIND[i])<0){
				print("+busy@%s\r\n", SCRIPT_BIND[i].NAME);
			}
			return 0;
		}
	}
	return -1;
}

static s32 scriptStart(scriptBind_t* binding){
	if(appPrcSqu){		return -1;	}
	memset(appCmdOrg,0,APP_CMD_LEN);
	strcpy(appCmdOrg, binding->NAME);
	appPrcSqu = 1;
	appIndx = 0;
	SCRIPT = binding->SCRIPT;
	return 0;
}

static u8 app_isALabel(const char* str){
	s32 len = strlen(str);
	if(len >= 3){
		if((strncmp(str, "$$", 2)==0) && (str[len-1] == ':')){	return 1;	}
	}
	return 0;
}

static s16 script_promise(const char ORG, cb_prm cb){


}

static u8 perTimeoutTick = 0;
void appPrc(u8 tick){
	char str[APP_CMD_LEN] = {0};
	switch(appPrcSqu){
	case 1:
		if(strncmp(SCRIPT[appIndx].req, APP_END, strlen(APP_END)) == 0){
			print("+ok@%s\r\n", appCmdOrg);
			appPrcSqu = 0;
			break;
		}
		else if(app_isALabel(SCRIPT[appIndx].req)){
			appIndx++;
			break;
		}
		else if(sscanf(SCRIPT[appIndx].req, "$$.delay %d", &appDelay) == 1){
			appPrcSqu = 90;
			break;
		}
		else if(sscanf(SCRIPT[appIndx].req, "$$.jmp %s", str) == 1){
			// if(appDebugEn) print("take: %s\n", str);
			// appPrcSqu = 90;
			//appPrcSqu ++;
			appIndx++;
			break;
		}
		appPrcSqu ++;
		appTick = 0;
		//break;

	case 2:
		memset(appCmdResp, 0, APP_CMD_LEN);
		print485("%s\r\n", SCRIPT[appIndx].req);
		appPrcSqu ++;
		perTimeoutTick = 0;
		appResend = 0;
		break;

	case 3:
		appTick += tick;
		perTimeoutTick += tick;
		if(appCmdResp[0] == '+'){
			// jump anyway
			if(strncmp(SCRIPT[appIndx].res, APP_ANY, strlen(APP_ANY)) == 0){
				appIndx ++;
				appPrcSqu = 1;
				break;
			}
			else if(strncmp(SCRIPT[appIndx].res, appCmdResp, strlen(SCRIPT[appIndx].res)) == 0){
				appIndx ++;
				appPrcSqu = 1;
				break;
			}
			else if(strncmp("+err@", appCmdResp, strlen("+err@")) == 0){
				appPrcSqu = 0;
				print("+err@%s\r\n", appCmdOrg);
				break;
			}
			else if(appTick > SCRIPT[appIndx].delay){
				print("+timeout@%s\r\n", appCmdOrg);
				appPrcSqu = 0;
			}
			else if(strncmp(SCRIPT[appIndx].res, "+ok@", strlen("+ok@")) == 0){
				appPrcSqu--;	// retry
				break;
			}
		}
		else if(perTimeoutTick > 90){
			if(appResend < 3){
				appPrcSqu --;
				appResend ++;
			}
			else if(appTick > SCRIPT[appIndx].delay){
				print("+timeout@%s\r\n", appCmdOrg);
				appPrcSqu = 0;
			}
			break;
		}
		break;

	// systemp delay
	case 90:
		appTick += tick;
		if(appTick >= appDelay){
			appIndx ++;
			appPrcSqu = 1;
		}
		break;
	}
}

void appTakeResp(const char* ORG){
	if((appPrcSqu == 0) || (appDebugEn && appPrcSqu))	printS(ORG);	// message begins with '+', it means message is from bus
	strcpy(appCmdResp, ORG);
}

/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
