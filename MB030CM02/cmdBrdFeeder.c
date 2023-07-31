/**********************************************************
filename: cmdBrdFeeder.c
**********************************************************/

/************************����ͷ�ļ�***************************************************/
#include "cmdBrdFeeder.h"
#include "boardFeeder.h"
#include "string.h"
#include "stdio.h"

const char* HELP_brdFdr = {
	"Feeder Task Commands:"
	"\n x.help()"
};

/**********************************************************
 Private function
**********************************************************/

/**********************************************************
 Public function
**********************************************************/
u8 cmdBrdFdr(void *dev, const char* buff, void (*xprint)(const char* FORMAT_ORG, ...)){
	const char* FUNC;
	u8 x0,x1,x2,x3,x4;
	u16 y0;		
	u32 z0,z1,z2;
	s32 v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10;
	u8 tmp8;
	BrdFdrDev_T* fBoard = (BrdFdrDev_T*)dev;
	BrdFdrRsrc_T* p = &fBoard->rsrc;

	cSPIN_DEV_T *mr,*mx, *m;
	cSPIN_Rsrc_T *mrRsrc,*mxRsrc;
	fdrMotion_t *pFdrMCfg;

	mr = p->stprR;
	mx = p->stprX;
	mrRsrc = &p->stprR->rsrc;
	mxRsrc = &p->stprX->rsrc;

	// UI command
	if(strncmp(buff, "initial\r\n", strlen("initial\r\n"))==0){
		fBoard->TjcUIReset(p);
		return 1;
	}
	else if(sscanf(buff, "usetting.save %d %d %d %d", &v0,&v1,&v2,&v3) == 4){
		p->songliaobuchang = v0;
		p->songliaoxianwei = v1;
		p->songliaoshudu = v2;
		p->shouliaoshudu = v3;
		fBoard->TjcEepromWriteAll(p);
		return 1;
	}
	else if(sscanf(buff, "usetting.set %d %d %d %d", &v0,&v1,&v2,&v3) == 4){
		p->songliaobuchang = v0;
		p->songliaoxianwei = v1;
		p->songliaoshudu = v2;
		p->shouliaoshudu = v3;
		// set songliao speed
		mr->Hard_Stop(mrRsrc);
		mx->Hard_HiZ(mxRsrc);
		mx->RegSetMaxSpd_stepPerS(mxRsrc, songliaoxianweiToStep(p->songliaoshudu));
		mr->RegSetMaxSpd_stepPerS(mrRsrc, songliaoxianweiToStep(p->shouliaoshudu));
		mx->Soft_Stop(mxRsrc);
		return 1;
	}



	// motion page
	// motion.mselect.dwn n\x0D\x0A
	else if(sscanf(buff, "motion.mselect.dwn %d\r\n", &v0)==1){
		pFdrMCfg = &p->stprCfg[v0];
		v1 = pFdrMCfg->constSpd;
		v2 = pFdrMCfg->position;
		xprint("motion.t5.txt=\"%d\"", v1);	// current constant speed
		xprint("motion.h0.val=%d", v1);
		xprint("motion.n0.val=%d", v2);

		v1 = pFdrMCfg->runMod;
		if(v1 ==0){
			xprint("motion.r0.val=1");
			xprint("motion.r1.val=0");
			xprint("rLIndx=0");
			xprint("rRIndx=0");
		}
		else if(v1==1){
			xprint("motion.r0.val=0");
			xprint("motion.r1.val=1");
			xprint("rLIndx=0");
			xprint("rRIndx=0");
		}
		else if(v1==2){
			xprint("motion.r0.val=1");
			xprint("motion.r1.val=0");
			xprint("rLIndx=1");
			xprint("rRIndx=0");
		}
		else if(v1==3){
			xprint("motion.r0.val=0");
			xprint("motion.r1.val=1");
			xprint("rLIndx=1");
			xprint("rRIndx=0");
		}
		else if(v1==4){
			xprint("motion.r0.val=1");
			xprint("motion.r1.val=0");
			xprint("rLIndx=0");
			xprint("rRIndx=1");
		}
		else if(v1==5){
			xprint("motion.r0.val=0");
			xprint("motion.r1.val=1");
			xprint("rLIndx=0");
			xprint("rRIndx=1");
		}

		v1 = pFdrMCfg->stopMod;
		if(v1==0){	xprint("motion.c0.val=0");	}
		else{	xprint("motion.c0.val=1");	}
		return 1;
	}

	else if(sscanf(buff, "motion.mode_run %d", &v0)==1){
		pFdrMCfg = &p->stprCfg[v0];
		pFdrMCfg->runMod = 0;
		xprint("motion.r0.val=1");
		xprint("motion.r1.val=0");
		xprint("rLIndx=0");
		xprint("rRIndx=0");
		return 1;
	}
	else if(sscanf(buff, "motion.mode_move %d", &v0)==1){
		pFdrMCfg = &p->stprCfg[v0];
		pFdrMCfg->runMod = 1;
		xprint("motion.r0.val=0");
		xprint("motion.r1.val=1");
		xprint("rLIndx=0");
		xprint("rRIndx=0");
		return 1;
	}

	// motion.stopmod 1\x0D\x0A
	else if(sscanf(buff, "motion.stopmod %d", &v0)==1){
		pFdrMCfg = &p->stprCfg[v0];
		if(pFdrMCfg->stopMod==0){
			pFdrMCfg->stopMod = 1;
			xprint("motion.c0.val=1");
		}
		else{
			pFdrMCfg->stopMod = 0;
			xprint("motion.c0.val=0");
		}
		return 1;
	}
	// motion.setspd 0 952 \x0D\x0A
	else if(sscanf(buff, "motion.setspd %d %d", &v0, &v1)==2){
		pFdrMCfg = &p->stprCfg[v0];
		pFdrMCfg->constSpd = v1&0xffff;
		return 1;
		// todo: if under running....
	}
	// motion.position 0 952 \x0D\x0A
	else if(sscanf(buff, "motion.position %d %d", &v0, &v1)==2){
		pFdrMCfg = &p->stprCfg[v0];
		pFdrMCfg->position = v1&0xffff;
		return 1;
	}
	// motion.exit.dwn\x0D\x0A
	else if(strncmp(buff, "motion.exit.dwn", strlen("motion.exit.dwn"))==0){
		fBoard->TjcEepromWriteAll(p);
	}
	// motion.rotatel.dwn 0 \x0D\x0A
	else if(sscanf(buff, "motion.rotatel.dwn %d", &v0)==1){
		if(v0==0)	m=mx;
		else if(v0==1)	m=mr;
		pFdrMCfg = &p->stprCfg[v0];
		switch(pFdrMCfg->runMod){
			case 0:
				m->Run(&m->rsrc, 0, pFdrMCfg->constSpd);
				pFdrMCfg->runMod = 2;
				xprint("rLIndx=%d", 1);
//xprint("0.m->Run %d", pFdrMCfg->constSpd);
				break;
			case 1:
				m->Move_Step(&m->rsrc, 0, pFdrMCfg->position);
				pFdrMCfg->runMod = 3;
				xprint("rLIndx=%d", 1);
//xprint("1.m->Move_Step %d", pFdrMCfg->position);
				break;
			case 2:
				if(pFdrMCfg->stopMod)	m->Soft_Stop(&m->rsrc);
				m->Soft_HiZ(&m->rsrc);
				pFdrMCfg->runMod = 0;
				xprint("rLIndx=%d", 0);
//xprint("2.m->Soft_HiZ");
				break;
			case 3:
				if(pFdrMCfg->stopMod)	m->Soft_Stop(&m->rsrc);
				m->Soft_HiZ(&m->rsrc);
				pFdrMCfg->runMod = 1;
				xprint("rLIndx=%d", 0);
//xprint("3.m->Soft_HiZ");
				break;
			case 4:
				xprint("rRIndx=%d", 0);
				m->Run(&m->rsrc, 0, pFdrMCfg->constSpd);
				pFdrMCfg->runMod = 2;
				xprint("rLIndx=%d", 1);
//xprint("4.m->Run %d", pFdrMCfg->constSpd);
				break;
			case 5:
				xprint("rRIndx=%d", 0);
				m->Soft_HiZ(&m->rsrc);
				m->Move_Step(&m->rsrc, 0, pFdrMCfg->position);
				pFdrMCfg->runMod = 3;
				xprint("rLIndx=%d", 1);
//xprint("5.m->Move_Step %d", pFdrMCfg->position);
				break;
		}
		return 1;
	}
	// motion.rotater.dwn 0 \x0D\x0A
	else if(sscanf(buff, "motion.rotater.dwn %d", &v0)==1){
		if(v0==0)	m=mx;
		else if(v0==1)	m=mr;
		pFdrMCfg = &p->stprCfg[v0];
		switch(pFdrMCfg->runMod){
			case 0:
				m->Run(&m->rsrc, 1, pFdrMCfg->constSpd);
				pFdrMCfg->runMod = 4;
				xprint("rRIndx=%d", 1);
				break;
			case 1:
				m->Move_Step(&m->rsrc, 1, pFdrMCfg->position);
			print("pos:%d\n", pFdrMCfg->position);
				pFdrMCfg->runMod = 5;
				xprint("rRIndx=%d", 1);
				break;
			case 2:
				xprint("rLIndx=%d", 0);
				m->Run(&m->rsrc, 1, pFdrMCfg->constSpd);
				pFdrMCfg->runMod = 4;
				xprint("rRIndx=%d", 1);
				break;
			case 3:
				xprint("rLIndx=%d", 0);
				m->Soft_HiZ(&m->rsrc);
				m->Move_Step(&m->rsrc, 1, pFdrMCfg->position);
				pFdrMCfg->runMod = 5;
				xprint("rRIndx=%d", 1);
				break;
			case 4:
				if(pFdrMCfg->stopMod)	m->Soft_Stop(&m->rsrc);
				m->Soft_HiZ(&m->rsrc);
				pFdrMCfg->runMod = 0;
				xprint("rRIndx=%d", 0);
				break;
			case 5:
				if(pFdrMCfg->stopMod)	m->Soft_Stop(&m->rsrc);
				m->Soft_HiZ(&m->rsrc);
				pFdrMCfg->runMod = 1;
				xprint("rRIndx=%d", 0);
				break;
		}
		return 1;
	}

	// spdsetting.initial 0 \x0D\x0A
	else if(sscanf(buff, "spdsetting.initial %d", &v0) == 1){
		if(v0==0)	m=mx;
		else if(v0==1)	m=mr;
		m->Soft_HiZ(&m->rsrc);
		// acc spd
		m->RegGetAcc_stepPerSS(&m->rsrc, (u16*)&v1);
		v1 &= 0xffff;
		xprint("spdsetting.t6.txt=\"%d\"", v1);
		xprint("spdsetting.h0.val=%d", v1);
		// dec spd
		m->RegGetDec_stepPerSS(&m->rsrc, (u16*)&v1);
		v1 &= 0xffff;
		xprint("spdsetting.t7.txt=\"%d\"", v1);
		xprint("spdsetting.h1.val=%d", v1);
		// max spd
		m->RegGetMaxSpd_stepPerS(&m->rsrc, (u16*)&v1);
		v1 &= 0xffff;
		xprint("spdsetting.t8.txt=\"%d\"", v1);
		xprint("spdsetting.h2.val=%d", v1);
		// min spd
		m->RegGetMinSpd_stepPerS(&m->rsrc, (u16*)&v1);
		v1 &= 0xffff;
		xprint("spdsetting.t9.txt=\"%d\"", v1);
		xprint("spdsetting.h3.val=%d", v1);
		// fstp
		m->RegGetFSSpd_stepPerS(&m->rsrc, (u16*)&v1);
		v1 &= 0xffff;
		xprint("spdsetting.t12.txt=\"%d\"", v1);
		xprint("spdsetting.h5.val=%d", v1);
		// step
		m->RegGetStepSel(&m->rsrc, (u8*)&v1);
		v1 &= 0xff;
		switch(v1){
			case 0:	xprint("spdsetting.t10.txt=\"1/1\"");
				break;
			case 1:	xprint("spdsetting.t10.txt=\"1/2\"");
				break;
			case 2:	xprint("spdsetting.t10.txt=\"1/4\"");
				break;
			case 3:	xprint("spdsetting.t10.txt=\"1/8\"");
				break;
			case 4:	xprint("spdsetting.t10.txt=\"1/16\"");
				break;
			case 5:	xprint("spdsetting.t10.txt=\"1/32\"");
				break;
			case 6:	xprint("spdsetting.t10.txt=\"1/64\"");
				break;
			case 7:	xprint("spdsetting.t10.txt=\"1/128\"");
				break;
		}
		xprint("spdsetting.h4.val=%d", v1);
		return 1;
	}

	// spdsetting.save 1 15 15 15 15 15 3\x0D\x0A
	else if(sscanf(buff, "spdsetting.save %d %d %d %d %d %d %d",&v0,&v1,&v2,&v3,&v4,&v5,&v6) == 7){
		if(v0==0)	m=mx;
		else if(v0==1)	m=mr;
		m->RegSetAcc_stepPerSS(&m->rsrc, v1&0xffff);
		m->RegSetDec_stepPerSS(&m->rsrc, v2&0xffff);
		m->RegSetMaxSpd_stepPerS(&m->rsrc, v3&0xffff);
		m->RegSetMinSpd_stepPerS(&m->rsrc, v4&0xffff);
		m->RegSetFSSpd_stepPerS(&m->rsrc, v5&0xffff);
		m->RegSetStepSel(&m->rsrc, v6&0xff);
		m->SaveToROM(&m->rsrc);
		return 1;
	}

	// spdsetting.set 1 15 15 15 15 15 4\x0D\x0Amotion.mselect.dwn 1\x0D\x0A
	else if(sscanf(buff, "spdsetting.set %d %d %d %d %d %d %d",&v0,&v1,&v2,&v3,&v4,&v5,&v6) == 7){
		if(v0==0)	m=mx;
		else if(v0==1)	m=mr;
		m->RegSetAcc_stepPerSS(&m->rsrc, v1&0xffff);
		m->RegSetDec_stepPerSS(&m->rsrc, v2&0xffff);
		m->RegSetMaxSpd_stepPerS(&m->rsrc, v3&0xffff);
		m->RegSetMinSpd_stepPerS(&m->rsrc, v4&0xffff);
		m->RegSetFSSpd_stepPerS(&m->rsrc, v5&0xffff);
		m->RegSetStepSel(&m->rsrc, v6&0xff);
		return 1;
	}

	// spdsetting.default 0 \x0D\x0A
	else if(sscanf(buff, "spdsetting.default %d",&v0) == 1){
		if(v0==0)	m=mx;
		else if(v0==1)	m=mr;
		m->Default(&m->rsrc);
		m->SaveToROM(&m->rsrc);
		return 1;
	}

	// fsetting.initial 0 \x0D\x0A
	else if(sscanf(buff, "fsetting.initial %d", &v0) == 1){
		if(v0==0)	m=mx;
		else if(v0==1)	m=mr;
		m->RegGetKVal(&m->rsrc, (u8*)&v1, (u8*)&v2, (u8*)&v3, (u8*)&v4);
		// acc force, t6+h0
		v3 &= 0xff;
		xprint("fsetting.t6.txt=\"%d\"", v3);
		xprint("fsetting.h0.val=%d", v3);
		// run force, t7+h1
		v2 &= 0xff;
		xprint("fsetting.t7.txt=\"%d\"", v2);
		xprint("fsetting.h1.val=%d", v2);
		// dec force, t8+h2
		v4 &= 0xff;
		xprint("fsetting.t8.txt=\"%d\"", v4);
		xprint("fsetting.h2.val=%d", v4);
		// hld force, t9+h3
		v1 &= 0xff;
		xprint("fsetting.t9.txt=\"%d\"", v1);
		xprint("fsetting.h3.val=%d", v1);

		// int speed, t15+h5
		m->RegGetIntSpd_stepPerS(&m->rsrc, (u16*)&v1);
		v1 &= 0xffff;
		xprint("fsetting.t15.txt=\"%d\"", v1);
		xprint("fsetting.h5.val=%d", v1);
		// int slop, t16+h6
		m->RegGetStartSlp(&m->rsrc, (u8*)&v1);
		v1 &= 0xff;
		xprint("fsetting.t16.txt=\"%d\"", v1);
		xprint("fsetting.h6.val=%d", v1);
		// acc slop, t17+h7
		m->RegGetFnSlpAcc(&m->rsrc, (u8*)&v1);
		v1 &= 0xff;
		xprint("fsetting.t17.txt=\"%d\"", v1);
		xprint("fsetting.h7.val=%d", v1);
		// dec slop, t18+h8
		m->RegGetFnSlpDec(&m->rsrc, (u8*)&v1);
		v1 &= 0xff;
		xprint("fsetting.t18.txt=\"%d\"", v1);
		xprint("fsetting.h8.val=%d", v1);
		// volt volt comp, c0
		// temp volt comp, c1
		return 1;
	}
	// fsetting.save 1 15 15 15 15 15 3\x0D\x0A
	else if(sscanf(buff, "fsetting.save %d %d %d %d %d %d %d %d %d %d %d",&v0,&v1,&v2,&v3,&v4,&v5,&v6,&v7,&v8,&v9,&v10) == 11){
		if(v0==0)	m=mx;
		else if(v0==1)	m=mr;
		m->RegSetKVal(&m->rsrc, v4&0xff, v2&0xff, v1&0xff, v3&0xff);	// hold, run, acc, dec
		m->RegSetIntSpd_stepPerS(&m->rsrc, v5&0xffff);
		m->RegSetStartSlp(&m->rsrc, v6&0xff);
		m->RegSetFnSlpAcc(&m->rsrc, v7&0xff);
		m->RegSetFnSlpDec(&m->rsrc, v8&0xff);
		if(v9==0){	m->RegSetVsCompEn(&m->rsrc, 0);	}
		else{	m->RegSetVsCompEn(&m->rsrc, 1);	}
		if(v10==0){	m->RegSetVsCompEn(&m->rsrc, 0);	}
		else{	m->RegSetVsCompEn(&m->rsrc, 1);	}
		m->SaveToROM(&m->rsrc);
		return 1;
	}

	// fsetting.set 1 15 15 15 15 15 4\x0D\x0
	else if(sscanf(buff, "fsetting.set %d %d %d %d %d %d %d %d %d %d %d",&v0,&v1,&v2,&v3,&v4,&v5,&v6,&v7,&v8,&v9,&v10) == 11){
		if(v0==0)	m=mx;
		else if(v0==1)	m=mr;
		m->RegSetKVal(&m->rsrc, v4&0xff, v2&0xff, v1&0xff, v3&0xff);	// hold, run, acc, dec
		m->RegSetIntSpd_stepPerS(&m->rsrc, v5&0xffff);
		m->RegSetStartSlp(&m->rsrc, v6&0xff);
		m->RegSetFnSlpAcc(&m->rsrc, v7&0xff);
		m->RegSetFnSlpDec(&m->rsrc, v8&0xff);
		if(v9==0){	m->RegSetVsCompEn(&m->rsrc, 0);	}
		else{	m->RegSetVsCompEn(&m->rsrc, 1);	}
		if(v10==0){	m->RegSetVsCompEn(&m->rsrc, 0);	}
		else{	m->RegSetVsCompEn(&m->rsrc, 1);	}
		return 1;
	}

	// fsetting.default 0 \x0D\x0A
	else if(sscanf(buff, "fsetting.default %d",&v0) == 1){
		if(v0==0)	m=mx;
		else if(v0==1)	m=mr;
		m->Default(&m->rsrc);
		m->SaveToROM(&m->rsrc);
		return 1;
	}

	else if(strncmp(buff, "btnsend.click ", strlen("btnsend.click "))==0){
		// fBoard->startThreadSend(p);
		p->event |= BIT(0);
		return 1;
	}

	else if(strncmp(buff, "btnauto.click ", strlen("btnauto.click "))==0){
		if(p->mode > 0){
			p->mode = 0;	// toggle
			xprint("p6.pic=1");
		}
		else{
			p->mode = 1;
			xprint("p6.pic=2");
		}
		return 1;
	}

	else if(strncmp(buff, "btnstop.dwn", strlen("btnstop.dwn"))==0){
		fBoard->Stop(p);
		return 1;
	}
	else if(strncmp(buff, "btnstop.up", strlen("btnstop.up"))==0){
		fBoard->StopRecover(p);
		return 1;
	}

	else if(sscanf(buff, "psw.new %d", &v0) == 1){
		p->psw = v0;
		fBoard->TjcEepromWriteAll(p);
		return 1;
	}

//	else if(sscanf(buff, "fdr.usetting %d %d %d ", &v0, &v1,&v2) == 3){
//		p->songliaobuchang = v0;
//		p->songliaoshudu = v1;
//		p->songliaoxianwei = v2;
//		fBoard->TjcEepromWriteAll(p);
//		print("+ok@fdr.usetting %d %d %d ", v0, v1,v2);
//		return 1;
//	}
//	else if(strncmp(buff, "fdr.usetting  ", strlen("fdr.usetting  "))==0){
//		s32 x = fBoard->TjcEepromReadAll(p);
//		if( x < 0){
//			print("+err@fdr.usetting(%d)", x);
//			return 1;
//		}
//		print("+ok@fdr.usetting(%d,%d,%d)", p->songliaobuchang, p->songliaoshudu, p->songliaoxianwei);
//		return 1;
//	}

	return 0;
}

/**********************************************************
 == THE END ==
**********************************************************/

