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
#include "board.h"

/*
 * LOCAL FUNCTION DECLARATION
 *****************************************************************************************
 */
static appTmrDev_t* thrdTmr;

static void threadPollingHanlder(void* e);

uint32_t thrd_tick[THREAD_DELAY_COUNT];
uint32_t thrd_en = 0;    

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
 
void thread_delay_init(appTmrDev_t* ptmr){
    thrdTmr = ptmr;
    thrdTmr->start(&thrdTmr->rsrc, 1, ISR_REPEAT, threadPollingHanlder, NULL);
}
    
void thread_delay(uint32_t ms){
    uint8_t tickIndx;
    u32 t=0;
    // will loop until there is a free tick availid
    for(tickIndx=0;1;tickIndx++){
        if(tickIndx >= THREAD_DELAY_COUNT){
            tickIndx = 0;
        }
        else if((thrd_en & BIT(tickIndx)) == 0){
            thrd_en |= BIT(tickIndx);
            thrd_tick[tickIndx] = 0;
            do{
                thrdTmr->rsrc.pollingWhileDelay();
            }while(thrd_tick[tickIndx] < ms);
            thrd_en &= (0xffffffff^BIT(tickIndx));  // release this tick
            break;
        }
    }
}
