/**********************************************************
filename: ui_common.h
**********************************************************/
#ifndef _UI_COMMON_H
#define _UI_COMMON_H

#include "misc.h"
/*****************************************************************************
 @ typedefs
****************************************************************************/
#define UI_NAME_LEN			12	// max len of component's name
#define UI_TEXT_MAX_LEN		32	// max len of text
#define UI_MAX_EVENT		4	// 最大回调事件函数的数量
#define UI_EVNT_NAME_LEN	8	// 事件名称最长8个字节

//typedef void (*uiCB)(int argc, void* argv[]);
typedef void (*uiCB)(int argc, ...);
typedef void (*uiCB_s)(const char*, ...);

typedef struct{
	char evnt[UI_EVNT_NAME_LEN];
	uiCB cb;
}EventCB_t;

#endif
