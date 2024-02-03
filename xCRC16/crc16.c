
#include "crc16.h"
#include "main.h"

#define INITIAL_CRC_CC3     0x1D0F
#define CRC_CCITT_POLY            0x1021     //CRC-CCITT, polynormial 0x1021.


void CRC8_Init(void)
{

  /* USER CODE BEGIN CRC_Init 0 */

  /* USER CODE END CRC_Init 0 */

  /* USER CODE BEGIN CRC_Init 1 */

  /* USER CODE END CRC_Init 1 */
  hcrc.Instance = CRC;
  hcrc.Init.DefaultPolynomialUse = DEFAULT_POLYNOMIAL_DISABLE;
  hcrc.Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_ENABLE;
  hcrc.Init.GeneratingPolynomial = 101;
  hcrc.Init.CRCLength = CRC_POLYLENGTH_7B;
  hcrc.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_NONE;
  hcrc.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_DISABLE;
  hcrc.InputDataFormat = CRC_INPUTDATA_FORMAT_BYTES;
  if (HAL_CRC_Init(&hcrc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CRC_Init 2 */

  /* USER CODE END CRC_Init 2 */

}


uint8_t crc8(const uint8_t *ptr, int count){
    return(HAL_CRC_Calculate(&hcrc, (uint32_t *)ptr, count));
}




uint16_t CRC16(const uint8_t *ptr, int count, uint16_t crcInit)
{
    uint16_t crc;
    uint8_t i;

    crc = crcInit;
    while(--count >= 0 )
    {
        crc = crc ^ ((uint16_t) (*ptr++ << 8));
        for(i = 0; i < 8; i++)
        {
            if( crc & 0x8000 )
            {
                crc = (crc << 1) ^ CRC_CCITT_POLY;
            }
            else
            {
                crc = crc << 1;
            }
        }
    }

    return crc;
}

