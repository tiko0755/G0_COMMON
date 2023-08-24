#include "stm_flash.h"
#include <string.h>
#include <stdlib.h>
#include "user_log.h"

static uint32_t GetPage(uint32_t Addr);
static FLASH_EraseInitTypeDef EraseInitStruct = {0};
static u8* stm_flsh_buff = NULL;
static uint32_t stmFlshAddr = 0, stmFlshBytes = 0;
static CB2 stmFlshCmplt = NULL;

static void stmFlash_cmplt(s32 stat, void* e){
    log("<%s stat:%d >", __func__, stat);
}

int32_t ioWriteAsyn(uint16_t addr, const uint8_t *pDat, uint16_t nBytes, CB2 cmplt){
    stmFlshAddr = addr;
    stmFlshBytes = nBytes;
    stmFlshCmplt = cmplt;
    
    // check if the area to be programed is empty
    u8 emptyChk = 0xff;
    u32 i;
    for(i=0; (i<nBytes) && (emptyChk==0xff); i++){
        emptyChk &= *(u8*)(STM_FLASH_START_ADDR+i);
    }
    
    HAL_FLASH_Unlock();
    /* Fill EraseInit structure*/
    EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.PageAddress = STM_FLASH_START_ADDR;
    EraseInitStruct.NbPages     = STM_FLASH_USED_PAGES;

    if (HAL_FLASHEx_Erase_IT(&EraseInitStruct) != HAL_OK)
    {
        HAL_FLASH_Lock();
        return  -2;
    }    
   
    return 0;
    
    // need erase first
    if(emptyChk != 0xff){
        if(stm_flsh_buff){
            free(stm_flsh_buff);
            return -1;
        }
        stm_flsh_buff = malloc(FLASH_PAGE_SIZE);
        memcpy(stm_flsh_buff, (u8*)STM_FLASH_START_ADDR, FLASH_PAGE_SIZE);
        HAL_FLASH_Unlock();
        /* Fill EraseInit structure*/
        EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
        EraseInitStruct.PageAddress = STM_FLASH_START_ADDR;
        EraseInitStruct.NbPages     = STM_FLASH_USED_PAGES;
        /* Note: If an erase operation in Flash memory also concerns data in the data or instruction cache,
         you have to make sure that these data are rewritten before they are accessed during code
         execution. If this cannot be done safely, it is recommended to flush the caches by setting the
         DCRST and ICRST bits in the FLASH_CR register. */
        if (HAL_FLASHEx_Erase_IT(&EraseInitStruct) != HAL_OK)
        {
            HAL_FLASH_Lock();
            return  -2;
        }
    }

    
    

    
//    
//    

//    
//    
//  /* Get the 1st page to erase */
//  u32 FirstPage = GetPage(STM_FLASH_START_ADDR);

//  /* Get the number of pages to erase from 1st page */
//  u32 NbOfPages = GetPage(STM_FLASH_END_ADDR) - FirstPage + 1;



//  log("<%s eraseTim:%d >", HAL_GetTick()-tick);
//  tick = HAL_GetTick();
//    
    // program 32bit
    u32 x = pDat[0];    x <<= 8;
    x |= pDat[1];   x <<= 8;
    x |= pDat[2];   x <<= 8;
    x |= pDat[3];

    HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, (STM_FLASH_START_ADDR+addr), x);
    
//    HAL_FLASH_Program_IT(FLASH_TYPEPROGRAM_HALFWORD, (STM_FLASH_START_ADDR+addr), x);
//  
//  
//    HAL_FLASH_Lock();
    return 0;
}




u8* ioReadDMA(uint16_t addr){
    return((u8*)(STM_FLASH_START_ADDR+addr));
}

void HAL_FLASH_EndOfOperationCallback(uint32_t ReturnValue){
    log("<%s ReturnValue:%d >", __func__, ReturnValue);
    stmFlash_cmplt(ReturnValue,NULL);

}

void HAL_FLASH_OperationErrorCallback(uint32_t ReturnValue){
    log("<%s ReturnValue:%d >", __func__, ReturnValue);
    stmFlash_cmplt(ReturnValue,NULL);
}

/**
  * @brief  Gets the page of a given address
  * @param  Addr: Address of the FLASH Memory
  * @retval The page of a given address
  */
static uint32_t GetPage(uint32_t Addr)
{
  return (Addr - FLASH_BASE) / FLASH_PAGE_SIZE;;
}

