/**********************************************************
filename: ui_pic.c
**********************************************************/
/************************����ͷ�ļ�***************************************************/
#include "misc.h"
#include "stdlib.h"
#include "stdio.h"
#include "stdarg.h"
#include <string.h>
#include "ui_pic.h"

/**********************************************************
 Private function
**********************************************************/
// pgMain.tbStatus
static u8 ui_pic_cmd(pic_rsrc_t*, const char* MSG);
static s8 ui_pic_Set(pic_rsrc_t* rsrc, const char* attr, const char* FORMAT, ...);
static s8 ui_pic_bindOn(pic_rsrc_t* rsrc, const char* EVNT, uiCB cb);
static void pic_setup(
	void *p,
	char* parentsName,
	const char* NAME,
	void (*printLCD)(const char* FORMAT_ORG, ...)
);
/**********************************************************
 Public function
**********************************************************/
static void pic_setup(
	void *p,
	char* parentsName,
	const char* NAME,
	void (*printLCD)(const char* FORMAT_ORG, ...)
){
	pic_t *dev;
	pic_rsrc_t *rsrc;

	memset(p,0,sizeof(pic_t));
	dev = (pic_t*)p;
	rsrc = &dev->rsrc;
	rsrc->parentsName = parentsName;
	strcpy(rsrc->name, NAME);
	rsrc->uiPrint = printLCD;

	dev->cmd = ui_pic_cmd;
	dev->set = ui_pic_Set;
	dev->bind = ui_pic_bindOn;
}

picBoxNode* picListInsert(picBoxNode** head,
	char *parentsName,
	const char* NAME,
	void (*printLCD)(const char* FORMAT_ORG, ...)
){
	picBoxNode* newnode = (picBoxNode*)malloc(sizeof(picBoxNode));
	if(newnode == NULL)	return NULL;

	memset(newnode,0,sizeof(picBoxNode));
	pic_setup(&newnode->obj, parentsName, NAME, printLCD);

	if (*head == NULL)
	{
		*head = newnode;
		return newnode;
	}
	picBoxNode* tail = *head;
	while (tail->nxt != NULL)
	{
		tail = tail->nxt;
	}
	tail->nxt = newnode;
	return newnode;
}

static u8 ui_pic_cmd(pic_rsrc_t* rsrc, const char* MSG){
	s32 i,s32tmp;
	const char* msg;
	if(strncmp(MSG, rsrc->name, strlen(rsrc->name)) != 0){	return 0;	}
	msg = &MSG[strlen(rsrc->name)+1];
	if(sscanf(msg, "click %d", &s32tmp)==1){
		for(i=0;i<UI_MAX_EVENT;i++){
			if(strncmp(rsrc->cbTab[i].evnt, "click", strlen("click")) == 0){
				if(rsrc->cbTab[i].cb){	rsrc->cbTab[i].cb(1, s32tmp);	}
			}
		}
		return 1;
	}
	return 0;
}

static s8 ui_pic_Set(pic_rsrc_t* rsrc, const char* attr, const char* FORMAT, ...){
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
			rsrc->uiPrint("%s.%s.txt=\"%s\"", rsrc->parentsName, rsrc->name, buf);
		}
		else{
			rsrc->uiPrint("%s.%s.%s=%s", rsrc->parentsName, rsrc->name, attr, buf);
		}
		return 0;
	}
	else{	return -1;	}
}

static s8 ui_pic_bindOn(pic_rsrc_t* rsrc, const char* EVNT, uiCB cb){
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
