/**********************************************************
filename: ui_component.c
**********************************************************/

/************************����ͷ�ļ�***************************************************/
#include "misc.h"
#include "stdlib.h"
#include "stdio.h"
#include "stdarg.h"
#include <string.h>
#include "ui_component.h"

#include "thread_delay.h"
#include "user_log.h"

/**********************************************************
 Private function
**********************************************************/
// pgMain.tbStatus
static u8 uiComponent_cmd(uiComponent_rsrc_t*, const char* MSG);
static s8 uiComponent_set(uiComponent_rsrc_t* rsrc, const char* attr, const char* VAL);
static s8 uiComponent_bind(uiComponent_rsrc_t *, const char* EVNT, uiCB cb);
static void uiComponent_setup(
    void *p,
    char* parentsName,
    const char* NAME,
    void (*printLCD)(const char* FORMAT_ORG, ...)
);
/**********************************************************
 Public function
**********************************************************/
static void uiComponent_setup(
    void *p,
    char* parentsName,
    const char* NAME,
    void (*printLCD)(const char* FORMAT_ORG, ...)
){
    uiComponent_t *dev;
    uiComponent_rsrc_t *rsrc;

    memset(p,0,sizeof(uiComponent_t));
    dev = (uiComponent_t*)p;
    rsrc = &dev->rsrc;
    rsrc->parentsName = parentsName;
    strcpy(rsrc->name, NAME);
    rsrc->uiPrint = printLCD;

    dev->cmd = uiComponent_cmd;
    dev->set = uiComponent_set;
    dev->bind = uiComponent_bind;
}

uiComponentNode* uiComponentInsert(uiComponentNode** head,
    char *parentsName,
    const char* NAME,
    void (*printLCD)(const char* FORMAT_ORG, ...)
){
//    log("<%s parentsName:0x%08x >", __func__, (u32)parentsName);
    uiComponentNode* newnode = (uiComponentNode*)malloc(sizeof(uiComponentNode));
    if(newnode == NULL)    return NULL;

    memset(newnode,0,sizeof(uiComponentNode));
    uiComponent_setup(&newnode->obj, parentsName, NAME, printLCD);

    if (*head == NULL)
    {
        *head = newnode;
        return newnode;
    }
    uiComponentNode* tail = *head;
    while (tail->nxt != NULL)
    {
        tail = tail->nxt;
    }
    tail->nxt = newnode;
    return newnode;
}

static u8 uiComponent_cmd(uiComponent_rsrc_t* rsrc, const char* MSG){
//    log("<%s MSG:%s name:%s >", __func__, MSG, rsrc->name);
    s32 i,val;
    const char* msg;
    float f;
    s32 d;

    i = strlen(rsrc->name);
    if(i>0 && strncmp(MSG, rsrc->name, i) != 0){
        return 0;
    }
    msg = &MSG[i+1];
    log("<%s msg:%s >", __func__, msg);
    
    for(i=0;i<UI_MAX_EVENT;i++){
        if(rsrc->cbTab[i].cb == NULL){
            continue;
        }
        log("<%s event:%s >", __func__, rsrc->cbTab[i].evnt);
        if(rsrc->cbTab[i].cb(msg) == 0){
            log("</%s evnt:'%s' >", __func__, rsrc->cbTab[i].evnt);
            return 1;
        }
    }
    log("</%s 'unknown_event' >", __func__, rsrc->cbTab[i].evnt);
    return 2;
}

static s8 uiComponent_set(uiComponent_rsrc_t* rsrc, const char* attr, const char* VAL){
//    log("<%s attr:%s VAL:%s >", __func__, attr, VAL);
    if(strncmp(attr,UI_ATTR_TXT,strlen(UI_ATTR_TXT)) == 0){
        rsrc->uiPrint("%s.%s.%s=\"%s\"", rsrc->parentsName, rsrc->name, attr, VAL);
        if(rsrc->txt){
            memset(rsrc->txt,0,rsrc->txtSz);
            strcpy(rsrc->txt, VAL);
        }
    }
    else{
//        log("%s.%s.%s=%s", rsrc->parentsName, rsrc->name, attr, VAL);
        rsrc->uiPrint("%s.%s.%s=%s", rsrc->parentsName, rsrc->name, attr, VAL);
    }
//    log("</%s >", __func__);
    return 0;
}

static s8 uiComponent_bind(uiComponent_rsrc_t *rsrc, const char* EVNT, uiCB cb){
    s8 i;
    for(i=0;i<UI_MAX_EVENT;i++){
        if(rsrc->cbTab[i].cb == NULL){
            rsrc->cbTab[i].cb = cb;
            memset(rsrc->cbTab[i].evnt, 0, UI_EVNT_NAME_LEN);
            strcpy(rsrc->cbTab[i].evnt, EVNT);
            return i;
        }
    }
    return -1;
}


/**********************************************************
 == THE END ==
**********************************************************/
