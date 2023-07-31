/**********************************************************
filename: boardFeeder.c
**********************************************************/

/************************����ͷ�ļ�***************************************************/
#include "boardFeeder.h"
#include "misc.h"

#include "stdlib.h"
#include "stdio.h"
#include "stdarg.h"

#include <string.h>
#include "gpioDecal.h"
#include "cspin.h"
#include "board.h"

/**********************************************************
 Private function
**********************************************************/
static s32 brdFdrPolling(BrdFdrRsrc_T* p, u16 tick);
static void brdFdrStop(BrdFdrRsrc_T* p);
static void brdFdrRecover(BrdFdrRsrc_T* p);
static void brdFdrWritepin(BrdFdrRsrc_T*, u8 indx, u8 sta);
static void brdFdrTogglepin(BrdFdrRsrc_T* p, u8 indx);
static GPIO_PinState brdFdrReadpin(BrdFdrRsrc_T*, u8 indx);

static void brdFdrAllStop(BrdFdrRsrc_T* p);

static void updateStatus(BrdFdrRsrc_T* p, const char* INFO, u8 tmr);

static void TjcEepromWriteAll(BrdFdrRsrc_T* pRsrc);
static s8 TjcEepromReadAll(BrdFdrRsrc_T* pRsrc);
static void TjcCounterInc(BrdFdrRsrc_T* pRsrc);
static void TjcUIReset(BrdFdrRsrc_T* p);

// UI 
//const u8 END_CODE[3] = {0xff};
//static void TjcPolling(BrdFdrRsrc_T* p, u8 tickUnit);
//static void TjcRefreshOutput(TJC4024T032_Rsrc_T*, u8);
//static void TjcRefreshInput(TJC4024T032_Rsrc_T*, u8);
//static u8 TjcFetchLine(TJC4024T032_Rsrc_T*, char*, u16 len);
//static s16 TjcSendCmd(TJC4024T032_Rsrc_T* p, const char* CMD, ...);

// UI functions
static s8 UI_tagFlash(BrdFdrRsrc_T* pRsrc, u8 en);
static s8 UI_sendingFlash(BrdFdrRsrc_T* pRsrc, u8 en);
static s8 UI_strippFlash(BrdFdrRsrc_T* pRsrc, u8 en);
static s8 UI_eStopFlash(BrdFdrRsrc_T* pRsrc, u8 en);

#define UI_EVNT_COUNT	8
static s32 (*cbTagSense[UI_EVNT_COUNT])(u8 argc, ...);	// callback while tag sense
static s32 (*cbTagRemove[UI_EVNT_COUNT])(u8 argc, ...);	// callback while tag remove
static s32 (*cbPBtnDwn[UI_EVNT_COUNT])(u8 argc, ...);		// callback while pull material Button Pressdown
static s32 (*cbPBtnUp[UI_EVNT_COUNT])(u8 argc, ...);		// callback while pull material Button PressUp
static s32 (*cbEBtnDwn[UI_EVNT_COUNT])(u8 argc, ...);		// callback while eStopButton Pressdown
static s32 (*cbEBtnUp[UI_EVNT_COUNT])(u8 argc, ...);		// callback while eStopButton PressUp
static s32 (*cbSBtnDwn[UI_EVNT_COUNT])(u8 argc, ...);		// callback while strikeButton Pressdown
static s32 (*cbSBtnUp[UI_EVNT_COUNT])(u8 argc, ...);		// callback while strikeButton PressUp

//static void TjcModeAuto(TJC4024T032_Rsrc_T* rsrc);
//static void TjcModeMan(TJC4024T032_Rsrc_T* rsrc);
//static void TjcModeUConf(TJC4024T032_Rsrc_T* rsrc);
//static void TjcModeFConf(TJC4024T032_Rsrc_T* rsrc);
//static void TjcModeMConf(TJC4024T032_Rsrc_T* rsrc);
//
//static void TjcRefreshOutput(TJC4024T032_Rsrc_T*, u8);
//static void TjcRefreshInput(TJC4024T032_Rsrc_T*, u8);

static void TjcMotionReload(BrdFdrRsrc_T* p, u8 axis);

//static void TjcEepromWriteAll(BrdFdrRsrc_T* pRsrc);
//static s8 TjcEepromReadAll(BrdFdrRsrc_T* pRsrc);
//static void TjcCounterInc(TJC4024T032_Rsrc_T* pRsrc);

static void fdrCbInputRaising(u8 indx);
static void fdrCbInputFalling(u8 indx);

static s32 fdrStartThreadSend(BrdFdrRsrc_T*);
static void fdrTaskSend(BrdFdrRsrc_T*, u8);

/**********************************************************
 Public function
**********************************************************/
void BrdFdrSetup(
	void *pDev,
	const char* NAME,
	cSPIN_DEV_T* stprs,
	const PIN_T* pinIn,
	const PIN_T* pinOut,
	void (*printLCD)(const char* FORMAT_ORG, ...),
	s8 (*EepromRead)   (u16 addr, u8 *pDat, u16 nBytes),
	s8 (*EepromWrite)  (u16 addr, const u8 *pDat, u16 nBytes),
	u16 romBase
){
	BrdFdrDev_T* p = (BrdFdrDev_T*) pDev;
	BrdFdrRsrc_T* rsrc = &p->rsrc;
	memset(p, 0, sizeof(BrdFdrDev_T));
	strcpy(p->rsrc.name, NAME);
	
	rsrc->stprX = &stprs[1];
	rsrc->stprR = &stprs[0];
	rsrc->IN = pinIn;
	rsrc->OUT = pinOut;
	
	rsrc->RegRead = EepromRead;
	rsrc->RegWrite = EepromWrite;
	rsrc->romBase = romBase;
	
//	rsrc->IN[0] = &port1->PD0;
//	rsrc->IN[1] = &port1->PD1;
//	rsrc->IN[2] = &port1->PC0;
//	rsrc->IN[3] = &port1->PC1;
//	rsrc->IN[4] = &port1->PA1;
	
	rsrc->print = printLCD;
	rsrc->CbInputFalling = fdrCbInputFalling;
	rsrc->CbInputRaising = fdrCbInputRaising;	
	
//	rsrc->OUT[0] = &port1->PD3;
//	rsrc->OUT[1] = &port1->PD2;
//	rsrc->OUT[2] = &port1->PC3;
//	rsrc->OUT[3] = &port1->PC2;
	
//	as_INPUT_PULLUP(*rsrc->IN[0]);
//	as_INPUT_PULLUP(*rsrc->IN[1]);
//	as_INPUT_PULLUP(*rsrc->IN[2]);
//	as_INPUT_PULLUP(*rsrc->IN[3]);
//	as_INPUT_PULLUP(*rsrc->IN[4]);
//
//	as_OUTPUT_PP_NOPULL_LOW(*rsrc->OUT[0]);
//	as_OUTPUT_PP_NOPULL_LOW(*rsrc->OUT[1]);
//	as_OUTPUT_PP_NOPULL_LOW(*rsrc->OUT[2]);
//	as_OUTPUT_PP_NOPULL_LOW(*rsrc->OUT[3]);
	
	p->Polling = brdFdrPolling;
	p->Stop = brdFdrStop;
	p->StopRecover = brdFdrRecover;
	p->writePin = brdFdrWritepin;
	p->togglePin = brdFdrTogglepin;
	p->readPin = brdFdrReadpin;
	p->allStop = brdFdrAllStop;
	p->startThreadSend = fdrStartThreadSend;
	p->TjcUIReset = TjcUIReset;
	p->TjcEepromWriteAll = TjcEepromWriteAll;
	p->TjcEepromReadAll = TjcEepromReadAll;
	p->TjcCounterInc = 	TjcCounterInc;
	p->updateStatus = updateStatus;

	rsrc->squ = 0;
	rsrc->mode = 0;
	rsrc->dbgEn = 1;

	TjcEepromReadAll(rsrc);

	// set songliao speed
	rsrc->stprX->Soft_Stop(&rsrc->stprX->rsrc);
	rsrc->stprR->Soft_Stop(&rsrc->stprR->rsrc);
	rsrc->stprX->RegSetMaxSpd_stepPerS(&rsrc->stprX->rsrc, songliaoxianweiToStep(p->rsrc.songliaoshudu));
	
	TjcUIReset(rsrc);

	fdrStartThreadSend(rsrc);
}
/**********************************************************
 read data
**********************************************************/
/**********************************************************
 fdrFb6480Polling
**********************************************************/
#define TJC_BUFF_LEN 200
#define FDR_SQU_MAN	100
static u16 testTick = 0;
static u16 senseToggleTick;

static s32 brdFdrPolling(BrdFdrRsrc_T* p, u16 tick) {
	u8 i;
	// tag sense and remove action
	for(i=0;i<FDR_IN_CNT;i++){
		p->inSeries[i] <<= 1;
		if(readPin(p->IN[i]) == GPIO_PIN_RESET){	p->inSeries[i] |= BIT(0);	}
		if((p->inSeries[i] & 0x03) == 0x01 && p->CbInputRaising){
			p->CbInputRaising(i);
			// updateStatus(p, "raise", 0);
		}
		else if((p->inSeries[i] & 0x03) == 0x02 && p->CbInputFalling){	
			p->CbInputFalling(i);
			// updateStatus(p, "failling", 0);
		}
	}

	// label sense
	if((p->inSeries[FER_IN_TAG_SNS] & (BIT(0)|BIT(3))) == BIT(0)){
		UI_tagFlash(p,1);	//start flash label
	}
	// label remove
	else if((p->inSeries[FER_IN_TAG_SNS] & (BIT(0)|BIT(3))) == BIT(3)){
		UI_tagFlash(p,0);	//stop flash label
	}
	
	fdrTaskSend(p, tick);

	return 0;
}

static s32 fdrStartThreadSend(BrdFdrRsrc_T* p){
	p->tSendSqu = 1;
	return 0;
}

static void fdrTaskSend(BrdFdrRsrc_T* p, u8 t){
	switch (p->tSendSqu){
		case 0:	// polling to start sending
			p->event &= 0xffffffff^BIT(0);
			p->tSendSqu = 1;
		case 1:	// wait for send signal
			if(p->stopLock){
				updateStatus(p, "E-Stop..", 3);
				p->event = 0;
				break;
			}
			if(p->inSeries[FER_IN_TAG_SNS]){	break;	}
			brdFdrWritepin(p, FDR_OUT_SEND, 0);	// no label sense any more
			if((p->mode==1) && ((p->inSeries[FER_IN_TAG_SNS] & (BIT(0) | BIT(3))) == BIT(3)) ){	// manual mode
				// auto send tag while tag senseless
				p->tSendSqu++;
			}
			else if((p->mode==1) && ((p->event & BIT(0)) == BIT(0))){	// manual mode
				// auto send tag while tag senseless
				p->tSendSqu++;
				p->event &= 0xffffffff^BIT(0);
			}
			else if((p->mode==0) && ((p->inSeries[FER_IN_SEND] & (BIT(0) | BIT(3))) == BIT(0)) ){
				if(p->inSeries[FER_IN_TAG_SNS] == 0x00)
					p->tSendSqu++;
			}
			break;
		case 2:
			updateStatus(p, "start sending..", 3);
			brdFdrWritepin(p, FDR_OUT_SEND, 0);	// tells start sending
			p->stprX->Move_Step(&p->stprX->rsrc, (cSPIN_Direction_T)1, songliaoxianweiToStep(p->songliaoxianwei));
			p->print("sendSqu=1");	// LCD "send" icon start flash
			p->stprR->Run(&p->stprR->rsrc, (cSPIN_Direction_T)1, 500);
			p->tSendSqu++;
			break;
		case 3:
			if((p->inSeries[FER_IN_TAG_SNS] & (BIT(0)|BIT(3))) == BIT(0) && p->songliaobuchang>0){
				p->stprX->Hard_Stop(&p->stprX->rsrc);
				p->stprX->Move_Step(&p->stprX->rsrc, (cSPIN_Direction_T)1, songliaobuchangToStep(p->songliaobuchang));
				p->tSendSqu++;
				// brdFdrWritepin(p, FDR_OUT_SEND, 1);	// tells start sending complete
			}
			else if(((p->stprX->Get_Status(&p->stprX->rsrc)>>5) & 0x03) == 0){
				p->tSendSqu = 0;
				p->print("sendSqu=0");	// LCD "send" icon stop flash
				p->stprR->Soft_Stop(&p->stprR->rsrc);
				updateStatus(p, "senseless", 2);
			}
			break;
		case 4:
			if(((p->stprX->Get_Status(&p->stprX->rsrc)>>5) & 0x03) == 0){
				p->print("sendSqu=0");	// LCD "send" icon stop flash
				p->stprR->Hard_Stop(&p->stprR->rsrc);
				p->tSendSqu = 0;
				brdFdrWritepin(p, FDR_OUT_SEND, 1);	// tells start sending complete
			}
			break;
		case 99:
			p->stprX->Hard_Stop(&p->stprX->rsrc);
			p->stprR->Hard_Stop(&p->stprR->rsrc);
			p->tSendSqu = 0;
			break;
		default:
			p->tSendSqu = 0;
			break;
	}
}

static void updateStatus(BrdFdrRsrc_T* p, const char* INFO, u8 tmr){
	p->print("feeder.tStatus.txt=\"%s\"", INFO);	// update status
	if(tmr==0)	return;
	p->print("feeder.StatusClearTmr=%d", tmr);	// auto disappear
}

/**********************************************************
 frdFb6480Stop
**********************************************************/
static void brdFdrStop(BrdFdrRsrc_T* p){
	p->stprR->Hard_Stop(&p->stprR->rsrc);
	p->stprX->Hard_Stop(&p->stprX->rsrc);
	p->stopLock = 1;
}

static void brdFdrRecover(BrdFdrRsrc_T* p){
	p->stopLock = 0;
	p->squ = 0;
//	p->mode = 0;
//	p->dbgEn = 1;
//	TjcEepromReadAll(p);
//	TjcUIReset(p);
}

static void brdFdrWritepin(BrdFdrRsrc_T* p, u8 indx, u8 sta){
	if(indx > 4)	return;
	if(sta)	writePin(p->OUT[indx], GPIO_PIN_SET);
	else	writePin(p->OUT[indx], GPIO_PIN_RESET);
}

static void brdFdrTogglepin(BrdFdrRsrc_T* p, u8 indx){
	if(indx > 4)	return;
	togglePin(p->OUT[indx]);
}

static GPIO_PinState brdFdrReadpin(BrdFdrRsrc_T* p, u8 indx){
	if(indx > 5)	return 0;
	return(readPin(p->IN[indx]));
}

static void brdFdrAllStop(BrdFdrRsrc_T* p){
	p->stprR->Soft_Stop(&p->stprR->rsrc);
	p->stprX->Soft_Stop(&p->stprX->rsrc);
}

static void brdFdrAllStopRelease(BrdFdrRsrc_T* p){

}

// UI reset
static void TjcUIReset(BrdFdrRsrc_T* p){

	p->print("page feeder");
	
	// songliaobuchang
	p->print("feeder.t4.txt=\"%d.%d\"", p->songliaobuchang/10, p->songliaobuchang%10);
	HAL_Delay(1);
	// songliaoxianwei
	p->print("feeder.t5.txt=\"%d\"", p->songliaoxianwei);
	HAL_Delay(1);
	// songliaoshudu
	p->print("feeder.t6.txt=\"%d\"", p->songliaoshudu);
	HAL_Delay(1);
	// shouliaoshudu
	p->print("feeder.t7.txt=\"%d\"", p->shouliaoshudu);
	HAL_Delay(1);

	// status
	p->print("feeder.tStatus.txt=\"Initial Done\"");
	HAL_Delay(1);
	
	// e-Stop up
	p->print("feeder.p2.pic=31");
	HAL_Delay(1);
	
	// set psw 
	p->print("psw=%d", p->psw);
	HAL_Delay(1);
	
	// mode: auto
	p->print("feeder.p6.pic=1");
	HAL_Delay(1);
	p->mode = 0;
	
	// lock
	p->lock = 1;
	p->print("feeder.tLock.pic=4");
	HAL_Delay(1);
	p->print("feederLock=1");
	HAL_Delay(1);
	
	// counter
	p->print("feeder.tCounter.txt=\"%d\"", p->counter);

	UI_tagFlash(p,0);	//stop flash label
	p->print("sendSqu=0");	HAL_Delay(1);

//	// mainUI button
//	monitor->SendCmd(mRsrc, "mainUI.p2.pic=56");	HAL_Delay(1); //send button
//	monitor->SendCmd(mRsrc, "mainUI.p3.pic=60");	HAL_Delay(1); //stop button
//	monitor->SendCmd(mRsrc, "mainUI.p4.pic=54");	HAL_Delay(1); //pop button

//	monitor->SendCmd(mRsrc, "lock=1");	HAL_Delay(1);
//	monitor->SendCmd(mRsrc, "mainUI.t12.pic=63");	HAL_Delay(1);
}

static void TjcEepromWriteAll(BrdFdrRsrc_T* pRsrc){
	u32 checksum = 0, check = 1;
	u32 val[20] = {0}, i;

	if(pRsrc->RegWrite == NULL)	return -1;

	i = 0;
	val[i++] = pRsrc->counter;
	val[i++] = pRsrc->songliaobuchang;
	val[i++] = pRsrc->songliaoxianwei;
	val[i++] = pRsrc->songliaoshudu;
	val[i++] = pRsrc->shouliaoshudu;
	val[i++] = pRsrc->boliaochidu;
	val[i++] = pRsrc->psw;

	val[i++] = pRsrc->mode;
	val[i++] = pRsrc->lock;

	val[i++] = pRsrc->stprCfg[0].constSpd;
	val[i++] = pRsrc->stprCfg[1].constSpd;

	val[i++] = pRsrc->stprCfg[0].position;
	val[i++] = pRsrc->stprCfg[1].position;

	val[i++] = pRsrc->stprCfg[0].runMod;
	val[i++] = pRsrc->stprCfg[1].runMod;

	val[i++] = pRsrc->stprCfg[0].stopMod;
	val[i++] = pRsrc->stprCfg[1].stopMod;

	for(i=0;i<16;i++)	checksum += val[i];

	// there are 21 u32 save, including checksum
	pRsrc->RegWrite(pRsrc->romBase, (u8*)&checksum, 4);
	pRsrc->RegWrite(pRsrc->romBase + 4, (u8*)val, 16*sizeof(u32));

	return 0;
}

static s8 TjcEepromReadAll(BrdFdrRsrc_T* pRsrc){
	u32 checksum = 0, check = 1;
	u32 val[16] = {0},i;
	
	if(pRsrc->RegRead == NULL)	return -1;

	// there are 21 u32 save, including checksum
	pRsrc->RegRead(pRsrc->romBase, (u8*)&check, 4);
	pRsrc->RegRead(pRsrc->romBase + 4, (u8*)val, 16*sizeof(u32));

	for(i=0;i<16;i++)	checksum += val[i];

	i = 0;
	if(checksum == check){
		pRsrc->counter = val[i++];
		pRsrc->songliaobuchang = val[i++];
		pRsrc->songliaoxianwei = val[i++];
		pRsrc->songliaoshudu = val[i++];
		pRsrc->shouliaoshudu = val[i++];
		pRsrc->boliaochidu = val[i++];
		pRsrc->psw = val[i++];
		
		pRsrc->mode = val[i++];
		pRsrc->lock = val[i++];

		pRsrc->stprCfg[0].constSpd = val[i++];
		pRsrc->stprCfg[1].constSpd = val[i++];

		pRsrc->stprCfg[0].position = val[i++];
		pRsrc->stprCfg[1].position = val[i++];

		pRsrc->stprCfg[0].runMod = val[i++];
		pRsrc->stprCfg[1].runMod = val[i++];

		pRsrc->stprCfg[0].stopMod = val[i++];
		pRsrc->stprCfg[1].stopMod = val[i++];
		return 0;
	}
	return -2;
}

static void TjcCounterInc(BrdFdrRsrc_T* pRsrc){
	pRsrc->counter++;
	TjcEepromWriteAll(pRsrc);
	pRsrc->print("mainUI.counter.txt=\"%d\"", pRsrc->counter);
}

// UI function
static s8 UI_tagFlash(BrdFdrRsrc_T* pRsrc, u8 en){
	if(en)	pRsrc->print("labelSqu=1");
	else{
		pRsrc->print("labelSqu=0");
		pRsrc->print("vis pLabel,0");
	}
	return 0;
}

static s8 UI_sendingFlash(BrdFdrRsrc_T* pRsrc, u8 en){
	if(en)	pRsrc->print("sendSqu=1");
	else	pRsrc->print("sendSqu=0");
	return 0;
}

static s8 UI_strippFlash(BrdFdrRsrc_T* pRsrc, u8 en){
	if(en)	pRsrc->print("strippSqu=1");
	else	pRsrc->print("strippSqu=0");
	return 0;
}

static s8 UI_eStopFlash(BrdFdrRsrc_T* pRsrc, u8 en){
	if(en)	pRsrc->print("eStopSqu=1");
	else	pRsrc->print("eStopSqu=0");
	HAL_Delay(1);
	return 0;
}


static void fdrCbInputFalling(u8 indx){
	// print("falling@%d\n", indx);
}

static void fdrCbInputRaising(u8 indx){
	// print("raising@%d\n", indx);
}

static void TjcMotionReload(BrdFdrRsrc_T* p, u8 axis){

}

// 6.28mm = 200step  => 0.0314mm/Step
// len in mm
u32 boliaochiduToStep(u16 len){
	u32 rtn = len;	
	rtn = (len*10000)/314;
	return rtn;
}

// 6.11mm = 200step  => 0.0306mm/Step
// len in mm
u32 songliaoxianweiToStep(u16 len){
	u32 rtn = len;	
	rtn = (len*10000)/306;
	return rtn;
}

// 6.11mm = 200step  => 0.0306mm/Step
// len in 0.1mm
u32 songliaobuchangToStep(u16 len){
	u32 rtn = len;	
	rtn = (len*1000)/306;
	return rtn;
}

/**********************************************************
 == THE END ==
**********************************************************/

