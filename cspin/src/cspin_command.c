/******************** (C) COPYRIGHT 2015 INCUBECN *****************************
* File Name          : cspin_command.c
* Author             : Tiko Zhong
* Date First Issued  : 12/01/2015
* Description        : This file provides a set of functions needed to manage the
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "cspin_command.h"
#include "string.h"
#include "stdio.h"
#include "cspin.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

#define L6480_DEF_CNT 8
CMD_DEF_T L6480_CMD_DEF[L6480_DEF_CNT] = {
	{"fwd", "1"},
	{"rev", "0"},
	{"rst", "0"},
	{"cpy", "8"},
	{"bl", "2"},
	{"rh", "4"},
	{"gh", "5"},
	{"bh", "6"},
};

const char L6480_HELP[] = {
	"Stepper Task Commands:"
	"\n x.help()"
//	"\n x.rename(\"newName\")"
	"\n x.regSave()"
	"\n x.status()"
	"\n x.getSpd()"
	"\n x.getAdc()"
	"\n x.run(dir,speed)"
	"\n x.move(dir,microStep)"
	
	"\n x.goto(absPos)"
	"\n x.goto(fwd/rev,absPos)"

	"\n x.goUntil(cpy/rst,fwd/rev,speed)"
	"\n x.releaseSw(cpy/rst,fwd/rev)"
	"\n x.goHome()"
	"\n x.goMark()"
	
	
	"\n x.resetPos()"
	"\n x.stop(%d)"
	//0:fwd/1:rev,step/s
	
	"\n register set/get:"
	"\n x.setAbsPos(%d)"
	"\n x.setMarkPos(%d)"
	"\n x.setAcc(%d)"
	"\n x.setDec(%d)"
	"\n x.setMaxSpd(%d)"
	"\n x.setMinSpd(%d)"
	"\n x.SetLSpdOpt(%d)"
	"\n x.setFsSpd(%d)"
	"\n x.setBoostMode(%d)"
	"\n x.setAccDuty(u8)"
	"\n x.setDecDuty(u8)"
	"\n x.setRunDuty(u8)"
	"\n x.setHoldDuty(u8)"
	"\n x.setIntSpd(%val)"
	"\n x.setStartSlope(%val)"
	"\n x.setAccFinalSlope(%val)"
	"\n x.setDecFinalSlope(%val)"
	"\n x.SetKTherm(%d)"
	"\n x.SetOcdTh(%hex)"
	"\n x.SetStallTh(%hex)"
	"\n x.SetStepSel(%hex)"
	"\n x.SetSyncSel(%hex)"
	"\n x.SetSyncEn(%hex)"
#if(USING_L6480)
	"\n x.SetGateTcc"
	"\n x.SetGateI"
	"\n x.SetGateTboost"
	"\n x.SetGateTDT()"
	"\n x.SetGateTBlank"
#endif	
	"\n x.SetSwMode"
	"\n x.SetVsCompEn"
	"\n x.SetOCSDEn"
	"\n x.SetUVLOVal"
	"\n x.SetVccVal"
	"\n x.SetPwmFrq"
	"\n"

//	"\n x.getMarkPos()"
//	"\n x.getAcc()"
//	"\n x.getDec()"
//	"\n x.getMaxSpd()"
//	"\n x.getMinSpd()"
//	"\n x.GetLSpdOpt()"
//	"\n x.getFsSpd()"
//	"\n x.getBoostMode()"
//	"\n x.getAccDuty()"
//	"\n x.getDecDuty()"
//	"\n x.getRunDuty()"
//	"\n x.getHoldDuty()"
//	"\n x.getIntSpd()"
//	"\n x.getstartslope()"
//	"\n x.getAccFinalSlope()"
//	"\n x.getDecFinalSlope()"
//	"\n x.GetKTherm()"
//	"\n x.GetOcdTh()"
//	"\n x.GetStallTh()"
//	"\n x.GetStepSel()"
//	"\n x.GetSyncSel()"
//	"\n x.GetSyncSelEn()"
//	"\n x.GetGateTcc"
//	"\n x.GetGateI"
//	"\n x.GetGateTBoost"
//	"\n x.GetGateTDT"
//	"\n x.GetGateTBlank"
//	"\n x.GetSwMode"
//	"\n x.GetVsCompEn"
//	"\n x.GetOCSDEn"
//	"\n x.GetUVLOVal"
//	"\n x.GetVccVal"
//	"\n x.GetPwmFrq"
};

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/* Public function prototypes -----------------------------------------------*/	
u8 l6480Cmd(
	void *dev, 	
	const char* CMD, 
	u8 brdAddr, 
	void (*xprintS)(const char* MSG), 
	void (*xprint)(const char* FORMAT_ORG, ...)
){
	const char* FUNC;
	u8 x0,x1,x2,x3;
	u16 y0;		
	u32 z0,z1,z2;
	s32 num0,num1,num2,num3,num4,num5;
	u8 tmp8;
	cSPIN_DEV_T* pDev = dev;
	cSPIN_Rsrc_T* pRsrc = &pDev->rsrc;
	
	if(strncmp(CMD, pRsrc->name, strlen(pRsrc->name)) != 0)	return 0;
	FUNC = &CMD[strlen(pRsrc->name)];
	
	//%devName.help()
	if(strncmp(FUNC, ".help", strlen(".help")) == 0){
		xprintS(L6480_HELP);
		xprint("+ok@%d.%s.help()\r\n", brdAddr, pRsrc->name);
		return 1;
	}
	//%devName.regSave()
	else if(strncmp(FUNC, ".regsave  ", strlen(".regsave  ")) == 0){
		if(pDev->SaveToROM(pRsrc)<0)
			xprint("+err@%d.%s.regsave()\r\n", brdAddr, pRsrc->name);
		else	xprint("+ok@%d.%s.regsave()\r\n", brdAddr, pRsrc->name);
		return 1;
	}
	
	//%dev.status()
	else if(strncmp(FUNC, ".status  ", strlen(".status  "))==0){
		y0 = pDev->Get_Status(&pDev->rsrc);
		xprint("+ok@%d.%s.status(0x%x)\r\n", brdAddr, pRsrc->name,y0);
		return 1;
	}
	//%dev.readreg(regAddr)		return: dev.readreg(regAddr, regVal)
	else if(sscanf(FUNC, ".readreg %d", &z0)==1){
		z1 = pDev->Get_Param(&pDev->rsrc, (cSPIN_Regs_T)z0);
		xprint("+ok@%d.%s.readreg(0x%x, 0x%x)\r\n", brdAddr, pRsrc->name, z0, z1);
		return 1;
	}
	//%dev.writereg(regAddr, regVal)
	else if(sscanf(FUNC, ".writereg %d %d", &z0, &z1)==2){
		pDev->Set_Param(&pDev->rsrc, (cSPIN_Regs_T)z0, z1);
		xprint("+ok@%d.%s.writereg(%d,%d)\r\n", brdAddr, pRsrc->name, z0, z1);
		return 1;
	}
	//%dev.runtimeout(direction,speed,time_s)
	else if(sscanf(FUNC, ".runtimeout %d %d %d ", &z0, &z1, &z2)==3 ){
		pDev->RunTimeout(&pDev->rsrc, (cSPIN_Direction_T)z0, z1, z2);
		xprint("+ok@%d.%s.runtimeout(%d,%d,%d)\r\n", brdAddr, pRsrc->name, z0,z1,z2);
		return 1;
	}
	//%dev.runtimeout(direction,speed)
	else if(sscanf(FUNC, ".run %d %d ", &z0, &z1)==2){
		pDev->Run(&pDev->rsrc, (cSPIN_Direction_T)z0, z1);
		xprint("+ok@%d.%s.run(%d,%d)\r\n", brdAddr, pRsrc->name, z0, z1);
		return 1;
	}
	//%dev.move(direction,step)
	else if(sscanf(FUNC, ".move %d %d ", &z0, &z1)==2){
		pDev->Move_Step(&pDev->rsrc, (cSPIN_Direction_T)z0, z1);
		xprint("+ok@%d.%s.move(%d,%d)\r\n", brdAddr, pRsrc->name, z0,z1);
		return 1;
	}

	/***********************************************************
	"\n %name.goHome()
	@
		This command can be given anytime and is immediately executed.
	************************************************************/
	else if(strncmp(FUNC, ".gohome  ", strlen(".gohome  ")) == 0){
		pDev->Go_Home(pRsrc);
		xprint("+ok@%d.%s.gohome()\r\n", brdAddr, pRsrc->name);
		return 1;
	}

	/***********************************************************
	"\n %name.goMark()
	@
		This command can be given anytime and is immediately executed.
	************************************************************/
	else if(strncmp(FUNC, ".gomark  ", strlen(".gomark  ")) == 0){
		pDev->Go_Mark(pRsrc);
		xprint("+ok@%d.%s.gomark()\r\n", brdAddr, pRsrc->name);
		return 1;
	}

	/***********************************************************
	"\n %name.goto(fwd/rev,absPos)
	@ spd	: steps/s,
		This command can be given anytime and is immediately executed.
	************************************************************/
	else if(sscanf(FUNC, ".goto %d %d", &z0, &z1)==2){
		pDev->Go_To_Dir(pRsrc,(cSPIN_Direction_T)z0,z1);
		xprint("+ok@%d.%s.goto(%d)\r\n", brdAddr, pRsrc->name, z0, z1);
		return 1;
	}
//
//	/***********************************************************
//	"\n %name.goto(%absPos)
//	@ absPos	:
//		This command can be given anytime and is immediately executed.
//	************************************************************/
//	else if(sscanf(FUNC, ".goto %d ", &z0)==1){
//		pDev->Go_To(pRsrc,z0);
//		xprint("+ok@%d.%s.goto(%d)\r\n", brdAddr, pRsrc->name, z0);
//		return 1;
//	}
//
//	/***********************************************************
//	"\n %name.goUntil(cpy/rst,fwd/rev,speed)
//	@ spd	: steps/s,
//		This command can be given anytime and is immediately executed.
//	************************************************************/
//	else if(sscanf(FUNC, ".gountil %d %d %d", &z0,&z1,&z2)==3){
//		pDev->Go_Until(pRsrc, (cSPIN_Action_T)z0, (cSPIN_Direction_T)z1, Speed_Steps_to_Par(z2));
//		xprint("+ok@%d.%s.gountil(%d,%d,%d)\r\n", brdAddr, pRsrc->name, z0,z1,z2);
//		return 1;
//	}
//
//	/***********************************************************
//	"\n %name.releaseSw(cpy/rst,fwd/rev)
//	@
//		This command can be given anytime and is immediately executed.
//	************************************************************/
//	else if(sscanf(FUNC, ".releasesw %d %d", &z0,&z1)==2){
//		pDev->Release_SW(pRsrc, (cSPIN_Action_T)z0, (cSPIN_Direction_T)z1);
//		xprint("+ok@%d.%s.releasesw(%d,%d)\r\n", brdAddr, pRsrc->name, z0,z1);
//		return 1;
//	}
//
	//%dev.stop(mod)
	else if(sscanf(FUNC, ".stop %d ", &z0)==1){
		switch(z0){
			case 0:	//softStop, without hold, mosfet in hiz
				pDev->Soft_HiZ(&pDev->rsrc);
				xprint("+ok@%d.%s.stop(\"soft_hiz\")\r\n", brdAddr, pRsrc->name);
				return 1;
			case 1:	//softStop, with hold
				pDev->Soft_Stop(&pDev->rsrc);
				xprint("+ok@%d.%s.stop(\"soft_hold\")\r\n", brdAddr, pRsrc->name);
				return 1;
			case 2:	//hardStop, without hold, mosfet in hiz
				pDev->Hard_HiZ(&pDev->rsrc);
				xprint("+ok@%d.%s.stop(\"hard_hiz\")\r\n", brdAddr, pRsrc->name);
				return 1;
			case 3:	//hardStop, with hold
				pDev->Hard_Stop(&pDev->rsrc);
				xprint("+ok@%d.%s.stop(\"hard_hold\")\r\n", brdAddr, pRsrc->name);
				return 1;
			default:	//softStop, with hold
				pDev->Soft_Stop(&pDev->rsrc);
				xprint("+ok@%d.%s.stop(\"soft_hold\")\r\n", brdAddr, pRsrc->name);
				return 1;
		}
	}

	//%dev.reset()
	else if(strncmp(FUNC, ".reset  ", strlen(".reset  "))==0){
		pDev->ResetDev(&pDev->rsrc);
		xprint("+ok@%d.%s.reset()\r\n", brdAddr, pRsrc->name);
		return 1;
	}
	//%dev.default()
	else if(strncmp(FUNC, ".default  ", strlen(".default  "))==0){
		pDev->Default(&pDev->rsrc);
		xprint("+ok@%d.%s.default()\r\n", brdAddr, pRsrc->name);
		return 1;
	}
	//%dev.printall()
	else if(strncmp(FUNC, ".printall  ", strlen(".printall  "))==0){
		pDev->PrintRegs(&pDev->rsrc);
		xprint("+ok@%d.%s.printall()\r\n", brdAddr, pRsrc->name);
		return 1;
	}

//	/***********************************************************
//	@	_set_param() Group
//	************************************************************/
//	/***********************************************************
//	@	dev.setAbsPos(pos), writable only stoped
//	@ val:[-power(2,21),power(2,21)-1]
//	************************************************************/
//	else if(sscanf(FUNC, ".setabspos %d ", &num0)==1){
//		if(pDev->RegSetAbsPos(&pDev->rsrc, (u32)num0))
//			xprint("+err@%d.%s.setabspos('busy')\r\n", brdAddr, pRsrc->name);
//		else	xprint("+ok@%d.%s.setabspos(%d)\r\n", brdAddr, pRsrc->name, (u32)num0);
//		return 1;
//	}
//	else if(strncmp(FUNC, ".getabspos  ", strlen(".getabspos  "))==0){
//		pDev->RegGetAbsPos(&pDev->rsrc, &num0);
//		xprint("+ok@%d.%s.getabspos(%d)\r\n", brdAddr, pRsrc->name, num0);
//		return 1;
//	}
//
//	/***********************************************************
//	@	dev.setElPos(pos), writable only stoped
//	@ val:9Bit
//	************************************************************/
//	else if(sscanf(FUNC, ".setelpos %d ", &num0)==1){
//		if(pDev->RegSetElPos(&pDev->rsrc, (u16)num0))
//			xprint("+err@%d.%s.setelpos('busy')\r\n", brdAddr, pRsrc->name);
//		else	xprint("+ok@%d.%s.setelpos(%d)\r\n", brdAddr, pRsrc->name, (u16)num0);
//		return 1;
//	}
//	else if(strncmp(FUNC, ".getelpos  ", strlen(".getelpos  "))==0){
//		if(pDev->RegGetElPos(&pDev->rsrc, &y0))
//			xprint("+err@%d.%s.getelpos('busy')\r\n", brdAddr, pRsrc->name);
//		else xprint("+ok@%d.s.getelpos(%d)\r\n", brdAddr, pRsrc->name, y0);
//		return 1;
//	}
//
//	/***********************************************************
//	"\n %name.setMarkPos(%val), always writable
//	@ val:[-power(2,21),power(2,21)-1]
//	************************************************************/
//	else if(sscanf(FUNC, ".setmarkpos %d ", &num0)==1){
//		if(pDev->RegSetMarkPos(&pDev->rsrc, (u32)num0))
//			xprint("+err@%d.%s.setmarkpos('busy')\r\n", brdAddr, pRsrc->name);
//		else xprint("+ok@%d.%s.setmarkpos(%d)\r\n", brdAddr, pRsrc->name, (u32)num0);
//		return 1;
//	}
//	else if(strncmp(FUNC, ".getmarkpos  ", strlen(".getmarkpos  "))==0){
//		if(pDev->RegGetMarkPos(&pDev->rsrc, &z0))
//			xprint("+err@%d.%s.getmarkpos('busy')\r\n", brdAddr, pRsrc->name);
//		else	xprint("+ok@%d.%s.getmarkpos(%d)\r\n", brdAddr, pRsrc->name, z0);
//		return 1;
//	}
//
	/***********************************************************
	"\n %name.getcurSpd()
	@ val:[0..15625step/s], 1LSB = 0.015 step/s
	************************************************************/
//	else if(strncmp(FUNC, ".getcurspd  ", strlen(".getcurspd  "))==0){
//		if(pDev->RegGetSpeed(&pDev->rsrc, &z0))
//			xprint("+err@%d.%s.getcurspd('busy')\r\n", brdAddr, pRsrc->name);
//		else	xprint(  "+ok@%d.%s.getcurspd(%d)\r\n", brdAddr, pRsrc->name, z0);
//		return 1;
//	}


/***********************************************************
desc: set speed profit
req	: %dev.speed(%minSpd, %maxSpd, %acc, %dec, %fulStp, %stp)
res	: +ok@%dev.speed(%minSpd, %maxSpd, %acc, %dec, %fulStp, %stp)
************************************************************/
else if(sscanf(FUNC, ".speed %d %d %d %d %d %d ", &num0, &num1, &num2, &num3, &num4, &num5)==6){
	pDev->RegSetMinSpd_stepPerS(&pDev->rsrc, num0);
	pDev->RegSetMaxSpd_stepPerS(&pDev->rsrc, num1);
	pDev->RegSetAcc_stepPerSS(&pDev->rsrc, num2);
	pDev->RegSetDec_stepPerSS(&pDev->rsrc, num3);
	pDev->RegSetFSSpd_stepPerS(&pDev->rsrc, num4);
	pDev->RegSetStepSel(&pDev->rsrc, num5);
	xprint(  "+ok@%d.%s.speed(%d,%d,%d,%d,%d,%d)\r\n", brdAddr, pRsrc->name, num0,num1,num2,num3,num4,num5);
	return 1;
}

/***********************************************************
desc: get speed profit
req	: %dev.speed()
res	: +ok@%dev.speed(%minSpd, %maxSpd, %acc, %dec, %fulStp, %stp)
************************************************************/
else if(strncmp(FUNC, ".speed  ", strlen(".speed  "))==0){
	num0 = 0;
	num1 = 0;
	num2 = 0;
	num3 = 0;
	num4 = 0;
	num5 = 0;
	pDev->RegGetMinSpd_stepPerS(&pDev->rsrc, &num0);
	pDev->RegGetMaxSpd_stepPerS(&pDev->rsrc, &num1);
	pDev->RegGetAcc_stepPerSS(&pDev->rsrc, &num2);
	pDev->RegGetDec_stepPerSS(&pDev->rsrc, &num3);
	pDev->RegGetFSSpd_stepPerS(&pDev->rsrc, &num4);
	pDev->RegGetStepSel(&pDev->rsrc, &num5);
	xprint(  "+ok@%d.%s.speed(%d,%d,%d,%d,%d,%d)\r\n", brdAddr, pRsrc->name, num0,num1,num2,num3,num4,num5);
	return 1;
}

/***********************************************************
desc: set speed profit
req	: %dev.force(%hold, %run, %acc, %dec)
res	: +ok@%dev.force(%hold, %run, %acc, %dec)
************************************************************/
else if(sscanf(FUNC, ".force %d %d %d %d", &num0, &num1, &num2, &num3)==4){
	pDev->RegSetKVal(&pDev->rsrc, num0, num1, num2, num3);
	xprint(  "+ok@%d.%s.force(%d,%d,%d,%d)\r\n", brdAddr, pRsrc->name, num0,num1,num2,num3);
	return 1;
}

/***********************************************************
desc: get force profit
req	: %dev.force()
res	: +ok@%dev.speed(%minSpd, %maxSpd, %acc, %dec, %fulStp, %stp)
************************************************************/
else if(strncmp(FUNC, ".force  ", strlen(".force  "))==0){
	num0 = 0;
	num1 = 0;
	num2 = 0;
	num3 = 0;
	pDev->RegGetKVal(&pDev->rsrc, &num0, &num1, &num2, &num3);
	xprint(  "+ok@%d.%s.force(%d,%d,%d,%d)\r\n", brdAddr, pRsrc->name, num0,num1,num2,num3);
	return 1;
}

//	/***********************************************************
//	"\n %name.setAcc(%val), write when stop
//	@ val:[14.55..59590 step/s2], 1LSB = 14.55 step/s2
//	************************************************************/
//	else if(sscanf(FUNC, ".setacc %d ", &num0)==1){
//		if(pDev->RegSetAcc(&pDev->rsrc, (u16)num0))
//			xprint("+err@%d.%s.setacc('busy')\r\n", brdAddr, pRsrc->name);
//		else	xprint(  "+ok@%d.%s.setacc(%d)\r\n", brdAddr, pRsrc->name, (u16)num0);
//		return 1;
//	}
//	else if(strncmp(FUNC, ".getacc  ", strlen(".getacc  "))==0){
//		if(pDev->RegGetAcc(&pDev->rsrc, &y0))
//			xprint("+err@%d.%s.getacc('busy')\r\n", brdAddr, pRsrc->name);
//		else	xprint(  "+ok@%d.%s.getacc(%d)\r\n", brdAddr, pRsrc->name, y0);
//		return 1;
//	}
//
//	/***********************************************************
//	"\n %name.setDec(%val), write when stop
//	@ val:[14.55..59590 step/s2], 1LSB = 14.55 step/s2
//	************************************************************/
//	else if(sscanf(FUNC, ".setdec %d ", &num0)==1){
//		if(pDev->RegSetDec(&pDev->rsrc, (u16)num0))
//			xprint("+err@%d.%s.setdec('busy')\r\n", brdAddr, pRsrc->name);
//		else	xprint(  "+ok@%d.%s.setdec(%d)\r\n", brdAddr, pRsrc->name, (u16)num0);
//		return 1;
//	}
//	else if(strncmp(FUNC, ".getdec  ", strlen(".getdec  "))==0){
//		if(pDev->RegGetDec(&pDev->rsrc, &y0))
//			xprint("+err@%d.%s.getdec('busy')\r\n", brdAddr, pRsrc->name);
//		else	xprint(  "+ok@%d.%s.getdec(%d)\r\n", brdAddr, pRsrc->name, y0);
//		return 1;
//	}
//
//	/***********************************************************
//	"\n %name.setMaxSpd(%speed), always writeable
//	@ val:[15.25..15610 step/s], 1LSB = 15.25 step/s
//	************************************************************/
//	else if(sscanf(FUNC, ".setmaxspd %d ", &num0)==1){
//		if(pDev->RegSetMaxSpd(&pDev->rsrc, (u16)num0))
//			xprint("+err@%d.%s.setmaxspd('busy')\r\n", brdAddr, pRsrc->name);
//		else	xprint(  "+ok@%d.%s.setmaxspd(%d)\r\n", brdAddr, pRsrc->name, (u16)num0);
//		return 1;
//	}
//	else if(strncmp(FUNC, ".getmaxspd  ", strlen(".getmaxspd  "))==0){
//		if(pDev->RegGetMaxSpd(&pDev->rsrc, &y0))
//			xprint("+err@%d.%s.getmaxspd('busy')\r\n", brdAddr, pRsrc->name);
//		else	xprint(  "+ok@%d.%s.getmaxspd(%d)\r\n", brdAddr, pRsrc->name, y0);
//		return 1;
//	}
//
//	/***********************************************************
//	"\n %name.setMinSpd(%speed), writable only stoped
//	@ val:[0..976.3 step/s], 1LSB = 0.238 step/s
//	************************************************************/
//	else if(sscanf(FUNC, ".setminspd %d ", &num0)==1){
//		if(pDev->RegSetMinSpd(&pDev->rsrc, num0&BIT_LEN(12)))
//			xprint("+err@%d.%s.setminspd('busy')\r\n", brdAddr, pRsrc->name);
//		else	xprint(  "+ok@%d.%s.setminspd(%d)\r\n", brdAddr, pRsrc->name, num0);
//		return 1;
//	}
//	else if(strncmp(FUNC, ".getminspd  ", strlen(".getminspd  "))==0){
//		if(pDev->RegGetMinSpd(&pDev->rsrc, &y0))
//			xprint(  "+err@%d.%s.getminspd('busy')\r\n", brdAddr, pRsrc->name);
//		else xprint(  "+ok@%d.%s.getminspd(%d)\r\n", brdAddr, pRsrc->name, y0);
//		return 1;
//	}
//
//	/***********************************************************
//	"\n %name.SetLSpdOpt(%b), writable only stoped
//	@ val:in
//	************************************************************/
//	else if(sscanf(FUNC, ".setlspdopt %d ", &num0)==1){
//		if(pDev->RegSetLowSpdOpt(&pDev->rsrc, num0))
//			xprint(  "+err@%d.%s.setlspdopt('busy')\r\n", brdAddr, pRsrc->name);
//		else	xprint(  "+ok@%d.%s.setlspdopt(%d)\r\n", brdAddr, pRsrc->name, num0&0x01);
//		return 1;
//	}
//	else if(strncmp(FUNC, ".getlspdopt  ", strlen(".getlspdopt  "))==0){
//		if(pDev->RegGetLowSpdOpt(&pDev->rsrc, &x0))
//			xprint("+err@%d.%s.getlspdopt('busy')\r\n", brdAddr, pRsrc->name);
//		else	xprint(  "+ok@%d.%s.getlspdopt(%d)\r\n", brdAddr, pRsrc->name, x0);
//		return 1;
//	}
//
//	/***********************************************************
//	"\n %name.setFsSpd(%val), always writeable
//	@ val:i7.63..15625, 1LSB = 15.25step/S
//	************************************************************/
//	else if(sscanf(FUNC, ".setfsspd %d ", &num0)==1){
//		if(pDev->RegSetFSSpd(&pDev->rsrc, num0))
//			xprint("+err@%d.%s.setfsspd('busy')\r\n", brdAddr, pRsrc->name);
//		else	xprint(  "+ok@%d.%s.setfsspd(%d)\r\n", brdAddr, pRsrc->name, (u16)num0);
//		return 1;
//	}
//	else if(strncmp(FUNC, ".getfsspd  ", strlen(".getfsspd  "))==0){
//		if(pDev->RegGetFSSpd(&pDev->rsrc, &y0))
//			xprint("+err@%d.%s.getfsspd('busy')\r\n", brdAddr, pRsrc->name);
//		else	xprint(  "+ok@%d.%s.getfsspd(%d)\r\n", brdAddr, pRsrc->name, y0);
//		return 1;
//	}
//
//	/***********************************************************
//	"\n %name.setBoostMode(%val)
//	@ val:1:enable
//	************************************************************/
//	else if(sscanf(FUNC, ".setboostmode %d ", &num0)==1){
//		if(pDev->RegSetBoostMode(&pDev->rsrc, num0))
//			xprint("+err@%d.%s.setboostmode('busy')\r\n", brdAddr, pRsrc->name);
//		else	xprint(  "+ok@%d.%s.setboostmode(%d)\r\n", brdAddr, pRsrc->name, num0&0x01);
//		return 1;
//	}
//	else if(strncmp(FUNC, ".getboostmode  ", strlen(".getboostmode  "))==0){
//		if(pDev->RegGetBoostMode(&pDev->rsrc, &x0))
//			xprint("+err@%d.%s.getboostmode('busy')\r\n", brdAddr, pRsrc->name);
//		else	xprint(  "+ok@%d.%s.getboostmode(%d)\r\n", brdAddr, pRsrc->name, x0);
//		return 1;
//	}
//
//	//set drive current, always writeable
//	//%dev.setkval(accK,runK,decK,holdK)
//	else if(sscanf(FUNC, ".setkval %d %d %d %d ", &num0,&num1,&num2,&num3)==4){
//		tmp8 = 0;
//		if(pDev->RegSetKValAcc(&pDev->rsrc, (u8)num0))	tmp8 |= BIT(0);
//		if(pDev->RegSetKValRun(&pDev->rsrc, (u8)num1))	tmp8 |= BIT(1);
//		if(pDev->RegSetKValDec(&pDev->rsrc, (u8)num2))	tmp8 |= BIT(2);
//		if(pDev->RegSetKValHold(&pDev->rsrc, (u8)num3))	tmp8 |= BIT(3);
//		if(tmp8)	xprint("+err@%d.%s.setkval(0x%x)\r\n", brdAddr, pRsrc->name,tmp8);
//		else	xprint(  "+ok@%d.%s.setkval(acc#%d,run#%d,dec#%d,hold#%d)\r\n", brdAddr, pRsrc->name, (u8)num0,(u8)num1,(u8)num2,(u8)num3);
//		return 1;
//	}
//	else if(strncmp(FUNC, ".getkval  ", strlen(".getkval  "))==0){
//		tmp8 = 0;
//		if(pDev->RegGetKValAcc(&pDev->rsrc, &x0))	tmp8 |= BIT(0);
//		if(pDev->RegGetKValRun(&pDev->rsrc, &x1))	tmp8 |= BIT(1);
//		if(pDev->RegGetKValDec(&pDev->rsrc, &x2))	tmp8 |= BIT(2);
//		if(pDev->RegGetKValHold(&pDev->rsrc, &x3))	tmp8 |= BIT(3);
//		if(tmp8	)	xprint("+err@%d.%s.getkval(0x%x)\r\n", brdAddr, pRsrc->name,tmp8);
//		else xprint("+ok@%d.%s.getkval(acc#%d,run#%d,dec#%d,hold#%d)\r\n", brdAddr, pRsrc->name, x0,x1,x2,x3);
//		return 1;
//	}
//
//	/***********************************************************
//	"\n %name.setIntSpd(%val), writable when high impendance
//	@ val:0..976.5 step/s, 1LSB = 0.0596 step/s
//	************************************************************/
//	else if(sscanf(FUNC, ".setintspd %d ", &num0)==1){
//		if(pDev->RegSetIntSpd(&pDev->rsrc, num0))
//			xprint("+err@%d.%s.setintspd('busy')\r\n", brdAddr, pRsrc->name);
//		else	xprint(  "+ok@%d.%s.setintspd(%d)\r\n", brdAddr, pRsrc->name, num0);
//		return 1;
//	}
//	else if(strncmp(FUNC, ".getintspd  ", strlen(".getintspd  "))==0){
//		if(pDev->RegGetIntSpd(&pDev->rsrc, &y0))
//			xprint("+err@%d.%s.getintspd('busy')\r\n", brdAddr, pRsrc->name);
//		else	xprint("+ok@%d.%s.getintspd(%d)\r\n", brdAddr, pRsrc->name, y0);
//		return 1;
//	}
//
//	/***********************************************************
//	"\n %name.setStartSlope(%val), writable when high impendance
//	@ val:0.. 0.004 s/step, 1LSB = 0.000015
//	************************************************************/
//	else if(sscanf(FUNC, ".setstartslope %d ", &num0)==1){
//		if(pDev->RegSetStartSlp(&pDev->rsrc, num0))
//			xprint("+err@%d.%s.setstartslope('busy')\r\n", brdAddr, pRsrc->name);
//		else	xprint(  "+ok@%d.%s.setstartslope(%d)\r\n", brdAddr, pRsrc->name, num0);
//		return 1;
//	}
//	else if(strncmp(FUNC, ".getstartslope  ", strlen(".getstartslope  "))==0){
//		if(pDev->RegGetStartSlp(&pDev->rsrc, &x0))
//			xprint("+err@%d.%s.getstartslope('busy')\r\n", brdAddr, pRsrc->name);
//		else	xprint("+ok@%d.%s.getstartslope(%d)\r\n", brdAddr, pRsrc->name, x0);
//		return 1;
//	}
//
//	/***********************************************************
//	"\n %name.setAccFinalSlope(%val), writable when high impendance
//	@ val:0.. 0.004 s/step, 1LSB = 0.000015
//	************************************************************/
//	else if(sscanf(FUNC, ".setaccfinalslope %d ", &num0)==1){
//		if(pDev->RegSetFnSlpAcc(&pDev->rsrc, num0))
//			xprint("+err@%d.%s.setaccfinalslope('busy')\r\n", brdAddr, pRsrc->name);
//		else	xprint(  "+ok@%d.%s.setaccfinalslope(%d)\r\n", brdAddr, pRsrc->name, num0);
//		return 1;
//	}
//	else if(strncmp(FUNC, ".getaccfinalslope  ", strlen(".getaccfinalslope  "))==0){
//		if(pDev->RegGetFnSlpAcc(&pDev->rsrc, &x0))
//			xprint("+err@%d.%s.getaccfinalslope('busy')\r\n", brdAddr, pRsrc->name);
//		else	xprint("+ok@%d.%s.getaccfinalslope(%d)\r\n", brdAddr, pRsrc->name, x0);
//		return 1;
//	}
//
//	/***********************************************************
//	"\n %name.setDecFinalSlope(%val), writable when high impendance
//	@ val:0.. 0.004 s/step, 1LSB = 0.000015
//	************************************************************/
//	else if(sscanf(FUNC, ".setdecfinalslope %d ", &num0)==1){
//		if(pDev->RegSetFnSlpDec(&pDev->rsrc, num0))
//			xprint("+err@%d.%s.setdecfinalslope('busy')\r\n", brdAddr, pRsrc->name);
//		else	xprint(  "+ok@%d.%s.setdecfinalslope(%d)\r\n", brdAddr, pRsrc->name, num0);
//		return 1;
//	}
//	else if(strncmp(FUNC, ".getdecfinalslope  ", strlen(".getdecfinalslope  "))==0){
//		if(pDev->RegGetFnSlpDec(&pDev->rsrc, &x0))
//			xprint("+err@%d.%s.getdecfinalslope('busy')\r\n", brdAddr, pRsrc->name);
//		else	xprint("+ok@%d.%s.getdecfinalslope(%d)\r\n", brdAddr, pRsrc->name, x0);
//		return 1;
//	}
//
//	/***********************************************************
//	"\n %name.SetKTherm(%d), always writable
//	@ val:1..1.46875, 1LSB = 0.03125
//	************************************************************/
//	else if(sscanf(FUNC, ".setktherm %d ", &num0)==1){
//		if(pDev->RegSetKTherm(&pDev->rsrc, num0))
//			xprint("+err@%d.%s.setktherm('busy')\r\n", brdAddr, pRsrc->name);
//		else	xprint(  "+ok@%d.%s.setktherm(%d)\r\n", brdAddr, pRsrc->name, num0);
//		return 1;
//	}
//	else if(strncmp(FUNC, ".getktherm  ", strlen(".getktherm  "))==0){
//		if(pDev->RegGetKTherm(&pDev->rsrc, &x0))
//			xprint("+err@%d.%s.getktherm('busy')\r\n", brdAddr, pRsrc->name);
//		else	xprint("+ok@%d.%s.getktherm(%d)\r\n", brdAddr, pRsrc->name, x0);
//		return 1;
//	}
//
//	/***********************************************************
//	"\n %name.getAdc(%d)
//	@ val:
//	************************************************************/
//	else if(strncmp(FUNC, ".getadc  ", strlen(".getadc  "))==0){
//		if(pDev->RegGetAdcOut(&pDev->rsrc, &x0))
//			xprint("+err@%d.%s.getadc('busy')\r\n", brdAddr, pRsrc->name);
//		else	xprint("+ok@%d.%s.getadc(%d)\r\n", brdAddr, pRsrc->name, x0);
//		return 1;
//	}
//
//	/***********************************************************
//	"\n %name.SetOcdTh(%d), always writable
//	@ val:31.25..1000mV, 1LSB = 31.25mV
//	************************************************************/
//	else if(sscanf(FUNC, ".setocdth %d ", &num0)==1){
//		if(pDev->RegSetOCDTh(&pDev->rsrc, num0))
//			xprint("+err@%d.%s.setocdth('busy')\r\n", brdAddr, pRsrc->name);
//		else	xprint(  "+ok@%d.%s.setocdth(%d)\r\n", brdAddr, pRsrc->name, num0);
//		return 1;
//	}
//	else if(strncmp(FUNC, ".getocdth  ", strlen(".getocdth  "))==0){
//		if(pDev->RegGetOCDTh(&pDev->rsrc, &x0))
//			xprint("+err@%d.%s.getocdth('busy')\r\n", brdAddr, pRsrc->name);
//		else	xprint("+ok@%d.%s.getocdth(%d)\r\n", brdAddr, pRsrc->name, x0);
//		return 1;
//	}
//
//	/***********************************************************
//	"\n %name.SetStallTh(%d), always writable
//	@ val:31.25..1000mV, 1LSB = 31.25mV
//	************************************************************/
//	else if(sscanf(FUNC, ".setstallth %d ", &num0)==1){
//		if(pDev->RegSetStallTh(&pDev->rsrc, num0))
//			xprint("+err@%d.%s.setstallth('busy')\r\n", brdAddr, pRsrc->name);
//		else	xprint(  "+ok@%d.%s.setstallth(%d)\r\n", brdAddr, pRsrc->name, num0);
//		return 1;
//	}
//	else if(strncmp(FUNC, ".getstallth  ", strlen(".getstallth  "))==0){
//		if(pDev->RegGetStallTh(&pDev->rsrc, &x0))
//			xprint("+err@%d.%s.getstallth('busy')\r\n", brdAddr, pRsrc->name);
//		else	xprint("+ok@%d.%s.getstallth(%d)\r\n", brdAddr, pRsrc->name, x0);
//		return 1;
//	}
//
//	/***********************************************************
//	"\n %name.SetStepSel(%d), writable when high impendance
//	@ val:0..7
//	************************************************************/
//	else if(sscanf(FUNC, ".setstepsel %d ", &num0)==1){
//		if(pDev->RegSetStepSel(&pDev->rsrc, num0))
//			xprint("+err@%d.%s.setstepsel('busy')\r\n", brdAddr, pRsrc->name);
//		else	xprint(  "+ok@%d.%s.setstepsel(%d)\r\n", brdAddr, pRsrc->name, num0);
//		return 1;
//	}
//	else if(strncmp(FUNC, ".getstepsel  ", strlen(".getstepsel  "))==0){
//		if(pDev->RegGetStepSel(&pDev->rsrc, &x0))
//			xprint("+err@%d.%s.getstepsel('busy')\r\n", brdAddr, pRsrc->name);
//		else	xprint("+ok@%d.%s.getstepsel(%d)\r\n", brdAddr, pRsrc->name, x0);
//		return 1;
//	}
//
//	/***********************************************************
//	"\n %name.SetSyncSel(%d), writable when high impendance
//	@ val:0..7
//	************************************************************/
//	else if(sscanf(FUNC, ".setsyncsel %d ", &num0)==1){
//		if(pDev->RegSetSyncSel(&pDev->rsrc, num0))
//			xprint("+err@%d.%s.setsyncsel('busy')\r\n", brdAddr, pRsrc->name);
//		else	xprint(  "+ok@%d.%s.setsyncsel(%d)\r\n", brdAddr, pRsrc->name, num0);
//		return 1;
//	}
//	else if(strncmp(FUNC, ".getsyncsel  ", strlen(".getsyncsel  "))==0){
//		if(pDev->RegGetSyncSel(&pDev->rsrc, &x0))
//			xprint("+err@%d.%s.getsyncsel('busy')\r\n", brdAddr, pRsrc->name);
//		else	xprint("+ok@%d.%s.getsyncsel(%d)\r\n", brdAddr, pRsrc->name, x0);
//		return 1;
//	}
//
//	/***********************************************************
//	"\n %name.SetSyncEn(%d), writable when high impendance
//	@ val:0..1
//	************************************************************/
//	else if(sscanf(FUNC, ".setsyncen %d ", &num0)==1){
//		if(pDev->RegSetSyncSel(&pDev->rsrc, num0))
//			xprint("+err@%d.%s.setsyncen('busy')\r\n", brdAddr, pRsrc->name);
//		else	xprint(  "+ok@%d.%s.setsyncen(%d)\r\n", brdAddr, pRsrc->name, num0);
//		return 1;
//	}
//	else if(strncmp(FUNC, ".getsyncen  ", strlen(".getsyncen  "))==0){
//		if(pDev->RegGetSyncSel(&pDev->rsrc, &x0))
//			xprint("+err@%d.%s.getsyncen('busy')\r\n", brdAddr, pRsrc->name);
//		else	xprint("+ok@%d.%s.getsyncen(%d)\r\n", brdAddr, pRsrc->name, x0);
//		return 1;
//	}
//
//#if(USING_L6480)
//	/***********************************************************
//	"\n %name.SetGateTcc, writable when high impendance
//	@
//	************************************************************/
//	else if(sscanf(FUNC, ".setgatetcc %d ", &num0)==1){
//		if(pDev->RegSetGateTcc(&pDev->rsrc, num0))
//			xprint("+err@%d.%s.setgatetcc('busy')\r\n", brdAddr, pRsrc->name);
//		else	xprint(  "+ok@%d.%s.setgatetcc(%d)\r\n", brdAddr, pRsrc->name, num0);
//		return 1;
//	}
//	else if(strncmp(FUNC, ".getgatetcc  ", strlen(".getgatetcc  "))==0){
//		if(pDev->RegGetGateTcc(&pDev->rsrc, &x0))
//			xprint("+err@%d.%s.getgatetcc('busy')\r\n", brdAddr, pRsrc->name);
//		else	xprint("+ok@%d.%s.getgatetcc(%d)\r\n", brdAddr, pRsrc->name, x0);
//		return 1;
//	}
//
//	/***********************************************************
//	"\n %name.SetGateI, writable when high impendance
//	@
//	************************************************************/
//	else if(sscanf(FUNC, ".setgatei %d ", &num0)==1){
//		if(pDev->RegSetGateCurrent(&pDev->rsrc, num0))
//			xprint(  "+err@%d.%s.setgatei('busy')\r\n", brdAddr, pRsrc->name);
//		else	xprint(  "+ok@%d.%s.setgatei(%d)\r\n", brdAddr, pRsrc->name, num0);
//		return 1;
//	}
//	else if(strncmp(FUNC, ".getgatei  ", strlen(".getgatei  "))==0){
//		if(pDev->RegGetGateCurrent(&pDev->rsrc, &x0))
//			xprint(  "+err@%d.%s.getgatei('busy')\r\n", brdAddr, pRsrc->name);
//		else	xprint("+ok@%d.%s.getgatei(%d)\r\n", brdAddr, pRsrc->name, x0);
//		return 1;
//	}
//
//	/***********************************************************
//	"\n %name.SetGateTboost, writable when high impendance
//	@
//	************************************************************/
//	else if(sscanf(FUNC, ".setgatetboost %d ", &num0)==1){
//		if(pDev->RegSetGateTBoost(&pDev->rsrc, num0))
//			xprint("+err@%d.%s.setgatetboost('busy')\r\n", brdAddr, pRsrc->name);
//		else	xprint(  "+ok@%d.%s.setgatetboost(%d)\r\n", brdAddr, pRsrc->name, num0);
//		return 1;
//	}
//	else if(strncmp(FUNC, ".getgatetboost  ", strlen(".getgatetboost  "))==0){
//		if(pDev->RegGetGateTBoost(&pDev->rsrc, &x0))
//			xprint("+err@%d.%s.getgatetboost('busy')\r\n", brdAddr, pRsrc->name);
//		else	xprint("+ok@%d.%s.getgatetboost(%d)\r\n", brdAddr, pRsrc->name, x0);
//		return 1;
//	}
//
//	/***********************************************************
//	"\n %name.SetGateTDT, writable when high impendance
//	@
//	************************************************************/
//	else if(sscanf(FUNC, ".setgatetdt %d ", &num0)==1){
//		if(pDev->RegSetGateTDeadTime(&pDev->rsrc, num0))
//			xprint("+err@%d.%s.setgatetdt('busy')\r\n", brdAddr, pRsrc->name);
//		else	xprint(  "+ok@%d.%s.setgatetdt(%d)\r\n", brdAddr, pRsrc->name, num0);
//		return 1;
//	}
//	else if(strncmp(FUNC, ".getgatetdt  ", strlen(".getgatetdt  "))==0){
//		if(pDev->RegGetGateTDeadTime(&pDev->rsrc, &x0))
//			xprint("+err@%d.%s.getgatetdt('busy')\r\n", brdAddr, pRsrc->name);
//		else	xprint("+ok@%d.%s.getgatetdt(%d)\r\n", brdAddr, pRsrc->name, x0);
//		return 1;
//	}
//
//	/***********************************************************
//	"\n %name.SetGateTBlank, writable when high impendance
//	@
//	************************************************************/
//	else if(sscanf(FUNC, ".setgatetblank %d ", &num0)==1){
//		if(pDev->RegSetGateTBlank(&pDev->rsrc, num0))
//			xprint("+err@%d.%s.setgatetblank('busy')\r\n", brdAddr, pRsrc->name);
//		else	xprint(  "+ok@%d.%s.setgatetblank(%d)\r\n", brdAddr, pRsrc->name, num0);
//		return 1;
//	}
//	else if(strncmp(FUNC, ".getgatetblank  ", strlen(".getgatetblank  "))==0){
//		if(pDev->RegGetGateTBlank(&pDev->rsrc, &x0))
//			xprint("+err@%d.%s.getgatetblank('busy')\r\n", brdAddr, pRsrc->name);
//		else	xprint("+ok@%d.%s.getgatetblank(%d)\r\n", brdAddr, pRsrc->name, x0);
//		return 1;
//	}
//#endif
//
//	/***********************************************************
//	"\n %name.SetSwMode, writable when high impendance
//	@
//	************************************************************/
//	else if(sscanf(FUNC, ".setswmode %d ", &num0)==1){
//		if(pDev->RegSetSwMode(&pDev->rsrc, num0))
//			xprint("+err@%d.%s.setswmode('busy')\r\n", brdAddr, pRsrc->name);
//		else	xprint(  "+ok@%d.%s.setswmode(%d)\r\n", brdAddr, pRsrc->name, num0);
//		return 1;
//	}
//	else if(strncmp(FUNC, ".getswmode  ", strlen(".getswmode  "))==0){
//		if(pDev->RegGetSwMode(&pDev->rsrc, &x0))
//			xprint("+err@%d.%s.getswmode('busy')\r\n", brdAddr, pRsrc->name);
//		else	xprint("+ok@%d.%s.getswmode(%d)\r\n", brdAddr, pRsrc->name, x0);
//		return 1;
//	}
//
//	/***********************************************************
//	"\n %name.SetVsCompEn, writable when high impendance
//	@
//	************************************************************/
//	else if(sscanf(FUNC, ".setvscompen %d ", &num0)==1){
//		if(pDev->RegSetVsCompEn(&pDev->rsrc, num0))
//			xprint("+err@%d.%s.setvscompen('busy')\r\n", brdAddr, pRsrc->name);
//		else	xprint(  "+ok@%d.%s.setvscompen(%d)\r\n", brdAddr, pRsrc->name, num0);
//		return 1;
//	}
//	else if(strncmp(FUNC, ".getvscompen  ", strlen(".getvscompen  "))==0){
//		if(pDev->RegGetVsCompEn(&pDev->rsrc, &x0))
//			xprint("+err@%d.%s.getvscompen('busy')\r\n", brdAddr, pRsrc->name);
//		else	xprint("+ok@%d.%s.getvscompen(%d)\r\n", brdAddr, pRsrc->name, x0);
//		return 1;
//	}
//
//	/***********************************************************
//	"\n %name.SetOCSDEn, writable when high impendance
//	@
//	************************************************************/
//	else if(sscanf(FUNC, ".setocsden %d ", &num0)==1){
//		if(pDev->RegSetOCSDEn(&pDev->rsrc, num0))
//			xprint("+err@%d.%s.setocsden('busy')\r\n", brdAddr, pRsrc->name);
//		else	xprint(  "+ok@%d.%s.setocsden(%d)\r\n", brdAddr, pRsrc->name, num0);
//		return 1;
//	}
//	else if(strncmp(FUNC, ".getocsden  ", strlen(".getocsden  "))==0){
//		if(pDev->RegGetOCSDEn(&pDev->rsrc, &x0))
//			xprint("+err@%d.%s.getocsden('busy')\r\n", brdAddr, pRsrc->name);
//		else	xprint("+ok@%d.%s.getocsden(%d)\r\n", brdAddr, pRsrc->name, x0);
//		return 1;
//	}
//
//	/***********************************************************
//	"\n %name.SetUVLOVal, writable when high impendance
//	@
//	************************************************************/
//	else if(sscanf(FUNC, ".setuvloval %d ", &num0)==1){
//		if(pDev->RegSetUVLOVal(&pDev->rsrc, num0))
//			xprint("+err@%d.%s.setuvloval('busy')\r\n", brdAddr, pRsrc->name);
//		else	xprint(  "+ok@%d.%s.setuvloval(%d)\r\n", brdAddr, pRsrc->name, num0);
//		return 1;
//	}
//	else if(strncmp(FUNC, ".getuvloval  ", strlen(".getuvloval  "))==0){
//		if(pDev->RegGetUVLOVal(&pDev->rsrc, &x0))
//			xprint("+err@%d.%s.getuvloval('busy')\r\n", brdAddr, pRsrc->name);
//		else	xprint("+ok@%d.%s.getuvloval(%d)\r\n", brdAddr, pRsrc->name, x0);
//		return 1;
//	}
//
//	/***********************************************************
//	"\n %name.SetVccVal, writable when high impendance
//	@
//	************************************************************/
//	else if(sscanf(FUNC, ".setvccval %d ", &num0)==1){
//		if(pDev->RegSetVccVal(&pDev->rsrc, num0))
//			xprint("+err@%d.%s.setvccval('busy')\r\n", brdAddr, pRsrc->name);
//		else	xprint(  "+ok@%d.%s.setvccval(%d)\r\n", brdAddr, pRsrc->name, num0);
//		return 1;
//	}
//	else if(strncmp(FUNC, ".getvccval  ", strlen(".getvccval  "))==0){
//		if(pDev->RegGetVccVal(&pDev->rsrc, &x0))
//			xprint("+err@%d.%s.getvccval('busy')\r\n", brdAddr, pRsrc->name);
//		else	xprint("+ok@%d.%s.setvccval(%d)\r\n", brdAddr, pRsrc->name, x0);
//		return 1;
//	}
//
//	/***********************************************************
//	"\n %name.SetPwmFrq, writable when high impendance
//	@
//	************************************************************/
//	else if(sscanf(FUNC, ".setpwmfrq %d ", &num0)==1){
//		if(pDev->RegSetPwmFrq(&pDev->rsrc, num0))
//			xprint("+err@%d.%s.setpwmfrq('busy')\r\n", brdAddr, pRsrc->name);
//		else	xprint(  "+ok@%d.%s.setpwmfrq(%d)\r\n", brdAddr, pRsrc->name, num0);
//		return 1;
//	}
//	else if(strncmp(FUNC, ".getpwmfrq  ", strlen(".getpwmfrq  "))==0){
//		if(pDev->RegGetPwmFrq(&pDev->rsrc, &x0))
//			xprint("+err@%d.%s.getpwmfrq('busy')\r\n", brdAddr, pRsrc->name);
//		else	xprint("+ok@%d.%s.getpwmfrq(%d)\r\n", brdAddr, pRsrc->name, x0);
//		return 1;
//	}
//
//	/***********************************************************
//	"\n %name.getAdcOut, always writable
//	@
//	************************************************************/
//	else if(strncmp(FUNC, ".getadcout  ", strlen(".getadcout  "))==0){
//		if(pDev->RegGetAdcOut(&pDev->rsrc, &x0))
//			xprint("+err@%d.%s.getadcout('busy')\r\n", brdAddr, pRsrc->name);
//		else	xprint("+ok@%d.%s.getadcout(%d)\r\n", brdAddr, pRsrc->name, x0);
//		return 1;
//	}
//
//	else if(sscanf(FUNC, ".setconf %d ", &num0)==1){
//		if(pDev->RegSetCofig(&pDev->rsrc, num0))
//			xprint("+err@%d.%s.setconf('busy')\r\n", brdAddr, pRsrc->name);
//		else	xprint(  "+ok@%d.%s.setconf(%d)\r\n", brdAddr, pRsrc->name, num0);
//		return 1;
//	}
//	else if(strncmp(FUNC, ".getconf  ", strlen(".getconf  "))==0){
//		if(pDev->RegGetCofig(&pDev->rsrc, (u16*)&num0))
//			xprint("+err@%d.%s.getconf('busy')\r\n", brdAddr, pRsrc->name);
//		else	xprint("+ok@%d.%s.getconf(0x%04x)\r\n", brdAddr, pRsrc->name, num0&0xffff);
//		return 1;
//	}

	return 0;
}

/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
