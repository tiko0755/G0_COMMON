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
     
void as_OUTPUT_PP_NOPULL_LOW     (const PIN_T* PIN);
void as_OUTPUT_PP_NOPULL_HIGH    (const PIN_T* PIN);
void as_OUTPUT_PP_PULLUP_LOW     (const PIN_T* PIN);
void as_OUTPUT_PP_PULLUP_HIGH    (const PIN_T* PIN);
void as_OUTPUT_PP_PULLDWN_LOW    (const PIN_T* PIN);
void as_OUTPUT_PP_PULLDWN_HIGH   (const PIN_T* PIN);
void as_OUTPUT_OD_NOPULL_LOW     (const PIN_T* PIN);
void as_OUTPUT_OD_NOPULL_HIGH    (const PIN_T* PIN);
void as_OUTPUT_OD_PULLUP_LOW     (const PIN_T* PIN);
void as_OUTPUT_OD_PULLUP_HIGH    (const PIN_T* PIN);
void as_INPUT_NOPULL             (const PIN_T* PIN);
void as_INPUT_PULLUP             (const PIN_T* PIN);
void as_INPUT_PULLDWN            (const PIN_T* PIN);

void writePin(const PIN_T*, GPIO_PinState);
void togglePin(const PIN_T*);
GPIO_PinState readPin(const PIN_T*);
void writePinX(const PIN_T*, GPIO_PinState, ...);
void togglePinX(const PIN_T*, ...);
u32 readPinPinX(const PIN_T*, ...);

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
