/**********************************************************
filename: tmc2160a_dev.c
**********************************************************/

/************************����ͷ�ļ�***************************************************/
#include "tmc2160a_dev.h"
#include "string.h"
#include "TMC2160.H"    // from TMC Library
#include "thread_delay.h"
#include "board.h"

/**********************************************************
 local function
**********************************************************/
static void tmc2160_Enable(TMC2160A_rsrc_t* pRsrc);
static void tmc2160_Disable(TMC2160A_rsrc_t* pRsrc);
static void tmc2160_SetMicroStep(TMC2160A_rsrc_t* pRsrc, u16 mSteps);
static void tmc2160_SetCurrent(TMC2160A_rsrc_t* pRsrc, u16 mA);
static void tmc2160_Default(TMC2160A_rsrc_t* pRsrc);   
static void tmc2160_ReadWriteArray(TMC2160A_rsrc_t* r, uint8_t *data, size_t length);
static void tmc2160_WriteReg(TMC2160A_rsrc_t* r, u8 regAddr, u32 dat);

static s32 tmc2160_SaveConf(TMC2160A_rsrc_t* r);
static s32 tmc2160_ReadConf(TMC2160A_rsrc_t* r);
/**********************************************************
 Public function
**********************************************************/
void TMC2160A_dev_Setup(
    TMC2160A_dev_t *d, 
    const char* NAME,
    const PIN_T* CS,
    const PIN_T* EN,
    const PIN_T* DIA0,
    const PIN_T* DIA1,
    SPI_HandleTypeDef* hspi, 
    u8 ch,
    u16 ioBase,
    s32 (*ioWrite)(u16 addr, const u8 *pDat, u16 nBytes),
    s32 (*ioRead)(u16 addr, u8 *pDat, u16 nBytes)
){
    memset(d,0,sizeof(TMC2160A_dev_t));
    TMC2160A_rsrc_t* r = &d->rsrc;
#if TMC2160A_USING_NAME
    strcpy(r->name, NAME);
#endif
    r->CS = CS; 
    r->EN = EN;
    r->DIA0 = DIA0;
    r->DIA1 = DIA1;
    r->hspi = hspi;
    r->ioBase = ioBase;
    r->ioRead = ioRead;
    r->ioWrite = ioWrite;
	
    d->Enable = tmc2160_Enable;
    d->Disable = tmc2160_Disable;
    d->readWriteArray = tmc2160_ReadWriteArray;	
	
    HAL_GPIO_WritePin(r->CS->GPIOx, r->CS->GPIO_Pin, GPIO_PIN_SET);
    tmc2160_init(&r->obj, ch, &r->conf, tmc2160_defaultRegisterResetState);
    
//    tmc2160_WriteReg(r, 0x10, 0x00070A03);
//    tmc2160_WriteReg(r, 0x6c, 0x02410153);
//    tmc2160_WriteReg(r, 0x70, 0xC40C001E);
//    tmc2160_WriteReg(r, 0x11, 0x0000000A);
//    tmc2160_WriteReg(r, 0x00, 0x00000004);
//    tmc2160_WriteReg(r, 0x13, 0x000001F4);

    tmc2160_WriteReg(r, 0x10, 0x00070A03);
    tmc2160_WriteReg(r, 0x6c, 0x02410153);
    tmc2160_WriteReg(r, 0x70, 0xC40C001E);
    tmc2160_WriteReg(r, 0x11, 0x0000000A);
    tmc2160_WriteReg(r, 0x00, 0x00000004);
    tmc2160_WriteReg(r, 0x13, 0x000001f0);
    tmc2160_WriteReg(r, 0x14, 0x00000100);
    tmc2160_WriteReg(r, 0x15, 0x000000f0);

    tmc2160_Enable(r);
}

static void tmc2160_WriteReg(TMC2160A_rsrc_t* r, u8 regAddr, u32 dat){
    u8 buff[5] = {0};

    buff[0] = regAddr|BIT(7);
    buff[4] = dat&0xff;    dat>>=8;
    buff[3] = dat&0xff;    dat>>=8;
    buff[2] = dat&0xff;    dat>>=8;
    buff[1] = dat&0xff;
    
    tmc2160_ReadWriteArray(r, buff, 5);
}


static void tmc2160_Enable(TMC2160A_rsrc_t* r){
    HAL_GPIO_WritePin(r->EN->GPIOx, r->EN->GPIO_Pin, GPIO_PIN_RESET);
}
static void tmc2160_Disable(TMC2160A_rsrc_t* r){
    HAL_GPIO_WritePin(r->EN->GPIOx, r->EN->GPIO_Pin, GPIO_PIN_SET);
}
static void tmc2160_SetMicroStep(TMC2160A_rsrc_t* r, u16 mSteps){

}
static void tmc2160_SetCurrent(TMC2160A_rsrc_t* r, u16 mA){

}

static s32 tmc2160_SaveConf(TMC2160A_rsrc_t* r){
    u32 checkSum = 0xa5;
    s32 i;
    u16 addr = r->ioBase + 4;
    if(r->ioWrite==NULL){
        return -1;
    }
    
//    buff[2] = ;
//    buff[3] = r->mStep;
    
    r->ioWrite(addr, (const u8*)&r->mA, 2);
    checkSum += r->mA;
    addr += 2;
    thread_delay(5);
    
    r->ioWrite(addr, (const u8*)&r->mStep, 2);
    checkSum += r->mStep;  
    addr += 2;
    thread_delay(5);
    
    r->ioWrite(r->ioBase, (const u8*)&checkSum, 4);  
    return 0;
}

static s32 tmc2160_ReadConf(TMC2160A_rsrc_t* r){
    u32 checkSum = 0xa5, checkCode;
    s32 i;
    u16 addr = r->ioBase+4;
    if(r->ioRead==NULL){
        return -1;
    }
    
    r->ioRead(r->ioBase, (u8*)&checkCode, 4); 
//    log("<%s checkCode:%08x >", __func__, checkCode);
    
    r->ioRead(addr, (u8*)&r->mA, 2);
    checkSum += r->mA;
    addr += 4;  
//    log("<%s thickNess:%08x >", __func__, thickNess);
    
    r->ioRead(addr, (u8*)&r->mStep, 2);   
    checkSum += r->mStep;  
    addr += 4;
//    log("<%s perRev_r:%08x >", __func__, perRev_r);
    
 
    
    if(checkCode != checkSum){
//        log("<%s checkCode:0x%08x checkSum:0x%08x >", __func__,checkCode,checkSum);
        tmc2160_Default(r);
        return -2;
    }
    return 0;
}

static void tmc2160_Default(TMC2160A_rsrc_t* r){
}  


static void tmc2160_ReadWriteArray(TMC2160A_rsrc_t* r, uint8_t *data, size_t length){
    u8 rxBuff[16];
//    print("<%s data:0x%02x %02x%02x%02x%02x>\r\n",__func__,data[0],data[1],data[2],data[3],data[4]);
    HAL_GPIO_WritePin(r->CS->GPIOx, r->CS->GPIO_Pin, GPIO_PIN_RESET);
    HAL_SPI_TransmitReceive(r->hspi, data, rxBuff, length, 5);
    memcpy(data,rxBuff,5);
//    print("<%s rx:0x%02x%02x%02x%02x%02x>\r\n",__func__,rxBuff[0],rxBuff[1],rxBuff[2],rxBuff[3],rxBuff[4]);
    HAL_GPIO_WritePin(r->CS->GPIOx, r->CS->GPIO_Pin, GPIO_PIN_SET);
}

/**********************************************************
 == THE END ==
**********************************************************/
