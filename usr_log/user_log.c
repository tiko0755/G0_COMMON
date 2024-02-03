/**
 *****************************************************************************************
 *
 * @file user_log.c
 *
 * @brief User function Implementation.
 *
 *****************************************************************************************
 */

/*
 * INCLUDE FILES
 *****************************************************************************************
 */
#include "user_log.h"
#include <stdint.h>
#include <stddef.h >
#include "stdarg.h"
#include "stdio.h"
#include "string.h"

/*
 * DEFINES
 *****************************************************************************************
 */
#define MAX_LOG_LEN (128)

/*
 * LOCAL VARIABLE DEFINITIONS
 *****************************************************************************************
 */
static cbLogPrintS log_printS = NULL;

void log(const char* FORMAT_ORG, ...){
    if(log_printS == NULL){
        return;
    }
    va_list ap;
    char buf[MAX_LOG_LEN] = {0};
    strcpy(buf, "log:");
    int16_t bytes;
    //take string
    va_start(ap, FORMAT_ORG);
    bytes = vsnprintf(&buf[strlen("log:")], MAX_LOG_LEN, FORMAT_ORG, ap);
    va_end(ap);
    if(bytes <= 0){    return;    }
    strcat(buf, "\r\n");
    //send out by uart
    log_printS(buf);  // SHOULD continue to try lower UART api
}

void log_raw(const char* FORMAT_ORG, ...){
    if(log_printS == NULL){
        return;
    }
    va_list ap;
    char buf[MAX_LOG_LEN] = {0};
    int16_t bytes;
    //take string
    va_start(ap, FORMAT_ORG);
    bytes = vsnprintf(buf, MAX_LOG_LEN, FORMAT_ORG, ap);
    va_end(ap);
    if(bytes <= 0){    return;    }
    //send out by uart
    log_printS(buf);  // SHOULD continue to try lower UART api
}

void logInitial(cbLogPrintS xPrintS){
    log_printS = xPrintS;
}
