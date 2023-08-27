/**********************************************************
filename: cw2217.c
**********************************************************/

/************************����ͷ�ļ�***************************************************/
#include "cw2217.h"
#include "misc.h"
#include "string.h"
#include "user_log.h"
#include "thread_delay.h"

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

#define CONFIG_MODE_RESTART     0x30
#define CONFIG_MODE_ACTIVE      0x00
#define CONFIG_MODE_SLEEP       0xF0
#define CONFIG_UPDATE_FLG       0x80
#define IC_VCHIP_ID             0xA0
#define IC_READY_MARK           0x0C

#define GPIO_ENABLE_MIN_TEMP    0
#define GPIO_ENABLE_MAX_TEMP    0
#define GPIO_ENABLE_SOC_CHANGE  0
#define GPIO_SOC_IRQ_VALUE      0x0    /* 0x7F */
#define DEFINED_MAX_TEMP        45
#define DEFINED_MIN_TEMP        0

#define CWFG_NAME               "cw221X"
#define SIZE_OF_PROFILE         80
#define USER_RSENSE             1000  /* mhom rsense * 1000  for convenience calculation */

#define queue_delayed_work_time  8000
#define queue_start_work_time    50

#define CW_SLEEP_20MS           20
#define CW_SLEEP_10MS           10
#define CW_UI_FULL              100
#define COMPLEMENT_CODE_U16     0x8000
#define CW_SLEEP_100MS          100
#define CW_SLEEP_200MS          200
#define CW_SLEEP_COUNTS         100
#define CW_TRUE                 1
#define CW_RETRY_COUNT          3
#define CW_VOL_UNIT             1000
#define CW_LOW_VOLTAGE_REF      2500
#define CW_LOW_VOLTAGE          3000
#define CW_LOW_VOLTAGE_STEP     10

#define CW221X_NOT_ACTIVE          1
#define CW221X_PROFILE_NOT_READY   2
#define CW221X_PROFILE_NEED_UPDATE 3

#define CW2215_MARK             0x80
#define CW2217_MARK             0x40
#define CW2218_MARK             0x00

#define CW2215_DEV_ADDR     (0x64<<1)

static u8 config_profile_info[SIZE_OF_PROFILE] = {
    0x50,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0xA6,0xC2,0xC8,0xD1,0xC9,0xC7,0x8F,0x52,
    0x25,0xFF,0xE4,0x9E,0x77,0x5D,0x4C,0x41,
    0x2F,0x26,0x1B,0x50,0x16,0xDB,0x16,0xC4,
    0xC5,0xCA,0xCA,0xCA,0xC9,0xC9,0xC3,0xC9,
    0xCB,0xC6,0xB2,0xA1,0x97,0x8F,0x8A,0x84,
    0x85,0x8A,0x8E,0x94,0x9D,0x87,0x6D,0xAC,
    0x80,0x00,0x57,0x10,0x00,0x82,0x8D,0x00,
    0x00,0x00,0x64,0x11,0x91,0x7F,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x4B
};


/**********************************************************
 Private function
**********************************************************/
static int cw_write_profile(cw2217_rsrc_t *r,const u8* buf);
static int cw221X_startActive(cw2217_rsrc_t *r);
static int cw221X_sleep(cw2217_rsrc_t *r);
static int cw_get_chip_id(cw2217_rsrc_t *r);
static int cw_get_voltage(cw2217_rsrc_t* r);
static int cw_get_capacity(cw2217_rsrc_t* r);
static int cw_get_temp(cw2217_rsrc_t *r);
static long get_complement_code(unsigned short raw_code);
static int cw_get_current(cw2217_rsrc_t *r);
static int cw_get_cycle_count(cw2217_rsrc_t *r);
static int cw_get_soh(cw2217_rsrc_t *r);
static int cw_get_fw_version(cw2217_rsrc_t *r);
static int cw_update_data(cw2217_rsrc_t *r);
static int cw_init_data(cw2217_rsrc_t *r);
static int cw_config_start_ic(cw2217_rsrc_t *r);
static int cw221X_get_state(cw2217_rsrc_t *r);
static int cw_init(cw2217_rsrc_t *r);
static void cw_bat_work(cw2217_rsrc_t *r);

static void cwBat_start(cw2217_rsrc_t *r, u16 interval);
static void cwBat_stop(cw2217_rsrc_t *r);
static void cwBat_tmrHandler(void* e);

/**********************************************************
 Public function
**********************************************************/
void cw2217_setup(
    cw2217_dev_t *d,
    IIC_IO_Dev_T *pIIC,
    appTmrDev_t* tmr
){
    memset(d,0,sizeof(cw2217_dev_t));
    d->rsrc.iicDev = pIIC;
    d->rsrc.tmr = tmr;
    
    d->update_chip_id = cw_get_chip_id;
    d->update_voltage = cw_get_voltage;
    d->write_profile = cw_write_profile;
    d->update_more = cw_update_data;
    
    d->startActive = cw221X_startActive;
    
    d->init = cw_init;

    
}

static void cwBat_tmrHandler(void* e){
    cw2217_rsrc_t* r = (cw2217_rsrc_t*)e;
    
    switch(r->squ){
        case 1:{ //  
            break;
        }
    }

}

static void cwBat_start(cw2217_rsrc_t *r, u16 interval){
    r->pollingTim = interval;
    r->tmr->start(&r->tmr->rsrc, interval, POLLING_REPEAT, cwBat_tmrHandler, r);
}

static void cwBat_stop(cw2217_rsrc_t *r){
    r->tmr->stop(&r->tmr->rsrc);
}

/* CW221X iic write profile function */
static int cw_write_profile(cw2217_rsrc_t *r,const u8* buf)
{
	int ret;
	int i;

	for (i = 0; i < SIZE_OF_PROFILE; i++) {
        ret = r->iicDev->Write(&r->iicDev->rsrc, CW2215_DEV_ADDR, REG_BAT_PROFILE + i, &buf[i], 1);
		if (ret < 0) {
			log("IIC error %d\n", ret);
			return ret;
		}
	}

	return ret;
}

/* 
 * CW221X Active function 
 * The CONFIG register is used for the host MCU to configure the fuel gauge IC. The default value is 0xF0,
 * SLEEP and RESTART bits are set. To power up the IC, the host MCU needs to write 0x30 to exit shutdown
 * mode, and then write 0x00 to restart the gauge to enter active mode. To reset the IC, the host MCU needs
 * to write 0xF0, 0x30 and 0x00 in sequence to this register to complete the restart procedure. The CW221X
 * will reload relevant parameters and settings and restart SOC calculation. Note that the SOC may be a
 * different value after reset operation since it is a brand-new calculation based on the latest battery status.
 * CONFIG [3:0] is reserved. Don't do any operation with it.
 */
static int cw221X_startActive(cw2217_rsrc_t *r)
{
	int ret;
	unsigned char reg_val = CONFIG_MODE_RESTART;

    ret = r->iicDev->Write(&r->iicDev->rsrc, CW2215_DEV_ADDR, REG_MODE_CONFIG, &reg_val, 1);
	if (ret < 0)
		return ret;
    
    thread_delay(2);

	reg_val = CONFIG_MODE_ACTIVE;
    ret = r->iicDev->Write(&r->iicDev->rsrc, CW2215_DEV_ADDR, REG_MODE_CONFIG, &reg_val, 1);
	if (ret < 0)
		return ret;
    
	thread_delay(2);

	return 0;
}

/* 
 * CW221X Sleep function 
 * The CONFIG register is used for the host MCU to configure the fuel gauge IC. The default value is 0xF0,
 * SLEEP and RESTART bits are set. To power up the IC, the host MCU needs to write 0x30 to exit shutdown
 * mode, and then write 0x00 to restart the gauge to enter active mode. To reset the IC, the host MCU needs
 * to write 0xF0, 0x30 and 0x00 in sequence to this register to complete the restart procedure. The CW221X
 * will reload relevant parameters and settings and restart SOC calculation. Note that the SOC may be a
 * different value after reset operation since it is a brand-new calculation based on the latest battery status.
 * CONFIG [3:0] is reserved. Don't do any operation with it.
 */
static int cw221X_sleep(cw2217_rsrc_t *r)
{
	int ret;
	unsigned char reg_val = CONFIG_MODE_RESTART;    // 0X30

    ret = r->iicDev->Write(&r->iicDev->rsrc, CW2215_DEV_ADDR, REG_MODE_CONFIG, &reg_val, 1);
	if (ret < 0)
		return ret;
	thread_delay(2);

	reg_val = CONFIG_MODE_SLEEP;
    ret = r->iicDev->Write(&r->iicDev->rsrc, CW2215_DEV_ADDR, REG_MODE_CONFIG, &reg_val, 1);

	if (ret < 0)
		return ret;
	thread_delay(2);

	return 0;
}

/*
 * The 0x00 register is an UNSIGNED 8bit read-only register. Its value is fixed to 0xA0 in shutdown
 * mode and active mode.
 */
static int cw_get_chip_id(cw2217_rsrc_t *r)
{
	int ret;
	unsigned char reg_val;
	int chip_id;

    ret = r->iicDev->Read(&r->iicDev->rsrc, CW2215_DEV_ADDR, REG_CHIP_ID, &reg_val, 1);
	if (ret < 0){
		return ret;
    }
	r->chip_id = reg_val;
	return 0;
}

/*
 * The VCELL register(0x02 0x03) is an UNSIGNED 14bit read-only register that updates the battery voltage continuously.
 * Battery voltage is measured between the VCELL pin and VSS pin, which is the ground reference. A 14bit
 * sigma-delta A/D converter is used and the voltage resolution is 312.5uV. (0.3125mV is *5/16)
 */
static int cw_get_voltage(cw2217_rsrc_t* r)
{
	int ret;
	u8 reg_val[2] = {0 , 0};
	unsigned int voltage;

//	ret = cw_read_word(cw_bat->client, REG_VCELL_H, reg_val);
    ret = r->iicDev->Read(&r->iicDev->rsrc, CW2215_DEV_ADDR, REG_VCELL_H, reg_val, 2);
	if (ret < 0)
		return ret;
	voltage = (reg_val[0] << 8) + reg_val[1];
	voltage = voltage  * 5 / 16;
	r->voltage = voltage;

	return 0;
}

/*
 * The SOC register(0x04 0x05) is an UNSIGNED 16bit read-only register that indicates the SOC of the battery. The
 * SOC shows in % format, which means how much percent of the battery's total available capacity is
 * remaining in the battery now. The SOC can intrinsically adjust itself to cater to the change of battery status,
 * including load, temperature and aging etc.
 * The high byte(0x04) contains the SOC in 1% unit which can be directly used if this resolution is good
 * enough for the application. The low byte(0x05) provides more accurate fractional part of the SOC and its
 * LSB is (1/256) %.
 */
static int cw_get_capacity(cw2217_rsrc_t* r)
{
	int ret;
	unsigned char reg_val[2] = { 0, 0 };
	int ui_100 = CW_UI_FULL;
	int soc_h;
	int soc_l;
	int ui_soc;
	int remainder;

    ret = r->iicDev->Read(&r->iicDev->rsrc, CW2215_DEV_ADDR, REG_VCELL_H, reg_val, 2);
	if (ret < 0)
		return ret;
	soc_h = reg_val[0];
	soc_l = reg_val[1];
	ui_soc = ((soc_h * 256 + soc_l) * 100)/ (ui_100 * 256);
	remainder = (((soc_h * 256 + soc_l) * 100 * 100) / (ui_100 * 256)) % 100;
	if (ui_soc >= 100){
		ui_soc = 100;
	}
	r->ic_soc_h = soc_h;
	r->ic_soc_l = soc_l;
	r->ui_soc = ui_soc;

	return 0;
}

/*
 * The TEMP register is an UNSIGNED 8bit read only register. 
 * It reports the real-time battery temperature
 * measured at TS pin. The scope is from -40 to 87.5 degrees Celsius, 
 * LSB is 0.5 degree Celsius. TEMP(C) = - 40 + Value(0x06 Reg) / 2 
 */
static int cw_get_temp(cw2217_rsrc_t *r)
{
	int ret;
	unsigned char reg_val;
	int temp;

    ret = r->iicDev->Read(&r->iicDev->rsrc, CW2215_DEV_ADDR, REG_TEMP, &reg_val, 1);
    if (ret < 0)
		return ret;

	temp = (int)reg_val * 10 / 2 - 400;
	r->temp = temp;

	return 0;
}

/* get complement code function, unsigned short must be U16 */
static long get_complement_code(unsigned short raw_code)
{
	long complement_code;
	int dir;

	if (0 != (raw_code & COMPLEMENT_CODE_U16)){
		dir = -1;
		raw_code =  (0xFFFF - raw_code) + 1;
	} else {
		dir = 1;
	}
	complement_code = (long)raw_code * dir;

	return complement_code;
}

/*
 * CURRENT is a SIGNED 16bit register(0x0E 0x0F) that reports current A/D converter result of the voltage across the
 * current sense resistor, 10mohm typical. The result is stored as a two's complement value to show positive
 * and negative current. Voltages outside the minimum and maximum register values are reported as the
 * minimum or maximum value.
 * The register value should be divided by the sense resistance to convert to amperes. The value of the
 * sense resistor determines the resolution and the full-scale range of the current readings. The LSB of 0x0F
 * is (52.4/32768)uV for CW2215 and CW2217. The LSB of 0x0F is (125/32768)uV for CW2218.
 * The default value is 0x0000, stands for 0mA. 0x7FFF stands for the maximum charging current and 0x8001 stands for
 * the maximum discharging current.
 */
static int cw_get_current(cw2217_rsrc_t *r)
{
	int ret;
	unsigned char reg_val[2] = {0 , 0};
	long long cw_current; /* use long long type to guarantee 8 bytes space*/
	unsigned short current_reg;  /* unsigned short must u16 */

    ret = r->iicDev->Read(&r->iicDev->rsrc, CW2215_DEV_ADDR, REG_VCELL_H, reg_val, 2);
	if (ret < 0)
		return ret;

	current_reg = (reg_val[0] << 8) + reg_val[1];
	cw_current = get_complement_code(current_reg);
	if(((r->fw_version) & (CW2215_MARK != 0)) || ((r->fw_version) & (CW2217_MARK != 0))){
		cw_current = cw_current * 1600 / USER_RSENSE;
	}else if((r->fw_version != 0) && (r->fw_version & (0xC0 == CW2218_MARK))){
		cw_current = cw_current * 3815 / USER_RSENSE;
	}else{
		r->cw_current = 0;
	}
	r->cw_current = cw_current;

	return 0;
}

/*
 * CYCLECNT is an UNSIGNED 16bit register(0xA4 0xA5) that counts cycle life of the battery. The LSB of 0xA5 stands
 * for 1/16 cycle. This register will be clear after enters shutdown mode
 */
static int cw_get_cycle_count(cw2217_rsrc_t *r)
{
	int ret;
	unsigned char reg_val[2] = {0, 0};
	int cycle;

    ret = r->iicDev->Read(&r->iicDev->rsrc, CW2215_DEV_ADDR, REG_VCELL_H, reg_val, 2);
	if (ret < 0)
		return ret;

	cycle = (reg_val[0] << 8) + reg_val[1];
	r->cycle = cycle / 16;

	return 0;
}

/*
 * SOH (State of Health) is an UNSIGNED 8bit register(0xA6) that represents the level of battery aging by tracking
 * battery internal impedance increment. When the device enters active mode, this register refresh to 0x64
 * by default. Its range is 0x00 to 0x64, indicating 0 to 100%. This register will be clear after enters shutdown
 * mode.
 */
static int cw_get_soh(cw2217_rsrc_t *r)
{
	int ret;
	unsigned char reg_val;
	int soh;

    ret = r->iicDev->Read(&r->iicDev->rsrc, CW2215_DEV_ADDR, REG_VCELL_H, &reg_val, 1);
	if (ret < 0)
		return ret;

	soh = reg_val;
	r->soh = soh;

	return 0;
}

/*
 * FW_VERSION register reports the firmware (FW) running in the chip. It is fixed to 0x00 when the chip is
 * in shutdown mode. When in active mode, Bit [7:6] = '01' stand for the CW2217, Bit [7:6] = '00' stand for 
 * the CW2218 and Bit [7:6] = '10' stand for CW2215.
 * Bit[5:0] stand for the FW version running in the chip. Note that the FW version is subject to update and 
 * contact sales office for confirmation when necessary.
*/
static int cw_get_fw_version(cw2217_rsrc_t *r)
{
	int ret;
	unsigned char reg_val;
	int fw_version;

    ret = r->iicDev->Read(&r->iicDev->rsrc, CW2215_DEV_ADDR, REG_VCELL_H, &reg_val, 1);
	if (ret < 0)
		return ret;

	fw_version = reg_val; 
	r->fw_version = fw_version;

	return 0;
}

static int cw_update_data(cw2217_rsrc_t *r)
{
    if(r->isReady == 0){
        return -1;
    }
    
	int ret = 0;
	ret += cw_get_voltage(r);
	ret += cw_get_capacity(r);
	ret += cw_get_temp(r);
	ret += cw_get_current(r);
	ret += cw_get_cycle_count(r);
	ret += cw_get_soh(r);
	return ret;
}

static int cw_init_data(cw2217_rsrc_t *r)
{
	int ret = 0;
	
	ret = cw_get_fw_version(r);
	if(ret != 0){
		return ret;
	}
	ret += cw_get_chip_id(r);
    ret += cw_update_data(r);
	return ret;
}

/*CW221X update profile function, Often called during initialization*/
static int cw_config_start_ic(cw2217_rsrc_t *r)
{
	int ret;
	unsigned char reg_val;
	int count = 0;

	ret = cw221X_sleep(r);
	if (ret < 0)
		return ret;	
    
	/* update new battery info */
	ret = cw_write_profile(r, config_profile_info);
	if (ret < 0)
		return ret;
    
	/* set UPDATE_FLAG AND SOC INTTERRUP VALUE*/
	reg_val = CONFIG_UPDATE_FLG | GPIO_SOC_IRQ_VALUE;   
    ret = r->iicDev->Write(&r->iicDev->rsrc, CW2215_DEV_ADDR, REG_SOC_ALERT, &reg_val, 1);
	if (ret < 0)
		return ret;
    
	/*close all interruptes*/
	reg_val = 0; 
	ret = r->iicDev->Write(&r->iicDev->rsrc, CW2215_DEV_ADDR, REG_SOC_ALERT, &reg_val, 1);
	if (ret < 0)
		return ret;
    
	ret = cw221X_startActive(r);
	if (ret < 0) 
		return ret;
    
	while (CW_TRUE) {
        ret = r->iicDev->Read(&r->iicDev->rsrc, CW2215_DEV_ADDR, REG_IC_STATE, &reg_val, 1);
		if (IC_READY_MARK == (reg_val & IC_READY_MARK))
			break;
		count++;
		if (count >= CW_SLEEP_COUNTS){
			cw221X_sleep(r);
            log("</%s 'err' count:%d tick:%d >", __func__, count, HAL_GetTick());
			return -1;
		}
        thread_delay(100);
	}
	return 0;
}

/*
 * Get the cw221X running state
 * Determine whether the profile needs to be updated 
*/
static int cw221X_get_state(cw2217_rsrc_t *r)
{
	int ret;
	unsigned char reg_val;
	int i;
	int reg_profile;
    
    ret = r->iicDev->Read(&r->iicDev->rsrc, CW2215_DEV_ADDR, REG_MODE_CONFIG, &reg_val, 1);
	if (ret < 0)
		return ret;
	if (reg_val != CONFIG_MODE_ACTIVE)
		return CW221X_NOT_ACTIVE;

    ret = r->iicDev->Read(&r->iicDev->rsrc, CW2215_DEV_ADDR, REG_SOC_ALERT, &reg_val, 1);
	if (ret < 0)
		return ret;
	if (0x00 == (reg_val & CONFIG_UPDATE_FLG))
		return CW221X_PROFILE_NOT_READY;

	for (i = 0; i < SIZE_OF_PROFILE; i++) {
        ret = r->iicDev->Read(&r->iicDev->rsrc, CW2215_DEV_ADDR, (REG_BAT_PROFILE + i), &reg_val, 1);
		if (ret < 0)
			return ret;
		reg_profile = REG_BAT_PROFILE + i;
		if (config_profile_info[i] != reg_val)
			break;
	}
	if ( i != SIZE_OF_PROFILE)
		return CW221X_PROFILE_NEED_UPDATE;

	return 0;
}

/*CW221X init function, Often called during initialization*/
static int cw_init(cw2217_rsrc_t *r)
{
	int ret,i;

    for(i=0;;i++){
        ret = cw_get_chip_id(r);
        if (ret == 0) {
            break;
        }
        else if(i > 100){
            log("<%s err:'NOT_get_chip_id'>", __func__);
            return -1;
        }
        thread_delay(5);
    }

	if (r->chip_id != IC_VCHIP_ID){
		return -2;
	}

	ret = cw221X_get_state(r);
	if (ret < 0) {
		return -3;
	}

	if (ret != 0) {
		ret = cw_config_start_ic(r);
		if (ret < 0)
			return -4;
	}
    r->isReady = 1;
	return 0;
}

/**********************************************************
 == THE END ==
**********************************************************/
