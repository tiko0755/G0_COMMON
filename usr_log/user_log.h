/**
 *****************************************************************************************
 *
 * @file user_log.h
 *
 * @brief Header file - User Function
 *
 *****************************************************************************************
 */
#ifndef __USER_LOG_H__
#define __USER_LOG_H__

#include <stdint.h>

typedef void (*cbLogPrintS)(const char*);
 
/**
 *****************************************************************************************
 * @brief 
 *****************************************************************************************
 */
void log(const char* FORMAT_ORG, ...);
void log_raw(const char* FORMAT_ORG, ...);
void logInitial(cbLogPrintS xPrintS);

#endif
