/**********************************************************
filename: cw2217.h
**********************************************************/
#ifndef __CW2217_H__
#define __CW2217_H__

#include "misc.h"
#include "IIC_IO.h"
#include "app_timer.h"


#define REG_CHIP_ID             0x00
#define REG_VCELL_H             0x02
#define REG_VCELL_L             0x03
#define REG_SOC_INT             0x04
#define REG_SOC_DECIMAL         0x05
#define REG_TEMP                0x06
#define REG_MODE_CONFIG         0x08
#define REG_GPIO_CONFIG         0x0A
#define REG_SOC_ALERT           0x0B
#define REG_TEMP_MAX            0x0C
#define REG_TEMP_MIN            0x0D
#define REG_CURRENT_H           0x0E
#define REG_CURRENT_L           0x0F
#define REG_T_HOST_H            0xA0
#define REG_T_HOST_L            0xA1
#define REG_USER_CONF           0xA2
#define REG_CYCLE_H             0xA4
#define REG_CYCLE_L             0xA5
#define REG_SOH                 0xA6
#define REG_IC_STATE            0xA7
#define REG_FW_VERSION          0xAB
#define REG_BAT_PROFILE         0x10

/*****************************************************************************
 @ typedefs
****************************************************************************/
#pragma pack(push,4)        // push current align bytes, and then set 4 bytes align

typedef struct{
    IIC_IO_Dev_T *iicDev;
    appTmrDev_t* tmr;
    CB2 cmplt;
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
    u16 tick,tim,pollingTim;
    u8 squ, isReady;
    
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
    
    int (*startActive)(cw2217_rsrc_t *r);

}cw2217_dev_t;

#pragma pack(pop)           //recover align bytes from 4 bytes

void cw2217_setup(
    cw2217_dev_t *pDev,
    IIC_IO_Dev_T *pIIC,
    appTmrDev_t* tmr
);

#endif
