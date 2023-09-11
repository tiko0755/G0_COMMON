/**********************************************************
filename: motion_path_2axis.h
**********************************************************/
#ifndef _UI_MOTION_PATH_2AXIS_H_
#define _UI_MOTION_PATH_2AXIS_H_

#include "misc.h"
#include "usr_typedef.h"

/*****************************************************************************
 @ typedefs
****************************************************************************/
#pragma pack(push,4)    // push current align bytes, and then set 4 bytes align
typedef struct{
    s32 xPos;
    s32 yPos;
    u16 xSpd;
    u16 ySpd;
    u32 delay;
}mPath2Pos_t;

typedef struct mPath2PosNode{
    mPath2Pos_t val;
    struct mPath2PosNode* nxt;
} mPath2PosNode;

typedef s32 (*mPath2CB_goPos)(u8 axis, u16 spd, s32 pos);
typedef s32 (*mPath2CB_getPos)(u8 axis, s32* pos);
typedef s32 (*mPath2CB_stop)(u8 axis);

typedef struct {
    mPath2CB_goPos gotoPos;
    mPath2CB_getPos getPos;
    mPath2CB_stop stopPos;
    CB2 cmplt;
    
    mPath2PosNode* pathHead;
    mPath2PosNode* pathCur;
    mPath2PosNode* pathPrv;
    
    u32 targetR;
    u32 targetY;
    u8 squ;
    u16 tick;
    u16 tim;
}mPath2Rsrc_t;

typedef struct {
    mPath2Rsrc_t rsrc;
    s32 (*addTo)(mPath2Rsrc_t*r, mPath2PosNode**, s32 posX, s32 posY, u16 spdX, u16 spdY, u32 delay);
    s32 (*removeAll)(mPath2Rsrc_t* r, mPath2PosNode** p);
    s32 (*start)(mPath2Rsrc_t*r, mPath2PosNode* lst, CB2 cmplt);
    void (*stop)(mPath2Rsrc_t* r);
    void (*polling)(void*, u16 interval);
}mPath2Dev_t;

#pragma pack(pop)       //recover align bytes from 4 bytes

s32 mPath_setup(mPath2Dev_t* d, mPath2CB_goPos goTargetCB, mPath2CB_getPos getPosCB, mPath2CB_stop stopCB);

#endif

