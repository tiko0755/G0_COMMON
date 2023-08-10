/**********************************************************
filename: ui_txtBox.c
**********************************************************/

/************************����ͷ�ļ�***************************************************/
#include "misc.h"
#include "stdlib.h"
#include "stdio.h"
#include "stdarg.h"
#include <string.h>
#include "ui_txtbox.h"

#include "board.h"

/**********************************************************
 Private function
**********************************************************/
// pgMain.tbStatus
static u8 uiTxtbox_cmd(tbox_rsrc_t*, const char* MSG);
static s8 uiTxtbox_set(tbox_rsrc_t* rsrc, const char* attr, const char* FORMAT, ...);
static s8 uiTxtbox_get(tbox_rsrc_t* rsrc, const char* attr, char* content);
static s8 uiTxtbox_bind(tbox_rsrc_t* rsrc, const char* EVNT, uiCB);
static void tbox_setup(
    void *p,
    char *parentsName,
    const char* NAME,
    void (*printLCD)(const char* FORMAT_ORG, ...)
);
/**********************************************************
 Public function
**********************************************************/
static void tbox_setup(
    void *p,
    char *parentsName,
    const char* NAME,
    void (*printLCD)(const char* FORMAT_ORG, ...)
){
    textbox_t *dev;
    tbox_rsrc_t *rsrc;

    memset(p,0,sizeof(textbox_t));
    dev = (textbox_t*)p;
    rsrc = &dev->rsrc;
    rsrc->parentsName = parentsName;
    strcpy(rsrc->name, NAME);
    rsrc->uiPrint = printLCD;

    dev->cmd = uiTxtbox_cmd;
    dev->set = uiTxtbox_set;
		dev->sync = uiTxtbox_get;

    dev->bind = uiTxtbox_bind;
}

txtBoxNode* tbListInsert(txtBoxNode** head,
    char *parentsName,
    const char* NAME,
    void (*printLCD)(const char* FORMAT_ORG, ...)
){
    txtBoxNode* newnode = (txtBoxNode*)malloc(sizeof(txtBoxNode));
    if(newnode==NULL)    return NULL;

    memset(newnode,0,sizeof(txtBoxNode));
    tbox_setup(&newnode->obj, parentsName, NAME, printLCD);

    if (*head == NULL)
    {
        *head = newnode;
        return newnode;
    }
    txtBoxNode* tail = *head;
    while (tail->nxt != NULL)
    {
        tail = tail->nxt;
    }
    tail->nxt = newnode;
    return newnode;
}

static u8 uiTxtbox_cmd(tbox_rsrc_t* rsrc, const char* MSG){
    s32 i;
    char str[UI_TEXT_MAX_LEN] = {0};
    const char* msg;
    i = strlen(rsrc->name);
    if(i>0 && strncmp(MSG, rsrc->name, i) != 0){
        return 0;
    }
    msg = &MSG[i+1];
    if(strncmp(msg, "edit ", strlen("edit ")) == 0){
        strcpy(str, &msg[strlen("edit ")]);
        for(i=0;i<UI_MAX_EVENT;i++){
            if(strncmp(rsrc->cbTab[i].evnt, "edit", strlen("edit")) == 0){
                if(rsrc->cbTab[i].cb){    rsrc->cbTab[i].cb(1, str);    }
            }
        }
        if(strncmp(str, rsrc->txt, strlen(rsrc->txt)) != 0){
            for(i=0;i<UI_MAX_EVENT;i++){
                if(strncmp(rsrc->cbTab[i].evnt, "change", strlen("change")) == 0){
                    if(rsrc->cbTab[i].cb){    rsrc->cbTab[i].cb(1, str);    }
                }
            }
        }
        memset(rsrc->txt,0,UI_TEXT_MAX_LEN);
        strcpy(rsrc->txt, str);
        return 1;
    }
    else if(strncmp(msg, "click ", strlen("click ")) == 0){
        for(i=0;i<UI_MAX_EVENT;i++){
            if(strncmp(rsrc->cbTab[i].evnt, "click", strlen("click")) == 0){
                if(rsrc->cbTab[i].cb){    rsrc->cbTab[i].cb(1, str);    }
            }
        }
        return 1;
    }
    return 0;
}

static s8 uiTxtbox_set(tbox_rsrc_t* rsrc, const char* attr, const char* FORMAT, ...){
    va_list ap;
    char buf[UI_TEXT_MAX_LEN] = {0};
    s16 bytes;
    //take string
    va_start(ap, FORMAT);
    bytes = vsnprintf(buf, UI_TEXT_MAX_LEN, FORMAT, ap);
    va_end(ap);
    //send out
    if(bytes>=0){
        if(strncmp(attr,"txt",strlen("txt")) == 0){
            rsrc->uiPrint("%s.%s.%s=\"%s\"", rsrc->parentsName, rsrc->name, attr, buf);
            memset(rsrc->txt,0,UI_TEXT_MAX_LEN);
            strcpy(rsrc->txt, buf);
        }
        else{
            rsrc->uiPrint("%s.%s.%s=%s", rsrc->parentsName, rsrc->name, attr, buf);
        }
        return 0;
    }
    else{    return -1;    }
}

static s8 uiTxtbox_get(tbox_rsrc_t* rsrc, const char* attr, char* content){
		rsrc->uiPrint("get %s.%s.%s", rsrc->parentsName, rsrc->name, attr);
		return 0;
}


static s8 uiTxtbox_bind(tbox_rsrc_t* rsrc, const char* EVNT, uiCB cb){
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
