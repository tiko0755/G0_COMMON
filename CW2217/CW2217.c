/**********************************************************
filename: cw2217.c
**********************************************************/

/************************����ͷ�ļ�***************************************************/
#include "cw2217.h"
#include "misc.h"
#include "user_log.h"

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
#define CW_SLEEP_COUNTS         50
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

#define CW2215_DEV_ADDR     (0xC8)

static u8 config_profile_info[SIZE_OF_PROFILE] = {
	0x5A,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xAF,0xA0,
	0xAF,0xC2,0xB0,0xA6,0xD7,0xC1,0xB6,0xFF,0xFF,0xF5,
	0xBE,0x94,0x81,0x6A,0x5C,0x56,0x4F,0xD0,0xBC,0xDE,
	0x71,0xCC,0xC6,0xCA,0xD0,0xD5,0xD4,0xD4,0xD1,0xCD,
	0xCB,0xCA,0xCE,0xBE,0xA2,0x94,0x8B,0x84,0x7C,0x77,
	0x81,0x90,0xA8,0x8A,0x64,0x54,0x20,0x00,0xAB,0x10,
	0x00,0xB0,0x51,0x00,0x00,0x00,0x64,0x28,0xD3,0x3F,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF2
};


/**********************************************************
 Private function
**********************************************************/
static int cw_write_profile(cw2217_rsrc_t *r,const u8* buf);
static int cw_get_chip_id(cw2217_rsrc_t *r);
static int cw_get_voltage(cw2217_rsrc_t* r);

/**********************************************************
 Public function
**********************************************************/
void cw2217_setup(
    cw2217_dev_t *d,
    IIC_IO_Dev_T *pIIC
){
    d->rsrc.iicDev = pIIC;
    
    d->get_chip_id = cw_get_chip_id;
    d->get_voltage = cw_get_voltage;
    d->write_profile = cw_write_profile;
    
    cw_get_chip_id(&d->rsrc);
    
}

///**********************************************************
// read data
//**********************************************************/
//s8 readBuf(AT24CXX_Rsrc_T* pRsrc, u16 regAddr, u8 *pBuf, u16 nBytes){
//    IIC_IO_Dev_T* iic = pRsrc->pIIC;
//    if(nBytes == 0 )    return -1;
//    if(regAddr+nBytes-1 >= pRsrc->chip.capacityBytes)    return -2;
//    if(pRsrc->chip.capacityBytes >= AT24C32.capacityBytes)
//        return(iic->Read16(&iic->rsrc, pRsrc->slvWrtAddr, regAddr, pBuf, nBytes));
//    else
//        return(iic->Read(&iic->rsrc, pRsrc->slvWrtAddr, regAddr, pBuf, nBytes));
//}

///**********************************************************
// write data
//**********************************************************/
//s8 writePage(AT24CXX_Rsrc_T* pRsrc, u16 regAddr, const u8 *pBuf, u16 nBytes){
//    IIC_IO_Dev_T* iic = pRsrc->pIIC;
//    if((nBytes > pRsrc->chip.pageSizeBytes) || nBytes==0 )    return -3;
//    if(pRsrc->chip.capacityBytes >= AT24C32.capacityBytes)
//        return(iic->Write16(&iic->rsrc, pRsrc->slvWrtAddr, regAddr, pBuf, nBytes));
//    else
//        return(iic->Write(&iic->rsrc, pRsrc->slvWrtAddr, regAddr, pBuf, nBytes));
//}

//s8 writeBuf(AT24CXX_Rsrc_T* pRsrc, u16 regAddr, const u8 *pBuf, u16 nBytes){
//    u16 thsAddr = regAddr, count;
//    u8 tmpU8;
//    s8 rtn;

//    //if more than capacity
//    if(nBytes == 0 )    return -4;
//    if(thsAddr+nBytes-1 >= pRsrc->chip.capacityBytes)    return -5;

//    for(count=0; (nBytes-count)>0; ){
//        tmpU8 = thsAddr^0xffff;
//        tmpU8 &= pRsrc->chip.pageSizeBytes-1;
//        tmpU8 += 1;    //bytes can be write it this page from regAddr
//        if(nBytes-count < tmpU8)    tmpU8 = nBytes-count;
//        rtn = writePage(pRsrc, thsAddr, &pBuf[count], tmpU8);
//        if( rtn<0)    return rtn;
//        HAL_Delay(TW_MS);
//        thsAddr += tmpU8;
//        count += tmpU8;
//    }
//    return 0;
//}


///* CW221X iic read function */
//static int cw_read(cw2217_rsrc_t *r, unsigned char reg, unsigned char buf[])
//{
//	int ret;

//	ret = i2c_smbus_read_i2c_block_data( client, reg, 1, buf);
//	if (ret < 0)
//		printk("IIC error %d\n", ret);

//	return ret;
//}

///* CW221X iic write function */
//static int cw_write(struct i2c_client *client, unsigned char reg, unsigned char const buf[])
//{
//	int ret;

//	ret = i2c_smbus_write_i2c_block_data( client, reg, 1, &buf[0] );
//	if (ret < 0)
//		printk("IIC error %d\n", ret);

//	return ret;
//}

///* CW221X iic read word function */
//static int cw_read_word(struct i2c_client *client, unsigned char reg, unsigned char buf[])
//{
//	int ret;
//	unsigned char reg_val[2] = { 0, 0 };
//	unsigned int temp_val_buff;
//	unsigned int temp_val_second;

//	ret = i2c_smbus_read_i2c_block_data( client, reg, 2, reg_val );
//	if (ret < 0)
//		printk("IIC error %d\n", ret);
//	temp_val_buff = (reg_val[0] << 8) + reg_val[1];

//	msleep(4);
//	ret = i2c_smbus_read_i2c_block_data( client, reg, 2, reg_val );
//	if (ret < 0)
//		printk("IIC error %d\n", ret);
//	temp_val_second = (reg_val[0] << 8) + reg_val[1];

//	if (temp_val_buff != temp_val_second) {
//		msleep(4);
//		ret = i2c_smbus_read_i2c_block_data( client, reg, 2, reg_val );
//		if (ret < 0)
//			printk("IIC error %d\n", ret);
//		temp_val_buff = (reg_val[0] << 8) + reg_val[1];
//	}

//	buf[0] = reg_val[0];
//	buf[1] = reg_val[1];

//	return ret;
//}

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
//static int cw221X_active(struct cw_battery *cw_bat)
//{
//	int ret;
//	unsigned char reg_val = CONFIG_MODE_RESTART;



//	ret = cw_write(cw_bat->client, REG_MODE_CONFIG, &reg_val);
//    
//	if (ret < 0)
//		return ret;
//	msleep(CW_SLEEP_20MS);  /* Here delay must >= 20 ms */

//	reg_val = CONFIG_MODE_ACTIVE;
//	ret = cw_write(cw_bat->client, REG_MODE_CONFIG, &reg_val);
//	if (ret < 0)
//		return ret;
//	msleep(CW_SLEEP_10MS);

//	return 0;
//}

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
//static int cw221X_sleep(struct cw_battery *cw_bat)
//{
//	int ret;
//	unsigned char reg_val = CONFIG_MODE_RESTART;

//	cw_printk("\n");

//	ret = cw_write(cw_bat->client, REG_MODE_CONFIG, &reg_val);
//	if (ret < 0)
//		return ret;
//	msleep(CW_SLEEP_20MS);  /* Here delay must >= 20 ms */

//	reg_val = CONFIG_MODE_SLEEP;
//	ret = cw_write(cw_bat->client, REG_MODE_CONFIG, &reg_val);
//	if (ret < 0)
//		return ret;
//	msleep(CW_SLEEP_10MS);

//	return 0;
//}

/*
 * The 0x00 register is an UNSIGNED 8bit read-only register. Its value is fixed to 0xA0 in shutdown
 * mode and active mode.
 */
static int cw_get_chip_id(cw2217_rsrc_t *r)
{
    log("<%s >", __func__);
	int ret;
	unsigned char reg_val;
	int chip_id;

    ret = r->iicDev->Read(&r->iicDev->rsrc, CW2215_DEV_ADDR, REG_CHIP_ID, &reg_val, 1);
	if (ret < 0)
		return ret;

	r->chip_id = reg_val;
    log("<%s id:0x%02x >", __func__, reg_val);
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
//static int cw_get_capacity(struct cw_battery *cw_bat)
//{
//	int ret;
//	unsigned char reg_val[2] = { 0, 0 };
//	int ui_100 = CW_UI_FULL;
//	int soc_h;
//	int soc_l;
//	int ui_soc;
//	int remainder;

//	ret = cw_read_word(cw_bat->client, REG_SOC_INT, reg_val);
//	if (ret < 0)
//		return ret;
//	soc_h = reg_val[0];
//	soc_l = reg_val[1];
//	ui_soc = ((soc_h * 256 + soc_l) * 100)/ (ui_100 * 256);
//	remainder = (((soc_h * 256 + soc_l) * 100 * 100) / (ui_100 * 256)) % 100;
//	if (ui_soc >= 100){
//		cw_printk("CW2015[%d]: UI_SOC = %d larger 100!!!!\n", __LINE__, ui_soc);
//		ui_soc = 100;
//	}
//	cw_bat->ic_soc_h = soc_h;
//	cw_bat->ic_soc_l = soc_l;
//	cw_bat->ui_soc = ui_soc;

//	return 0;
//}

/*
 * The TEMP register is an UNSIGNED 8bit read only register. 
 * It reports the real-time battery temperature
 * measured at TS pin. The scope is from -40 to 87.5 degrees Celsius, 
 * LSB is 0.5 degree Celsius. TEMP(C) = - 40 + Value(0x06 Reg) / 2 
 */
//static int cw_get_temp(struct cw_battery *cw_bat)
//{
//	int ret;
//	unsigned char reg_val;
//	int temp;

//	ret = cw_read(cw_bat->client, REG_TEMP, &reg_val);
//	if (ret < 0)
//		return ret;

//	temp = (int)reg_val * 10 / 2 - 400;
//	cw_bat->temp = temp;

//	return 0;
//}

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

///*
// * CURRENT is a SIGNED 16bit register(0x0E 0x0F) that reports current A/D converter result of the voltage across the
// * current sense resistor, 10mohm typical. The result is stored as a two's complement value to show positive
// * and negative current. Voltages outside the minimum and maximum register values are reported as the
// * minimum or maximum value.
// * The register value should be divided by the sense resistance to convert to amperes. The value of the
// * sense resistor determines the resolution and the full-scale range of the current readings. The LSB of 0x0F
// * is (52.4/32768)uV for CW2215 and CW2217. The LSB of 0x0F is (125/32768)uV for CW2218.
// * The default value is 0x0000, stands for 0mA. 0x7FFF stands for the maximum charging current and 0x8001 stands for
// * the maximum discharging current.
// */
//static int cw_get_current(struct cw_battery *cw_bat)
//{
//	int ret;
//	unsigned char reg_val[2] = {0 , 0};
//	long long cw_current; /* use long long type to guarantee 8 bytes space*/
//	unsigned short current_reg;  /* unsigned short must u16 */

//	ret = cw_read_word(cw_bat->client, REG_CURRENT_H, reg_val);
//	if (ret < 0)
//		return ret;

//	current_reg = (reg_val[0] << 8) + reg_val[1];
//	cw_current = get_complement_code(current_reg);
//	if(((cw_bat->fw_version) & (CW2215_MARK != 0)) || ((cw_bat->fw_version) & (CW2217_MARK != 0))){
//		cw_current = cw_current * 1600 / USER_RSENSE;
//	}else if((cw_bat->fw_version != 0) && (cw_bat->fw_version & (0xC0 == CW2218_MARK))){
//		cw_current = cw_current * 3815 / USER_RSENSE;
//	}else{
//		cw_bat->cw_current = 0;
//		printk("error! cw221x frimware read error!\n");
//	}
//	cw_bat->cw_current = cw_current;

//	return 0;
//}

///*
// * CYCLECNT is an UNSIGNED 16bit register(0xA4 0xA5) that counts cycle life of the battery. The LSB of 0xA5 stands
// * for 1/16 cycle. This register will be clear after enters shutdown mode
// */
//static int cw_get_cycle_count(struct cw_battery *cw_bat)
//{
//	int ret;
//	unsigned char reg_val[2] = {0, 0};
//	int cycle;

//	ret = cw_read_word(cw_bat->client, REG_CYCLE_H, reg_val);
//	if (ret < 0)
//		return ret;

//	cycle = (reg_val[0] << 8) + reg_val[1];
//	cw_bat->cycle = cycle / 16;

//	return 0;
//}

///*
// * SOH (State of Health) is an UNSIGNED 8bit register(0xA6) that represents the level of battery aging by tracking
// * battery internal impedance increment. When the device enters active mode, this register refresh to 0x64
// * by default. Its range is 0x00 to 0x64, indicating 0 to 100%. This register will be clear after enters shutdown
// * mode.
// */
//static int cw_get_soh(struct cw_battery *cw_bat)
//{
//	int ret;
//	unsigned char reg_val;
//	int soh;

//	ret = cw_read(cw_bat->client, REG_SOH, &reg_val);
//	if (ret < 0)
//		return ret;

//	soh = reg_val;
//	cw_bat->soh = soh;

//	return 0;
//}

///*
// * FW_VERSION register reports the firmware (FW) running in the chip. It is fixed to 0x00 when the chip is
// * in shutdown mode. When in active mode, Bit [7:6] = '01' stand for the CW2217, Bit [7:6] = '00' stand for 
// * the CW2218 and Bit [7:6] = '10' stand for CW2215.
// * Bit[5:0] stand for the FW version running in the chip. Note that the FW version is subject to update and 
// * contact sales office for confirmation when necessary.
//*/
//static int cw_get_fw_version(struct cw_battery *cw_bat)
//{
//	int ret;
//	unsigned char reg_val;
//	int fw_version;

//	ret = cw_read(cw_bat->client, REG_FW_VERSION, &reg_val);
//	if (ret < 0)
//		return ret;

//	fw_version = reg_val; 
//	cw_bat->fw_version = fw_version;

//	return 0;
//}

//static int cw_update_data(struct cw_battery *cw_bat)
//{
//	int ret = 0;

//	ret += cw_get_voltage(cw_bat);
//	ret += cw_get_capacity(cw_bat);
//	ret += cw_get_temp(cw_bat);
//	ret += cw_get_current(cw_bat);
//	ret += cw_get_cycle_count(cw_bat);
//	ret += cw_get_soh(cw_bat);
//	printk("vol = %d  current = %ld cap = %d temp = %d\n", 
//		cw_bat->voltage, cw_bat->cw_current, cw_bat->ui_soc, cw_bat->temp);

//	return ret;
//}

//static int cw_init_data(struct cw_battery *cw_bat)
//{
//	int ret = 0;
//	
//	ret = cw_get_fw_version(cw_bat);
//	if(ret != 0){
//		return ret;
//	}
//	ret += cw_get_chip_id(cw_bat);
//	ret += cw_get_voltage(cw_bat);
//	ret += cw_get_capacity(cw_bat);
//	ret += cw_get_temp(cw_bat);
//	ret += cw_get_current(cw_bat);
//	ret += cw_get_cycle_count(cw_bat);
//	ret += cw_get_soh(cw_bat);
//	
//	printk("chip_id = %d vol = %d  cur = %ld cap = %d temp = %d  fw_version = %d\n", 
//		cw_bat->chip_id, cw_bat->voltage, cw_bat->cw_current, cw_bat->ui_soc, cw_bat->temp, cw_bat->fw_version);

//	return ret;
//}

///*CW221X update profile function, Often called during initialization*/
//static int cw_config_start_ic(struct cw_battery *cw_bat)
//{
//	int ret;
//	unsigned char reg_val;
//	int count = 0;

//	ret = cw221X_sleep(cw_bat);
//	if (ret < 0)
//		return ret;	

//	/* update new battery info */
//	ret = cw_write_profile(cw_bat->client, config_profile_info);
//	if (ret < 0)
//		return ret;

//	/* set UPDATE_FLAG AND SOC INTTERRUP VALUE*/
//	reg_val = CONFIG_UPDATE_FLG | GPIO_SOC_IRQ_VALUE;   
//	ret = cw_write(cw_bat->client, REG_SOC_ALERT, &reg_val);
//	if (ret < 0)
//		return ret;

//	/*close all interruptes*/
//	reg_val = 0; 
//	ret = cw_write(cw_bat->client, REG_GPIO_CONFIG, &reg_val); 
//	if (ret < 0)
//		return ret;

//	ret = cw221X_active(cw_bat);
//	if (ret < 0) 
//		return ret;

//	while (CW_TRUE) {
//		msleep(CW_SLEEP_100MS);
//		cw_read(cw_bat->client, REG_IC_STATE, &reg_val);
//		if (IC_READY_MARK == (reg_val & IC_READY_MARK))
//			break;
//		count++;
//		if (count >= CW_SLEEP_COUNTS) {
//			cw221X_sleep(cw_bat);
//			return -1;
//		}
//	}

//	return 0;
//}

///*
// * Get the cw221X running state
// * Determine whether the profile needs to be updated 
//*/
//static int cw221X_get_state(struct cw_battery *cw_bat)
//{
//	int ret;
//	unsigned char reg_val;
//	int i;
//	int reg_profile;

//	ret = cw_read(cw_bat->client, REG_MODE_CONFIG, &reg_val);
//	if (ret < 0)
//		return ret;
//	if (reg_val != CONFIG_MODE_ACTIVE)
//		return CW221X_NOT_ACTIVE;

//	ret = cw_read(cw_bat->client, REG_SOC_ALERT, &reg_val);
//	if (ret < 0)
//		return ret;
//	if (0x00 == (reg_val & CONFIG_UPDATE_FLG))
//		return CW221X_PROFILE_NOT_READY;

//	for (i = 0; i < SIZE_OF_PROFILE; i++) {
//		ret = cw_read(cw_bat->client, (REG_BAT_PROFILE + i), &reg_val);
//		if (ret < 0)
//			return ret;
//		reg_profile = REG_BAT_PROFILE + i;
//		cw_printk("0x%2x = 0x%2x\n", reg_profile, reg_val);
//		if (config_profile_info[i] != reg_val)
//			break;
//	}
//	if ( i != SIZE_OF_PROFILE)
//		return CW221X_PROFILE_NEED_UPDATE;

//	return 0;
//}

///*CW221X init function, Often called during initialization*/
//static int cw_init(struct cw_battery *cw_bat)
//{
//	int ret;

//	cw_printk("\n");
//	ret = cw_get_chip_id(cw_bat);
//	if (ret < 0) {
//		printk("iic read write error");
//		return ret;
//	}
//	if (cw_bat->chip_id != IC_VCHIP_ID){
//		printk("not cw221X\n");
//		return -1;
//	}

//	ret = cw221X_get_state(cw_bat);
//	if (ret < 0) {
//		printk("iic read write error");
//		return ret;
//	}

//	if (ret != 0) {
//		ret = cw_config_start_ic(cw_bat);
//		if (ret < 0)
//			return ret;
//	}
//	cw_printk("cw221X init success!\n");

//	return 0;
//}

//static void cw_bat_work(struct work_struct *work)
//{
//	struct delayed_work *delay_work;
//	struct cw_battery *cw_bat;
//	int ret;

//	delay_work = container_of(work, struct delayed_work, work);
//	cw_bat = container_of(delay_work, struct cw_battery, battery_delay_work);

//	ret = cw_update_data(cw_bat);
//	if (ret < 0)
//		printk(KERN_ERR "iic read error when update data");

//	#ifdef CW_PROPERTIES
//	#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 1, 0)
//	power_supply_changed(&cw_bat->cw_bat); 
//	#else
//	power_supply_changed(cw_bat->cw_bat); 
//	#endif
//	#endif

//	queue_delayed_work(cw_bat->cwfg_workqueue, &cw_bat->battery_delay_work, msecs_to_jiffies(queue_delayed_work_time));
//}

/**********************************************************
 == THE END ==
**********************************************************/
