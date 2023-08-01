/******************** (C) COPYRIGHT 2022 INCUBECN *****************************
* File Name          : script_demo.c
* Author             : Tiko Zhong
* Date First Issued  : 09/21,2022
* Description        :
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "script_demo.h"

/*
start 'disconnect\r\n';
$00:
req '1.input.readpin(9)\r\n' 50;
res.switch '(9,0)' $timeout;
req '2.m0.homing(64000)\r\n' 50;
req '2.m1.homing(64000)\r\n' 50;
req '2.m0.pos()\r\n' 50;
$01:
req '1.input.readpin(9)\r\n' 50;
res.switch '(9,0)' $timeout;
req '2.m0.homing(64000)\r\n' 50;
req '2.m1.homing(64000)\r\n' 50;
req '2.m0.pos()\r\n' 50;
$02:
req '1.input.readpin(9)\r\n' 50;
res.switch '(9,0)' $timeout;
req '2.m0.homing(64000)\r\n' 50;
req '2.m1.homing(64000)\r\n' 50;
req '2.m0.pos()\r\n' 50;
$03:
req '1.input.readpin(9)\r\n' 50;
res.switch '(9,0)' $timeout;
req '2.m0.homing(64000)\r\n' 50;
req '2.m1.homing(64000)\r\n' 50;
req '2.m0.pos()\r\n' 50;
jmp $00;
 * */

const char SCRIPT_0[] = {
	"start 'script0\r\n';"
"loop:"
	"delay 100;"
	"print 'script0..\r\n';"
	"req '1.reg.read 0\r\n' 100;"
	"res.switch '(0,123)' loop;"
"$timeout:"
	"print '+timeout@SCRIPT_0\r\n';"
	"ret;"
"isr_cmd:"
	"reti;"
};

const char SCRIPT_1[] = {
	"start 'script1\r\n';"
	"delay 1000;"
"loop:"
	"print 'script1..\r\n';"
	"req '1.reg.read 1\r\n' 100;"
	"delay 100;"
	"res.switch '(1,11)' loop;"
"$timeout:"
	"print '+timeout@SCRIPT_1\r\n';"
	"ret;"
"isr_cmd:"
	"reti;"
};

const char SCRIPT_2[] = {
	"start 'script2\r\n';"
	"delay 1500;"
"loop:"
	"print 'script2..\r\n';"
	"req '1.reg.read 2\r\n' 100;"
	"delay 100;"
	"res.switch '(2,12)' loop;"
"$timeout:"
	"print '+timeout@SCRIPT_2\r\n';"
	"ret;"
"isr_cmd:"
	"reti;"
};

const char SCRIPT_3[] = {
	"start 'script3\r\n';"
	"delay 2000;"
"loop:"
	"delay 100;"
	"print 'script3..\r\n';"
	"req '1.reg.read 3\r\n' 100;"
	"res.switch '(3,13)' loop;"
"$timeout:"
	"print '+timeout@SCRIPT_3\r\n';"
	"ret;"
"isr_cmd:"
	"reti;"
};

const char SCRIPT_MAIN[] = {
	"start 'script_main\r\n';"
"loop:"
	"print 'script main..\r\n';"
	"req '1.reg.read 4\r\n' 1000;"
	"delay 100;"
	"res.switch '(4,14)' loop;"
"$timeout:"
	"print '+timeout@script_main\r\n';"
	"ret;"
"isr_cmd:"
	"reti;"
};

SCRIPT_T STARTS[SCRIPT_COUNT] = {
	{SCRIPT_0, NULL},
	{SCRIPT_1, NULL},
	{SCRIPT_2, NULL},
	{SCRIPT_3, NULL},
//	{NULL, NULL},
};

/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
