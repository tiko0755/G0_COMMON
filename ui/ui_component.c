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
    char str[UI_TEXT_MAX_LEN] = {0};
    const char* msg;

    i = strlen(rsrc->name);
    if(i>0 && strncmp(MSG, rsrc->name, i) != 0){
//        log("</%s strncmp >", __func__);
        return 0;
    }
    msg = &MSG[i+1];
//    log("<%s i:%d msg:%s >", __func__, i, msg);

    if(sscanf(msg, UI_EVNT_EDIT_NUM, &val) == 1){
        for(i=0;i<UI_MAX_EVENT;i++){
            if(strncmp(rsrc->cbTab[i].evnt, UI_EVNT_EDIT_NUM, strlen(UI_EVNT_EDIT_NUM)) == 0){
                if(rsrc->cbTab[i].cb){    rsrc->cbTab[i].cb(1, val);    }
            }
        }
//        log("</%s UI_EVNT_EDIT_NUM >", __func__);
        return 1;
    }
    else if(sscanf(msg, UI_EVNT_EDIT_STR, str) == 1){
        for(i=0;i<UI_MAX_EVENT;i++){
            if(strncmp(rsrc->cbTab[i].evnt, UI_EVNT_EDIT_STR, strlen(UI_EVNT_EDIT_STR)) == 0){
                if(rsrc->cbTab[i].cb){    rsrc->cbTab[i].cb(1, str);    }
            }
        }
//        log("</%s UI_EVNT_EDIT_STR >", __func__);
        return 1;
    }
    else if(strncmp(msg, UI_EVNT_CLICK, strlen(UI_EVNT_CLICK)) == 0){
        msg = &msg[strlen(UI_EVNT_CLICK)+1];
        for(i=0;i<UI_MAX_EVENT;i++){
            if(strncmp(rsrc->cbTab[i].evnt, UI_EVNT_CLICK, strlen(UI_EVNT_CLICK)) == 0){
                if(rsrc->cbTab[i].cb){    rsrc->cbTab[i].cb(1, msg);    }
            }
        }
//        log("</%s UI_EVNT_CLICK >", __func__);
        return 1;
    }
    else if(strncmp(msg, UI_EVNT_DCLICK, strlen(UI_EVNT_DCLICK)) == 0){
        msg = &msg[strlen(UI_EVNT_DCLICK)+1];
        for(i=0;i<UI_MAX_EVENT;i++){
            if(strncmp(rsrc->cbTab[i].evnt, UI_EVNT_CLICK, strlen(UI_EVNT_CLICK)) == 0){
                if(rsrc->cbTab[i].cb){    rsrc->cbTab[i].cb(1, msg);    }
            }
        }
//        log("</%s UI_EVNT_DCLICK >", __func__);
        return 1;
    }
//    log("</%s >", __func__);
    return 0;
}

static s8 uiComponent_set(uiComponent_rsrc_t* rsrc, const char* attr, const char* VAL){
//    log("<%s attr:%s VAL:%s >", __func__, attr, VAL);
    thread_delay(500);
    if(strncmp(attr,UI_ATTR_TXT,strlen(UI_ATTR_TXT)) == 0){
        rsrc->uiPrint("%s.%s.%s=\"%s\"", rsrc->parentsName, rsrc->name, attr, VAL);
        if(rsrc->txt){
            memset(rsrc->txt,0,rsrc->txtSz);
            strcpy(rsrc->txt, VAL);
        }
    }
    else{
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
