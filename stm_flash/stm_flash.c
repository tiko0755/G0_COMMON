#include "stm_flash.h"
#include <string.h>
#include <stdlib.h>
#include "user_log.h"
#include "thread_delay.h"

static uint32_t GetPage(uint32_t Addr);
static void stmFlsh_printPG(uint8_t *);

int32_t stmFlsh_write(uint16_t addr, const uint8_t *pDat, uint16_t nBytes){
    uint32_t startAddr, endAddr;
    u32 i,tick;

    u8 mem[FLASH_PAGE_SIZE];
    memcpy(mem, (u8*)STM_FLASH_START_ADDR, FLASH_PAGE_SIZE);
    
//    printS("raw:\n");
//    stmFlsh_printPG(mem);

    startAddr = STM_FLASH_START_ADDR + addr;
    endAddr = startAddr + nBytes -1;

    // Halfworld align
    if(startAddr & BIT(0)){
        startAddr -= 1;
    }

//    log("<%s startAddr:0x%08x >", __func__, startAddr);
//    log("<%s endAddr:0x%08x >", __func__, endAddr);
    
    // check if the area to be programed is empty
    u8 emptyChk = 0xff;
    for(i=startAddr; (i<=endAddr) && (emptyChk==0xff); i+=2){
        emptyChk &=  mem[i - STM_FLASH_START_ADDR];
        emptyChk &= mem[i - STM_FLASH_START_ADDR + 1];
    }
//    log("<%s chk:0x%02x >", __func__, emptyChk);
    
    memcpy(&mem[addr], pDat, nBytes);
  
//    printS("merge:\n");
//    stmFlsh_printPG(mem);

    
    if(emptyChk != 0xff){
        tick = HAL_GetTick();
        stmFlsh_format();
//        log("<%s eraseTim: %d >", __func__, HAL_GetTick()-tick);
        startAddr = STM_FLASH_START_ADDR;
        endAddr = STM_FLASH_END_ADDR;
    }

    HAL_FLASH_Unlock();
    tick = HAL_GetTick();
    // program 32bit
    u16 x; 
    for(i=startAddr; i<=endAddr; i+=2){
        x = mem[i-STM_FLASH_START_ADDR + 1];    x <<= 8;
        x |= mem[i-STM_FLASH_START_ADDR];
        if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, i, x) != HAL_OK){
            log("<%s error i:%04d \tx:0x%04x >", __func__, i, x);
            return -2;
        }
        // per 20 count, polling other task
        if(i%20 == 19){
            thread_delay(0);   // just do polling job
        }
    }
    HAL_FLASH_Lock();
    
//    printS("prog:\n");
//    stmFlsh_printPG(mem);
    return 0;
}

s32 stmFlsh_format(void){
    FLASH_EraseInitTypeDef EraseInitStruct = {0};
    HAL_FLASH_Unlock();
    /* Fill EraseInit structure*/
    EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.PageAddress = STM_FLASH_START_ADDR;
    EraseInitStruct.NbPages     = STM_FLASH_USED_PAGES;
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
    memcpy(pDat, stmFlsh_readDMA(addr), nBytes);
    return 0;
}

u8* stmFlsh_readDMA(uint16_t addr){
    return((u8*)(STM_FLASH_START_ADDR+addr));
}

/**
  * @brief  Gets the page of a given address
  * @param  Addr: Address of the FLASH Memory
  * @retval The page of a given address
  */
static uint32_t GetPage(uint32_t Addr)
{
  return (Addr - FLASH_BASE) / FLASH_PAGE_SIZE;
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

