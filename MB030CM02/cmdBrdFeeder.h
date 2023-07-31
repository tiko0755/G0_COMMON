/**********************************************************
filename: cmdBrdFeeder.h
**********************************************************/
#ifndef _CMD_BRD_FDR_H_
#define _CMD_BRD_FDR_H_

#include "misc.h"

/*****************************************************************************
 @ typedefs
****************************************************************************/
extern const char* HELP_brdFdr;
u8 cmdBrdFdr(void *dev, const char* buff, void (*xprint)(const char* FORMAT_ORG, ...));
#endif
