/**********************************************************
filename: uartLcdUi.h
**********************************************************/
#ifndef _uartLcdUI_H
#define _uartLcdUI_H

#include "misc.h"
#include "uartDev.h"
#include "ui_page.h"

/*****************************************************************************
 @ typedefs
****************************************************************************/
#pragma pack(push,4)        // push current align bytes, and then set 4 bytes align
typedef u16 (*uiFetchLine)(char* line, u16 len);
typedef void (*uiPrint)(const char* FORMAT_ORG, ...);

typedef struct{
//    UartDev_t *pUartDev;
    appTmrDev_t* tObj;
    uiPrint print;
    uiFetchLine fetch;
    uiPageNode* pageLst;
    const char* curPage;
//    u16 tick;
    char ver[12];
    u8 hasReset;
}uiRsrc_T;

typedef struct{
    uiRsrc_T rsrc;
//    void (*Polling)(uiRsrc_T* rsrc, u16 tick);
    //basic
    uiPage_t* (*NewPage)(uiRsrc_T *rsrc, const char* NAME);
    // place component
    uiComponent_t* (*PlaceComponent)(uiRsrc_T*, const char* PAGE, const char* COMPONENT, u8 txtLen);
    
    s32 (*Reset)(uiRsrc_T*);
    s8 (*Page)(uiRsrc_T*, const char* PAGE);
    s8 (*Set)(uiRsrc_T*, const char* PAGE, const char* COMPONENT, const char* ATTR, const char* FORMAT_ORG, ...);
    s8 (*Get)(uiRsrc_T*, const char* PAGE, const char* COMPONENT, const char* ATTR);
    s8 (*Visual)(uiRsrc_T *r, const char* PAGE, const char* COMPONENT, u8 vis);
    // draw command
    s8 (*Fill)(uiRsrc_T *r, const char* PAGE, u16 x, u16 y, u16 w, u16 h, const char* COLOR);

//    s8 (*SetX)(uiRsrc_T*, const char* COMPONENT, const char* ATTR, const char* FORMAT_ORG, ...);

    uiComponent_t* (*GetComponent)(uiRsrc_T*,  const char* PAGE_NAME, const char* COMPONENT_NAME);
//    textboxPic_t* (*GetTxtBxPic)(uiRsrc_T*, const char* PAGE_NAME, const char* COMPONENT_NAME);
//    pic_t* (*GetPic)(uiRsrc_T*, const char* PAGE_NAME, const char* COMPONENT_NAME);

    // bind evnet group
    s8 (*Bind)(uiRsrc_T*, const char* PAGE, const char* COMPONENT, const char* EVENT, uiCB cb);
}uiDev_T;

#pragma pack(pop)           //recover align bytes from 4 bytes

void uiSetup(
    void *pDev,
    appTmrDev_t* tObj,
    uiPrint xPrint,
    uiFetchLine xFetchLine
);
    
#endif
