/******************** (C) COPYRIGHT 2015 TIKO **********************************
* File Name          : led_flash.c
* Author             : Tiko Zhong
* Date First Issued  : 11/18/2023
* Description        : 
*                      
********************************************************************************
* History:
* AGU05,2023: V0.0    
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "led_flash.h"
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

#include "board.h"

/* Public variables ---------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define LED_FLASH_TIM   (10)    // in ms
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static u16 ledTickTmr = 128;
static u16 ledTickTmrPrv = 128;
static u16 ledFlshTz = 0;
static appTmrDev_t* ledFlsh_tmr;
static const PIN_T* ledPin;

/* Private function prototypes -----------------------------------------------*/
static u16 led_flash_setTickTmr(u16 interval);


/**
  * @brief tack per 8ms
  * @param none
  * @retval None
  */
static void led_flash_handler(void* e){
    HAL_GPIO_TogglePin(ledPin->GPIOx, ledPin->GPIO_Pin);
    
    if(ledFlshTz > 1){
        ledFlshTz --;
    }
    else if(ledFlshTz == 1){
        ledFlshTz = 0;
        led_flash_setTickTmr(ledTickTmrPrv);
    }
}

static u16 led_flash_setTickTmr(u16 interval){
    ledTickTmr = interval;
    if(ledTickTmr <= LED_FLASH_TIM){
        ledTickTmr = LED_FLASH_TIM;
    }
    ledFlsh_tmr->start(&ledFlsh_tmr->rsrc, ledTickTmr, POLLING_REPEAT, led_flash_handler, NULL);
    return ledTickTmr;
}

void led_flash_init(appTmrDev_t* tObj, const PIN_T* pin, u16 interval){
    ledFlsh_tmr = tObj;
    ledPin = pin;
    ledTickTmrPrv = led_flash_setTickTmr(interval);
}

void led_flash_answer(u16 times, u16 interval){
    ledFlshTz = times;    
    led_flash_setTickTmr(interval);
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
