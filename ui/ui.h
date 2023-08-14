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

typedef struct{
    UartDev_t *pUartDev;
    char ver[12];
    void (*uiPrint)(const char* FORMAT_ORG, ...);
    uiPageNode* pageLst;
    u16 tick;
    u8 hasLoaded;
}uiRsrc_T;

typedef struct{
    uiRsrc_T rsrc;
    void (*Polling)(uiRsrc_T* rsrc, u16 tick);
    //basic
    uiPage_t* (*NewPage)(uiRsrc_T *rsrc, const char* NAME);
    // place component
    textbox_t* (*PlaceTxtBx)(uiRsrc_T*, const char* PAGE, const char* COMPONENT, u8 txtLen);
    textboxPic_t* (*PlaceTxtBxPic)(uiRsrc_T*, const char* PAGE, const char* COMPONENT, u8 txtLen);
    pic_t* (*PlacePic)(uiRsrc_T*, const char* PAGE, const char* COMPONENT);

    s8 (*Set)(uiRsrc_T*, const char* PAGE, const char* COMPONENT, const char* ATTR, const char* FORMAT_ORG, ...);
    s8 (*Get)(uiRsrc_T*, const char* PAGE, const char* COMPONENT, const char* ATTR);
    s8 (*Visual)(uiRsrc_T *r, const char* PAGE, const char* COMPONENT, u8 vis);

    textbox_t* (*GetTxtBx)(uiRsrc_T*,  const char* PAGE_NAME, const char* COMPONENT_NAME);
    textboxPic_t* (*GetTxtBxPic)(uiRsrc_T*, const char* PAGE_NAME, const char* COMPONENT_NAME);
    pic_t* (*GetPic)(uiRsrc_T*, const char* PAGE_NAME, const char* COMPONENT_NAME);

    // bind evnet group
    s8 (*Bind)(uiRsrc_T*, const char* PAGE, const char* COMPONENT, const char* EVENT, uiCB cb);
}uiDev_T;

#pragma pack(pop)           //recover align bytes from 4 bytes

void uiSetup(
    void *pDev,
    UartDev_t* uartDev,
    void (*printLCD)(const char* FORMAT_ORG, ...)
);

#endif
