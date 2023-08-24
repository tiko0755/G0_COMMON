#ifndef __STM_FLASH__
#define __STM_FLASH__

/*
  Author:    Tiko
  (1.0.0)
  First release.
*/

#ifdef __cplusplus
 extern "C" {
#endif

#include "misc.h"
#include "usr_typedef.h"
#include "stm32f3xx_hal_flash.h"

#ifndef FLASH_SIZE
#define FLASH_SIZE            (256*1024)    // should be same as MCU flash
#endif

// the last two page(2K bytes) for this instance
#define STM_FLASH_USED_PAGES   (1)   // how many pages will be used for this object

#define STM_FLASH_START_ADDR   (FLASH_BASE + FLASH_SIZE - (STM_FLASH_USED_PAGES * FLASH_PAGE_SIZE))     /* Start @ of user Flash area */
#define STM_FLASH_END_ADDR     (FLASH_BASE + FLASH_SIZE - 1)                                            /* End @ of user Flash area */

#define STM_FLASH_TOTAL_SZ_BYTES  (STM_FLASH_END_ADDR - STM_FLASH_START_ADDR +1)


int32_t ioWriteAsyn(uint16_t addr, const uint8_t *pDat, uint16_t nBytes, CB2 cmplt);

u8* ioReadDMA(uint16_t addr);


#ifdef __cplusplus
}
#endif

#endif
