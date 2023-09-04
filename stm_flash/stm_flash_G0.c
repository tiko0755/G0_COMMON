#include "stm_flash.h"
#include <string.h>
#include <stdlib.h>
#include "user_log.h"
#include "thread_delay.h"

static uint32_t GetPage(uint32_t Addr);
static void stmFlsh_printPG(uint8_t *);

uint8_t STM_FLASH_USED_PAGES        = (1);   // how many pages will be used for this object
uint32_t STM_FLASH_START_ADDR       = ADDR_FLASH_PAGE_31;     /* Start @ of user Flash area */
uint32_t STM_FLASH_END_ADDR         = (ADDR_FLASH_PAGE_31 + FLASH_PAGE_SIZE - 1);   /* End @ of user Flash area */
uint32_t STM_FLASH_TOTAL_SZ_BYTES   = FLASH_PAGE_SIZE;

int32_t stmFlsh_initial(uint32_t startPage, uint8_t pages){
    STM_FLASH_USED_PAGES       = pages;   // how many pages will be used for this object
    STM_FLASH_START_ADDR       = startPage;     /* Start @ of user Flash area */
    STM_FLASH_END_ADDR         = (STM_FLASH_START_ADDR + (STM_FLASH_USED_PAGES * FLASH_PAGE_SIZE) - 1);   /* End @ of user Flash area */
    STM_FLASH_TOTAL_SZ_BYTES   = (STM_FLASH_END_ADDR - STM_FLASH_START_ADDR +1);
    return 0;
}

int32_t stmFlsh_write(uint16_t addr, const uint8_t *pDat, uint16_t nBytes){
    uint32_t startAddr, endAddr;
    u32 i,j,tick;
    u8 mem[FLASH_PAGE_SIZE], *p;

    memcpy(mem, (u8*)STM_FLASH_START_ADDR, FLASH_PAGE_SIZE);
    
    startAddr = STM_FLASH_START_ADDR + addr;
    endAddr = startAddr + nBytes -1;
    
//    log("<%s startAddr:0x%08x >", __func__, startAddr);
//    log("<%s endAddr:0x%08x >", __func__, endAddr);
//    log("raw:");
//    stmFlsh_printPG(mem);
//    thread_delay(500);
    
    // check if the area to be programed is empty
    uint8_t emptyChk = 0xff;
    for(i=startAddr & 0xfffffff8; (i <= (endAddr & 0xfffffff8)) && (emptyChk==0xff); i+=8){
        emptyChk &=  *&mem[(i+0)-STM_FLASH_START_ADDR];
        emptyChk &=  *&mem[(i+1)-STM_FLASH_START_ADDR];
        emptyChk &=  *&mem[(i+2)-STM_FLASH_START_ADDR];
        emptyChk &=  *&mem[(i+3)-STM_FLASH_START_ADDR];
        emptyChk &=  *&mem[(i+4)-STM_FLASH_START_ADDR];
        emptyChk &=  *&mem[(i+5)-STM_FLASH_START_ADDR];
        emptyChk &=  *&mem[(i+6)-STM_FLASH_START_ADDR];
        emptyChk &=  *&mem[(i+7)-STM_FLASH_START_ADDR];
    }
//    log("<%s emptyChk:0x%02x >", __func__, emptyChk);
//    thread_delay(500);
    
    // merge
    memcpy(&mem[addr], pDat, nBytes);
//    log("merge:");
//    stmFlsh_printPG(mem);
//    thread_delay(100);
    
    if(emptyChk != 0xff){
        tick = HAL_GetTick();
        stmFlsh_format();
//        log("<%s eraseTim: %d >", __func__, HAL_GetTick()-tick);
        startAddr = STM_FLASH_START_ADDR;
        endAddr = STM_FLASH_END_ADDR; 
    }
//    else{
//        log("<%s doNOT_need_erease >", __func__);
//    }
    
    HAL_FLASH_Unlock();
    tick = HAL_GetTick();

//    log("<%s startAddr:0x%08x >", __func__, startAddr);
//    log("<%s endAddr:0x%08x >", __func__, endAddr);
    uint64_t x;
    for(i=startAddr,j=0; i<=(endAddr | 0x07); i+=8){
        j++;
        uint32_t ix = i & 0xfffffff8;
        memcpy((u8*)&x, &mem[ix-STM_FLASH_START_ADDR], 8);

//        log("<%s ix:%08x >", __func__, ix);
//        log("<%s ix-STM_FLASH_START_ADDR:0x%08x >", __func__,ix-STM_FLASH_START_ADDR);
//        log("<%s x:0x%08x >", __func__, x);
        
        // per 20 count, polling other task
        if(j%20 == 19){
            thread_delay(0);   // just do polling job
        }
        
        if(x==0xffffffffffffffff){
            continue;
        }        
        
        if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, ix, x) != HAL_OK){
            log("<%s error ix:%08x \tx:0x%016x >", __func__, ix, x);
            return -2;
        }

    }
    HAL_FLASH_Lock();
    
//    log("prog:");
//    stmFlsh_printPG(mem);
    return 0;
}

s32 stmFlsh_format(void){
    FLASH_EraseInitTypeDef EraseInitStruct = {0};
    HAL_FLASH_Unlock();
    /* Fill EraseInit structure*/
    EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.Page = GetPage(STM_FLASH_START_ADDR);
    EraseInitStruct.NbPages     = STM_FLASH_USED_PAGES;
    EraseInitStruct.Banks = FLASH_BANK_1;
    
    u32 pgErr;
    if(HAL_FLASHEx_Erase(&EraseInitStruct, &pgErr) != HAL_OK){
        log("<%s pgErr:%d >", __func__, pgErr);
        HAL_FLASH_Lock();
        return -1;
    }
    HAL_FLASH_Lock();
    return 0;
}

int32_t stmFlsh_read(uint16_t addr, uint8_t *pDat, uint16_t nBytes){
//    log("<%s addr:0x%08x nBytes:%d>", __func__, addr, nBytes);
    memcpy(pDat, stmFlsh_readDMA(addr), nBytes);
    return 0;
}

u8* stmFlsh_readDMA(uint16_t addr){
//    log("<%s addr:0x%08x>", __func__, addr);
    return((u8*)(STM_FLASH_START_ADDR+addr));
}


/**
  * @brief  Gets the page of a given address
  * @param  Addr: Address of the FLASH Memory
  * @retval The page of a given address
  */
static uint32_t GetPage(uint32_t Addr)
{
  uint32_t page = 0;

  if (Addr < (FLASH_BASE + FLASH_BANK_SIZE))
  {
    /* Bank 1 */
    page = (Addr - FLASH_BASE) / FLASH_PAGE_SIZE;
  }
  else
  {
    /* Bank 2 */
    page = (Addr - (FLASH_BASE + FLASH_BANK_SIZE)) / FLASH_PAGE_SIZE;
  }

  return page;
}

void stmFlsh_print(XPrint xprnt){
    int i;
    uint8_t* p = stmFlsh_readDMA(0);
    for(i=0;i<FLASH_PAGE_SIZE;i++){
        xprnt("%02x ", p[i]);
        if(i%16 == 15){
            xprnt("\n");
            thread_delay(0);   // just do polling job
        }
    }
}

static void stmFlsh_printPG(uint8_t* mem){
    int i;
    for(i=0;i<FLASH_PAGE_SIZE;i++){
        log_raw("%02x ", mem[i]);
        if(i%16 == 15){
            log_raw("\n");
            thread_delay(0);   // just do polling job
        }
    }
}


