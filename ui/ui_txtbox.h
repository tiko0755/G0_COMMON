/**********************************************************
filename: ui_txtBox.h
**********************************************************/
#ifndef _UI_TXTBOX_H
#define _UI_TXTBOX_H

#include "misc.h"
#include "ui_common.h"
/*****************************************************************************
 @ typedefs
****************************************************************************/
typedef struct{
    char* parentsName;
    char name[UI_NAME_LEN];
    char txt[UI_TEXT_MAX_LEN];
    void (*uiPrint)(const char* FORMAT_ORG, ...);
    EventCB_t cbTab[UI_MAX_EVENT];
}tbox_rsrc_t;

typedef struct{
    tbox_rsrc_t rsrc;
    u8 (*cmd)(tbox_rsrc_t*, const char* MSG);
    s8 (*set)(tbox_rsrc_t*, const char* attr, const char* FORMAT, ...);
    s8 (*bind)(tbox_rsrc_t*, const char* EVNT, uiCB);
}textbox_t;

typedef struct txtBoxNode{
    textbox_t obj;
    struct txtBoxNode* nxt;
} txtBoxNode;

txtBoxNode* tbListInsert(txtBoxNode** head,
    char *parentsName,
    const char* NAME,
    void (*printLCD)(const char* FORMAT_ORG, ...)
);

#endif
