/**
 *****************************************************************************************
 *
 * @file thread_delay.c
 *
 * @brief thread delay function Implementation.
 *
 *****************************************************************************************
 */

/*
 * INCLUDE FILES
 *****************************************************************************************
 */
#include "misc.h"
#include "thread_delay.h"
#include <stdio.h>
#include <string.h>


/*
 * LOCAL FUNCTION DECLARATION
 *****************************************************************************************
 */
static appTmrDev_t* thrdTmr;

static void threadPollingHanlder(void* e);

static uint16_t thrd_tick[THREAD_DELAY_COUNT];
static uint32_t thrd_en = 0;    

/*
 * GLOBAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
static void threadPollingHanlder(void* e){
    uint8_t i;
    for(i=0;i<THREAD_DELAY_COUNT;i++){
        thrd_tick[i]++;
    }
}
 
void thread_delay_init(appTmrDev_t* tmr){
    thrdTmr = tmr;
    thrdTmr->start(&thrdTmr->rsrc, 1, ISR_REPEAT, threadPollingHanlder, NULL);
}

static uint16_t thrd_tick[THREAD_DELAY_COUNT];
    

void thread_delay(uint16_t ms){
    uint8_t tickIndx;

    // will loop until there is a free tick availid
    for(tickIndx=0;1;tickIndx++){
        if(tickIndx >= THREAD_DELAY_COUNT){
            tickIndx = 0;
        }
        if((thrd_en & BIT(tickIndx)) == 0){
            thrd_en |= BIT(tickIndx);
            thrd_tick[tickIndx] = 0;
            do{
                if(thrdTmr->rsrc.pollingWhileDelay)
                    thrdTmr->rsrc.pollingWhileDelay();
            }while(thrd_tick[tickIndx] < ms);
            thrd_en &= (0xffffffff^BIT(tickIndx));  // release this tick
            return;
        }
    }    
}
