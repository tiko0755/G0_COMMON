/******************** (C) COPYRIGHT 2015 INCUBECN *****************************
* File Name          : inputCmd.c
* Author             : Tiko Zhong
* Date First Issued  : 12/01/2015
* Description        : This file provides a set of functions needed to manage the
*
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "tmcCmd.h"
#include "tmc429.h"
#include "string.h"
#include "stdio.h"
#include "board.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
const char TMC_HELP[] = {
	"steper command:"
	"\n .init()"
	"\n .rotate(motor,v)"
	"\n .moveto(motor,pos)"
	"\n .moveby(motor,pos)"
	"\n .hard_stop(motor)"
	"\n .motion_conf(axis,vmax,amax,pulsDiv,rampDiv,pMul,pDiv)"
	"\n .motion_conf(addr,val)"
	"\n .save(axis)"
	"\n .info(axis)"
	"\n"
};

/* Private function prototypes -----------------------------------------------*/

/*******************************************************************************
* Function Name  : inputCmd
* Description    : gpio function commands
* Input          : 
									: 
* Output         : None
* Return         : None 
*******************************************************************************/
u8 tmcCmd(void *dev, char* CMD, u8 brdAddr, void (*xprint)(const char* FORMAT_ORG, ...)){
	s32 i,j,k;
	u32 vu0,vu1,vu2,vu3,vu4,vu5,vu6,vu7,vu8,vu9;
	s32 vs0,vs1,vs2,vs3,vs4,vs5,vs6,vs7,vs8,vs9;
	const char* line;
	TMC429_DEV* p = dev;
	TMC429_RSRC *pRsrc = &p->rsrc;

	if(strncmp(CMD, pRsrc->name, strlen(pRsrc->name)) != 0)	return 0;
	line = &CMD[strlen(pRsrc->name)];

	//.help()
	if(strncmp(line, ".help", strlen(".help")) == 0){
		xprint("+ok@%d.%s.help()\r\n%s", brdAddr, pRsrc->name, TMC_HELP);
		return 1;
	}

	//.init()
	if(strncmp(line, ".init", strlen(".init")) == 0){
		p->Init(pRsrc);
		xprint("+ok@%d.%s.init()\r\n", brdAddr, pRsrc->name);
		return 1;
	}

	//.rotate(motor,v)
	else if(sscanf(line, ".rotate %d %d", &vs0, &vs1)==2){
		p->rotate(pRsrc, vs0, vs1);
		xprint("+ok@%d.%s.rotate(%d,%d)\r\n", brdAddr, pRsrc->name, vs0, vs1);
		return 1;
	}

	//.moveto(motor,pos)
	else if(sscanf(line, ".moveto %d %d", &vs0, &vs1)==2){
		p->moveTo(pRsrc, vs0, vs1);
		tickStart  = HAL_GetTick();
		spdTestEn = 1;
		xprint("+ok@%d.%s.moveto(%d,%d)\r\n", brdAddr, pRsrc->name, vs0, vs1);
		return 1;
	}

	//.moveby(motor,pos)
	else if(sscanf(line, ".moveby %d %d", &vs0, &vs1)==2){
		p->moveBy(pRsrc, vs0, vs1);
		tickStart  = HAL_GetTick();
		spdTestEn = 1;
		xprint("+ok@%d.%s.moveby(%d,%d)\r\n", brdAddr, pRsrc->name, vs0, vs1);
		return 1;
	}

	//.hard_stop(motor)
	else if(sscanf(line, ".hard_stop %d %d", &vs0)==1){
		p->HardStop(pRsrc, vs0);
		xprint("+ok@%d.%s.hard_stop(%d)\r\n", brdAddr, pRsrc->name, vs0);
		return 1;
	}

	//.motion_conf(axis,vmax,amax,pulsDiv,rampDiv,pMul,pDiv)
	else if(sscanf(line, ".motion_conf %d %d %d %d %d %d %d",&vs0,&vs1,&vs2,&vs3,&vs4,&vs5,&vs6)==7){
		p->motionConf(pRsrc, vs0,vs1,vs2,vs3,vs4,vs5,vs6);
		xprint("+ok@%d.%s.motion_conf(%d,%d,%d,%d,%d,%d,%d)\r\n", brdAddr, pRsrc->name,vs0,vs1,vs2,vs3,vs4,vs5,vs6);
		return 1;
	}

	//.motion_conf(addr,val)
	else if(sscanf(line, ".motion_conf %d",&vs0)==1){
		if(vs0>=TMC429_MOTOR_COUNT){
			xprint("+err@%d.%s.motion_conf(\"over\")\r\n", brdAddr, pRsrc->name,vs0);
			return 1;
		}
		p->motionConf_update(pRsrc,vs0);
		xprint("VMAX\t%d\n", pRsrc->axisReg[vs0].V_MAX);
		xprint("AMAX\t%d\n", pRsrc->axisReg[vs0].A_MAX);
		xprint("PULSE_DIV\t%d\n", pRsrc->axisReg[vs0].RampDiv_PulseDiv.PULSE_DIV);
		xprint("RAMP_DIV\t%d\n", pRsrc->axisReg[vs0].RampDiv_PulseDiv.RAMP_DIV);
		xprint("PMUL\t%d\n", pRsrc->axisReg[vs0].PMul_PDiv.PMUL);
		xprint("PDIV\t%d\n", pRsrc->axisReg[vs0].PMul_PDiv.PDIV);
		xprint("+ok@%d.%s.motion_conf(%d)\r\n", brdAddr, pRsrc->name,vs0);
		return 1;
	}

	//.save(axis)
	else if(sscanf(line, ".save %d", &vs0)==1){
		if(vs0 >= 3){
			xprint("+err@%d.%s.save(\"0..2\")\r\n", brdAddr, pRsrc->name, vs0);
			return 1;
		}
		p->save(pRsrc, (u32)vs0);
		xprint("+ok@%d.%s.save(%d)\r\n", brdAddr, pRsrc->name, vs0);
		return 1;
	}
	else if(strncmp(line, ".save", strlen(".save")) == 0){
		p->save(pRsrc, 0);
		p->save(pRsrc, 1);
		p->save(pRsrc, 2);
		xprint("+ok@%d.%s.save()\r\n", brdAddr, pRsrc->name);
		return 1;
	}

	//.reg_(addr,val)
	else if(sscanf(line, ".reg 0x%x 0x%x", &vs0, &vs1)==2){
		vu0 = p->WriteReg(pRsrc, (u32)vs0, (u32)vs1);
		xprint("+ok@%d.%s.writereg(0x%02x,0x%02x,0x%08x)\r\n", brdAddr, pRsrc->name, vu0, vs0, vs1);
		return 1;
	}

	//.reg_(addr,val)
	else if(sscanf(line, ".reg 0x%x %d", &vs0, &vs1)==2){
		vu0 = p->WriteReg(pRsrc, (u32)vs0, (u32)vs1);
		xprint("+ok@%d.%s.writereg(0x%02x,0x%02x,%d)\r\n", brdAddr, pRsrc->name, vu0, vs0, vs1);
		return 1;
	}

	//.reg_(addr)
	else if(sscanf(line, ".reg 0x%x", &vs0)==1){
		vu0 = p->ReadReg(pRsrc, (u32)vs0, (u32*)&vs1);
		xprint("+ok@%d.%s.readreg(0x%02x,0x%02x,0x%08x)\r\n", brdAddr, pRsrc->name, vu0, vs0, vs1);
		return 1;
	}

	//.info(axis)
	else if(sscanf(line, ".info %d", &vs0)==1){
		if(vs0 >= 3){
			xprint("+ok@%d.%s.info(\"over\")\r\n", brdAddr, pRsrc->name);
			return 1;
		}

		//HAL_IWDG_Refresh(&hiwdg);

		vs0 &= 0x03;
		vu0 = p->regR_X_TARGET(pRsrc, vs0, &vs1);
		xprint("xTaget\t%d\n", vs1);

		vu0 = p->regR_X_ACTUAL(pRsrc, vs0, &vs1);
		xprint("xActual\t%d\n", vs1);

		vu0 = p->regR_V_TARGET(pRsrc, vs0, (s16*)&vs1);
		xprint("vTarget\t%d\n", 0x0000ffff&vs1);

		vu0 = p->regR_V_ACTUAL(pRsrc, vs0, (s16*)&vs1);
		xprint("vActual\t%d\n", (s16)vs1);

		vu0 = p->regR_A_ACTUAL(pRsrc, vs0, (s16*)&vu1);
		xprint("aActual\t%d\n", (s16)vu1);

		vu0 = p->regR_V_MIN(pRsrc, vs0, (u16*)&vu1);
		xprint("vMin\t%d\n", vu1&0x0000ffff);
		console.TxPolling(&console.rsrc);

		vu0 = p->regR_V_MAX(pRsrc, vs0, (u16*)&vu1);
		xprint("vMax\t%d\n", 0x0000ffff&vu1);

		vu0 = p->regR_A_MAX(pRsrc, vs0, (u16*)&vu1);
		xprint("aMax\t%d\n", 0x07ff&vu1);

//		vu0 = p->regR_CurrentScaling(pRsrc, vs0,
//				&pRsrc->axisReg[vs0].IS_AGTAT,
//				&pRsrc->axisReg[vs0].IS_ALEAT,
//				&pRsrc->axisReg[vs0].IS_V0,
//				&pRsrc->axisReg[vs0].A_THRESHOLD
//		);
//		xprint("IS_AGTAT\t%d\n", pRsrc->axisReg[vs0].IS_AGTAT);
//		xprint("IS_ALEAT\t%d\n", pRsrc->axisReg[vs0].IS_ALEAT);
//		xprint("IS_V0\t%d\n", pRsrc->axisReg[vs0].IS_V0);
//		xprint("A_THRESHOLD\t%d\n", pRsrc->axisReg[vs0].A_THRESHOLD);
//
//		p->regR_PMUL_PDIV(pRsrc, vs0, &pRsrc->axisReg[vs0].PMUL, &pRsrc->axisReg[vs0].PDIV);
//		xprint("PMUL\t%d\n", pRsrc->axisReg[vs0].PMUL);
//		xprint("PDIV\t%d\n", pRsrc->axisReg[vs0].PDIV);
//
//		p->regR_RampMode_RefConf_lp(pRsrc, vs0, &pRsrc->axisReg[vs0].RAMP_MODE, &pRsrc->axisReg[vs0].REF_CONF, &pRsrc->axisReg[vs0].lp);
//		xprint("RAMP_MODE\t%d\n", pRsrc->axisReg[vs0].RAMP_MODE);
//		xprint("REF_CONF\t%d\n", pRsrc->axisReg[vs0].REF_CONF);
//		xprint("lp\t%d\n", pRsrc->axisReg[vs0].lp);
//
//		p->regR_RampDiv_PulseDiv_Usrs(pRsrc, vs0, &pRsrc->axisReg[vs0].RAMP_DIV, &pRsrc->axisReg[vs0].PULSE_DIV, &pRsrc->axisReg[vs0].USRS);
//		xprint("RAMP_DIV\t%d\n", pRsrc->axisReg[vs0].RAMP_DIV);
//		xprint("PULSE_DIV\t%d\n", pRsrc->axisReg[vs0].PULSE_DIV);
//		xprint("USRS\t%d\n", pRsrc->axisReg[vs0].USRS);

		p->regR_IFConf(pRsrc, (u16*)&vu0);
		xprint("IF_CONF\t0x%06x\n", 0x0000ffff&vu0);

		p->regR_Version(pRsrc, &vu0);
		xprint("ver\t0x%08x\n", vu0);

		p->regR_refsw(pRsrc, (u8*)&vu0);
		xprint("refsw\t0x%08x\n", 0x0000003f&vu0);

		vu0 = p->regR_SMGP(pRsrc, &vu0);
		xprint("SMGP\t0x%08x\n", vu0);

		xprint("+ok@%d.%s.info(%d,0x%02x)\r\n", brdAddr, pRsrc->name, vs0, vu0);
		return 1;
	}

	//.regx(addr,val)
	else if(sscanf(line, ".regx %d", &vs0)==1){
		if(vs0 >= 3){
			xprint("+ok@%d.%s.regx(\"over\")\r\n", brdAddr, pRsrc->name);
			return 1;
		}
		vu0 = p->regR_V_MIN(pRsrc, vs0, (u16*)&vu1);
		xprint("STA[%02x]\tvMin[%d]\n", vu0, vu1);

		vu0 = p->regR_V_MAX(pRsrc, vs0, (u16*)&vu1);
		xprint("STA[%02x]\tvMax[%d]\n", vu0, (u16)vu1);

		vu0 = p->regR_V_TARGET(pRsrc, vs0, (s16*)&vs1);
		xprint("STA[%02x]\tvTarget[%d]\n", vu0, vs1);

		vu0 = p->regR_A_MAX(pRsrc, vs0, (u16*)&vu1);
		xprint("STA[%02x]\taMax[%d]\n", vu0, (u16)vu1);

		vu0 = p->regR_A_ACTUAL(pRsrc, vs0, (s16*)&vu1);
		xprint("STA[%02x]\taActual[%d]\n", vu0, (s16)vu1);

		vu0 = p->regR_V_ACTUAL(pRsrc, vs0, (s16*)&vs1);
		xprint("STA[%02x]\tvActual[%d]\n", vu0, (s16)vs1);

		vu0 = p->regR_X_ACTUAL(pRsrc, vs0, &vs1);
		xprint("STA[%02x]\txActual[%d]\n", vu0, (u16)vs1);

		xprint("+ok@%d.%s.regx(%d)\r\n", brdAddr, pRsrc->name, vs0);
		return 1;
	}

	//.reg_x_target(addr,val)
	else if(sscanf(line, ".reg_x_actual %d %d ", &vs0, &vs1)==2){
		vu0 = p->regW_X_ACTUAL(pRsrc, vs0, vs1);
		xprint("+ok@%d.%s.reg_x_actual(0x%02x,%d,%d)\r\n", brdAddr, pRsrc->name, vu0, vs0, vs1);
		return 1;
	}
	//.reg_x_target(addr,val)
	else if(sscanf(line, ".reg_x_target %d %d ", &vs0, &vs1)==2){
		vu0 = p->regW_X_TARGET(pRsrc, vs0, vs1);
		xprint("+ok@%d.%s.reg_x_target(0x%02x,%d,%d)\r\n", brdAddr, pRsrc->name, vu0, vs0, vs1);
		return 1;
	}
	//.reg_x_target(addr,val)
	else if(sscanf(line, ".reg_x_target %d", &vs0)==1){
		vu0 = p->regR_X_TARGET(pRsrc, vs0, &vs1);
		xprint("+ok@%d.%s.reg_x_target(0x%02x,%d,%d)\r\n", brdAddr, pRsrc->name, vu0, vs0, vs1);
		return 1;
	}

	//.reg_x_target(addr,val)
	else if(sscanf(line, ".reg_v_target %d %d ", &vs0, &vs1)==2){
		vu0 = p->regW_V_TARGET(pRsrc, vs0, vs1);
		xprint("+ok@%d.%s.reg_v_target(0x%02x,%d,%d)\r\n", brdAddr, pRsrc->name, vu0, vs0, vs1);
		return 1;
	}

	else if(sscanf(line, ".interrupt %d %d", &vs0, &vs1)==2){
		vs1 &= 0xff;
		vu0 = p->regW_INTERRUPT(pRsrc, vs0, 0xff, vs1);
		xprint("+ok@%d.%s.interupt(%d,0xff,0x%02x)\r\n", brdAddr, pRsrc->name, vs0,vs1);
		return 1;
	}
	else if(sscanf(line, ".interrupt %d", &vs0)==1){
		vu0 = p->regR_INTERRUPT(pRsrc, vs0, &vu1, &vu2);
		xprint("+ok@%d.%s.interupt(%d,0x%02x,0x%02x)\r\n", brdAddr, pRsrc->name, vs0, vu0&0xff, vu2&0xff);
		return 1;
	}

	else if(strncmp(line, ".switches", strlen(".switches"))==0){
		vu0 = p->regR_refsw(pRsrc, &vu1);
		xprint("+ok@%d.%s.switches(%d,0x%02x,0x%02x)\r\n", brdAddr, pRsrc->name, vu0&0xff, vu1&0xff);
		return 1;
	}


//	//.reg(addr)
//	else if(sscanf(line, ".regx %d", &i)==1){
//		dev->readRegister(pRsrc, (u8)(0xff&i), &j);
//		xprint("+ok@%d.%s.reg(0x%08x,0x%08x)\r\n", brdAddr, pRsrc->name, i, j);
//		return 1;
//	}
//
//	// .stop
//	else if(strncmp(line, ".stop", strlen(".stop")) == 0){
//		dev->stop(pRsrc);
//		xprint("+ok@%d.%s.stop()\r\n", brdAddr, pRsrc->name);
//		return 1;
//	}
//
//	// .run vel
//	else if(sscanf(line, ".run %d", &i)==1){
//		dev->rotate(pRsrc, i);
//		xprint("+ok@%d.%s.run(%d)\r\n", brdAddr, pRsrc->name, i);
//		return 1;
//	}
//
//	// .moveto pos
//	else if(sscanf(line, ".moveto %d", &i)==1){
//		dev->moveTo(pRsrc, i);
//		xprint("+ok@%d.%s.movto(%d)\r\n", brdAddr, pRsrc->name, i);
//		return 1;
//	}

	else{
		xprint("+unknown@%s", CMD);
		return 1;
	}
	return 0;
}

/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
