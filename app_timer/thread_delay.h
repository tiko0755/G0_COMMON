/**
 ****************************************************************************************
 *
 * @file thread_delay.h
 *
 * @brief Header file - User Function
 *
 ****************************************************************************************
 */
#ifndef __THREAD_DELAY_H__
#define __THREAD_DELAY_H__

/*
 * INCLUDE FILES
 *****************************************************************************************
 */
#include <stdint.h>
#include "usr_typedef.h"
#include "app_timer.h"

// Allow THREAD_DELAY_COUNT run at the same time
#define    THREAD_DELAY_COUNT (32)  // 32 maxmium

/*
 * typedef
 *****************************************************************************************
 */
 
//#pragma pack(push,4)        // push current align bytes, and then set 4 bytes align

//#pragma pack(pop)           //recover align bytes from 4 bytes

/*
 * GLOBAL FUNCTION DECLARATION
 *****************************************************************************************
 */
extern uint32_t thrd_en;
extern uint32_t thrd_tick[THREAD_DELAY_COUNT];

void thread_delay_init(appTmrDev_t* ptmr);
void thread_delay(uint32_t ms);

#endif

