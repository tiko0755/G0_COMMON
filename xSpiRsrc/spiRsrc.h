/**
  ******************************************************************************
  * File Name          : gpioConfigOp.h
  * Description        : This file contains all the functions prototypes for 
  *                      the gpio  
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _SPI_RESOURCE_H
#define _SPI_RESOURCE_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "misc.h"
	 
extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi2;
extern SPI_HandleTypeDef hspi3;

s8 spiConfig(
	SPI_HandleTypeDef* handler,
	u32 firstBit, 
	uint32_t CLKPolarity,
	uint32_t CLKPhase,
	uint32_t BaudRatePrescaler
);

//void SPI1_IRQHandler(void);
//void SPI2_IRQHandler(void);
//void SPI3_IRQHandler(void);

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
