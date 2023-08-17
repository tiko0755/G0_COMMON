/**********************************************************
filename: tmc2160a_dev.c
**********************************************************/

/************************����ͷ�ļ�***************************************************/
#include "tmc2160a_dev.h"
#include "string.h"
#include "TMC2160.H"    // from TMC Library
#include "board.h"

/**********************************************************
 local function
**********************************************************/
static void tmc2160_Enable(TMC2160A_rsrc_t* pRsrc);
static void tmc2160_Disable(TMC2160A_rsrc_t* pRsrc);
static void tmc2160_SetMicroStep(TMC2160A_rsrc_t* pRsrc, u16 mSteps);
static void tmc2160_SetCurrent(TMC2160A_rsrc_t* pRsrc, u16 mA);
static void tmc2160_Save(TMC2160A_rsrc_t* pRsrc);
static void tmc2160_Default(TMC2160A_rsrc_t* pRsrc);   
static void tmc2160_ReadWriteArray(TMC2160A_rsrc_t* r, uint8_t *data, size_t length);

static void tmc2160_WriteReg(TMC2160A_rsrc_t* r, u8 regAddr, u32 dat);

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
    s8 (*ioWrite)(u16 addr, const u8 *pDat, u16 nBytes),
    s8 (*ioRead)(u16 addr, u8 *pDat, u16 nBytes)
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
    
    //#define R10 0x00070A03  // IHOLD_IRUN
    //#define R6C 0x02410153  // CHOPCONF |64mcroStep
    //#define R70 0xC40C001E  // PWMCONF
    //#define R11 0x0000000A  // 
    //#define R00 0x00000004  // 
    //#define R13 0x000001F4  //   
    tmc2160_WriteReg(r, 0x10, 0x00070A03);
    tmc2160_WriteReg(r, 0x6c, 0x02410153);
    tmc2160_WriteReg(r, 0x70, 0xC40C001E);
    tmc2160_WriteReg(r, 0x11, 0x0000000A);
    tmc2160_WriteReg(r, 0x00, 0x00000004);
    tmc2160_WriteReg(r, 0x13, 0x000001F4);
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
static void tmc2160_Save(TMC2160A_rsrc_t* r){
    u8 buff[4] = {0},i;

    buff[1] = r->mA>>8;
    buff[2] = r->mA;
    buff[3] = r->mStep;

    buff[0] = 0xa5;        // make check code
    for(i=1;i<4;i++){    buff[0] ^= buff[i];    }

    r->ioWrite(r->ioBase, buff, 4);
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
