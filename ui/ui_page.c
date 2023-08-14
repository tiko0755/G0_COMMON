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

#include "board.h"

/**********************************************************
 Private function
**********************************************************/
static u8 uiPage_cmd(uiPageRsrc_t*, const char* MSG);
static void uiPage_setColor(uiPageRsrc_t*, u16 color);
static void uiPage_initial(uiPageRsrc_t*);
static s8 uiPageSet(uiPageRsrc_t*, const char* COMPONENT, const char* ATTR, const char* VAL);

static textbox_t* uiPage_placeTxtBx(uiPageRsrc_t*, const char* NAME, u8 txtMax);
static textboxPic_t* uiPage_placeTxtBxPic(uiPageRsrc_t*, const char* NAME, u8 txtMax);
static pic_t* uiPage_placePic(uiPageRsrc_t*, const char* NAME);

static textbox_t* uiPage_getTxtBx(uiPageRsrc_t*, const char* NAME);
static textboxPic_t* uiPage_getTxtBxPic(uiPageRsrc_t*, const char* NAME);
static pic_t* uiPage_getPic(uiPageRsrc_t*, const char* NAME);

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

    d->cmd = uiPage_cmd;
    d->setBColor = uiPage_setColor;
    d->initial = uiPage_initial;
    d->Set = uiPageSet;
    d->placeTxtBx = uiPage_placeTxtBx;
    d->placeTxtBxPic = uiPage_placeTxtBxPic;
    d->placePic = uiPage_placePic;
    d->getTxtBx = uiPage_getTxtBx;
    d->getTxtBxPic = uiPage_getTxtBxPic;
    d->getPic = uiPage_getPic;
}

uiPageNode* uiPageListInsert(uiPageNode** head,
    const char* NAME,
    void (*printLCD)(const char* FORMAT_ORG, ...)
){
    uiPageNode* newnode = (uiPageNode*)malloc(sizeof(uiPageNode));
    if(newnode == NULL)    return NULL;

    memset(newnode,0,sizeof(uiPageNode));
    uiPageSetup(&newnode->obj, NAME, printLCD);

    if (*head == NULL)
    {
        *head = newnode;
        return newnode;
    }
    uiPageNode* tail = *head;
    while (tail->nxt != NULL)
    {
        tail = tail->nxt;
    }
    tail->nxt = newnode;
    return newnode;
}

static u8 uiPage_cmd(uiPageRsrc_t* rsrc, const char* MSG){
    txtBoxNode *tbCurNode;
    txtBoxPicNode *tbPicCurNode;
    picBoxNode *picCurNode;
    const char* msg;
    if(strncmp(MSG, rsrc->name, strlen(rsrc->name)) != 0)    return 0;
    msg = &MSG[strlen(rsrc->name)+1];    // take component name, do not include '.'
    for(picCurNode=rsrc->picLst;picCurNode!=NULL;picCurNode=picCurNode->nxt){
        if(picCurNode->obj.cmd(&picCurNode->obj.rsrc, msg))    return 1;
    }
    for(tbCurNode=rsrc->tbLst;tbCurNode!=NULL;tbCurNode=tbCurNode->nxt){
        if(tbCurNode->obj.cmd(&tbCurNode->obj.rsrc, msg))    return 1;
    }
    for(tbPicCurNode=rsrc->tbPicLst;tbPicCurNode!=NULL;tbPicCurNode=tbPicCurNode->nxt){
        if(tbPicCurNode->obj.cmd(&tbPicCurNode->obj.rsrc, msg))    return 1;
    }
    return 0;
}

static void uiPage_setColor(uiPageRsrc_t* rsrc, u16 color){
    rsrc->uiPrint("%s.pco=%d", rsrc->name, color);
}

static void uiPage_initial(uiPageRsrc_t* rsrc){
}

static textbox_t* uiPage_placeTxtBx(uiPageRsrc_t *r, const char* NAME, u8 txtMax){
    char* txt = NULL;
    if(txtMax){
        txt = (char*)malloc(sizeof(txtMax));
        if(txt == NULL)    return NULL;    
    }
    
    txtBoxNode* p = tbListInsert(&r->tbLst, r->name, NAME, r->uiPrint);
    if(p == NULL){    return NULL;    }
    p->obj.rsrc.txt = txt;
    return (&p->obj);
}
static textboxPic_t* uiPage_placeTxtBxPic(uiPageRsrc_t *r, const char* NAME, u8 txtMax){
    char* txt = NULL;
    if(txtMax){
        txt = (char*)malloc(sizeof(txtMax));
        if(txt == NULL)    return NULL;    
    }

    txtBoxPicNode* p = tbPicListInsert(&r->tbPicLst, r->name, NAME, r->uiPrint);
    if(p == NULL){    return NULL;    }
    p->obj.rsrc.txt = txt;
    return (&p->obj);
}

static pic_t* uiPage_placePic(uiPageRsrc_t *r, const char* NAME){
    picBoxNode* p = picListInsert(&r->picLst, r->name, NAME, r->uiPrint);
    if(p == NULL){    return NULL;    }
    return (&p->obj);
}

static textbox_t* uiPage_getTxtBx(uiPageRsrc_t *r, const char* NAME){
    txtBoxNode *curNode;
    for(curNode=r->tbLst; curNode!=NULL; curNode=curNode->nxt){
        if(strncmp(curNode->obj.rsrc.name, NAME, strlen(NAME)) == 0){
            return &curNode->obj;
        }
    }
    return NULL;
}

static textboxPic_t* uiPage_getTxtBxPic(uiPageRsrc_t *r, const char* NAME){
    txtBoxPicNode *curNode;
    for(curNode=r->tbPicLst; curNode!=NULL; curNode=curNode->nxt){
        if(strncmp(curNode->obj.rsrc.name, NAME, strlen(NAME)) == 0){
            return &curNode->obj;
        }
    }
    return NULL;
}

static pic_t* uiPage_getPic(uiPageRsrc_t *r, const char* NAME){
    picBoxNode *curNode;
    for(curNode=r->picLst; curNode!=NULL; curNode=curNode->nxt){
        if(strncmp(curNode->obj.rsrc.name, NAME, strlen(NAME)) == 0){
            return &curNode->obj;
        }
    }
    return NULL;
}


static s8 uiPageSet(uiPageRsrc_t *r, const char* COMPONENT, const char* ATTR, const char* VAL){
    picBoxNode *picNode;
    for(picNode=r->picLst; picNode!=NULL; picNode=picNode->nxt){
        if(strncmp(picNode->obj.rsrc.name, COMPONENT, strlen(COMPONENT)) == 0){
            if(picNode->obj.set(&picNode->obj.rsrc, ATTR, VAL) == 0){    return 0;    }
            return -2;
        }
    }

    txtBoxNode *tbNode;
    for(tbNode=r->tbLst; tbNode!=NULL; tbNode=tbNode->nxt){
        if(strncmp(tbNode->obj.rsrc.name, COMPONENT, strlen(COMPONENT)) == 0){
            if(tbNode->obj.set(&tbNode->obj.rsrc, ATTR, VAL) == 0){    return 0;    }
            return -3;
        }
    }

    txtBoxPicNode *tbpNode;
    for(tbpNode=r->tbPicLst; tbpNode!=NULL; tbpNode=tbpNode->nxt){
        if(strncmp(tbpNode->obj.rsrc.name, COMPONENT, strlen(COMPONENT)) == 0){
            if(tbpNode->obj.set(&tbpNode->obj.rsrc, ATTR, VAL) == 0){    return 0;    }
            return -4;
        }
    }
    return -1;
}

/**********************************************************
 == THE END ==
**********************************************************/
