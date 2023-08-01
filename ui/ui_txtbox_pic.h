/**********************************************************
filename: ui_txtBox_pic.h
**********************************************************/
#ifndef _UI_TXTBOX_PIC_H
#define _UI_TXTBOX_PIC_H

#include "misc.h"
#include "ui_common.h"
/*****************************************************************************
 @ typedefs
****************************************************************************/
typedef struct{
	char* parentsName;
	char name[UI_NAME_LEN];
	char txt[UI_TEXT_MAX_LEN];
	u16 pic;
	void (*uiPrint)(const char* FORMAT_ORG, ...);
	EventCB_t cbTab[UI_MAX_EVENT];
}tboxPic_rsrc_t;

typedef struct{
	tboxPic_rsrc_t rsrc;
	u8 (*cmd)(tboxPic_rsrc_t*, const char* MSG);
	s8 (*set)(tboxPic_rsrc_t*, const char* attr, const char* FORMAT, ...);
	s8 (*bind)(tboxPic_rsrc_t*, const char* EVNT, uiCB);
}textboxPic_t;

typedef struct txtBoxPicNode{
	textboxPic_t obj;
	struct txtBoxPicNode* nxt;
} txtBoxPicNode;


txtBoxPicNode* tbPicListInsert(txtBoxPicNode** head,
	char *parentsName,
	const char* NAME,
	void (*printLCD)(const char* FORMAT_ORG, ...)
);
#endif