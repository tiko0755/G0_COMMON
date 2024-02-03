/******************** (C) COPYRIGHT 2015 INCUBECN *****************************
* File Name          : vRegulator_command.c
* Author             : Tiko Zhong
* Date First Issued  : Apr/16/2022
* Description        : This file provides a set of functions needed to manage the
*                      digital DC regulator
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "vRegulator_command.h"
#include "vRegulator.h"
#include "string.h"
#include "stdio.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
const char DCV_HELP[] = {
	"Stepper Task Commands:"
	"\n %brd.help()"
	"\n %brd.mv(%mv)"
	"\n %brd.mv()"
	"\n %brd.on()"
	"\n %brd.off()"
	"\n %brd.cal_uv(%measure_uv)"
	"\n %brd.config(%limit_mv,%poweron_mv,%isPowerOn)"
	"\n %brd.adc()"
	"\n %brd.default()"
	"\n %brd.info()"
	"\r\n"
};

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/* Public function prototypes -----------------------------------------------*/	
u8 vRegulatorCmd(
	void *dev, 	
	const char* CMD, 
	u8 brdAddr, 
	void (*xprintS)(const char* MSG), 
	void (*xprint)(const char* FORMAT_ORG, ...)
){
	const char* FUNC;
	s32 z0,z1,z2;
	dcvDev_t* pDev = dev;
	dcvRsrc_t* pRsrc = &pDev->rsrc;
	
	if(strncmp(CMD, pRsrc->name, strlen(pRsrc->name)) != 0)	return 0;
	FUNC = &CMD[strlen(pRsrc->name)];
	
	//.help()
	if(strncmp(FUNC, ".help", strlen(".help")) == 0){
		xprintS(DCV_HELP);
		xprint("+ok@%d.%s.help()\r\n", brdAddr, pRsrc->name);
		return 1;
	}

	//%.mv(mv)
	else if(sscanf(FUNC, ".mv %d ", &z0) == 1 ){
		pDev->setVolt(pRsrc, z0);
		xprint("+ok@%d.%s.mv(%d)\r\n", brdAddr, pRsrc->name, z0);
		return 1;
	}
	//.get()
	else if(strncmp(FUNC, ".mv ", strlen(".mv ")) == 0){
		z0 = pDev->getVolt(pRsrc);
		xprint("+ok@%d.%s.mv(%d)\r\n", brdAddr, pRsrc->name, z0);
		return 1;
	}

	//.get()
	else if(sscanf(FUNC, ".cal_uv %d", &z0) == 1){
		pDev->callibrate_uv(pRsrc, z0);
		xprint("+ok@%d.%s.cal_uv(%d)\r\n", brdAddr, pRsrc->name, z0);
		return 1;
	}

	//.config()
	else if(sscanf(FUNC, ".config %d %d %d", &z0, &z1, &z2) == 3){
		pDev->config(pRsrc, z0, z1, z2);
		xprint("+ok@%d.%s.config(%d,%d,%d)\r\n", brdAddr, pRsrc->name, z0, z1, z2);
		return 1;
	}

	//.getAdc
	else if(strncmp(FUNC, ".adc ", strlen(".adc ")) == 0){
		z0 = pDev->getADC(pRsrc);
		xprint("+ok@%d.%s.adc(%d)\r\n", brdAddr, pRsrc->name, z0);
		return 1;
	}

	//.default
	else if(strncmp(FUNC, ".default ", strlen(".default ")) == 0){
		pDev->defaultx(pRsrc);
		xprint("+ok@%d.%s.default()\r\n", brdAddr, pRsrc->name);
		return 1;
	}

	//.info
	else if(strncmp(FUNC, ".info ", strlen(".info ")) == 0){
		xprint("+ok@%d.%s.info(%d,%d,%d,%d,%d,%d)\r\n", brdAddr, pRsrc->name,
			pRsrc->calVolt_uv, pRsrc->calDuty, pRsrc->calAdc,
			pRsrc->limit_mv, pRsrc->powerOn_mv, pRsrc->isPowerOn
		);
		return 1;
	}

	//.on
	else if(strncmp(FUNC, ".on", strlen(".on")) == 0){
		z0 = pDev->turnOn(pRsrc);
		if(z0 < 0){
			xprint("+err@%d.%s.on()\r\n", brdAddr, pRsrc->name);
		}
		else{
			xprint("+ok@%d.%s.on(%d)\r\n", brdAddr, pRsrc->name, z0);
		}
		return 1;
	}

	//.off
	else if(strncmp(FUNC, ".off", strlen(".off")) == 0){
		pDev->turnOff(pRsrc);
		xprint("+ok@%d.%s.off()\r\n", brdAddr, pRsrc->name, pRsrc->prvVolt);
		return 1;
	}



	return 0;
}

/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
