/**********************************************************
filename: uartLcdUi.c
**********************************************************/

/************************����ͷ�ļ�***************************************************/
#include "ui.h"
#include "misc.h"

#include "stdlib.h"
#include "stdio.h"
#include "stdarg.h"
#include <string.h>

/**********************************************************
 Private function
**********************************************************/
// pgMain.tbStatus
static uiPage_t* uiNewPage(uiRsrc_T *rsrc, const char* NAME);
static textbox_t* uiPlaceTxtBx(uiRsrc_T*, const char* PAGE, const char* COMPONENT);
static textboxPic_t* uiPlaceTxtBxPic(uiRsrc_T*, const char* PAGE, const char* COMPONENT);
static pic_t* uiPlacePic(uiRsrc_T*, const char* PAGE, const char* COMPONENT);
static textbox_t* uiGetTxtBx(uiRsrc_T*,  const char* PAGE_NAME, const char* COMPONENT_NAME);
static textboxPic_t* uiGetTxtBxPic(uiRsrc_T*, const char* PAGE_NAME, const char* COMPONENT_NAME);
static pic_t* uiGetPic(uiRsrc_T*, const char* PAGE_NAME, const char* COMPONENT_NAME);

static void uiPolling(uiRsrc_T*, u16 tick);
static s8 uiVisual(uiRsrc_T *r, const char* PAGE, const char* COMPONENT, u8 vis);
static s8 uiSet(uiRsrc_T*, const char* PAGE, const char* COMPONENT, const char* ATTR, const char* FORMAT_ORG, ...);
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
	if(uiWaitReady(pr, 3000) < 0){
		print("LCD is not ready\n");
	}
	// component setup
	pd->Polling = uiPolling;
	pd->Set = uiSet;
	pd->Bind = uiBind;
	pd->NewPage = uiNewPage;
	pd->PlacePic = uiPlacePic;
	pd->PlaceTxtBx = uiPlaceTxtBx;
	pd->PlaceTxtBxPic = uiPlaceTxtBxPic;
	pd->GetPic = uiGetPic;
	pd->GetTxtBx = uiGetTxtBx;
	pd->GetTxtBxPic = uiGetTxtBxPic;
	pd->Visual = uiVisual;
}
/**********************************************************
 read data
**********************************************************/
static uiPage_t* uiNewPage(uiRsrc_T *rsrc, const char* NAME){
	uiPageNode* newnode = (uiPageNode*)malloc(sizeof(uiPageNode));
	if(newnode == NULL)	return NULL;
	memset(newnode,0,sizeof(uiPageNode));

	uiPageSetup(&newnode->obj, NAME, rsrc->uiPrint);

	if (rsrc->pageLst == NULL)
	{
		rsrc->pageLst = newnode;
		return &newnode->obj;
	}
	uiPageNode* tail = rsrc->pageLst;
	while (tail->nxt != NULL){	tail = tail->nxt;	}
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

static textbox_t* uiPlaceTxtBx(uiRsrc_T *r, const char* PAGE, const char* COMPONENT){
	uiPageNode *pgNode = uiGetPageNode(r,PAGE);
	if(pgNode==NULL){	return NULL;	}
	return(pgNode->obj.placeTxtBx(&pgNode->obj.rsrc, COMPONENT));
}

static textboxPic_t* uiPlaceTxtBxPic(uiRsrc_T *r, const char* PAGE, const char* COMPONENT){
	uiPageNode *pgNode = uiGetPageNode(r,PAGE);
	if(pgNode==NULL){	return NULL;	}
	return(pgNode->obj.placeTxtBxPic(&pgNode->obj.rsrc, COMPONENT));
}

static pic_t* uiPlacePic(uiRsrc_T *r, const char* PAGE, const char* COMPONENT){
	uiPageNode *pgNode = uiGetPageNode(r,PAGE);
	if(pgNode==NULL){	return NULL;	}
	return(pgNode->obj.placePic(&pgNode->obj.rsrc, COMPONENT));
}

/**********************************************************
 ui polling
**********************************************************/
static void uiPolling(uiRsrc_T* rsrc, u16 tick){
	UNUSED(tick);
	char buff[UI_TEXT_MAX_LEN] = {0};
	uiPageNode* pgNode;
	if(fetchLineFromRingBuffer(&rsrc->pUartDev->rsrc.rxRB, buff, UI_TEXT_MAX_LEN)){
		print("LCD:%s", buff);
		for (pgNode=rsrc->pageLst; pgNode != NULL; pgNode=pgNode->nxt)
		{
			if(pgNode->obj.cmd(&pgNode->obj.rsrc, buff)){	break;	}
		}
	}
}

static s8 uiWaitReady(uiRsrc_T* rsrc, u32 timeout){
	u32 tick = 0;
	char buff[UI_TEXT_MAX_LEN] = {0};

	while(tick < timeout){
		rsrc->uiPrint("readVer=1");
		HAL_Delay(100);
		rsrc->pUartDev->RxPolling(&rsrc->pUartDev->rsrc);
		if(fetchLineFromRingBuffer(&rsrc->pUartDev->rsrc.rxRB, buff, UI_TEXT_MAX_LEN)){
			if(sscanf(buff, "feeder.tVer.edit %s", rsrc->ver) == 1){
				print("LCD Ver%s\n", rsrc->ver);
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
	if(bytes < 0){	return -1;	}

	for (pgNode=r->pageLst; pgNode != NULL; pgNode=pgNode->nxt)
	{
		if(strncmp(PAGE, pgNode->obj.rsrc.name, strlen(pgNode->obj.rsrc.name)) != 0){	continue;	}
		return (pgNode->obj.Set(&pgNode->obj.rsrc, COMPONENT, ATTR, buf));
	}
	return -2;
}

static s8 uiVisual(uiRsrc_T *r, const char* PAGE, const char* COMPONENT, u8 vis){
	r->uiPrint("vis %s.%s %d", PAGE, COMPONENT, vis);
}

static s8 uiBind(uiRsrc_T *r, const char* PAGE, const char* COMPONENT, const char* EVENT, uiCB cb){
	textbox_t* tb = uiGetTxtBx(r,  PAGE, COMPONENT);
	if(tb != NULL){	return(tb->bind(&tb->rsrc, EVENT, cb));	}
	textboxPic_t* tbp = uiGetTxtBxPic(r,  PAGE, COMPONENT);
	if(tbp != NULL){	return(tbp->bind(&tbp->rsrc, EVENT, cb));	}
	pic_t* pic = uiGetPic(r,  PAGE, COMPONENT);
	if(pic != NULL){	return(pic->bind(&pic->rsrc, EVENT, cb));	}
	return -1;
}

static textbox_t* uiGetTxtBx(uiRsrc_T *r,  const char* PAGE, const char* COMPONENT){
	uiPageNode *pgNode = uiGetPageNode(r,PAGE);
	if(pgNode==NULL){	return NULL;	}
	return(pgNode->obj.getTxtBx(&pgNode->obj.rsrc, COMPONENT));
}

static textboxPic_t* uiGetTxtBxPic(uiRsrc_T *r, const char* PAGE, const char* COMPONENT){
	uiPageNode *pgNode = uiGetPageNode(r,PAGE);
	if(pgNode==NULL){	return NULL;	}
	return(pgNode->obj.getTxtBxPic(&pgNode->obj.rsrc, COMPONENT));
}

static pic_t* uiGetPic(uiRsrc_T *r, const char* PAGE, const char* COMPONENT){
	uiPageNode *pgNode = uiGetPageNode(r,PAGE);
	if(pgNode==NULL){	return NULL;	}
	return(pgNode->obj.getPic(&pgNode->obj.rsrc, COMPONENT));
}

/**********************************************************
 == THE END ==
**********************************************************/

