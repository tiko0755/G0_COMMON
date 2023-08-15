/**********************************************************
filename: IIC_IO.c
**********************************************************/

/************************包含头文件***************************************************/
#include "IIC_IO.h"
#include "gpioDecal.h"

#define TICK_IIC 1
/**********************************************************
 Private function
**********************************************************/
//lower io
static void IIC_START(IIC_IO_Rsrc_T* pRsrc);
static void IIC_RESTART(IIC_IO_Rsrc_T* pRsrc);
static void IIC_STOP(IIC_IO_Rsrc_T* pRsrc);
static void IIC_WriteAck(IIC_IO_Rsrc_T* pRsrc);
static void IIC_WriteNoAck(IIC_IO_Rsrc_T* pRsrc);
static u8 IIC_WaitAck(IIC_IO_Rsrc_T* pRsrc);
static void IIC_WriteByte(IIC_IO_Rsrc_T* pRsrc, u8 dat);
static u8 IIC_ReadByte(IIC_IO_Rsrc_T* pRsrc);
static void IIC_delayUs(u8);
//interface io
static s8 iic_io_Write(IIC_IO_Rsrc_T *pRsrc, u8 slaveAddr, u8 regAddr, const u8* pBuf, u8 len);
static s8 iic_io_Read(IIC_IO_Rsrc_T *pRsrc, u8 slaveAddr, u8 regAddr, u8* pBuf, u8 len);
static s8 iic_io_Write_Reg16(IIC_IO_Rsrc_T *pRsrc, u8 slaveAddr, u16 regAddr, const u8* pBuf, u8 len);
static s8 iic_io_Read_Reg16(IIC_IO_Rsrc_T *pRsrc, u8 slaveAddr, u16 regAddr, u8* pBuf, u8 len);
static s8 iicioRead(IIC_IO_Rsrc_T *pRsrc, u8 slaveWrtAddr, u16 regAddr, u8 isRegAddr16B, u8* pBuf, u8 len);
static s8 iicioWrite(IIC_IO_Rsrc_T *pRsrc, u8 slaveWrtAddr, u16 regAddr, u8 isRegAddr16B, const u8* pBuf, u8 len);
/**********************************************************
 Public function
**********************************************************/
void IIC_IO_Setup(IIC_IO_Dev_T *pDev, const PIN_T* SCL, const PIN_T* SDA){
    IIC_IO_Rsrc_T *pRsrc = &pDev->rsrc;
    //config1
    pRsrc->SCL = SCL;
    pRsrc->SDA = SDA;
    //config2
    as_OUTPUT_OD_NOPULL_HIGH(SCL);
    as_OUTPUT_OD_NOPULL_HIGH(SDA);
    //registe op
    pDev->Read = iic_io_Read;
    pDev->Write = iic_io_Write;
    pDev->Read16 = iic_io_Read_Reg16;
    pDev->Write16 = iic_io_Write_Reg16;
}

/**********************************************************
* Function Name  : tca6408Read
* Description    : 
* Input          : None
* Output         : None
* Return         : None
**********************************************************/
static s8 iic_io_Read(IIC_IO_Rsrc_T *pRsrc, u8 slaveWrtAddr, u8 regAddr, u8* pBuf, u8 len){
    return(iicioRead(pRsrc, slaveWrtAddr, regAddr, 0, pBuf,len));
}
static s8 iic_io_Read_Reg16(IIC_IO_Rsrc_T *pRsrc, u8 slaveWrtAddr, u16 regAddr, u8* pBuf, u8 len){
    return(iicioRead(pRsrc, slaveWrtAddr, regAddr, 1, pBuf,len));
}
/**********************************************************
* Function Name  : tca6408Write
* Description    : 
* Input          : None
* Output         : None
* Return         : None
**********************************************************/
static s8 iic_io_Write(IIC_IO_Rsrc_T *pRsrc, u8 slaveWrtAddr, u8 regAddr, const u8* pBuf, u8 len){
    return(iicioWrite(pRsrc, slaveWrtAddr, regAddr, 0, pBuf, len));
}
static s8 iic_io_Write_Reg16(IIC_IO_Rsrc_T *pRsrc, u8 slaveWrtAddr, u16 regAddr, const u8* pBuf, u8 len){
    return(iicioWrite(pRsrc, slaveWrtAddr, regAddr, 1, pBuf, len));
}
/**********************************************************
* Function Name  : tca6408Write
* Description    : 
* Input          : None
* Output         : None
* Return         : None
**********************************************************/
static s8 iicioWrite(IIC_IO_Rsrc_T *pRsrc, u8 slaveWrtAddr, u16 regAddr, u8 isRegAddr16B, const u8* pBuf, u8 len){
    u8 i;
    u16 thsAddr = regAddr;
    //make start
    IIC_START(pRsrc);
    //device addr
    IIC_WriteByte(pRsrc, slaveWrtAddr);
    if(IIC_WaitAck(pRsrc) == 1)    {    IIC_STOP(pRsrc);    return -1;    }
    //word addr
    if(isRegAddr16B){
        i = thsAddr>>8;
        IIC_WriteByte(pRsrc, i);
        if(IIC_WaitAck(pRsrc) == 1)    {    IIC_STOP(pRsrc);    return -1;    }
    }
    i = thsAddr&0x00ff;
    IIC_WriteByte(pRsrc, i);
    if(IIC_WaitAck(pRsrc) == 1)    {    IIC_STOP(pRsrc);    return -1;    }    
    //write data    
    for(i=0;i<len;i++){
        IIC_WriteByte(pRsrc,pBuf[i]);
        if(IIC_WaitAck(pRsrc) == 1){    IIC_STOP(pRsrc);    return -1;    }
    }        
    //stop
    IIC_STOP(pRsrc);
    //wait twr=5ms
    //HAL_Delay(5);
    return 0;
}

static s8 iicioRead(IIC_IO_Rsrc_T *pRsrc, u8 slaveWrtAddr, u16 regAddr, u8 isRegAddr16B, u8* pBuf, u8 len){
    u8 i;
    u16 thsAddr = regAddr;
    //make start
    IIC_START(pRsrc);
    //device addr
    IIC_WriteByte(pRsrc, slaveWrtAddr);    
    if(IIC_WaitAck(pRsrc) == 1)    {    IIC_STOP(pRsrc);    return -1;    }
    //word addr
    if(isRegAddr16B){
        i = thsAddr>>8;
        IIC_WriteByte(pRsrc, i);
        if(IIC_WaitAck(pRsrc) == 1)    {    IIC_STOP(pRsrc);    return -1;    }    
    }
    i = thsAddr&0x00ff;
    IIC_WriteByte(pRsrc, i);
    if(IIC_WaitAck(pRsrc) == 1)    {    IIC_STOP(pRsrc);    return -1;    }    
    //re-start
    IIC_RESTART(pRsrc);
    //device addr
    IIC_WriteByte(pRsrc, slaveWrtAddr|0x01);
    if(IIC_WaitAck(pRsrc) == 1)    {    IIC_STOP(pRsrc);    return -1;    }
    //read data
    for(i=0;i<len;i++){
        pBuf[i] = IIC_ReadByte(pRsrc);
        if(i <len-1)    IIC_WriteAck(pRsrc);
        else    IIC_WriteNoAck(pRsrc);
    }
    //stop
    IIC_STOP(pRsrc);
    return 0;
}

/**********************************************************
 Lower IO
**********************************************************/
static void IIC_START(IIC_IO_Rsrc_T* pRsrc){
    IIC_RESTART(pRsrc);
}

static void IIC_RESTART(IIC_IO_Rsrc_T* pRsrc){
    HAL_GPIO_WritePin(pRsrc->SDA->GPIOx, pRsrc->SDA->GPIO_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(pRsrc->SCL->GPIOx, pRsrc->SCL->GPIO_Pin, GPIO_PIN_SET);        
    HAL_GPIO_WritePin(pRsrc->SDA->GPIOx, pRsrc->SDA->GPIO_Pin, GPIO_PIN_RESET);        IIC_delayUs(2);
    HAL_GPIO_WritePin(pRsrc->SCL->GPIOx, pRsrc->SCL->GPIO_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(pRsrc->SDA->GPIOx, pRsrc->SDA->GPIO_Pin, GPIO_PIN_SET);
}

static void IIC_STOP(IIC_IO_Rsrc_T* pRsrc){
    HAL_GPIO_WritePin(pRsrc->SCL->GPIOx, pRsrc->SCL->GPIO_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(pRsrc->SDA->GPIOx, pRsrc->SDA->GPIO_Pin, GPIO_PIN_RESET);    IIC_delayUs(2);
    HAL_GPIO_WritePin(pRsrc->SCL->GPIOx, pRsrc->SCL->GPIO_Pin, GPIO_PIN_SET);        
    HAL_GPIO_WritePin(pRsrc->SDA->GPIOx, pRsrc->SDA->GPIO_Pin, GPIO_PIN_SET);    
}

/*
    Before enter, SCL, SDA are both low
*/
static u8 IIC_ReadByte(IIC_IO_Rsrc_T* pRsrc){
    u8 i,rtn=0;

    HAL_GPIO_WritePin(pRsrc->SDA->GPIOx, pRsrc->SDA->GPIO_Pin, GPIO_PIN_SET);    
    for(i=0;i<8;i++){
        HAL_GPIO_WritePin(pRsrc->SCL->GPIOx, pRsrc->SCL->GPIO_Pin, GPIO_PIN_SET);    
        IIC_delayUs(3);
        
        rtn <<= 1;
        if(HAL_GPIO_ReadPin(pRsrc->SDA->GPIOx, pRsrc->SDA->GPIO_Pin) == GPIO_PIN_SET)    rtn |= 0x01;
        
        HAL_GPIO_WritePin(pRsrc->SCL->GPIOx, pRsrc->SCL->GPIO_Pin, GPIO_PIN_RESET);
        IIC_delayUs(5);
    }
    return rtn;
}

static void IIC_WriteAck(IIC_IO_Rsrc_T* pRsrc){
    //HAL_GPIO_WritePin(pRsrc->SDA->GPIOx, pRsrc->SDA->GPIO_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(pRsrc->SDA->GPIOx, pRsrc->SDA->GPIO_Pin, GPIO_PIN_RESET);    
    HAL_GPIO_WritePin(pRsrc->SCL->GPIOx, pRsrc->SCL->GPIO_Pin, GPIO_PIN_SET);        IIC_delayUs(6);
    HAL_GPIO_WritePin(pRsrc->SCL->GPIOx, pRsrc->SCL->GPIO_Pin, GPIO_PIN_RESET);    
}
static void IIC_WriteNoAck(IIC_IO_Rsrc_T* pRsrc){
    HAL_GPIO_WritePin(pRsrc->SDA->GPIOx, pRsrc->SDA->GPIO_Pin, GPIO_PIN_SET);        
    HAL_GPIO_WritePin(pRsrc->SCL->GPIOx, pRsrc->SCL->GPIO_Pin, GPIO_PIN_SET);        IIC_delayUs(5);
    HAL_GPIO_WritePin(pRsrc->SCL->GPIOx, pRsrc->SCL->GPIO_Pin, GPIO_PIN_RESET);    
}

static void IIC_WriteByte(IIC_IO_Rsrc_T* pRsrc, u8 dat){
    u8 i,tmp = dat;
    for(i=0;i<8;i++){
        if(tmp & 0x80)    HAL_GPIO_WritePin(pRsrc->SDA->GPIOx, pRsrc->SDA->GPIO_Pin, GPIO_PIN_SET);
        else    HAL_GPIO_WritePin(pRsrc->SDA->GPIOx, pRsrc->SDA->GPIO_Pin, GPIO_PIN_RESET);    
        
        tmp <<= 1;
        HAL_GPIO_WritePin(pRsrc->SCL->GPIOx, pRsrc->SCL->GPIO_Pin, GPIO_PIN_SET);    
        IIC_delayUs(4);
        
        HAL_GPIO_WritePin(pRsrc->SCL->GPIOx, pRsrc->SCL->GPIO_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(pRsrc->SDA->GPIOx, pRsrc->SDA->GPIO_Pin, GPIO_PIN_SET);
        IIC_delayUs(2);
    }
}

//wait for 5ms
static u8 IIC_WaitAck(IIC_IO_Rsrc_T* pRsrc){
    u8 rtn,i;
    HAL_GPIO_WritePin(pRsrc->SCL->GPIOx, pRsrc->SCL->GPIO_Pin, GPIO_PIN_SET);        IIC_delayUs(5);    
    for(i=0;i<=5;i++){
        if(HAL_GPIO_ReadPin(pRsrc->SDA->GPIOx, pRsrc->SDA->GPIO_Pin) == GPIO_PIN_RESET){
            rtn = 0;    break;
        }
        else    rtn = 1;
    }    
    HAL_GPIO_WritePin(pRsrc->SCL->GPIOx, pRsrc->SCL->GPIO_Pin, GPIO_PIN_RESET);

    return rtn;
}

static void IIC_delayUs(u8 us){
    while(us){    us--;    }
}

/**********************************************************
 == THE END ==
**********************************************************/
