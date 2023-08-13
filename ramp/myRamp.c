/*
 * myRamp.c
 *
 */
#include <stdlib.h>
#include <string.h>
#include "myRamp.h"
#include "gpioDecal.h"
#include "board.h"

#define RAMP_TAB_LEN    (202)
#define RAMP_COMP_BASE  (99)

const u8 RAMP_TAB[RAMP_TAB_LEN] = {
1,1,1,1,2,2,2,2,2,3,3,3,3,4,4,5,5,6,6,7,7,8,9,9,10,11,12,13,14,15,17,18,20,21,23,24,26,28,30,32,35,37,39,42,45,47,50,54,57,60,64,67,71,75,79,83,87,91,96,100,105,110,114,119,124,129,134,139,144,150,155,160,165,170,175,180,185,190,195,200,204,209,213,217,221,225,229,232,235,238,241,244,246,248,250,251,253,254,254,255,255,255,255,254,254,253,251,250,248,246,244,241,238,235,232,229,225,221,217,213,209,204,200,195,190,185,180,175,170,165,160,155,150,144,139,134,129,124,119,114,110,105,100,96,91,87,83,79,75,71,67,64,60,57,54,50,47,45,42,39,37,35,32,30,28,26,24,23,21,20,18,17,15,14,13,12,11,10,9,9,8,7,7,6,6,5,5,4,4,3,3,3,3,2,2,2,2,2,1,1,1,1
};

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
static s32 ramp_moveBySpd(rampRsrc_t* r, s32 refPos, u16 spd);
static u8 ramp_isRotating(rampRsrc_t* r);

static void ramp_stopSoft(rampRsrc_t* r);
static void ramp_stop(rampRsrc_t* r);
static void ramp_periodJob(rampRsrc_t* r, u8 tick);
static void ramp_testMul(rampRsrc_t* r, u16 delta);
static void ramp_dirL(rampRsrc_t* r);
static void ramp_dirR(rampRsrc_t* r);

static u8 ramp_getRefl(rampRsrc_t* r);
static u8 ramp_getDir(rampRsrc_t* r);

static u8 ramp_isShelteredRefL(rampRsrc_t* r);
static u8 ramp_isShelteredRefR(rampRsrc_t* r);

static u16 rampDiv = 0;
static void ramp_computeDiv();
static u16 ramp_computeMul(u16 delta, u8* pComp);
static void ramp_updateSpdCur(rampRsrc_t* r);
static void ramp_rotate(rampRsrc_t* r, u16 targetSpd);
static void standaloe_rotate(rampRsrc_t* r, u16 targetSpd);
static u32 ramp_pulseCycle(rampRsrc_t* r, u16 mul);    // 返回当前mul设置,到达目标速度所需的pulse
static void ramp_test(rampRsrc_t* r, u16 spd);

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

    ramp_computeDiv();    // compute rampDiv
    r->pulsePerSpot = 10;
    r->spdMin = 10000;
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
    d->periodJob = ramp_periodJob;
    d->stop = ramp_stop;
    d->stopSoft = ramp_stopSoft;
    d->moveBy = ramp_moveBy;
    d->moveTo = ramp_moveTo;
    d->getDir = ramp_getDir;
    d->getRefl = ramp_getRefl;

    d->testMul = ramp_testMul;
    d->test = ramp_test;

    r->status |= BIT(RAMP_STATUS_BIT_DIR);
    ramp_dirR(r);

    // do NOT use "HAL_TIM_OnePulse_Start_IT" !!!
    if (HAL_TIM_OnePulse_Start(htim, tCh) != HAL_OK){    r->error |= BIT(0);    }
    
    return 0;
}

static void ramp_periodJob(rampRsrc_t* r, u8 tick){
    if(r->en == 0)    { return;    }
    switch(r->squ){
    case 0:
        break;
    case 1:        // change contrary direction
        if(r->spdCur > 0){    break;    }
        if(r->dirNxt == RAMP_DIR_RIGHT){    ramp_dirR(r);    }
        else{    ramp_dirL(r);        }
        ramp_rotate(r, r->spdTgtNxt);
        r->squ = 0;
        break;

    case 2:     // homing, wait stop
        if(r->spdCur > 0){    break;    }
        // rotate right by a very slow speed
        ramp_dirR(r);
        standaloe_rotate(r, 1000);
        r->squ ++;
        break;

    case 3:        // will reset squ in exti raising edge
        break;

    case 10:    // execute
        break;
    }
}

static void ramp_gohome(rampRsrc_t* r, u16 maxSpd){
    r->spdMax = maxSpd;
    ramp_moveTo(r,0);
}

static void ramp_homing(rampRsrc_t* r, u16 maxSpd){
    if(r->isHoming){    return;    }
    if(ramp_isShelteredRefL(r)){
        //print("homing by a very low speed\n");
        ramp_dirR(r);
        r->squ = 3;
        standaloe_rotate(r, 10000);
    }
    else{
        r->squ = 2;
        ramp_dirL(r);
        ramp_rotate(r,maxSpd);
    }
    r->isHoming = 1;
}

static u8 ramp_isHoming(rampRsrc_t* r){
    return (r->isHoming);
}

static void ramp_dirL(rampRsrc_t* r){
    // print("turn left\n");
    r->dirCur = 1;
    if(r->DIR)    writePin(*r->DIR, GPIO_PIN_SET);
    r->status |= BIT(RAMP_STATUS_BIT_DIR);
}
static void ramp_dirR(rampRsrc_t* r){
    // print("turn right\n");
    r->dirCur = 0;
    if(r->DIR)    writePin(*r->DIR, GPIO_PIN_RESET);
    r->status &= (0xff^BIT(RAMP_STATUS_BIT_DIR));
}

static void ramp_computeDiv(){
    u16 i;
    u16 x = 0;
    for(i=0;i<RAMP_TAB_LEN;i++){    x += RAMP_TAB[i];     }
    rampDiv = x;
}

static u16 ramp_computeMul(u16 delta, u8* pComp){
    u32 sum;
    s32 d,i;
    u32 mulX, restX;
    u16 mul;

    if(delta==0)    return 0;
    mul = delta;
    mulX = delta;
    restX = 0;
    memset(pComp,0,4);
    while(1){
        sum = 0;
        // RAMP_TAB is mirror, so we can do it like this
        for(i=0;i<RAMP_TAB_LEN/2;i++){    sum += RAMP_TAB[i]*mul/rampDiv;    }
        sum *= 2;

        d = delta - sum;
        if(d < 0){
            mul = mulX;
            i=0;
            while(restX>0){
                pComp[i++]++;
                restX--;
                if(i>3)    {    i = 0;    }
            }
            break;
        }
        else if(d == 0){    break;    }
        mulX = mul;
        restX = d;
        mul += 16;    // try next
    }
    return mul;
}

static void ramp_testMul(rampRsrc_t* r, u16 delta){
    u8 i;
    u16 x, tmp;
    s32 sum = 0;
    tmp = r->mul;

    r->mul = ramp_computeMul(delta, r->comp);
    for(i=0;i<RAMP_TAB_LEN;i++){
        x = RAMP_TAB[i]*r->mul/rampDiv;
        if(i>=RAMP_COMP_BASE && i<RAMP_COMP_BASE+4){    x += r->comp[i-RAMP_COMP_BASE];    }
        sum += x;
        print("%d\n",x);
        console.TxPolling(&console.rsrc);
//        HAL_IWDG_Refresh(&hiwdg);
    }
    r->mul = tmp;
    print("comp:%d\t%d\t%d\t%d\n", r->comp[0],r->comp[1],r->comp[2],r->comp[3]);
    print("delta:%d\t%d\t%d\t%d\n", delta, sum, r->mul, rampDiv);
}

static void ramp_rotate(rampRsrc_t* r, u16 targetSpd){
    u32 x;
    s32 delta;
    r->stopImmeditely = 0;
    if(r->spdTgt == targetSpd)    return;
    r->spdTgt = targetSpd;    // load new target speed

    r->accLock = 1;
    if(r->spdCur == r->spdTgt)    return;

    // by slow speed, do not use ramp
    if((r->spdCur <= r->spdMin) && ((r->spdTgt <= r->spdMin))){
        r->spdCur = r->spdMin;
        print("<%s sta:%d>", __func__,HAL_TIM_Base_GetState(r->htim));
        if( HAL_TIM_Base_GetState(r->htim) == HAL_TIM_STATE_READY){
            print("<%s 2>", __func__);
            x = 0xffff - r->spdMin;
            __HAL_TIM_SET_AUTORELOAD(r->htim, x);
            __HAL_TIM_SET_COMPARE(r->htim, r->tCh, x/2);
            HAL_TIM_Base_Stop_IT(r->htim);
            HAL_TIM_Base_Start_IT(r->htim);
        }
        return;
    }

    // acc or dec
    delta = r->spdCur - r->spdTgt;
    r->mul = ramp_computeMul(abs(delta), r->comp);    // cost 1~2ms, very bad

    r->rampIndx = 0;
    r->pulseCount = 0;
    if( HAL_TIM_Base_GetState(r->htim) == HAL_TIM_STATE_READY){
        ramp_updateSpdCur(r);    // reload spdCur
        r->rampIndx++;
        x = 0xffff - r->spdCur;
        __HAL_TIM_SET_AUTORELOAD(r->htim, x);
        __HAL_TIM_SET_COMPARE(r->htim, r->tCh, x/2);
        HAL_TIM_Base_Stop_IT(r->htim);
        HAL_TIM_Base_Start_IT(r->htim);
    }
    r->accLock = 0;
}

static void standaloe_rotate(rampRsrc_t* r, u16 targetSpd){
    u16 x;
    r->accLock = 1;
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

static u32 ramp_pulseCycle(rampRsrc_t* r, u16 mul){
    u32 pulse = 0;
    u32 x,i;
    for(i=0;i<RAMP_TAB_LEN;i++){
        x = RAMP_TAB[i]*mul/rampDiv;
        if(x){    pulse += r->pulsePerSpot;    }
    }
    return pulse;
}

static s32 ramp_rotateL(rampRsrc_t* r, u16 targetSpd){
    r->runMod = RAMP_RUN_MOD_SPD;    // SPEED MODE
    r->posReloadEn = 0;
    if((r->status & BIT(RAMP_STATUS_BIT_DIR)) == 0){
        ramp_rotate(r,targetSpd);
    }
    else{
        // current motion is rotate left, stop first
        ramp_rotate(r, 0);
        r->spdTgtNxt = targetSpd;
        r->dirNxt = RAMP_DIR_LEFT;
        r->squ = 1;
    }
    return 0;
}

static s32 ramp_rotateR(rampRsrc_t* r, u16 targetSpd){
    r->runMod = RAMP_RUN_MOD_SPD;    // SPEED MODE
    r->posReloadEn = 0;
    if((r->status & BIT(RAMP_STATUS_BIT_DIR)) != 0){
        ramp_rotate(r,targetSpd);
    }
    else{
        // current motion is rotate left, stop first
        ramp_rotate(r, 0);
        r->spdTgtNxt = targetSpd;
        r->dirNxt = RAMP_DIR_RIGHT;
        r->squ = 1;
    }
    return 0;
}

static s32 ramp_moveTo(rampRsrc_t* r, s32 targetPos){
    s32 x = targetPos;
    if(targetPos == r->posCur){    return 0;    }
    return(ramp_moveBy(r, x - r->posCur));
}

static s32 ramp_moveBy(rampRsrc_t* r, s32 refPos){
    return(ramp_moveBySpd(r, refPos, r->spdMax));
}

static s32 ramp_moveBySpd(rampRsrc_t* r, s32 refPos, u16 spd){
    u16 xMul;
    u16 xSpd;
    u32 posAbs = abs(refPos);
    if(r->status&BIT(RAMP_STATUS_BIT_RUN)){    return -1;}
    // compute max speed

    r->runMod = RAMP_RUN_MOD_POS;    // POSITION MODE
    r->posTgt = r->posCur + refPos;

    // turn right or left
    if(refPos  > 0){    ramp_dirR(r);    }
    else if(refPos < 0){    ramp_dirL(r);    }

    // turn off ramp, run as spdMin
    if(posAbs < 6400){
        r->posReloadEn = 0;        // do not load speed
        standaloe_rotate(r, r->spdMin);
        return 0;
    }

    for(xSpd=spd;1;){
        xMul = ramp_computeMul(xSpd, r->comp);
        r->pulsePerRampCycle = ramp_pulseCycle(r, xMul);
        if(r->pulsePerRampCycle <= posAbs){
            r->spdMax = xSpd;
            break;
        }
        else{
            xSpd -= (spd>>3);
        }
    }

    r->posReloadEn=1;
    ramp_rotate(r,r->spdMax);        // rotate with ramp, also set accLock=1 to make ramp

    return r->spdMax;
}

static u8 ramp_isRotating(rampRsrc_t* r){
    if(r->status&BIT(RAMP_STATUS_BIT_RUN)){    return 1;}
    return 0;
}

static void ramp_stopSoft(rampRsrc_t* r){
    ramp_rotate(r,0);
}

static void ramp_stop(rampRsrc_t* r){
    r->squ = 0;
    r->isHoming = 0;
    r->stopImmeditely = 1;
}

static void ramp_updateSpdCur(rampRsrc_t* r){
    u32 x;
    s32 spd = r->spdCur;
    while(r->rampIndx<RAMP_TAB_LEN){
        x = RAMP_TAB[r->rampIndx]*r->mul/rampDiv;
        if(x==0){    r->rampIndx++;    }
        else{
            if(r->rampIndx>=RAMP_COMP_BASE && r->rampIndx<RAMP_COMP_BASE+4){
                x += r->comp[r->rampIndx-RAMP_COMP_BASE];
            }
            // acc
            if(r->spdTgt > r->spdCur){    spd += x;    }
            // dec
            else if(r->spdCur > r->spdTgt){    spd -= x;    }
            if(spd > 65200){    r->spdCur = 65500;    }
            else  if(spd <= 0){    r->spdCur = 0;    }
            else {    r->spdCur = spd;    }
            break;
        }
    }
}

static u8 ramp_isShelteredRefL(rampRsrc_t* r){
    if(HAL_GPIO_ReadPin(r->REFL->GPIOx, r->REFL->GPIO_Pin) == GPIO_PIN_SET){    return 1;    }
    else{    return 0;    }
}

static u8 ramp_isShelteredRefR(rampRsrc_t* r){
    if(HAL_GPIO_ReadPin(r->REFR->GPIOx, r->REFR->GPIO_Pin) == GPIO_PIN_SET){    return 1;    }
    else{    return 0;    }
}

static u8 ramp_getRefl(rampRsrc_t* r){
    return(HAL_GPIO_ReadPin(r->REFL->GPIOx, r->REFL->GPIO_Pin));
}

static u8 ramp_getDir(rampRsrc_t* r){
    return(r->dirCur);
}

static void ramp_test(rampRsrc_t* r, u16 spd){
    u32 x;
    x = ramp_computeMul(spd, r->comp);
    r->pulsePerRampCycle = ramp_pulseCycle(r, x);
    print("%d\t%d\t%d\n", spd, x, r->pulsePerRampCycle);
}

static void ramp_isr(rampRsrc_t* r, TIM_HandleTypeDef *htim){
    u16 x, spdOld;
    u8 doRun = 1;

    if((r->en == 0) || (htim != r->htim))    return;

    // stop first
    HAL_TIM_Base_Stop_IT(htim);
    // reset speed status
    r->status &= (0xff^BIT(RAMP_STATUS_BIT_RUN));
    r->status &= (0xff^BIT(RAMP_STATUS_BIT_ACC));
    r->status &= (0xff^BIT(RAMP_STATUS_BIT_DEC));

    if(r->stopImmeditely){
        r->spdCur = 0;
        r->spdTgt = 0;
        r->stopImmeditely = 0;
        return;
    }

    spdOld = r->spdCur;
    r->pulseCount++;
    if((r->status & BIT(RAMP_STATUS_BIT_DIR)) == 0){    r->posCur--;    }
    else {    r->posCur++;    }

    // reload spdCur, use ramp mode
    if(r->accLock == 0){
        if(r->posReloadEn && abs(r->posTgt - r->posCur) <= r->pulsePerSpot*RAMP_TAB_LEN){
            r->spdTgt = 0;
            r->rampIndx = 0;
            r->pulseCount = 0;
            ramp_updateSpdCur(r);    // will edit r->spdCur
            r->posReloadEn = 0;
            r->pulseCount = 1;
        }
        if( (r->rampIndx < RAMP_TAB_LEN) && (r->pulseCount >= r->pulsePerSpot)){
            ramp_updateSpdCur(r);    // will edit r->spdCur
            r->pulseCount = 0;
            r->rampIndx++;
        }
        else if(r->rampIndx >= RAMP_TAB_LEN){    // to prevent from a accident
            if(r->spdCur > r->spdTgt){    r->spdCur--;    }
            else if(r->spdCur < r->spdTgt){    r->spdCur++;    }
            else if (r->spdCur == 0){
                doRun = 0;
                r->runMod = RAMP_RUN_MOD_STOP;
            }
        }
    }
    // speed standalone
    else if((r->runMod == RAMP_RUN_MOD_POS) && (r->posCur == r->posTgt)){
        doRun = 0;
    }

    if(doRun){
        r->status |= BIT(RAMP_STATUS_BIT_RUN);
        if(spdOld < r->spdCur){    r->status |= BIT(RAMP_STATUS_BIT_ACC);    }
        else if(spdOld > r->spdCur){    r->status |= BIT(RAMP_STATUS_BIT_DEC);    }
        // drive out
        x = 0xffff - r->spdCur;
        __HAL_TIM_SET_AUTORELOAD(htim, x);
        __HAL_TIM_SET_COMPARE(htim, r->tCh, x/2+1);
        HAL_TIM_Base_Start_IT(htim);
    }
}

static void ramp_isrRaisingRefL(rampRsrc_t* r, u16 GPIO_Pin){
    if((r->en == 0) || (r->REFL->GPIO_Pin != GPIO_Pin)){    return;    }
    r->stopImmeditely = 1;
}

static void ramp_isrFallingRefL(rampRsrc_t* r, u16 GPIO_Pin){
    if((r->en == 0) || (r->REFL->GPIO_Pin != GPIO_Pin)){    return;    }
    // in homing process
    if(r->squ == 3){
        r->stopImmeditely = 1;
        r->posCur = 0;
        r->isHoming = 0;
        r->squ = 0;
    }
}

static void ramp_isrRaisingRefR(rampRsrc_t* r, u16 GPIO_Pin){
    if((r->en == 0) || (r->REFR->GPIO_Pin != GPIO_Pin)){    return;    }
    r->stopImmeditely = 1;
}

static void ramp_isrFallingRefR(rampRsrc_t* r, u16 GPIO_Pin){
    if((r->en == 0) || (r->REFR->GPIO_Pin != GPIO_Pin)){    return;    }
//    // in homing process
//    if(r->squ == 3){
//        r->stopImmeditely = 1;
//        r->posCur = 0;
//        r->isHoming = 0;
//        r->squ = 0;
//    }
}
