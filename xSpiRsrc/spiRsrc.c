/**
  ******************************************************************************
  * File Name          : spiConfig.c
  * Description        : This file provides code for the configuration
  *                      of all used GPIO pins.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "spiRsrc.h"

//SPI_HandleTypeDef hspi1;		// already defined in main
//SPI_HandleTypeDef hspi2;
//SPI_HandleTypeDef hspi3;

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/* firstBit	:     SPI_FIRSTBIT_LSB	SPI_FIRSTBIT_MSB	                        */
/* CLKPolarity	: SPI_POLARITY_LOW	SPI_POLARITY_HIGH                         */
/* CLKPhase  		: SPI_PHASE_1EDGE   SPI_PHASE_2EDGE                           */
/* BaudRatePrescaler	:  SPI_BAUDRATEPRESCALER_2..256                         */
/*----------------------------------------------------------------------------*/
s8 spiConfig(
	SPI_HandleTypeDef* hspi,
	u32 firstBit, 
	uint32_t CLKPolarity,
	uint32_t CLKPhase,
	uint32_t BaudRatePrescaler
){

	if(hspi == NULL)	return -1;
	else if(hspi == &hspi1){
		hspi->Instance = SPI1;
	}
//	else if(hspi == &hspi2){
//		hspi->Instance = SPI2;
//	}
//	else if(hspi == &hspi3){
//		hspi->Instance = SPI3;
//	}
  hspi->Init.Mode = SPI_MODE_MASTER;
  hspi->Init.Direction = SPI_DIRECTION_2LINES;
  hspi->Init.DataSize = SPI_DATASIZE_8BIT;
  hspi->Init.CLKPolarity = CLKPolarity;	//SPI_POLARITY_HIGH;
  hspi->Init.CLKPhase = CLKPhase;				//SPI_PHASE_2EDGE;
  hspi->Init.NSS = SPI_NSS_SOFT;
  hspi->Init.BaudRatePrescaler = BaudRatePrescaler;	//SPI_BAUDRATEPRESCALER_4;
  hspi->Init.FirstBit = firstBit;			//SPI_FIRSTBIT_MSB;
  hspi->Init.TIMode = SPI_TIMODE_DISABLE;
  hspi->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi->Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(hspi) != HAL_OK)
  {
    return -1;
  }
	return 0;

}
	

///**
//  * @brief SPI1 Initialization Function
//  * @param None
//  * @retval None
//  */
//static void MX_SPI1_Init(void)
//{

//  /* USER CODE BEGIN SPI1_Init 0 */

//  /* USER CODE END SPI1_Init 0 */

//  /* USER CODE BEGIN SPI1_Init 1 */

//  /* USER CODE END SPI1_Init 1 */
//  /* SPI1 parameter configuration*/
//  hspi1.Instance = SPI1;
//  hspi1.Init.Mode = SPI_MODE_MASTER;
//  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
//  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
//  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
//  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
//  hspi1.Init.NSS = SPI_NSS_SOFT;
//  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
//  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
//  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
//  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
//  hspi1.Init.CRCPolynomial = 10;
//  if (HAL_SPI_Init(&hspi1) != HAL_OK)
//  {
//    Error_Handler();
//  }
//  /* USER CODE BEGIN SPI1_Init 2 */

//  /* USER CODE END SPI1_Init 2 */

//}

/**
  * @brief This function handles SPI1 global interrupt.
  */
//void SPI1_IRQHandler(void)
//{
//  /* USER CODE BEGIN SPI1_IRQn 0 */

//  /* USER CODE END SPI1_IRQn 0 */
//  HAL_SPI_IRQHandler(&hspi1);
//  /* USER CODE BEGIN SPI1_IRQn 1 */

//  /* USER CODE END SPI1_IRQn 1 */
//}

/**
  * @brief This function handles SPI1 global interrupt.
  */
//void SPI2_IRQHandler(void)
//{
//  /* USER CODE BEGIN SPI1_IRQn 0 */

//  /* USER CODE END SPI1_IRQn 0 */
//  HAL_SPI_IRQHandler(&hspi2);
//  /* USER CODE BEGIN SPI1_IRQn 1 */

//  /* USER CODE END SPI1_IRQn 1 */
//}

/**
  * @brief This function handles SPI1 global interrupt.
  */
//void SPI3_IRQHandler(void)
//{
//  /* USER CODE BEGIN SPI1_IRQn 0 */

//  /* USER CODE END SPI1_IRQn 0 */
//  HAL_SPI_IRQHandler(&hspi3);
//  /* USER CODE BEGIN SPI1_IRQn 1 */

//  /* USER CODE END SPI1_IRQn 1 */
//}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
