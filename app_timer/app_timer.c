/**
 *****************************************************************************************
 *
 * @file app_timer.c
 *
 * @brief app timer function Implementation.
 *
 *****************************************************************************************
 */

/*
 * INCLUDE FILES
 *****************************************************************************************
 */
#include "app_timer.h"
#include <stdio.h>
#include <string.h>

/*
 * LOCAL FUNCTION DECLARATION
 *****************************************************************************************
 */
static void appTmr_isr(appTmrRsrc_t* rsrc, uint16_t tick);
static void appTmr_polling(appTmrRsrc_t* rsrc);
static int32_t appTmr_start(appTmrRsrc_t* rsrc, uint16_t interval, app_timer_type_t type, CB1 hldr, void* e);
static void appTmr_stop(appTmrRsrc_t* rsrc);
static uint32_t appTmr_status(appTmrRsrc_t* rsrc);

/*
 * GLOBAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */

void setup_appTmr(appTmrDev_t *d){
    memset(&d->rsrc, 0, sizeof(appTmrDev_t));
    d->isr = appTmr_isr;
    d->polling = appTmr_polling;
    d->start = appTmr_start;
    d->stop = appTmr_stop;
    d->status = appTmr_status;
}

static int32_t appTmr_start(appTmrRsrc_t* r, uint16_t interval, app_timer_type_t type, CB1 hldr, void* e){
    if((type > ISR_ONESHOT)){   return -1;    }
    r->handler = NULL;
    r->type = type;
    r->tick = 0;
    r->interval = interval;
    r->handler = hldr;
    r->ptx = e;
    return 0;
}

static void appTmr_stop(appTmrRsrc_t* r){
    r->handler = NULL;
}

static uint32_t appTmr_status(appTmrRsrc_t* r){
    return (uint32_t)(r->handler);
}

/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */
static void appTmr_isr(appTmrRsrc_t* r, uint16_t tick){
    if(r->handler == NULL){ return;     }
    r->tick += tick;

    if(r->tick >= r->interval){
        if((r->type == POLLING_REPEAT) || (r->type == POLLING_ONESHOT)){
            return;
        }
        if(r->type == ISR_REPEAT){
            r->handler(r->ptx);
            r->tick = 0;
        }
        else if(r->type == ISR_ONESHOT){
            r->handler(r->ptx);
            r->handler = NULL;
        }
    }
}

static void appTmr_polling(appTmrRsrc_t* r){
    if(r->handler == NULL){ return;     }
    if(r->tick >= r->interval){
        if(r->type == POLLING_REPEAT){
            r->handler(r->ptx);
            r->tick = 0;
        }
        else if(r->type == POLLING_ONESHOT){
            r->handler(r->ptx);
            r->handler = NULL;
        }
    }
}

