/**********************************************************
filename: ui_page.c
**********************************************************/

/************************����ͷ�ļ�***************************************************/
#include "misc.h"
#include "stdlib.h"
#include "stdio.h"
#include "stdarg.h"
#include <string.h>
#include "ui_page.h"

#include "user_log.h"

/**********************************************************
 Private function
**********************************************************/
static u8 uiPage_cmd(uiPageRsrc_t*, const char* MSG);
static void uiPage_setColor(uiPageRsrc_t*, u16 color);
static void uiPage_initial(uiPageRsrc_t*);
static s8 uiPageSet(uiPageRsrc_t*, const char* COMPONENT, const char* ATTR, const char* VAL);

static uiComponent_t* uiPage_placeComponent(uiPageRsrc_t*, const char* NAME, u8 txtMax);
static uiComponent_t* uiPage_getComponent(uiPageRsrc_t*, const char* NAME);

/**********************************************************
 Public function
**********************************************************/
void uiPageSetup(
    void *p,
    const char* NAME,
    void (*printLCD)(const char* FORMAT_ORG, ...)
){
    uiPage_t *d = (uiPage_t*)p;
    memset(d,0,sizeof(uiPage_t));

    strcpy(d->rsrc.name, NAME);
    d->rsrc.uiPrint = printLCD;
//    log("<%s 0x%08x >", d->rsrc.name, (u32)d->rsrc.name);

    d->cmd = uiPage_cmd;
    d->setBColor = uiPage_setColor;
    d->initial = uiPage_initial;
    d->Set = uiPageSet;
    
    d->placeComponent = uiPage_placeComponent;
    d->getComponent = uiPage_getComponent;
    
}

static u8 uiPage_cmd(uiPageRsrc_t* rsrc, const char* MSG){
//    log("<%s MSG:%s >", __func__, MSG);
    uiComponentNode *curComponentNode;
    const char* msg;
    if(strncmp(MSG, rsrc->name, strlen(rsrc->name)) != 0)    return 0;
    msg = &MSG[strlen(rsrc->name)+1];    // take component name, do not include '.'
//    log("<%s msg:%s >", __func__, msg);
    for(curComponentNode=rsrc->componentLst; curComponentNode!=NULL; curComponentNode=curComponentNode->nxt){
        if(curComponentNode->obj.cmd(&curComponentNode->obj.rsrc, msg))    return 1;
    }
    return 0;
}

static void uiPage_setColor(uiPageRsrc_t* rsrc, u16 color){
    rsrc->uiPrint("%s.pco=%d", rsrc->name, color);
}

static void uiPage_initial(uiPageRsrc_t* rsrc){
}

static uiComponent_t* uiPage_placeComponent(uiPageRsrc_t *r, const char* NAME, u8 txtMax){
    char* txt = NULL;
    if(txtMax){
        txt = (char*)malloc(sizeof(txtMax));
        if(txt == NULL)    return NULL;    
    }
    
    uiComponentNode* p = uiComponentInsert(&r->componentLst, r->name, NAME, r->uiPrint);
    if(p == NULL){    return NULL;    }
    p->obj.rsrc.txt = txt;
    p->obj.rsrc.txtSz = txtMax;
    return (&p->obj);
}


static uiComponent_t* uiPage_getComponent(uiPageRsrc_t *r, const char* NAME){
    uiComponentNode *curNode;
    for(curNode=r->componentLst; curNode!=NULL; curNode=curNode->nxt){
        if(strncmp(curNode->obj.rsrc.name, NAME, strlen(NAME)) == 0){
            return &curNode->obj;
        }
    }
    return NULL;
}


static s8 uiPageSet(uiPageRsrc_t *r, const char* COMPONENT, const char* ATTR, const char* VAL){
    uiComponentNode *componentNode;
    for(componentNode=r->componentLst; componentNode!=NULL; componentNode=componentNode->nxt){
        if(strncmp(componentNode->obj.rsrc.name, COMPONENT, strlen(COMPONENT)) == 0){
            if(componentNode->obj.set(&componentNode->obj.rsrc, ATTR, VAL) == 0){    return 0;    }
            return -2;
        }
    }
    return -1;
}

/**********************************************************
 == THE END ==
**********************************************************/
