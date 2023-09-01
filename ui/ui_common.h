/**********************************************************
filename: ui_common.h
**********************************************************/
#ifndef _UI_COMMON_H
#define _UI_COMMON_H

#include "misc.h"
/*****************************************************************************
 @ typedefs
****************************************************************************/
#define UI_NAME_LEN            12    // max len of component's name
#define UI_TEXT_MAX_LEN        32    // max len of text
#define UI_MAX_EVENT        4    // 最大回调事件函数的数量
#define UI_EVNT_NAME_LEN    10    // 事件名称最长8个字节

#define COLOR_BACKGROUND    ("61277")     // background color
#define COLOR_RED           ("63488")     // red color
#define COLOR_GREEN         ("2016")      // green color

//#define UI_EVNT_EDIT        ("edit")
#define UI_EVNT_EDIT_STR    ("edit %s")
#define UI_EVNT_EDIT_NUM    ("edit %d")

//#define UI_EVNT_CHANGE      ("change")
#define UI_EVNT_CHANGE_NUM  ("change_num")
#define UI_EVNT_CHANGE_STR  ("change_str")

#define UI_EVNT_CLICK       ("click")
#define UI_EVNT_DCLICK      ("dclick")

#pragma pack(push,4)        // push current align bytes, and then set 4 bytes align

//typedef void (*uiCB)(int argc, void* argv[]);
typedef void (*uiCB)(int argc, ...);
typedef void (*uiCB_s)(const char*, ...);

typedef struct{
    char evnt[UI_EVNT_NAME_LEN];
    uiCB cb;
}EventCB_t;

#pragma pack(pop)           //recover align bytes from 4 bytes

#endif
