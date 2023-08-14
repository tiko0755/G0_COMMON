/**********************************************************
filename: ui_pic.h
**********************************************************/
#ifndef _UI_PIC_H
#define _UI_PIC_H

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
    u16 pic;
}pic_rsrc_t;

typedef struct{
    pic_rsrc_t rsrc;
    u8 (*cmd)(pic_rsrc_t*, const char* MSG);
    s8 (*set)(pic_rsrc_t*, const char* attr, const char* FORMAT, ...);
    s8 (*get)(pic_rsrc_t*, const char* attr);
    s8 (*bind)(pic_rsrc_t*, const char* EVNT, uiCB);
}pic_t;

typedef struct picBoxNode{
    pic_t obj;
    struct picBoxNode* nxt;
} picBoxNode;

#pragma pack(pop)           //recover align bytes from 4 bytes


picBoxNode* picListInsert(picBoxNode** head,
    char *parentsName,
    const char* NAME,
    void (*printLCD)(const char* FORMAT_ORG, ...)
);

#endif
