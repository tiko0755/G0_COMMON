/**********************************************************
filename: tmc2160a_dev.h
**********************************************************/
#ifndef __TMC2160A_DEV_H__
#define __TMC2160A_DEV_H__

#include "misc.h"
#include "tmc2160.h"

#define TMC2160A_USING_NAME	0

/*****************************************************************************
 @ typedefs
****************************************************************************/
#pragma pack(push,4)        // push current align bytes, and then set 4 bytes align
typedef struct{
#if TMC2160A_USING_NAME
	char name[DEV_NAME_LEN];
#endif
    
    
    // lower 
    TMC2160TypeDef obj;
    ConfigurationTypeDef conf;
}TMC2160A_rsrc_t;

typedef struct{
	TMC2160A_rsrc_t rsrc;
	//basic
	void (*polling)(TMC2160A_rsrc_t* r, u8 tick);
    void (*readWriteArray)(TMC2160A_rsrc_t* r, uint8_t *data, size_t length);
}TMC2160A_dev_t;

#pragma pack(pop)        //recover align bytes from 4 bytes

void TMC2160A_dev_Setup(
	TMC2160A_dev_t *pDev, 
	const char* NAME,
	SPI_HandleTypeDef* hspi, 
	const PIN_T CS
);

    
    
    
#endif
