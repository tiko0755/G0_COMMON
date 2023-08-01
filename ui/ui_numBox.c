/**********************************************************
filename: ui_numBox.c
**********************************************************/

/************************����ͷ�ļ�***************************************************/
#include "misc.h"
#include "stdlib.h"
#include "stdio.h"
#include "stdarg.h"
#include <string.h>
#include "ui_numbox.h"

/**********************************************************
 Private function
**********************************************************/
// pgMain.tbStatus
static u8 ui_numbox_cmd(numBox_rsrc_t*, const char* MSG);
static void ui_numbox_setColor(numBox_rsrc_t*, u16 color);
void ui_numbox_SetNum(numBox_rsrc_t*, float num);
static s8 ui_numbox_bindOnChanged(numBox_rsrc_t* rsrc, uiCB);

/**********************************************************
 Public function
**********************************************************/
void numBox_setup(
	void *p,
	const char* NAME,
	void (*printLCD)(const char* FORMAT_ORG, ...)
){
	numBox_t *dev;
	numBox_rsrc_t *rsrc;

	memset(p, 0, sizeof(numBox_t));
	dev = (numBox_t*)p;
	rsrc = &dev->rsrc;

	strcpy(rsrc->name, NAME);
	rsrc->uiPrint = printLCD;

	dev->cmd = ui_numbox_cmd;
	dev->setColor = ui_numbox_setColor;
	dev->setNum = ui_numbox_SetNum;
	dev->bindOnChanged = ui_numbox_bindOnChanged;
}

static u8 ui_numbox_cmd(numBox_rsrc_t* rsrc, const char* MSG){
	s32 i,num;
	char xName[UI_NAME_LEN] = {0};
	if(sscanf(MSG, "%s %d", xName, &num)==2){
		if(strncmp(xName, rsrc->name, strlen(xName)) != 0){
			return 0;
		}
		if(num == rsrc->num){
			return 0;
		}
		for(i=0;i<UI_MAX_EVENT;i++){
			if( rsrc->onChangedTab[i] != NULL){
				rsrc->onChangedTab[i](num);
			}
		}
		rsrc->num = num;
		return 1;
	}
	return 0;
}

static void ui_numbox_setColor(numBox_rsrc_t* rsrc, u16 color){
	rsrc->uiPrint("%s.pco=%d", rsrc->name, color);
}

void ui_numbox_SetNum(numBox_rsrc_t* rsrc, float num){
	rsrc->uiPrint("%s.val=%0.3f", rsrc->name, num);
	rsrc->num = num;
}

static s8 ui_numbox_bindOnChanged(numBox_rsrc_t* rsrc, uiCB cb){
	s8 i;
	for(i=0;i<UI_MAX_EVENT;i++){
		if(rsrc->onChangedTab[i] == NULL){
			rsrc->onChangedTab[i] = cb;
			return i;
		}
	}
	return -1;
}


/**********************************************************
 == THE END ==
**********************************************************/
