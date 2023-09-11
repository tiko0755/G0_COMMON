/**********************************************************
filename: motion_path_2axis.c
**********************************************************/

/************************����ͷ�ļ�***************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "motion_path_2axis.h"
#include "user_log.h"
#include "thread_delay.h"

/**********************************************************
 Private variable
**********************************************************/

// move to workplace
static s32 mPath2_add(mPath2Rsrc_t*r, mPath2PosNode** lst, s32 posX, s32 posY, u16 spdX, u16 spdY, u32 delay);
static s32 mPath2_removeAll(mPath2Rsrc_t* r, mPath2PosNode** p);
static s32 mPath2_start(mPath2Rsrc_t*r, mPath2PosNode* lst, CB2 cmplt);
static void mPath2_polling(void*, u16 interval);
static void mPath2_stop(mPath2Rsrc_t* r);

/**********************************************************
 Public function
**********************************************************/
#define WP_SQU_Y

s32 mPath_setup(
    mPath2Dev_t* d, 
    mPath2CB_goPos goTargetCB, 
    mPath2CB_getPos getPosCB, 
    mPath2CB_stop stopCB
){
    memset(d,0,sizeof(mPath2Dev_t));
    d->rsrc.gotoPos = goTargetCB;
    d->rsrc.getPos = getPosCB;
    d->rsrc.stopPos = stopCB;
    
    d->addTo = mPath2_add;
    d->removeAll = mPath2_removeAll;
    d->start = mPath2_start;
    d->stop = mPath2_stop;
    d->polling = mPath2_polling;
    
    return 0;
}

static void mPath2_polling(void* e, u16 interval){
    mPath2Dev_t* d = (mPath2Dev_t*)e;
    mPath2Rsrc_t *r = &d->rsrc;
    mPath2Pos_t plot;
    s32 x,y;

    switch(r->squ){
        case 1:{
            if(r->pathCur == NULL){
                log("<%s 'completed'>", __func__);
                r->squ = 0;
                if(r->cmplt){
                    r->cmplt(0, r);
                }
                break;
            }
            plot = r->pathCur->val;
            r->gotoPos(0, plot.xSpd, plot.xPos);
            r->gotoPos(1, plot.ySpd, plot.yPos);
            r->tim = plot.delay;
            r->tick = 0;
            r->squ++;
            log("<%s %d:\t[%d,%d]@[%d,%d] >", __func__,HAL_GetTick(),plot.xPos,plot.yPos,plot.xSpd,plot.ySpd);
            break;
        }
        case 2:{
            if((r->getPos(0, &x)<0) || (r->getPos(1, &y)<0)){
                r->squ--;
                log("<%s fetal_error >", __func__);
                break;
            
            }
            plot = r->pathCur->val;
            if((plot.xPos==x) && (plot.yPos==y)){
                r->pathPrv = r->pathCur;
                r->pathCur = r->pathCur->nxt;
                r->squ--;
                break;
            }
            r->tick += interval;
            if(r->tick >= r->tim){
                r->pathPrv = r->pathCur;
                r->pathCur = r->pathCur->nxt;
                r->squ--;
                r->stopPos(0);
                r->stopPos(1);
                log("<%s msg:'stop_before_target' >", __func__);
                break;
            }
            break;
        }
    }
    
    
}

static s32 mPath2_add(mPath2Rsrc_t* r, mPath2PosNode** pHead, s32 posX, s32 posY, u16 spdX, u16 spdY, u32 delay){
    log("<%s >", __func__);
    mPath2PosNode* newnode = (mPath2PosNode*)malloc(sizeof(mPath2PosNode));
    if(newnode == NULL){
        log("</%s rtn:-1 >", __func__);
        return -1;
    }
    
    memset(newnode,0,sizeof(mPath2PosNode));
    newnode->val.xPos = posX;
    newnode->val.yPos = posY;
    newnode->val.xSpd = spdX;
    newnode->val.ySpd = spdY;
    newnode->val.delay = delay;


    if (*pHead == NULL)
    {
        *pHead  = newnode;
        log("</%s rtn:0 >", __func__);
        return 0;
    }
    mPath2PosNode* tail = *pHead;
    while (tail->nxt != NULL)
    {
        tail = tail->nxt;
    }
    tail->nxt = newnode;
    log("</%s rtn:1 >", __func__);
    return 1;
}

static s32 mPath2_removeAll(mPath2Rsrc_t* r, mPath2PosNode** p){
    mPath2PosNode* tmp;
    mPath2PosNode* cur = *p;
    while (cur != NULL)
    {
        tmp = cur;
        cur = cur->nxt;
        free(tmp);
    }
    *p = NULL;
    return 0;
}

static void mPath2_stop(mPath2Rsrc_t* r){
    r->squ = 0;
}

static s32 mPath2_start(mPath2Rsrc_t* r, mPath2PosNode* lst, CB2 cmplt){
    if(r->squ){
        return -1;
    }
    r->cmplt = cmplt;
    r->pathHead = lst;
    r->pathCur = lst;
    r->pathPrv = lst;
    r->squ = 1;
    return 0;
}


/**********************************************************
 == THE END ==
**********************************************************/

