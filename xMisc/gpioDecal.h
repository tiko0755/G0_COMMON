/**
  ******************************************************************************
  * File Name          : gpioConfigOp.h
  * Description        : This file contains all the functions prototypes for 
  *                      the gpio  
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _gpio_decal_H
#define _gpio_decal_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "misc.h"
	 
void as_OUTPUT_PP_NOPULL_LOW		(PIN_T PIN);
void as_OUTPUT_PP_NOPULL_HIGH	(PIN_T PIN);
void as_OUTPUT_PP_PULLUP_LOW		(PIN_T PIN);
void as_OUTPUT_PP_PULLUP_HIGH	(PIN_T PIN);
void as_OUTPUT_PP_PULLDWN_LOW	(PIN_T PIN);
void as_OUTPUT_PP_PULLDWN_HIGH	(PIN_T PIN);
void as_OUTPUT_OD_NOPULL_LOW		(PIN_T PIN);
void as_OUTPUT_OD_NOPULL_HIGH	(PIN_T PIN);
void as_OUTPUT_OD_PULLUP_LOW		(PIN_T PIN);
void as_OUTPUT_OD_PULLUP_HIGH	(PIN_T PIN);
void as_INPUT_NOPULL						(PIN_T PIN);
void as_INPUT_PULLUP						(PIN_T PIN);
void as_INPUT_PULLDWN					(PIN_T PIN);

void writePin(PIN_T, GPIO_PinState);
void togglePin(PIN_T);
GPIO_PinState readPin(PIN_T);
void writePinX(PIN_T, GPIO_PinState, ...);
void togglePinX(PIN_T, ...);
u32 readPinPinX(PIN_T, ...);	

#ifdef __cplusplus
}
#endif
#endif /*__ pinoutConfig_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
