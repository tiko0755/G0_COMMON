/**********************************************************
filename: ui_page.h
**********************************************************/
#ifndef _UI_PAGE_H
#define _UI_PAGE_H

#include "misc.h"
#include "ui_common.h"
#include "ui_txtbox.h"
#include "ui_txtbox_Pic.h"
#include "ui_pic.h"

/*****************************************************************************
 @ typedefs
****************************************************************************/
typedef struct{
    char name[UI_NAME_LEN];
    void (*uiPrint)(const char* FORMAT_ORG, ...);
    txtBoxNode *tbLst;
    txtBoxPicNode *tbPicLst;
    picBoxNode *picLst;
}uiPageRsrc_t;

typedef struct{
    uiPageRsrc_t rsrc;
    u8 (*cmd)(uiPageRsrc_t*, const char* MSG);
    void (*setBColor)(uiPageRsrc_t*, u16 color);
    void (*initial)(uiPageRsrc_t*);
    void (*gotoPage)(uiPageRsrc_t*);

    textbox_t* (*placeTxtBx)(uiPageRsrc_t*, const char* NAME);
    textboxPic_t* (*placeTxtBxPic)(uiPageRsrc_t*, const char* NAME);
    pic_t* (*placePic)(uiPageRsrc_t*, const char* NAME);
    s8 (*Set)(uiPageRsrc_t*, const char* COMPONENT, const char* ATTR, const char* VAL);

    textbox_t* (*getTxtBx)(uiPageRsrc_t*, const char* NAME);
    textboxPic_t* (*getTxtBxPic)(uiPageRsrc_t*, const char* NAME);
    pic_t* (*getPic)(uiPageRsrc_t*, const char* NAME);
}uiPage_t;

typedef struct uiPageNode{
    uiPage_t obj;
    struct uiPageNode* nxt;
} uiPageNode;

void uiPageSetup(
    void *p,
    const char* NAME,
    void (*printLCD)(const char* FORMAT_ORG, ...)
);

uiPageNode* uiPageListInsert(uiPageNode** head,
    const char* NAME,
    void (*printLCD)(const char* FORMAT_ORG, ...)
);

#endif
