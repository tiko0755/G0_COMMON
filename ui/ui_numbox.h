/**********************************************************
filename: ui_numBox.h
**********************************************************/
#ifndef _UI_NUMBOX_H
#define _UI_NUMBOX_H

#include "misc.h"
#include "ui_common.h"
/*****************************************************************************
 @ typedefs
****************************************************************************/
#pragma pack(push,4)        // push current align bytes, and then set 4 bytes align

typedef struct{
    char name[UI_NAME_LEN];
    float num;
    void (*uiPrint)(const char* FORMAT_ORG, ...);
    uiCB onChangedTab[UI_MAX_EVENT];
}numBox_rsrc_t;

typedef struct{
    numBox_rsrc_t rsrc;
    u8 (*cmd)(numBox_rsrc_t*, const char* MSG);
    void (*setColor)(numBox_rsrc_t*, u16 color);
    void (*setNum)(numBox_rsrc_t*, float num);
    s8 (*bindOnChanged)(numBox_rsrc_t*, uiCB);
}numBox_t;
#pragma pack(push,4)        // push current align bytes, and then set 4 bytes align

void numBox_setup(
    void *p,
    const char* NAME,
    void (*printLCD)(const char* FORMAT_ORG, ...)
);

#endif
