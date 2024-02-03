/**********************************************************
filename: led_flash.h
**********************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LED_FLASH_H__
#define __LED_FLASH_H__

#include "misc.h"
#include "app_timer.h"

/* output variables for extern function --------------------------------------*/
void led_flash_init(appTmrDev_t* tmr, const PIN_T* pin, u16 interval);        
void led_flash_answer(u16 times, u16 interval);

#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
