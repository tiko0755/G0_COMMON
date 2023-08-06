/******************** (C) COPYRIGHT 2022 INCUBECN *****************************
* File Name          : rampCmd.c
* Author             : Tiko Zhong
* Date First Issued  : 09/21,2022
* Description        : This file provides a set of functions needed to manage the
*                      stepper ramp generator
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "rampCmd.h"
#include "myramp.h"
#include "string.h"
#include "stdio.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define FUN_LEN 64
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
const char RAMP_HELP[] = {
	"ramp command:"
	"\n %brd.%dev.help()"
	"\n %brd.%dev.stops()"
	"\n %brd.%dev.stopi()"
	"\n %brd.%dev.gohome()/(maxspd)"
	"\n %brd.%dev.rotatel(spd)"
	"\n %brd.%dev.rotater(spd)"
	"\n %brd.%dev.moveto(absPos)/(absPos,maxspd)"
	"\n %brd.%dev.moveby(refPos)/(refPos,maxspd)"
	"\n %brd.%dev.homing(maxspd)"
	"\n %brd.%dev.ishoming()"
	"\n %brd.%dev.pos()"
	"\n %brd.%dev.spd()"
	"\n %brd.%dev.info()"
	"\n %brd.%dev.default()"
	"\n %brd.%dev.spd(min,max)/()"
	"\n"
};

/* Private function prototypes -----------------------------------------------*/

/*******************************************************************************
* Function Name  : rampCmd
* Description    : rampCmd function commands
* Input          : - CMD, command line
				 : - brdAddr, board address
				 : - print function
* Output         : None
* Return         : None 
*******************************************************************************/
u8 rampCmd(void *dev, char* CMD, u8 brdAddr, void (*xprint)(const char* FORMAT_ORG, ...)){
	s32 i,j;
	const char* line;
	rampDev_t *d = dev;
	rampRsrc_t *r = &d->rsrc;
		
	if(strncmp(CMD, r->name, strlen(r->name)) != 0)	return 0;
	line = &CMD[strlen(r->name)+1];

	//	.stops()
	if(strncmp(line, "stops", strlen("stops")) == 0){
		d->stopSoft(r);
		xprint("+ok@%d.%s.stops()\r\n", brdAddr, r->name);
	}

	//	.stopi()
	else if(strncmp(line, "stopi", strlen("stopi")) == 0){
		d->stop(r);
		xprint("+ok@%d.%s.stopi()\r\n", brdAddr, r->name);
	}

	else if(sscanf(line, "gohome %d", &i)==1){
		if(d->isRotating(r) == 0){
			d->gohome(r, i);
			xprint("+ok@%d.%s.homing(%d)\r\n", brdAddr, r->name, i);
		}
		else{
			xprint("+ok@%d.%s.gohome(\"busy\")\r\n", brdAddr, r->name);
		}
	}
	else if(strncmp(line, "gohome", strlen("gohome")) == 0){
		if(d->isRotating(r) == 0){
			d->gohome(r, r->spdMax);
			xprint("+ok@%d.%s.gohome()\r\n", brdAddr, r->name);
		}
		else{
			xprint("+ok@%d.%s.gohome(\"busy\")\r\n", brdAddr, r->name);
		}
	}

	//	.rotater(spd)
	else if(sscanf(line, "rotater %d", &i)==1){
		d->rotateR(r,i);
		xprint("+ok@%d.%s.rotater(%d)\r\n", brdAddr, r->name, i);
	}

	//	.moveto(absPos)
	else if(sscanf(line, "moveto %d %d", &i, &j)==2){
		if(d->isRotating(r) == 0){
			r->spdMax = j;
			d->moveTo(r,i);
			xprint("+ok@%d.%s.moveto(%d,%d)\r\n", brdAddr, r->name, i, r->spdMax);
		}
		else{
			xprint("+err@%d.%s.moveto(\"busy\")\r\n", brdAddr, r->name);
		}
	}
	else if(sscanf(line, "moveto %d", &i)==1){
		if(d->isRotating(r) == 0){
			d->moveTo(r,i);
			xprint("+ok@%d.%s.moveto(%d)\r\n", brdAddr, r->name, i);
		}
		else{
			xprint("+err@%d.%s.moveto(\"busy\")\r\n", brdAddr, r->name);
		}
	}

	//	.moveby(refPos)
	else if(sscanf(line, "moveby %d %d", &i,&j)==2){
		if(d->isRotating(r) == 0){
			r->spdMax = j;
			d->moveBy(r,i);
			xprint("+ok@%d.%s.moveby(%d,%d)\r\n", brdAddr, r->name, i, r->spdMax);
		}
		else{
			xprint("+ok@%d.%s.moveby(\"busy\")\r\n", brdAddr, r->name);
		}
	}
	else if(sscanf(line, "moveby %d", &i)==1){
		if(d->isRotating(r) == 0){
			d->moveBy(r,i);
			xprint("+ok@%d.%s.moveby(%d)\r\n", brdAddr, r->name, i);
		}
		else{
			xprint("+ok@%d.%s.moveby(\"busy\")\r\n", brdAddr, r->name);
		}
	}

	//.help()
	else if(strncmp(line, "help", strlen("help")) == 0){
		xprint("%s", RAMP_HELP);
		xprint("+ok@%d.%s.help()\r\n%s", brdAddr, r->name);
	}

	//	.rotatel(spd)
	else if(sscanf(line, "rotatel %d", &i)==1){
		d->rotateL(r,i);
		xprint("+ok@%d.%s.rotatel(%d)\r\n", brdAddr, r->name, i);
	}

	//	.homing(spd)
	else if(sscanf(line, "homing %d", &i)==1){
		d->homing(r,i);
		xprint("+ok@%d.%s.homing()\r\n", brdAddr, r->name, i);
	}

	//	.ishoming()
	else if(strncmp(line, "ishoming", strlen("ishoming")) == 0){
		xprint("+ok@%d.%s.ishoming(%d)\r\n", brdAddr, r->name, r->isHoming);
		r->isHoming = 0;	// will clear this flag
	}

	//	.pos()
	else if(strncmp(line, "pos", strlen("pos")) == 0){
		xprint("+ok@%d.%s.pos(%d)\r\n", brdAddr, r->name, r->posCur);
	}
	//	.spd()
	else if(strncmp(line, "spd", strlen("spd")) == 0){
		xprint("+ok@%d.%s.spd(%d)\r\n", brdAddr, r->name, r->spdCur);
	}
	//	.info()
	else if(strncmp(line, "info", strlen("info")) == 0){
		xprint("posCur:\t%d\n", r->posCur);
		xprint("spdCur:\t%d\n", r->spdCur);
		xprint("status:\t0x%02x\n", r->status);
		xprint("runMod:\t0x%02x\n", r->runMod);
		xprint("ref:\t%d\n", d->getRefl(r));
		xprint("dir:\t%d\n", d->getDir(r));
		xprint("+ok@%d.%s.info()\r\n", brdAddr, r->name);
	}

	//	.default()
	else if(strncmp(line, "default", strlen("default")) == 0){
		r->spdMin = 10000;
		r->spdMax = 64000;
		xprint("+ok@%d.%s.default()\r\n", brdAddr, r->name);
	}

	//	.speed(min,max)
	else if(sscanf(line, "speed %d %d", &i, &j)==2){
		r->spdMin = i;
		r->spdMax = j;
		xprint("+ok@%d.%s.speed(%d,%d)\r\n", brdAddr, r->name, i, j);
	}
	//	.speed()
	else if(strncmp(line, "speed", strlen("speed")) == 0){
		xprint("+ok@%d.%s.speed(%d,%d)\r\n", brdAddr, r->name, r->spdMin, r->spdMax);
	}

	else{
		xprint("+unknown@%s", CMD);
	}
	return 1;
}

/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
