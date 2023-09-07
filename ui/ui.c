/**********************************************************
filename: uartLcdUi.c
**********************************************************/

/************************����ͷ�ļ�***************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "ui.h"
#include "misc.h"
#include "user_log.h"

/**********************************************************
 Private function
**********************************************************/
// pgMain.tbStatus
static uiPage_t* uiNewPage(uiRsrc_T *rsrc, const char* NAME);

static uiComponent_t* uiPlaceComponent(uiRsrc_T*, const char* PAGE, const char* COMPONENT, u8 txtMax);
static uiComponent_t* uiGetComponent(uiRsrc_T*, const char* PAGE_NAME, const char* COMPONENT_NAME);

static uiComponent_t* uiPlaceComponent(uiRsrc_T*, const char* PAGE, const char* COMPONENT, u8 txtLen);

static void uiPolling(uiRsrc_T* r, u16 tick);
static s8 uiVisual(uiRsrc_T *r, const char* PAGE, const char* COMPONENT, u8 vis);
static s8 uiSet(uiRsrc_T*, const char* PAGE, const char* COMPONENT, const char* ATTR, const char* FORMAT_ORG, ...);
static s8 uiGet(uiRsrc_T*, const char* PAGE, const char* COMPONENT, char* ATTR);
static s8 uiBind(uiRsrc_T*, const char* PAGE, const char* COMPONENT, const char* EVENT, uiCB cb);
static uiPageNode* uiGetPageNode(uiRsrc_T *r, const char* PAGE);
static s8 uiWaitReady(uiRsrc_T* rsrc, u32 timeout);

/**********************************************************
 Public function
**********************************************************/
void uiSetup(
    void *pDev,
    UartDev_t* uartD,
    void (*printLCD)(const char* FORMAT_ORG, ...)
){
    uiDev_T* pd = (uiDev_T*)pDev;
    uiRsrc_T* pr = &pd->rsrc;
    pr->pUartDev = uartD;
    pr->uiPrint = printLCD;
    pr->hasLoaded = 0;

    pd->Polling = uiPolling;
    // component setup
    pd->Set = uiSet;
    pd->Bind = uiBind;
    pd->NewPage = uiNewPage;
    pd->PlaceComponent = uiPlaceComponent;
    pd->GetComponent = uiGetComponent;
    pd->Visual = uiVisual;
    
    pr->pUartDev->StartRcv(&pr->pUartDev->rsrc);
}
/**********************************************************
 read data
**********************************************************/
static uiPage_t* uiNewPage(uiRsrc_T *rsrc, const char* NAME){
    uiPageNode* newnode = (uiPageNode*)malloc(sizeof(uiPageNode));
    if(newnode == NULL)    return NULL;
    memset(newnode,0,sizeof(uiPageNode));

    uiPageSetup(&newnode->obj, NAME, rsrc->uiPrint);

    if (rsrc->pageLst == NULL)
    {
        rsrc->pageLst = newnode;
        return &newnode->obj;
    }
    uiPageNode* tail = rsrc->pageLst;
    while (tail->nxt != NULL){    tail = tail->nxt;    }
    tail->nxt = newnode;
    return &newnode->obj;
}

static uiPageNode* uiGetPageNode(uiRsrc_T *r, const char* PAGE){
    uiPageNode *pgNode;
    for (pgNode=r->pageLst; pgNode != NULL; pgNode=pgNode->nxt)
    {
        if(strncmp(PAGE, pgNode->obj.rsrc.name, strlen(pgNode->obj.rsrc.name)) == 0){
            return pgNode ;
        }
    }
    return NULL;
}

static uiComponent_t* uiPlaceComponent(uiRsrc_T* r, const char* PAGE, const char* COMPONENT, u8 txtLen){
    uiPageNode *pgNode = uiGetPageNode(r,PAGE);
    if(pgNode==NULL){    return NULL;    }
    return(pgNode->obj.placeComponent(&pgNode->obj.rsrc, COMPONENT, txtLen));
}

static uiComponent_t* uiGetComponent(uiRsrc_T* r, const char* PAGE, const char* COMPONENT){
    uiPageNode *pgNode = uiGetPageNode(r,PAGE);
    if(pgNode==NULL){    return NULL;    }
    return(pgNode->obj.getComponent(&pgNode->obj.rsrc, COMPONENT));
}


/**********************************************************
 ui polling
**********************************************************/
static u8 uiIsGetResp(u8* cmd, u16 len){
    return 0;
}

static void uiPolling(uiRsrc_T* rsrc, u16 tick){
    char buff[UI_TEXT_MAX_LEN] = {0};
    uiPageNode* pgNode;

    rsrc->tick += tick;
    if(rsrc->tick < 32){    return;        }
    rsrc->tick = 0;
        
    if(fetchLineFromRingBuffer(&rsrc->pUartDev->rsrc.rxRB, buff, UI_TEXT_MAX_LEN)){
        log("<%s buff:%s >", __func__, buff);
        if(strncmp(buff, "lcd.start", strlen("lcd.start")) == 0){
            rsrc->hasLoaded = 1;
        }
        // to meet get command, format: p[str]0xff 0xff 0xff
        else if(uiIsGetResp((u8*)buff, 111)){
        }
        for (pgNode=rsrc->pageLst; pgNode != NULL; pgNode=pgNode->nxt)
        {
            if(pgNode->obj.cmd(&pgNode->obj.rsrc, buff)){    break;    }
        }
    }
}

static s8 uiWaitReady(uiRsrc_T* rsrc, u32 timeout){
    u32 tick = 0;
    char buff[UI_TEXT_MAX_LEN] = {0};

    while(tick < timeout){
        rsrc->uiPrint("get pgxx.t_about.txt");
        HAL_Delay(100);
        rsrc->pUartDev->RxPolling(&rsrc->pUartDev->rsrc);
        if(fetchLineFromRingBuffer(&rsrc->pUartDev->rsrc.rxRB, buff, UI_TEXT_MAX_LEN)){
            if(sscanf(buff, "max tester %s", rsrc->ver) == 1){
                log("LCD Ver%s\n", rsrc->ver);
                return 0;
            }
        }
        tick += 100;
    }
    return -1;
}

static s8 uiSet(uiRsrc_T *r, const char* PAGE, const char* COMPONENT, const char* ATTR, const char* FORMAT_ORG, ...){
    uiPageNode* pgNode;
    va_list ap;
    char buf[UI_TEXT_MAX_LEN] = {0};
    s16 bytes;
    //take string
    va_start(ap, FORMAT_ORG);
    bytes = vsnprintf(buf, UI_TEXT_MAX_LEN, FORMAT_ORG, ap);
    va_end(ap);
    if(bytes < 0){    return -1;    }

    for(pgNode=r->pageLst; pgNode != NULL; pgNode=pgNode->nxt)
    {
        if(strncmp(PAGE, pgNode->obj.rsrc.name, strlen(pgNode->obj.rsrc.name)) != 0){    continue;    }
        return (pgNode->obj.Set(&pgNode->obj.rsrc, COMPONENT, ATTR, buf));
    }
    return -2;
}

static s8 uiGet(uiRsrc_T* r, const char* PAGE, const char* COMPONENT, char* ATTR){
    return 0;
}

static s8 uiVisual(uiRsrc_T *r, const char* PAGE, const char* COMPONENT, u8 vis){
    r->uiPrint("vis %s.%s %d", PAGE, COMPONENT, vis);
    return 0;
}

static s8 uiBind(uiRsrc_T *r, const char* PAGE, const char* COMPONENT, const char* EVENT, uiCB cb){
    uiComponent_t* obj = uiGetComponent(r,  PAGE, COMPONENT);
    if(obj != NULL){    return(obj->bind(&obj->rsrc, EVENT, cb));    }
    return -1;
}


/**********************************************************
 == THE END ==
**********************************************************/

