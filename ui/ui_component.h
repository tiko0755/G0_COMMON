/**********************************************************
filename: ui_component.h
**********************************************************/
#ifndef _UI_COMPONENT_H
#define _UI_COMPONENT_H

#include "misc.h"
#include "ui_common.h"
/*****************************************************************************
 @ typedefs
****************************************************************************/
#pragma pack(push,4)        // push current align bytes, and then set 4 bytes align
typedef struct{
    char* parentsName;
    char name[UI_NAME_LEN];
    void (*uiPrint)(const char* FORMAT_ORG, ...);
    EventCB_t cbTab[UI_MAX_EVENT];
    // text attr
    char* txt;
    u8 txtSz;
    
    s32 val;
    u16 pic;
    u8 type;
}uiComponent_rsrc_t;

typedef struct{
    uiComponent_rsrc_t rsrc;
    u8 (*cmd)(uiComponent_rsrc_t*, const char* MSG);
    s8 (*set)(uiComponent_rsrc_t*, const char* attr, const char* VAL);
    s8 (*get)(uiComponent_rsrc_t*, const char* attr);
    s8 (*bind)(uiComponent_rsrc_t*, const char* EVNT, uiCB);
}uiComponent_t;

typedef struct uiComponentNode{
    uiComponent_t obj;
    struct uiComponentNode* nxt;
} uiComponentNode;
#pragma pack(pop)           //recover align bytes from 4 bytes

uiComponentNode* uiComponentInsert(uiComponentNode** head,
    char *parentsName,
    const char* NAME,
    void (*printLCD)(const char* FORMAT_ORG, ...)
);
    
#endif
