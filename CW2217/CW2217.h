/**********************************************************
filename: cw2217.h
**********************************************************/
#ifndef __CW2217_H__
#define __CW2217_H__

#include "misc.h"
#include "IIC_IO.h"

/*****************************************************************************
 @ typedefs
****************************************************************************/
#pragma pack(push,4)        // push current align bytes, and then set 4 bytes align

typedef struct{
	IIC_IO_Dev_T *iicDev;
	s32  chip_id;
	s32  voltage;
	s32  ic_soc_h;
	s32  ic_soc_l;
	s32  ui_soc;
	s32  temp;
	long cw_current;
	s32  cycle;
	s32  soh;
	s32  fw_version;
} cw2217_rsrc_t;

typedef struct{
    cw2217_rsrc_t rsrc;
    
    int (*write_profile)(cw2217_rsrc_t *r,const u8* buf);
    int (*update_chip_id)(cw2217_rsrc_t *r);
    int (*update_voltage)(cw2217_rsrc_t* r);
    int (*update_capacity)(cw2217_rsrc_t* r);
    int (*update_temperature)(cw2217_rsrc_t* r);
    int (*update_current)(cw2217_rsrc_t* r);
    int (*update_cycleCount)(cw2217_rsrc_t* r);
    int (*update_SOH)(cw2217_rsrc_t* r);
    int (*update_fwVersion)(cw2217_rsrc_t* r);
    int (*update_more)(cw2217_rsrc_t* r);
    int (*init)(cw2217_rsrc_t* r);
    int (*update_statue)(cw2217_rsrc_t* r);
    int (*initX)(cw2217_rsrc_t* r);

    
    
}cw2217_dev_t;

#pragma pack(pop)           //recover align bytes from 4 bytes

void cw2217_setup(
    cw2217_dev_t *pDev,
    IIC_IO_Dev_T *pIIC
);

#endif
