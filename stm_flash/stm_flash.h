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
#include "stm_flash_page_def.h"

// the last two page(2K bytes) for this instance
extern uint8_t STM_FLASH_USED_PAGES;   // how many pages will be used for this object
extern uint32_t STM_FLASH_START_ADDR;     /* Start @ of user Flash area */
extern uint32_t STM_FLASH_END_ADDR;   /* End @ of user Flash area */
extern uint32_t STM_FLASH_TOTAL_SZ_BYTES;

int32_t stmFlsh_initial(uint32_t startPage, uint8_t pages);
int32_t stmFlsh_write(uint16_t addr, const uint8_t *pDat, uint16_t nBytes);
int32_t stmFlsh_read(uint16_t addr, uint8_t *pDat, uint16_t nBytes);

u8* stmFlsh_readDMA(uint16_t addr);
int32_t stmFlsh_format(void);
void stmFlsh_print(XPrint xprnt);

#ifdef __cplusplus
}
#endif

#endif
