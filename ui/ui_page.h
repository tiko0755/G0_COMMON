/**********************************************************
filename: ui_page.h
**********************************************************/
#ifndef _UI_PAGE_H
#define _UI_PAGE_H

#include "misc.h"
#include "ui_common.h"
#include "ui_component.h"

/*****************************************************************************
 @ typedefs
****************************************************************************/
#pragma pack(push,4)        // push current align bytes, and then set 4 bytes align
typedef struct{
    char name[UI_NAME_LEN];
    void (*uiPrint)(const char* FORMAT_ORG, ...);
    uiComponentNode *componentLst;
}uiPageRsrc_t;

typedef struct{
    uiPageRsrc_t rsrc;
    u8 (*cmd)(uiPageRsrc_t*, const char* MSG);
    void (*setBColor)(uiPageRsrc_t*, u16 color);
    void (*initial)(uiPageRsrc_t*);
    void (*gotoPage)(uiPageRsrc_t*);
    
    uiComponent_t* (*placeComponent)(uiPageRsrc_t*, const char* NAME, u8 txtMax);
    uiComponent_t* (*getComponent)(uiPageRsrc_t*, const char* NAME);

    s8 (*Set)(uiPageRsrc_t*, const char* COMPONENT, const char* ATTR, const char* VAL);
    s8 (*get)(uiPageRsrc_t*, const char* attr);
}uiPage_t;

typedef struct uiPageNode{
    uiPage_t obj;
    struct uiPageNode* nxt;
} uiPageNode;
#pragma pack(pop)           //recover align bytes from 4 bytes

void uiPageSetup(
    void *p,
    const char* NAME,
    void (*printLCD)(const char* FORMAT_ORG, ...)
);

#endif
