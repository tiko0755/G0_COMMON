/**********************************************************
filename: ui_component.h
**********************************************************/
#ifndef __UI_COMPONENT_H__
#define __UI_COMPONENT_H__

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
    
//    char txt[UI_TEXT_MAX_LEN];
}tbox_rsrc_t;

typedef struct{
    tbox_rsrc_t rsrc;
    u8 (*cmd)(tbox_rsrc_t*, const char* MSG);
    s8 (*set)(tbox_rsrc_t*, const char* attr, const char* FORMAT, ...);
	s8 (*get)(tbox_rsrc_t*, const char* attr, char* content);
    s8 (*bind)(tbox_rsrc_t*, const char* EVNT, uiCB);
}textbox_t;

typedef struct txtBoxNode{
    textbox_t obj;
    struct txtBoxNode* nxt;
} txtBoxNode;
#pragma pack(pop)           //recover align bytes from 4 bytes

txtBoxNode* tbListInsert(txtBoxNode** head,
    char *parentsName,
    const char* NAME,
    void (*printLCD)(const char* FORMAT_ORG, ...)
);

#endif
