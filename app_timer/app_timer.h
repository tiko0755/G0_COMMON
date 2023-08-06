/**
 ****************************************************************************************
 *
 * @file app_timer.h
 *
 * @brief Header file - User Function
 *
 ****************************************************************************************
 */
#ifndef __APP_TIMER_H__
#define __APP_TIMER_H__

/*
 * INCLUDE FILES
 *****************************************************************************************
 */

#include "usr_typedef.h"

/*
 * typedef
 *****************************************************************************************
 */
 
#pragma pack(push,4)        // push current align bytes, and then set 4 bytes align

/** @brief App timer node state variable. */
typedef enum
{
    POLLING_REPEAT     = 0,
    POLLING_ONESHOT,
    ISR_REPEAT,
    ISR_ONESHOT,
}app_timer_type_t;

typedef struct {
    CB1 handler;
    uint32_t tick,interval;
    void* ptx;
    app_timer_type_t type; 
}appTmrRsrc_t;

typedef struct {
    appTmrRsrc_t rsrc;
    void (*isr)(appTmrRsrc_t* rsrc, uint16_t tick);
    void (*polling)(appTmrRsrc_t* rsrc);
    int32_t (*start)(appTmrRsrc_t* rsrc, uint16_t interval, app_timer_type_t type, CB1 hldr, void* e);
    void (*stop)(appTmrRsrc_t* rsrc);
    uint32_t (*status)(appTmrRsrc_t* rsrc);
}appTmrDev_t;

#pragma pack(pop)           //recover align bytes from 4 bytes

/*
 * GLOBAL FUNCTION DECLARATION
 *****************************************************************************************
 */

void setup_appTmr(appTmrDev_t *dev);

#endif

