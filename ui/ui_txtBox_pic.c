/**********************************************************
filename: ui_txtBox_pic.c
**********************************************************/

/************************����ͷ�ļ�***************************************************/
#include "misc.h"
#include "stdlib.h"
#include "stdio.h"
#include "stdarg.h"
#include <string.h>
#include "ui_txtbox_pic.h"

#include "board.h"

/**********************************************************
 Private function
**********************************************************/
// pgMain.tbStatus
static u8 uiTxtboxPic_cmd(tboxPic_rsrc_t*, const char* MSG);
static s8 uiTxtboxPic_set(tboxPic_rsrc_t* rsrc, const char* attr, const char* FORMAT, ...);
static s8 uiTxtboxPic_bind(tboxPic_rsrc_t *, const char* EVNT, uiCB cb);
static void tboxPic_setup(
    void *p,
    char* parentsName,
    const char* NAME,
    void (*printLCD)(const char* FORMAT_ORG, ...)
);
/**********************************************************
 Public function
**********************************************************/
static void tboxPic_setup(
    void *p,
    char* parentsName,
    const char* NAME,
    void (*printLCD)(const char* FORMAT_ORG, ...)
){
    textboxPic_t *dev;
    tboxPic_rsrc_t *rsrc;

    memset(p,0,sizeof(textboxPic_t));
    dev = (textboxPic_t*)p;
    rsrc = &dev->rsrc;
    rsrc->parentsName = parentsName;
    strcpy(rsrc->name, NAME);
    rsrc->uiPrint = printLCD;

    dev->cmd = uiTxtboxPic_cmd;
    dev->set = uiTxtboxPic_set;
    dev->bind = uiTxtboxPic_bind;
}

txtBoxPicNode* tbPicListInsert(txtBoxPicNode** head,
    char *parentsName,
    const char* NAME,
    void (*printLCD)(const char* FORMAT_ORG, ...)
){
    txtBoxPicNode* newnode = (txtBoxPicNode*)malloc(sizeof(txtBoxPicNode));
    if(newnode == NULL)    return NULL;

    memset(newnode,0,sizeof(txtBoxPicNode));
    tboxPic_setup(&newnode->obj, parentsName, NAME, printLCD);

    if (*head == NULL)
    {
        *head = newnode;
        return newnode;
    }
    txtBoxPicNode* tail = *head;
    while (tail->nxt != NULL)
    {
        tail = tail->nxt;
    }
    tail->nxt = newnode;
    return newnode;
}

static u8 uiTxtboxPic_cmd(tboxPic_rsrc_t* rsrc, const char* MSG){
    s32 i,j;
    char str[UI_TEXT_MAX_LEN] = {0};
    const char* msg;
    i = strlen(rsrc->name);
    if(i>0 && strncmp(MSG, rsrc->name, i) != 0){
        return 0;
    }
    msg = &MSG[i+1];
    // textbox with picture
    if(sscanf(msg, "edit %d %s", &j, str)==2){
        for(i=0;i<UI_MAX_EVENT;i++){
            if(strncmp(rsrc->cbTab[i].evnt, "edit", strlen("edit")) == 0){
                if(rsrc->cbTab[i].cb){    rsrc->cbTab[i].cb(2, j, str);    }
            }
        }
        if(strncmp(str, rsrc->txt, strlen(str)) != 0){
            for(i=0;i<UI_MAX_EVENT;i++){
                if(strncmp(rsrc->cbTab[i].evnt, "change", strlen("change")) == 0){
                    if(rsrc->cbTab[i].cb){    rsrc->cbTab[i].cb(2, j, str);    }
                }
            }
        }
        memset(rsrc->txt,0,UI_TEXT_MAX_LEN);
        strcpy(rsrc->txt, str);
        return 1;
    }
    return 0;
}

static s8 uiTxtboxPic_set(tboxPic_rsrc_t* rsrc, const char* attr, const char* FORMAT, ...){
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

static s8 uiTxtboxPic_bind(tboxPic_rsrc_t *rsrc, const char* EVNT, uiCB cb){
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
