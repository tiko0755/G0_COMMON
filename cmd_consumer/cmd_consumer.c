/**
 *****************************************************************************************
 *
 * @file cmd_consumer.c
 *
 * @brief  consume command from a ring buffer
 *
 *****************************************************************************************
 */
 
/*
 * INCLUDE FILES
 *****************************************************************************************
 */
#include "cmd_consumer.h"
#include "string.h"
#include "misc.h"
//#include "app_log.h"
//#include "app_error.h"
//#include "gr_uartDev.h"    //fetchLineFromRingBuffer()
//#include "thsBoard.h"

//#include "board.h"

/*
 * PRIVATE FUNCTION DEFINITIONS
 *****************************************************************************************
 */
static void cmdConsumer_start(cmdConsumerRsrc_t* rsrc);
static void cmdConsumer_stop(cmdConsumerRsrc_t* r);
static u8 cmdConsumer_append(cmdConsumerRsrc_t*, cmd_consumer consumer);
static u8 cmdConsumer_remove(cmdConsumerRsrc_t*, cmd_consumer consumer);
static void cmdConsumerTmr_handle(void* p_ctx);

/*
 * LOCAL VARIABLE DEFINITIONS
 *****************************************************************************************
 */
void setup_cmdConsumer(cmdConsumerDev_t* d, 
    RINGBUFF_T* rb,             // command line in a ringbuffer
    cmd_fetchLine fetch,    // fetchLine method    
    XPrint xprint,              // print out
    cmd_forward xForward,            // print out
    appTmrDev_t* tmr,           // timer
    u16 interval                // unit in ms, polling rb each interval
){
    cmdConsumerRsrc_t* r = &d->rsrc;
    memset(d,0,sizeof(cmdConsumerDev_t));
    
    r->tmrID = tmr;
    r->interval = interval;
    r->rb = rb;    
    r->xprint = xprint;
    r->forward = xForward;
    r->fetchLine = fetch;
    
    d->start = cmdConsumer_start;
    d->stop = cmdConsumer_stop;
    d->append = cmdConsumer_append;
    d->remove = cmdConsumer_remove;

    // start a timer    
    cmdConsumer_start(r);
}

static void cmdConsumerTmr_handle(void* p_ctx){
    u8 cmdS[MAX_CMD_LEN] = {0}, executed;
    u16 len;
    cmdConsumerRsrc_t* r = (cmdConsumerRsrc_t*)p_ctx;
    int count = RingBuffer_GetCount(r->rb);
    if((count<=0) || (r->fetchLine==NULL))    return;
    
    // fetch a command line from ringbuffer
    len = r->fetchLine(r->rb, cmdS, MAX_CMD_LEN);
    if(len == 0){    return;    }

//    print("new line[%d]: ", len);
//    for(int x=0; x<len; x++){
//        print("%02x ", cmdS[x]);
//    }
//    print("\n");

    // consume this command line
    executed = 0;
    for(u8 i=0;i<CMD_CONSUMER_MAX;i++){
        if(r->consumers[i] == NULL){    continue;   }
        if(r->consumers[i](cmdS, len, r->xprint)){
            executed = 1;
            break;    
        }
    }
    
    // if NOT executed, forward out
    if((executed == 0) && (r->forward != NULL)){
        r->forward(cmdS, len);
    }
    
}

static void cmdConsumer_start(cmdConsumerRsrc_t* r){
    r->tmrID->start(&r->tmrID->rsrc, r->interval, POLLING_REPEAT, cmdConsumerTmr_handle, r);
}

static void cmdConsumer_stop(cmdConsumerRsrc_t* r){
    r->tmrID->stop(&r->tmrID->rsrc);
}

static u8 cmdConsumer_append(cmdConsumerRsrc_t* r, cmd_consumer consumer){
    u8 same = 0;
    u16 nullIndx = 0xffff;
    int32_t i;
    
    for(i=0;i<MAX_CMD_LEN;i++){
        if(consumer == r->consumers[i]){
            same = 1;
            break;
        }
        if((nullIndx==0xffff) && (r->consumers[i]==NULL)){
            nullIndx = i;
        }
    }
    if(same==0 && nullIndx!=0xffff){
        r->consumers[nullIndx] = consumer;
        return 1;
    }
    return 0;
}

static u8 cmdConsumer_remove(cmdConsumerRsrc_t* r, cmd_consumer consumer){
    return 0;
}

/************************************ END OF FILE ************************************/

