/*
 * myRamp.c
 *
 */
#include <stdlib.h>
#include <string.h>
#include "myRamp.h"
#include "gpioDecal.h"
#include "board.h"
#include "thread_delay.h"
#include "user_log.h"

#define RAMP_MIN_SPD    (0xffff/2)   //(0xFFFF*12/16)
#define RAMP_MAX_SPD    (0xFFFF-0x0F)

typedef struct{
    const u16 len;
    const u8* TAB;
} _rampTab_t;

#define RAMP_TAB_LEN_SIGMA167   (167)
const u8 RAMP_TAB_SIGMA167[RAMP_TAB_LEN_SIGMA167] = {
1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,17,18,20,21,23,24,26,28,30,32,35,37,39,42,45,47,50,54,57,60,64,67,71,75,79,83,87,91,96,100,105,110,114,119,124,129,134,139,144,150,155,160,165,170,175,180,185,190,195,200,204,209,213,217,221,225,229,232,235,238,241,244,246,248,250,251,253,254,255,254,253,251,250,248,246,244,241,238,235,232,229,225,221,217,213,209,204,200,195,190,185,180,175,170,165,160,155,150,144,139,134,129,124,119,114,110,105,100,96,91,87,83,79,75,71,67,64,60,57,54,50,47,45,42,39,37,35,32,30,28,26,24,23,21,20,18,17,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1
};

#define RAMP_TAB_LEN_SIGMA120   (120)
const u8 RAMP_TAB_SIGMA120[RAMP_TAB_LEN_SIGMA120] = {
7,8,9,9,10,11,12,13,14,15,17,18,20,21,23,24,26,28,30,32,35,37,39,42,45,47,50,54,57,60,64,67,71,75,79,83,87,91,96,100,105,110,114,119,124,129,134,139,144,150,155,160,165,170,175,180,185,190,195,200,200,195,190,185,180,175,170,165,160,155,150,144,139,134,129,124,119,114,110,105,100,96,91,87,83,79,75,71,67,64,60,57,54,50,47,45,42,39,37,35,32,30,28,26,24,23,21,20,18,17,15,14,13,12,11,10,9,9,8,7
};

#define RAMP_TAB_LEN_SINE64   (64)
const u8 RAMP_TAB_SINE64[RAMP_TAB_LEN_SINE64] = {
3,4,5,9,13,18,25,32,41,50,60,71,82,94,106,119,131,144,156,168,179,190,201,211,220,228,235,241,245,249,252,253,253,252,249,245,241,235,228,220,211,201,191,179,168,156,144,131,119,106,94,82,71,60,50,41,32,25,18,13,9,5,4,3
};
#define RAMP_TAB_LEN_SINE128   (128)
const u8 RAMP_TAB_SINE128[RAMP_TAB_LEN_SINE128] = {
1,1,2,2,4,5,7,9,11,14,16,20,23,27,31,35,39,44,49,54,59,64,70,76,82,87,94,100,106,112,118,125,131,137,143,150,156,162,168,174,179,185,190,196,201,206,211,215,219,223,227,231,234,237,240,242,244,246,248,249,250,251,251,251,251,250,249,248,246,244,242,240,237,234,231,227,223,219,215,210,205,200,195,190,185,179,173,167,161,155,149,143,137,130,124,118,111,105,99,93,87,81,75,70,64,59,53,48,44,39,35,30,26,23,19,16,13,11,8,6,5,3,2,2,1,1
};
const _rampTab_t RAMP_TAB[4] = {
    {RAMP_TAB_LEN_SIGMA167, RAMP_TAB_SIGMA167   },
    {RAMP_TAB_LEN_SINE64,   RAMP_TAB_SINE64     },
    {RAMP_TAB_LEN_SINE128,  RAMP_TAB_SINE128    },
    {RAMP_TAB_LEN_SIGMA120, RAMP_TAB_SIGMA120   },
};

//#define RAMP_TAB_LEN    (202)
//const u8 RAMP_TAB[RAMP_TAB_LEN] = {
//1,1,1,1,2,2,2,2,2,3,3,3,3,4,4,5,5,6,6,7,7,8,9,9,10,11,12,13,14,15,17,18,20,21,23,24,26,28,30,32,35,37,39,42,45,47,50,54,57,60,64,67,71,75,79,83,87,91,96,100,105,110,114,119,124,129,134,139,144,150,155,160,165,170,175,180,185,190,195,200,204,209,213,217,221,225,229,232,235,238,241,244,246,248,250,251,253,254,254,255,255,255,255,254,254,253,251,250,248,246,244,241,238,235,232,229,225,221,217,213,209,204,200,195,190,185,180,175,170,165,160,155,150,144,139,134,129,124,119,114,110,105,100,96,91,87,83,79,75,71,67,64,60,57,54,50,47,45,42,39,37,35,32,30,28,26,24,23,21,20,18,17,15,14,13,12,11,10,9,9,8,7,7,6,6,5,5,4,4,3,3,3,3,2,2,2,2,2,1,1,1,1
//};

static void ramp_isr(rampRsrc_t* rsrc, TIM_HandleTypeDef *htim);
static void ramp_isrFallingRefL(rampRsrc_t* rsrc, u16 GPIO_Pin);
static void ramp_isrRaisingRefL(rampRsrc_t* rsrc, u16 GPIO_Pin);
static void ramp_isrFallingRefR(rampRsrc_t* rsrc, u16 GPIO_Pin);
static void ramp_isrRaisingRefR(rampRsrc_t* rsrc, u16 GPIO_Pin);

static void ramp_gohome(rampRsrc_t* rsrc, u16 maxSpd);
static void ramp_homing(rampRsrc_t* rsrc, u16 maxSpd);
static u8 ramp_isHoming(rampRsrc_t* r);

static s32 ramp_rotateL(rampRsrc_t* r, u16 targetSpd);
static s32 ramp_rotateR(rampRsrc_t* r, u16 targetSpd);
static s32  ramp_moveTo(rampRsrc_t* rsrc, s32 targetPos);
static s32 ramp_moveBy(rampRsrc_t* rsrc, s32 refPos);
static u8 ramp_isRotating(rampRsrc_t* r);

static void ramp_stopSoft(rampRsrc_t* r);
static void ramp_stop(rampRsrc_t* r);
static void ramp_testMul(rampRsrc_t* r, u16 delta);
static void ramp_dirL(rampRsrc_t* r);
static void ramp_dirR(rampRsrc_t* r);

static s32 ramp_setSpeedTab(rampRsrc_t* r, rampTabIndx tabIdx);

static u8 ramp_getRefl(rampRsrc_t* r);
static u8 ramp_getDir(rampRsrc_t* r);

static u8 ramp_isShelteredRefL(rampRsrc_t* r);
static u8 ramp_isShelteredRefR(rampRsrc_t* r);
static u8 ramp_isSheltered(rampRsrc_t* r);

//static u16 rampDiv = 0;
//static u16 ramp_computeDiv(rampTabIndx tabIdx);

static void ramp_rotate(rampRsrc_t* r, u16 targetSpd);
static void standaloe_rotate(rampRsrc_t* r, u16 targetSpd);
static void ramp_test(rampRsrc_t* r, u16 spd);

#define DIR_LEFT_LEVEL    (GPIO_PIN_RESET)

s32 rampSetup(
    void* p,
    const char* NAME,
    TIM_HandleTypeDef *htim,
    u32 tCh,
    const PIN_T* DIR,
    const PIN_T* REFL,
    const PIN_T* REFR,
    u16 microStep
){
    rampDev_t* d = (rampDev_t*)p;
    rampRsrc_t* r = &d->rsrc;
    memset(p,0,sizeof(rampDev_t));
    r->htim = htim;
    r->tCh = tCh;
    r->DIR = DIR;
    r->REFL = REFL;
    r->REFR = REFR;
    r->mStep = microStep;
    strcpy(r->name, NAME);
    r->en = 1;

    r->pulsePerSpot = 10;
    r->spdMin = RAMP_MIN_SPD;
    r->spdMax = 65500;
    r->isHoming = 0;

    d->isr = ramp_isr;
    d->isrReleasedRefL = ramp_isrFallingRefL;
    d->isrShelteredRefL = ramp_isrRaisingRefL;
    d->isrReleasedRefR = ramp_isrFallingRefR;
    d->isrShelteredRefR = ramp_isrRaisingRefR;

    d->gohome = ramp_gohome;
    d->homing = ramp_homing;
    d->isHoming = ramp_isHoming;
    d->rotateL = ramp_rotateL;
    d->rotateR = ramp_rotateR;
    d->isRotating = ramp_isRotating;
    d->stop = ramp_stop;
    d->stopSoft = ramp_stopSoft;
    
    d->moveBy = ramp_moveBy;
    d->moveTo = ramp_moveTo;
    
    d->getDir = ramp_getDir;
    d->getRefl = ramp_getRefl;
    d->isSheltered = ramp_isSheltered;

    d->testMul = ramp_testMul;
    d->test = ramp_test;
    d->setSpeedTab = ramp_setSpeedTab;
    
    ramp_dirR(r);
    ramp_setSpeedTab(r, TAB_SIGMA202);

    // do NOT use "HAL_TIM_OnePulse_Start_IT" !!!
    if (HAL_TIM_OnePulse_Start(htim, tCh) != HAL_OK){    r->error |= BIT(0);    }
        
    return 0;
}

static void ramp_gohome(rampRsrc_t* r, u16 maxSpd){
    r->spdMax = maxSpd;
    ramp_moveTo(r,0);
}

static void ramp_homing(rampRsrc_t* r, u16 maxSpd){
    r->isHoming = 1;
    if(ramp_isShelteredRefL(r)){
        ramp_rotateR(r, RAMP_MIN_SPD);
        while(1){
            if(ramp_isShelteredRefL(r) == 0){
                break;
            }
            thread_delay(16);
        }
        ramp_rotateL(r, RAMP_MIN_SPD);
        while(1){
            if(ramp_isShelteredRefL(r)){
                r->isHoming = 0;
                r->posCur = 0;
                break;
            }
            thread_delay(16);
        }
    }
    else{
        ramp_rotateL(r,maxSpd);
        while(ramp_isShelteredRefL(r)==0){
            thread_delay(16);
        }
        ramp_homing(r, maxSpd);
    }
}

static u8 ramp_isHoming(rampRsrc_t* r){
    return (r->isHoming);
}

static void ramp_dirL(rampRsrc_t* r){
    // print("turn left\n");
    if(DIR_LEFT_LEVEL){
        if(r->DIR)    writePin(r->DIR, GPIO_PIN_SET);
    }
    else{
        if(r->DIR)    writePin(r->DIR, GPIO_PIN_RESET);
    }
    r->status &= (0xff^BIT(RAMP_STATUS_BIT_DIR));
}

static void ramp_dirR(rampRsrc_t* r){
    // print("turn right\n");
    if(DIR_LEFT_LEVEL){
        if(r->DIR)    writePin(r->DIR, GPIO_PIN_RESET);
    }
    else{
        if(r->DIR)    writePin(r->DIR, GPIO_PIN_SET);
    }
    r->status |= BIT(RAMP_STATUS_BIT_DIR);
}

static s32 ramp_setSpeedTab(rampRsrc_t* r, rampTabIndx tabIdx){
    u16 i;
    
    if(tabIdx > TAB_SINE128){
        return -1;
    }
    
    r->TAB = RAMP_TAB[tabIdx].TAB;
    r->tabLen = RAMP_TAB[tabIdx].len;
    r->div = 0;
    for(i=0;i<r->tabLen;i++){    
        r->div += r->TAB[i];     
    }
    return 0;
}

static void ramp_testMul(rampRsrc_t* r, u16 delta){
//    u8 i;
//    u16 x, tmp;
//    s32 sum = 0;
//    tmp = r->mul;

//    r->mul = ramp_computeMul(delta, r->comp);
//    for(i=0;i<RAMP_TAB_LEN;i++){
//        x = RAMP_TAB[i]*r->mul/rampDiv;
////        if(i>=RAMP_COMP_BASE && i<RAMP_COMP_BASE+4){    x += r->comp[i-RAMP_COMP_BASE];    }
//        sum += x;
//        print("%d\n",x);
//        console.TxPolling(&console.rsrc);
////        HAL_IWDG_Refresh(&hiwdg);
//    }
//    r->mul = tmp;
//    print("comp:%d\t%d\t%d\t%d\n", r->comp[0],r->comp[1],r->comp[2],r->comp[3]);
//    print("delta:%d\t%d\t%d\t%d\n", delta, sum, r->mul, rampDiv);
}

static void ramp_rotate(rampRsrc_t* r, u16 targetSpd){
    log("<%s targetSpd:%d >", __func__, targetSpd);
    u32 x,DIV;
    u16 spdStep;
    s32 delta;
    r->stopImmeditely = 0;

    r->tmrISR_Lock = 1;         // doNOT allow ISR
    r->spdTgt = targetSpd;      // load new target speed

    // will start from RAMP_MIN_SPD
    if(r->spdCur == 0){
        r->spdCur = r->spdMin;
    }
    
    r->mul = abs(r->spdTgt - r->spdCur);
    r->spdMod = 0;
    r->rampIndx = 0;
    
    r->pulseCount = 0;
    spdStep = (r->mul * r->TAB[r->rampIndx] + r->spdMod)/r->div;
    r->spdMod = (r->mul * r->TAB[r->rampIndx] + r->spdMod)%r->div;
    r->rampIndx++;

    // acc
    if(r->spdCur < r->spdTgt){
        r->spdCur += spdStep;
        if(r->spdCur > r->spdTgt){
            r->spdCur = r->spdTgt;
        }
    }
    // dec
    else if(r->spdCur > r->spdTgt){
        r->spdCur -= spdStep;
        if(r->spdCur < r->spdTgt){
            r->spdCur = r->spdTgt;
        }
    }
    
    while(1){
        if( HAL_TIM_Base_GetState(r->htim) == HAL_TIM_STATE_READY){
            r->tmrISR_Lock = 0;
            x = 0xffff - r->spdCur;
            __HAL_TIM_SET_AUTORELOAD(r->htim, x);
            __HAL_TIM_SET_COMPARE(r->htim, r->tCh, x/2);
            HAL_TIM_Base_Stop_IT(r->htim);
            HAL_TIM_Base_Start_IT(r->htim);
            break;
        }    
        thread_delay(4);
    }
    
    log("</%s >", __func__);
}

static void standaloe_rotate(rampRsrc_t* r, u16 targetSpd){
    u16 x;
    r->tmrISR_Lock = 1;
    r->spdCur = targetSpd;
    r->stopImmeditely = 0;
    if( HAL_TIM_Base_GetState(r->htim) == HAL_TIM_STATE_READY){
        x = 0xffff - r->spdCur;
        __HAL_TIM_SET_AUTORELOAD(r->htim, x);
        __HAL_TIM_SET_COMPARE(r->htim, r->tCh, x/2+1);
        HAL_TIM_Base_Stop_IT(r->htim);
        HAL_TIM_Base_Start_IT(r->htim);
    }
}

static s32 ramp_rotateL(rampRsrc_t* r, u16 targetSpd){
    log("<%s >", __func__);
    r->runMod = RAMP_RUN_MOD_SPD;    // SPEED MODE
        
    if((HAL_GPIO_ReadPin(r->DIR->GPIOx, r->DIR->GPIO_Pin) != DIR_LEFT_LEVEL) && (r->spdCur > RAMP_MIN_SPD)){
        log("<%s '!= DIR_LEFT_LEVEL'>", __func__);
        ramp_rotate(r, RAMP_MIN_SPD);
        while(1){
            if((r->spdCur == RAMP_MIN_SPD) || (r->spdCur == 0)){
                break;
            }
            thread_delay(16);
        }
    }
    ramp_dirL(r);
    ramp_rotate(r, targetSpd);
    log("</%s >", __func__);
    return 0;
}

static s32 ramp_rotateR(rampRsrc_t* r, u16 targetSpd){
    log("<%s >", __func__);
    r->runMod = RAMP_RUN_MOD_SPD;    // SPEED MODE
    if((HAL_GPIO_ReadPin(r->DIR->GPIOx, r->DIR->GPIO_Pin) == DIR_LEFT_LEVEL) && ((r->spdCur > RAMP_MIN_SPD))){
        log("<%s '== DIR_LEFT_LEVEL'>", __func__);
        ramp_rotate(r, RAMP_MIN_SPD);
        while(1){
            if((r->spdCur == RAMP_MIN_SPD) || (r->spdCur == 0)){
                break;
            }
            thread_delay(16);
        }
    }
    ramp_dirR(r);
    ramp_rotate(r, targetSpd);
    log("</%s >", __func__);
    return 0;
}

static s32 ramp_moveTo(rampRsrc_t* r, s32 targetPos){
    log("<%s spdMax:%d >", __func__, r->spdMax);
    s32 x = targetPos;
    if(targetPos == r->posCur){    return 0;    }

    if(r->posCur < targetPos){
        ramp_dirR(r);
    }
    else if(r->posCur > targetPos){
        ramp_dirL(r);
    }
    
    r->posTgt = targetPos;
    r->posAbsPrv = 0xffffffff;
    
    r->runMod = RAMP_RUN_MOD_POS;
    r->status &= (0xff ^ BIT(RAMP_STATUS_BIT_POS_DEC));
        
    ramp_rotate(r, r->spdMax);  //65000
    log("</%s spdMax:%d>", __func__, r->spdMax);
    return 0;
}

static s32 ramp_moveBy(rampRsrc_t* r, s32 refPos){
    return(ramp_moveTo(r, r->posCur+refPos));
}

static u8 ramp_isRotating(rampRsrc_t* r){
    if(r->status&BIT(RAMP_STATUS_BIT_RUN)){    return 1;}
    return 0;
}

static void ramp_stopSoft(rampRsrc_t* r){
    if(r->spdCur > RAMP_MIN_SPD){
        ramp_rotate(r,0);
    }
    while(1){
        if(r->spdCur == RAMP_MIN_SPD){
            break;
        }
        thread_delay(16);
    }
    r->runMod = RAMP_RUN_MOD_STOP;
}

static void ramp_stop(rampRsrc_t* r){
    r->runMod = RAMP_RUN_MOD_STOP;
    r->squ = 0;
    r->isHoming = 0;
    r->stopImmeditely = 1;
}

static u8 ramp_isShelteredRefL(rampRsrc_t* r){
    if(HAL_GPIO_ReadPin(r->REFL->GPIOx, r->REFL->GPIO_Pin) == GPIO_PIN_RESET){    return 1;    }
    else{    return 0;    }
}

static u8 ramp_isShelteredRefR(rampRsrc_t* r){
    if(HAL_GPIO_ReadPin(r->REFR->GPIOx, r->REFR->GPIO_Pin) == GPIO_PIN_RESET){    return 1;    }
    else{    return 0;    }
}

static u8 ramp_isSheltered(rampRsrc_t* r){
    u8 sta = 0;
    if(ramp_isShelteredRefL(r)){
        sta |= BIT(0);
    }
    if(ramp_isShelteredRefR(r)){
        sta |= BIT(1);
    }
    return sta;
}

static u8 ramp_getRefl(rampRsrc_t* r){
    return(HAL_GPIO_ReadPin(r->REFL->GPIOx, r->REFL->GPIO_Pin));
}

static u8 ramp_getDir(rampRsrc_t* r){
    return(readPin(r->DIR));
}

static void ramp_test(rampRsrc_t* r, u16 spd){
//    u32 x;
//    x = ramp_computeMul(spd, r->comp);
//    r->pulsePerRampCycle = ramp_pulseCycle(r, x);
}

static void ramp_isr(rampRsrc_t* r, TIM_HandleTypeDef *htim){
    u16 x, spdOld,spdStep;
    u32 absPos = 0;
    u8 spdManageInitial = 0;

    if((r->en == 0) || (htim != r->htim))    return;

    // stop first
    HAL_TIM_Base_Stop_IT(htim);
    // reset speed status
    r->status &= (0xff^BIT(RAMP_STATUS_BIT_RUN));
    r->status &= (0xff^BIT(RAMP_STATUS_BIT_ACC));
    r->status &= (0xff^BIT(RAMP_STATUS_BIT_DEC));

    r->pulseCount++;
    if((r->status & BIT(RAMP_STATUS_BIT_DIR)) == 0){    r->posCur--;    }
    else {    r->posCur++;    }

//log("%d", r->posCur);    
    
    if(r->tmrISR_Lock){
        return;
    }
    
    if(r->runMod == RAMP_RUN_MOD_STOP){
        r->spdCur = 0;
        return;
    }
    
    // position manage
    else if(r->runMod == RAMP_RUN_MOD_POS){
        // start to apply a dec to stop
        absPos = abs(r->posTgt - r->posCur);
        if((absPos == 0) || (absPos >= r->posAbsPrv)){
            r->spdCur = 0;
            return;
        }
        r->posAbsPrv = absPos;
        if((absPos <= r->tabLen*r->pulsePerSpot) && ((r->status & BIT(RAMP_STATUS_BIT_POS_DEC))==0)){
            r->spdTgt = r->spdMin;
            r->mul = abs(r->spdTgt - r->spdCur);
            r->spdMod = 0;
            r->rampIndx = 0;
            spdManageInitial = 1;
            r->status |= BIT(RAMP_STATUS_BIT_POS_DEC);
        }
    }

    spdOld = r->spdCur;
    if((r->pulseCount >= r->pulsePerSpot) || spdManageInitial){
        r->pulseCount = 0;
        if(r->rampIndx < r->tabLen){
            spdStep = (r->mul * r->TAB[r->rampIndx] + r->spdMod)/r->div;
            r->spdMod = (r->mul * r->TAB[r->rampIndx] + r->spdMod)%r->div;
            r->rampIndx++;
        }
        // to prevcent from accident
        else if(r->spdCur != r->spdTgt){
            spdStep = 1;
        }
        // acc
        if(r->spdCur < r->spdTgt){
            r->spdCur += spdStep;
            if(r->spdCur > r->spdTgt){
                r->spdCur = r->spdTgt;
            }
        }
        // dec
        else if(r->spdCur > r->spdTgt){
            r->spdCur -= spdStep;
            if(r->spdCur < r->spdTgt){
                r->spdCur = r->spdTgt;
            }
        }
    }
   
    // start pwm timer
    x = 0xffff - r->spdCur;
    __HAL_TIM_SET_AUTORELOAD(r->htim, x);
    __HAL_TIM_SET_COMPARE(r->htim, r->tCh, x/2);
    HAL_TIM_Base_Stop_IT(r->htim);
    HAL_TIM_Base_Start_IT(r->htim);
    // update status
    r->status |= BIT(RAMP_STATUS_BIT_RUN);
    if(spdOld < r->spdCur){    r->status |= BIT(RAMP_STATUS_BIT_ACC);    }
    else if(spdOld > r->spdCur){    r->status |= BIT(RAMP_STATUS_BIT_DEC);    }
}

static void ramp_isrRaisingRefL(rampRsrc_t* r, u16 GPIO_Pin){
    if((r->en == 0) || (r->REFL->GPIO_Pin != GPIO_Pin)){    return;    }
    if(ramp_isShelteredRefL(r)){
        r->runMod = RAMP_RUN_MOD_STOP;
    }
}

static void ramp_isrFallingRefL(rampRsrc_t* r, u16 GPIO_Pin){
    if((r->en == 0) || (r->REFL->GPIO_Pin != GPIO_Pin)){    return;    }
    if(ramp_isShelteredRefL(r)){
        r->runMod = RAMP_RUN_MOD_STOP;
    }
}

static void ramp_isrRaisingRefR(rampRsrc_t* r, u16 GPIO_Pin){
    if((r->en == 0) || (r->REFR->GPIO_Pin != GPIO_Pin)){    return;    }
    if(ramp_isShelteredRefR(r)){
        r->runMod = RAMP_RUN_MOD_STOP;
    }
}

static void ramp_isrFallingRefR(rampRsrc_t* r, u16 GPIO_Pin){
    if((r->en == 0) || (r->REFR->GPIO_Pin != GPIO_Pin)){    return;    }
    if(ramp_isShelteredRefR(r)){
        r->runMod = RAMP_RUN_MOD_STOP;  
    }
}
