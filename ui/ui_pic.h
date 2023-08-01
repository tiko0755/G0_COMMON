/**********************************************************
filename: ui_pic.h
**********************************************************/
#ifndef _UI_PIC_H
#define _UI_PIC_H

#include "misc.h"
#include "ui_common.h"
/*****************************************************************************
 @ typedefs
****************************************************************************/
typedef struct{
	char* parentsName;
	char name[UI_NAME_LEN];
	u16 pic;
	void (*uiPrint)(const char* FORMAT_ORG, ...);
	EventCB_t cbTab[UI_MAX_EVENT];
}pic_rsrc_t;

typedef struct{
	pic_rsrc_t rsrc;
	u8 (*cmd)(pic_rsrc_t*, const char* MSG);
	s8 (*set)(pic_rsrc_t*, const char* attr, const char* FORMAT, ...);
	s8 (*bind)(pic_rsrc_t*, const char* EVNT, uiCB);
}pic_t;

typedef struct picBoxNode{
	pic_t obj;
	struct picBoxNode* nxt;
} picBoxNode;

picBoxNode* picListInsert(picBoxNode** head,
	char *parentsName,
	const char* NAME,
	void (*printLCD)(const char* FORMAT_ORG, ...)
);

#endif
