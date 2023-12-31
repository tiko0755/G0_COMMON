/**********************************************************
filename: ui_txtBox_pic.h
**********************************************************/
#ifndef _UI_TXTBOX_PIC_H
#define _UI_TXTBOX_PIC_H

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
    char* txt;
}tboxPic_rsrc_t;

typedef struct{
    tboxPic_rsrc_t rsrc;
    u8 (*cmd)(tboxPic_rsrc_t*, const char* MSG);
    s8 (*set)(tboxPic_rsrc_t*, const char* attr, const char* FORMAT, ...);
    s8 (*get)(tboxPic_rsrc_t*, const char* attr);
    s8 (*bind)(tboxPic_rsrc_t*, const char* EVNT, uiCB);
}textboxPic_t;

typedef struct txtBoxPicNode{
    textboxPic_t obj;
    struct txtBoxPicNode* nxt;
} txtBoxPicNode;
#pragma pack(pop)           //recover align bytes from 4 bytes

txtBoxPicNode* tbPicListInsert(txtBoxPicNode** head,
    char *parentsName,
    const char* NAME,
    void (*printLCD)(const char* FORMAT_ORG, ...)
);
#endif
