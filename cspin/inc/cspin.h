/**
  ******************************************************************************
  * @file    cSPIN.h 
  * @author  IPC Rennes
  * @version V2.1
  * @date    September 02, 2021
  * @brief   Header for cspin.c module
  * @note    (C) COPYRIGHT 2013 STMicroelectronics
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CSPIN_H
#define __CSPIN_H

/* Includes ------------------------------------------------------------------*/
#include "misc.h"
#include "cspin_config.h"

/* Exported constants --------------------------------------------------------*/

#define CMD_PASS (0)
#define CMD_FAIL  (1)

/* cSPIN maximum number of bytes of command and arguments to set a parameter */
#define cSPIN_CMD_ARG_MAX_NB_BYTES (4)
/* cSPIN command + argument bytes number */
#define cSPIN_CMD_ARG_NB_BYTES_NOP              (1)
#define cSPIN_CMD_ARG_NB_BYTES_RUN              (4)
#define cSPIN_CMD_ARG_NB_BYTES_STEP_CLOCK       (1)
#define cSPIN_CMD_ARG_NB_BYTES_MOVE             (4)
#define cSPIN_CMD_ARG_NB_BYTES_GO_TO            (4)
#define cSPIN_CMD_ARG_NB_BYTES_GO_TO_DIR        (4)
#define cSPIN_CMD_ARG_NB_BYTES_GO_UNTIL         (4)
#define cSPIN_CMD_ARG_NB_BYTES_RELEASE_SW       (1)
#define cSPIN_CMD_ARG_NB_BYTES_GO_HOME          (1)
#define cSPIN_CMD_ARG_NB_BYTES_GO_MARK          (1)
#define cSPIN_CMD_ARG_NB_BYTES_RESET_POS        (1)
#define cSPIN_CMD_ARG_NB_BYTES_RESET_DEVICE     (1)
#define cSPIN_CMD_ARG_NB_BYTES_SOFT_STOP        (1)
#define cSPIN_CMD_ARG_NB_BYTES_HARD_STOP        (1)
#define cSPIN_CMD_ARG_NB_BYTES_SOFT_HIZ         (1)
#define cSPIN_CMD_ARG_NB_BYTES_HARD_HIZ         (1)
#define cSPIN_CMD_ARG_NB_BYTES_GET_STATUS       (1)
/* cSPIN response bytes number */
#define cSPIN_RSP_NB_BYTES_GET_STATUS           (2)    
/* Daisy chain command mask */
#define DAISY_CHAIN_COMMAND_MASK (0xFA)
 
/** cSPIN parameter min and max values
  */
#define cSPIN_CONF_PARAM_STALL_TH_MV_MAX           ((u16)(1000)) /* mV */

/** Register bits / masks
  */

/* cSPIN electrical position register masks */
#define cSPIN_ELPOS_STEP_MASK			((u8)0xC0)
#define cSPIN_ELPOS_MICROSTEP_MASK		((u8)0x3F)

/* cSPIN min speed register bit / mask */
#define cSPIN_LSPD_OPT		((u16) ((0x1) << 12))
#define cSPIN_MIN_SPEED_MASK	((u16)0x0FFF)

/* cSPIN full step boost bit */
#define cSPIN_BOOST_MODE        ((u16) ((0x1) << 10))
#define cSPIN_FS_SPD_MASK	((u16)0x03FF)

/* cSPIN Sync Output frequency enabling bit */
#define cSPIN_SYNC_EN           ((0x1) << 7)

/* cSPIN clock source monitoring enabling bit */
#define cSPIN_WD_EN     	((0x1) << 11)

/* Exported types ------------------------------------------------------------*/

//typedef uint8_t  bool;

/** 
  * @brief cSPIN Init structure definition
  */
typedef struct
{
  u32 ABS_POS;
  u16 EL_POS;
  u32 MARK;
  u32 SPEED;
  u16 ACC;
  u16 DEC;
  u16 MAX_SPEED;
  u16 MIN_SPEED;
  u16 FS_SPD;
  u8  KVAL_HOLD;
  u8  KVAL_RUN;
  u8  KVAL_ACC;
  u8  KVAL_DEC;
  u16 INT_SPD;
  u8  ST_SLP;
  u8  FN_SLP_ACC;
  u8  FN_SLP_DEC;
  u8  K_THERM;
  u8  ADC_OUT;
  u8  OCD_TH;
  u8  STALL_TH;
  u8  STEP_MODE;
  u8  ALARM_EN;
  u16 GATECFG1;		// for L6480 only
  u8  GATECFG2;  	// for L6480 only
  u16 CONFIG;
  u16 STATUS;
}cSPIN_RegsStruct_TypeDef;

/* cSPIN Low speed optimization */
typedef enum {
	cSPIN_LSPD_OPT_OFF		=((u16)0x0000),
	cSPIN_LSPD_OPT_ON			=((u16)cSPIN_LSPD_OPT)
} cSPIN_LSPD_OPT_TypeDef;

/* cSPIN Full step boost */
typedef enum {
	cSPIN_BOOST_MODE_OFF		=((u16)0x0000),
	cSPIN_BOOST_MODE_ON			=((u16)cSPIN_BOOST_MODE)
} cSPIN_BOOST_MODE_TypeDef;

/* cSPIN overcurrent threshold options */
typedef enum {
	cSPIN_OCD_TH_31_25mV		=((u8)0x00),
	cSPIN_OCD_TH_62_5mV			=((u8)0x01),
	cSPIN_OCD_TH_93_75mV		=((u8)0x02),
	cSPIN_OCD_TH_125mV			=((u8)0x03),
	cSPIN_OCD_TH_156_25mV		=((u8)0x04),
	cSPIN_OCD_TH_187_50mV		=((u8)0x05),
	cSPIN_OCD_TH_218_75mV		=((u8)0x06),
	cSPIN_OCD_TH_250mV			=((u8)0x07),
	cSPIN_OCD_TH_281_25mV		=((u8)0x08),
	cSPIN_OCD_TH_312_5mV		=((u8)0x09),
	cSPIN_OCD_TH_343_75mV		=((u8)0x0A),
	cSPIN_OCD_TH_375mV			=((u8)0x0B),
	cSPIN_OCD_TH_406_25mV		=((u8)0x0C),
	cSPIN_OCD_TH_437_5mV		=((u8)0x0D),
	cSPIN_OCD_TH_468_75mV		=((u8)0x0E),
	cSPIN_OCD_TH_500mV			=((u8)0x0F),
	cSPIN_OCD_TH_531_25mV		=((u8)0x10),
	cSPIN_OCD_TH_562_5mV		=((u8)0x11),
	cSPIN_OCD_TH_593_75mV		=((u8)0x12),
	cSPIN_OCD_TH_625mV			=((u8)0x13),
	cSPIN_OCD_TH_656_25mV		=((u8)0x14),
	cSPIN_OCD_TH_687_5mV		=((u8)0x15),
	cSPIN_OCD_TH_718_75mV		=((u8)0x16),
	cSPIN_OCD_TH_750mV			=((u8)0x17),
	cSPIN_OCD_TH_781_25mV		=((u8)0x18),
	cSPIN_OCD_TH_812_5mV		=((u8)0x19),
	cSPIN_OCD_TH_843_75mV		=((u8)0x1A),
	cSPIN_OCD_TH_875mV			=((u8)0x1B),
	cSPIN_OCD_TH_906_25mV		=((u8)0x1C),
	cSPIN_OCD_TH_937_75mV		=((u8)0x1D),
	cSPIN_OCD_TH_968_75mV		=((u8)0x1E),
	cSPIN_OCD_TH_1V		      =((u8)0x1F)        
} cSPIN_OCD_TH_TypeDef;

/* cSPIN STEP_MODE register masks */
typedef enum {
	cSPIN_STEP_MODE_STEP_SEL		=((u8)0x07),
	cSPIN_STEP_MODE_SYNC_SEL		=((u8)0x70),
	cSPIN_STEP_MODE_SYNC_EN			=((u8)0x80)
} cSPIN_STEP_MODE_Masks_TypeDef;

 /* cSPIN STEP_MODE register options */
/* cSPIN STEP_SEL options */
typedef enum {
	cSPIN_STEP_SEL_1			=((u8)0x00),
	cSPIN_STEP_SEL_1_2		=((u8)0x01),
	cSPIN_STEP_SEL_1_4		=((u8)0x02),
	cSPIN_STEP_SEL_1_8		=((u8)0x03),
	cSPIN_STEP_SEL_1_16		=((u8)0x04),
	cSPIN_STEP_SEL_1_32		=((u8)0x05),
	cSPIN_STEP_SEL_1_64		=((u8)0x06),
	cSPIN_STEP_SEL_1_128	        =((u8)0x07)
} cSPIN_STEP_SEL_TypeDef;

/* cSPIN SYNC_SEL options */
typedef enum {
	cSPIN_SYNC_SEL_DISABLED		=((u8)0x00),
  cSPIN_SYNC_SEL_1_2	=((u8)(cSPIN_SYNC_EN|0x00)),
	cSPIN_SYNC_SEL_1		=((u8)(cSPIN_SYNC_EN|0x10)),
	cSPIN_SYNC_SEL_2		=((u8)(cSPIN_SYNC_EN|0x20)),
	cSPIN_SYNC_SEL_4		=((u8)(cSPIN_SYNC_EN|0x30)),
	cSPIN_SYNC_SEL_8		=((u8)(cSPIN_SYNC_EN|0x40)),
	cSPIN_SYNC_SEL_16		=((u8)(cSPIN_SYNC_EN|0x50)),
	cSPIN_SYNC_SEL_32		=((u8)(cSPIN_SYNC_EN|0x60)),
	cSPIN_SYNC_SEL_64		=((u8)(cSPIN_SYNC_EN|0x70))
} cSPIN_SYNC_SEL_TypeDef;

/* cSPIN ALARM_EN register options */
typedef enum {
	cSPIN_ALARM_EN_OVERCURRENT				=((u8)0x01),
	cSPIN_ALARM_EN_THERMAL_SHUTDOWN		=((u8)0x02),
	cSPIN_ALARM_EN_THERMAL_WARNING		=((u8)0x04),
	cSPIN_ALARM_EN_UVLO     					=((u8)0x08),
	cSPIN_ALARM_EN_ADC_UVLO 					=((u8)0x10),
	cSPIN_ALARM_EN_STALL_DETECTION	  =((u8)0x20),
	cSPIN_ALARM_EN_SW_TURN_ON					=((u8)0x40),
	cSPIN_ALARM_EN_WRONG_NPERF_CMD		=((u8)0x80)
} cSPIN_ALARM_EN_TypeDef;

/* cSPIN TCC (Control current Time) options*/
typedef enum {
	cSPIN_TCC_125ns		=((u8)0x00),
  cSPIN_TCC_250ns		=((u8)0x01),
	cSPIN_TCC_375ns		=((u8)0x02),
	cSPIN_TCC_500ns		=((u8)0x03),
	cSPIN_TCC_625ns		=((u8)0x04),
	cSPIN_TCC_750ns		=((u8)0x05),
	cSPIN_TCC_875ns		=((u8)0x06),
	cSPIN_TCC_1000ns	=((u8)0x07),
	cSPIN_TCC_1125ns	=((u8)0x08),
	cSPIN_TCC_1250ns	=((u8)0x09),
	cSPIN_TCC_1375ns	=((u8)0x0A),
	cSPIN_TCC_1500ns	=((u8)0x0B),
	cSPIN_TCC_1625ns	=((u8)0x0C),
	cSPIN_TCC_1750ns	=((u8)0x0D),
	cSPIN_TCC_1875ns	=((u8)0x0E),
	cSPIN_TCC_2000ns	=((u8)0x0F),
	cSPIN_TCC_2125ns	=((u8)0x10),
	cSPIN_TCC_2250ns	=((u8)0x11),
	cSPIN_TCC_2375ns	=((u8)0x12),
	cSPIN_TCC_2500ns	=((u8)0x13),
	cSPIN_TCC_2625ns	=((u8)0x14),
	cSPIN_TCC_2750ns	=((u8)0x15),
	cSPIN_TCC_2875ns	=((u8)0x16),
	cSPIN_TCC_3000ns	=((u8)0x17),
	cSPIN_TCC_3125ns	=((u8)0x18),
	cSPIN_TCC_3250ns	=((u8)0x19),
	cSPIN_TCC_3375ns	=((u8)0x1A),
	cSPIN_TCC_3500ns	=((u8)0x1B),
	cSPIN_TCC_3625ns	=((u8)0x1C),
	cSPIN_TCC_3750ns	=((u8)0x1D),
	cSPIN_TCC_3750ns_bis	=((u8)0x1E),
	cSPIN_TCC_3750ns_ter	=((u8)0x1F)
} cSPIN_TCC_TypeDef;

/* cSPIN IGate options*/
typedef enum {
	cSPIN_IGATE_4mA		    =(((u8)0x00)<<5),
  cSPIN_IGATE_4mA_Bis		=(((u8)0x01)<<5),
	cSPIN_IGATE_8mA		  =(((u8)0x02)<<5),
	cSPIN_IGATE_16mA		=(((u8)0x03)<<5),
	cSPIN_IGATE_24mA		=(((u8)0x04)<<5),
	cSPIN_IGATE_32mA		=(((u8)0x05)<<5),
	cSPIN_IGATE_64mA		=(((u8)0x06)<<5),
	cSPIN_IGATE_96mA		=(((u8)0x07)<<5),
} cSPIN_IGATE_TypeDef;

/* cSPIN TBOOST (Turn off boost time) options*/
typedef enum {
	cSPIN_TBOOST_0ns			=(((u8)0x00)<<8),
  cSPIN_TBOOST_62_5__83_3__125ns	=(((u8)0x01)<<8),
	cSPIN_TBOOST_125ns		=(((u8)0x02)<<8),
	cSPIN_TBOOST_250ns		=(((u8)0x03)<<8),
	cSPIN_TBOOST_375ns		=(((u8)0x04)<<8),
	cSPIN_TBOOST_500ns		=(((u8)0x05)<<8),
	cSPIN_TBOOST_750ns		=(((u8)0x06)<<8),
	cSPIN_TBOOST_1000ns		=(((u8)0x07)<<8),
} cSPIN_TBOOST_TypeDef;

/* cSPIN external clock watchdog */
typedef enum {
	cSPIN_WD_EN_DISABLE		=((u16)0x0000),
	cSPIN_WD_EN_ENABLE		=((u16)cSPIN_WD_EN)
} cSPIN_WD_EN_TypeDef;

/* cSPIN TBLANK (Blanking time) options*/
typedef enum {
	cSPIN_TBLANK_125ns		=(((u8)0x00)<<5),
	cSPIN_TBLANK_250ns      =(((u8)0x01)<<5),
	cSPIN_TBLANK_375ns		=(((u8)0x02)<<5),
	cSPIN_TBLANK_500ns		=(((u8)0x03)<<5),
	cSPIN_TBLANK_625ns		=(((u8)0x04)<<5),
	cSPIN_TBLANK_750ns		=(((u8)0x05)<<5),
	cSPIN_TBLANK_875ns		=(((u8)0x06)<<5),
	cSPIN_TBLANK_1000ns		=(((u8)0x07)<<5),
} cSPIN_TBLANK_TypeDef;

/* cSPIN TDT (Dead time) options*/
typedef enum {
	cSPIN_TDT_125ns		=((u8)0x00),
  cSPIN_TDT_250ns		=((u8)0x01),
	cSPIN_TDT_375ns		=((u8)0x02),
	cSPIN_TDT_500ns		=((u8)0x03),
	cSPIN_TDT_625ns		=((u8)0x04),
	cSPIN_TDT_750ns		=((u8)0x05),
	cSPIN_TDT_875ns		=((u8)0x06),
	cSPIN_TDT_1000ns	=((u8)0x07),
	cSPIN_TDT_1125ns	=((u8)0x08),
	cSPIN_TDT_1250ns	=((u8)0x09),
	cSPIN_TDT_1375ns	=((u8)0x0A),
	cSPIN_TDT_1500ns	=((u8)0x0B),
	cSPIN_TDT_1625ns	=((u8)0x0C),
	cSPIN_TDT_1750ns	=((u8)0x0D),
	cSPIN_TDT_1875ns	=((u8)0x0E),
	cSPIN_TDT_2000ns	=((u8)0x0F),
	cSPIN_TDT_2125ns	=((u8)0x10),
	cSPIN_TDT_2250ns	=((u8)0x11),
	cSPIN_TDT_2375ns	=((u8)0x12),
	cSPIN_TDT_2500ns	=((u8)0x13),
	cSPIN_TDT_2625ns	=((u8)0x14),
	cSPIN_TDT_2750ns	=((u8)0x15),
	cSPIN_TDT_2875ns	=((u8)0x16),
	cSPIN_TDT_3000ns	=((u8)0x17),
	cSPIN_TDT_3125ns	=((u8)0x18),
	cSPIN_TDT_3250ns	=((u8)0x19),
	cSPIN_TDT_3375ns	=((u8)0x1A),
	cSPIN_TDT_3500ns	=((u8)0x1B),
	cSPIN_TDT_3625ns	=((u8)0x1C),
	cSPIN_TDT_3750ns	=((u8)0x1D),
	cSPIN_TDT_3875ns  =((u8)0x1E),
	cSPIN_TDT_4000ns  =((u8)0x1F)
} cSPIN_TDT_TypeDef;


/* cSPIN Config register masks */
typedef enum {
	cSPIN_CONFIG_OSC_SEL			=((u16)0x0007),
	cSPIN_CONFIG_EXT_CLK			=((u16)0x0008),
	cSPIN_CONFIG_SW_MODE			=((u16)0x0010),
	cSPIN_CONFIG_EN_VSCOMP		=((u16)0x0020),
	cSPIN_CONFIG_OC_SD				=((u16)0x0080),
	cSPIN_CONFIG_UVLOVAL			=((u16)0x0100),
	cSPIN_CONFIG_VCCVAL				=((u16)0x0200),
	cSPIN_CONFIG_F_PWM_DEC		=((u16)0x1C00),
	cSPIN_CONFIG_F_PWM_INT		=((u16)0xE000)
} cSPIN_CONFIG_Masks_TypeDef;

/* cSPIN Config register options */
typedef enum {
	cSPIN_CONFIG_INT_16MHZ							=((u16)0x0000),
	cSPIN_CONFIG_INT_16MHZ_OSCOUT_2MHZ	=((u16)0x0008),
	cSPIN_CONFIG_INT_16MHZ_OSCOUT_4MHZ	=((u16)0x0009),
	cSPIN_CONFIG_INT_16MHZ_OSCOUT_8MHZ	=((u16)0x000A),
	cSPIN_CONFIG_INT_16MHZ_OSCOUT_16MHZ	=((u16)0x000B),
	cSPIN_CONFIG_EXT_8MHZ_XTAL_DRIVE		=((u16)0x0004),
	cSPIN_CONFIG_EXT_16MHZ_XTAL_DRIVE		=((u16)0x0005),
	cSPIN_CONFIG_EXT_24MHZ_XTAL_DRIVE		=((u16)0x0006),
	cSPIN_CONFIG_EXT_32MHZ_XTAL_DRIVE		=((u16)0x0007),
	cSPIN_CONFIG_EXT_8MHZ_OSCOUT_INVERT	=((u16)0x000C),
	cSPIN_CONFIG_EXT_16MHZ_OSCOUT_INVERT	=((u16)0x000D),
	cSPIN_CONFIG_EXT_24MHZ_OSCOUT_INVERT	=((u16)0x000E),
	cSPIN_CONFIG_EXT_32MHZ_OSCOUT_INVERT	=((u16)0x000F)
} cSPIN_CONFIG_OSC_MGMT_TypeDef;

typedef enum {
	cSPIN_CONFIG_SW_HARD_STOP		=((u16)0x0000),
	cSPIN_CONFIG_SW_USER				=((u16)0x0010)
} cSPIN_CONFIG_SW_MODE_TypeDef;

typedef enum {
	cSPIN_CONFIG_VS_COMP_DISABLE	=((u16)0x0000),
	cSPIN_CONFIG_VS_COMP_ENABLE		=((u16)0x0020)
} cSPIN_CONFIG_EN_VSCOMP_TypeDef;

typedef enum {
	cSPIN_CONFIG_OC_SD_DISABLE		=((u16)0x0000),
	cSPIN_CONFIG_OC_SD_ENABLE			=((u16)0x0080)
} cSPIN_CONFIG_OC_SD_TypeDef;

typedef enum {
	cSPIN_CONFIG_UVLOVAL_LOW        =((u16)0x0000),
  cSPIN_CONFIG_UVLOVAL_HIGH				=((u16)0x0100),
} cSPIN_CONFIG_UVLOVAL_TypeDef;

typedef enum {
	cSPIN_CONFIG_VCCVAL_7_5V        =((u16)0x0200),
	cSPIN_CONFIG_VCCVAL_15V         =((u16)0x0300)
} cSPIN_CONFIG_VCCVAL_TypeDef;

typedef enum {
	cSPIN_CONFIG_PWM_DIV_1		=(((u16)0x00)<<13),
	cSPIN_CONFIG_PWM_DIV_2		=(((u16)0x01)<<13),
	cSPIN_CONFIG_PWM_DIV_3		=(((u16)0x02)<<13),
	cSPIN_CONFIG_PWM_DIV_4		=(((u16)0x03)<<13),
	cSPIN_CONFIG_PWM_DIV_5		=(((u16)0x04)<<13),
	cSPIN_CONFIG_PWM_DIV_6		=(((u16)0x05)<<13),
	cSPIN_CONFIG_PWM_DIV_7		=(((u16)0x06)<<13)
} cSPIN_CONFIG_F_PWM_INT_TypeDef;

typedef enum {
	cSPIN_CONFIG_PWM_MUL_0_625	=(((u16)0x00)<<10),
	cSPIN_CONFIG_PWM_MUL_0_75		=(((u16)0x01)<<10),
	cSPIN_CONFIG_PWM_MUL_0_875	=(((u16)0x02)<<10),
	cSPIN_CONFIG_PWM_MUL_1			=(((u16)0x03)<<10),
	cSPIN_CONFIG_PWM_MUL_1_25		=(((u16)0x04)<<10),
	cSPIN_CONFIG_PWM_MUL_1_5		=(((u16)0x05)<<10),
	cSPIN_CONFIG_PWM_MUL_1_75		=(((u16)0x06)<<10),
	cSPIN_CONFIG_PWM_MUL_2			=(((u16)0x07)<<10)
} cSPIN_CONFIG_F_PWM_DEC_TypeDef;

/* Status Register bit masks */
typedef enum {
	cSPIN_STATUS_HIZ			=(((u16)0x0001)),
	cSPIN_STATUS_BUSY			=(((u16)0x0002)),
	cSPIN_STATUS_SW_F			=(((u16)0x0004)),
	cSPIN_STATUS_SW_EVN		=(((u16)0x0008)),
	cSPIN_STATUS_DIR			=(((u16)0x0010)),
	cSPIN_STATUS_MOT_STATUS		=(((u16)0x0060)),
	cSPIN_STATUS_NOTPERF_CMD	=(((u16)0x0080)),
	cSPIN_STATUS_STCK_MOD			=(((u16)0x0100)),
	cSPIN_STATUS_UVLO					=(((u16)0x0200)),
	cSPIN_STATUS_UVLO_ADC	    =(((u16)0x0400)),
	cSPIN_STATUS_TH_SD				=(((u16)0x1800)),
	cSPIN_STATUS_OCD					=(((u16)0x2000)),
	cSPIN_STATUS_STEP_LOSS_A	=(((u16)0x4000)),
	cSPIN_STATUS_STEP_LOSS_B	=(((u16)0x8000))
} cSPIN_STATUS_Masks_TypeDef;

/* Status Register options */
typedef enum {
	cSPIN_STATUS_MOT_STATUS_STOPPED				=(((u16)0x0000)<<5),
	cSPIN_STATUS_MOT_STATUS_ACCELERATION	=(((u16)0x0001)<<5),
	cSPIN_STATUS_MOT_STATUS_DECELERATION	=(((u16)0x0002)<<5),
	cSPIN_STATUS_MOT_STATUS_CONST_SPD			=(((u16)0x0003)<<5)
} cSPIN_STATUS_TypeDef;

/* cSPIN internal register addresses */
typedef enum {
	cSPIN_ABS_POS	=((u8)0x01),
	cSPIN_EL_POS	=((u8)0x02),
	cSPIN_MARK		=((u8)0x03),
	cSPIN_SPEED		=((u8)0x04),
	cSPIN_ACC			=((u8)0x05),
	cSPIN_DEC			=((u8)0x06),
	cSPIN_MAX_SPEED		=((u8)0x07),
	cSPIN_MIN_SPEED		=((u8)0x08),
	cSPIN_FS_SPD			=((u8)0x15),
	cSPIN_KVAL_HOLD		=((u8)0x09),
	cSPIN_KVAL_RUN		=((u8)0x0A),
	cSPIN_KVAL_ACC		=((u8)0x0B),
	cSPIN_KVAL_DEC		=((u8)0x0C),
	cSPIN_INT_SPD			=((u8)0x0D),
	cSPIN_ST_SLP			=((u8)0x0E),
	cSPIN_FN_SLP_ACC	=((u8)0x0F),
	cSPIN_FN_SLP_DEC	=((u8)0x10),
	cSPIN_K_THERM			=((u8)0x11),
	cSPIN_ADC_OUT			=((u8)0x12),
	cSPIN_OCD_TH			=((u8)0x13),
	cSPIN_STALL_TH		=((u8)0x14),
	cSPIN_STEP_MODE		=((u8)0x16),
	cSPIN_ALARM_EN		=((u8)0x17),
	// only for L6480
	cSPIN6480_GATECFG1		=((u8)0x18),
	cSPIN6480_GATECFG2		=((u8)0x19),
	cSPIN6480_CONFIG			=((u8)0x1A),
	cSPIN6480_STATUS			=((u8)0x1B),
	// only for L6470
	cSPIN6470_CONFIG			=((u8)0x18),	// L6470
	cSPIN6470_STATUS			=((u8)0x19)
} cSPIN_Regs_T;

/* cSPIN command set */
typedef enum {
	cSPIN_NOP					=((u8)0x00),
	cSPIN_SET_PARAM		=((u8)0x00),
	cSPIN_GET_PARAM		=((u8)0x20),
	cSPIN_RUN					=((u8)0x50),
	cSPIN_STEP_CLOCK	=((u8)0x58),
	cSPIN_MOVE				=((u8)0x40),
	cSPIN_GO_TO				=((u8)0x60),
	cSPIN_GO_TO_DIR		=((u8)0x68),
	cSPIN_GO_UNTIL		=((u8)0x82),
  cSPIN_GO_UNTIL_ACT_CPY  =((u8)0x8A),
	cSPIN_RELEASE_SW				=((u8)0x92),
	cSPIN_GO_HOME						=((u8)0x70),
	cSPIN_GO_MARK						=((u8)0x78),
	cSPIN_RESET_POS					=((u8)0xD8),
	cSPIN_RESET_DEVICE			=((u8)0xC0),
	cSPIN_SOFT_STOP					=((u8)0xB0),
	cSPIN_HARD_STOP					=((u8)0xB8),
	cSPIN_SOFT_HIZ					=((u8)0xA0),
	cSPIN_HARD_HIZ					=((u8)0xA8),
	cSPIN_GET_STATUS				=((u8)0xD0),
	cSPIN_RESERVED_CMD1			=((u8)0xEB),
	cSPIN_RESERVED_CMD2			=((u8)0xF8)
} cSPIN_Commands_TypeDef;

/* cSPIN direction options */
typedef enum {
	FWD		=((u8)0x01),
	REV		=((u8)0x00)
} cSPIN_Direction_T;

/* cSPIN action options */
typedef enum {
	ACTION_RESET	=((u8)0x00),
	ACTION_COPY		=((u8)0x08)
} cSPIN_Action_T;

#define	MOTOR_RPT_ABSPOS_BIT 0
#define	MOTOR_RPT_ADCOUT_BIT 1
#define	MOTOR_RPT_SPEED_BIT	 2
#define	MOTOR_RPT_STATUS_BIT 3
#define	MOTOR_EEPROM_ERR_BIT 4

#define MOTOR_APP_POLLING_CYC 8

#define CSPIN_ROM_SIZE	300

typedef enum{
	CHIP_L6470	= 6470,
	CHIP_L6480 = 6480,
	CHIP_L6482	= 6482
}	CSPIN_CHIP;

typedef struct{
	char name[DEV_NAME_LEN];
	CSPIN_CHIP chip;
	/* hardware */
	SPI_HandleTypeDef* SPI_HANDLE;
	const PIN_T* CS;
	const PIN_T* SW_L;
	const PIN_T* SW_R;
	const PIN_T *FLG,*SYNC,*STCK;
//	const PIN_T* SCL;
//	const PIN_T* SDA;
	
	const PIN_T* nFlag;
	const PIN_T* nStby_nReset;

	__IO u16 flags;	
	__IO u32 tick;
	__IO u8 refLSeries;
	__IO u8 refRSeries;
	
	__IO u32 runTmr;
	u32 errorCode;
	
	/* registers */
	//cSPIN_RegsStruct_TypeDef reg;

  __IO u16 eepromAddrBase;
  s8 (*EepromWrite)  (u16 addr, const u8 *pDat, u16 nBytes);
  s8 (*EepromRead)   (u16 addr, u8 *pDat, u16 nBytes);

	/* callback */
	void (*callBackRefLTurnOn) 	(void);	//left sw turn on
	void (*callBackRefLRelease) (void);	//left sw release
	void (*callBackRefRTurnOn) 	(void);
	void (*callBackRefRRelease) (void);
	void (*callBackAbsPosChanged) (u32 pos);
	void (*callBackStatusChanged) (u16 status);
	void (*callBackAdcOutChanged) (u8 adc);
	/* print */
	void (*print)(const char* FORMAT_ORG, ...);
} cSPIN_Rsrc_T;

typedef struct{
	cSPIN_Rsrc_T rsrc;
	//operations
	u8 (*RegSetAbsPos)		(cSPIN_Rsrc_T* pRsrc, u32 param);
	u8 (*RegGetAbsPos)		(cSPIN_Rsrc_T* pRsrc, u32 *param);
	u8 (*RegSetElPos)			(cSPIN_Rsrc_T* pRsrc, u16 param);
	u8 (*RegGetElPos)			(cSPIN_Rsrc_T* pRsrc, u16 *param);
	u8 (*RegSetMarkPos)		(cSPIN_Rsrc_T* pRsrc, u32 param);
	u8 (*RegGetMarkPos)		(cSPIN_Rsrc_T* pRsrc, u32 *param);
	u8 (*RegGetSpd_StepPerS)	(cSPIN_Rsrc_T* pRsrc, u32 *spd);

	// speed profit
	u8 (*RegSetAcc_stepPerSS)			(cSPIN_Rsrc_T* pRsrc, u16 param);	// in step/(s*s)
	u8 (*RegGetAcc_stepPerSS)			(cSPIN_Rsrc_T* pRsrc, u16 *param);	// in step/(s*s)
	u8 (*RegSetDec_stepPerSS)			(cSPIN_Rsrc_T* pRsrc, u16 param);	// in step/(s*s)
	u8 (*RegGetDec_stepPerSS)			(cSPIN_Rsrc_T* pRsrc, u16 *param);	// in step/(s*s)
	u8 (*RegSetMaxSpd_stepPerS)		(cSPIN_Rsrc_T* pRsrc, u16 param);	// in step/s
	u8 (*RegGetMaxSpd_stepPerS)		(cSPIN_Rsrc_T* pRsrc, u16 *param);	// in step/s
	u8 (*RegSetMinSpd_stepPerS)		(cSPIN_Rsrc_T* pRsrc, u16 param);	// in step/s
	u8 (*RegGetMinSpd_stepPerS)		(cSPIN_Rsrc_T* pRsrc, u16 *param);	// in step/s
	u8 (*RegSetFSSpd_stepPerS)			(cSPIN_Rsrc_T* pRsrc, u16 param);	// in step/s
	u8 (*RegGetFSSpd_stepPerS)			(cSPIN_Rsrc_T* pRsrc, u16 *param);	// in step/s
	u8 (*RegSetStepSel)		(cSPIN_Rsrc_T* pRsrc, u8 param);	// [0]:1/1 [1]:1/2 ...
	u8 (*RegGetStepSel)		(cSPIN_Rsrc_T* pRsrc, u8 *param);

	u8 (*RegSetLowSpdOpt)	(cSPIN_Rsrc_T* pRsrc, u8 param);
	u8 (*RegGetLowSpdOpt)	(cSPIN_Rsrc_T* pRsrc, u8 *param);
	u8 (*RegSetBoostMode)	(cSPIN_Rsrc_T* pRsrc, u8 param);
	u8 (*RegGetBoostMode)	(cSPIN_Rsrc_T* pRsrc, u8 *param);

//	u8 (*RegSetKValHold)	(cSPIN_Rsrc_T* pRsrc, u8 param);
//	u8 (*RegGetKValHold)	(cSPIN_Rsrc_T* pRsrc, u8 *param);
//	u8 (*RegSetKValRun)		(cSPIN_Rsrc_T* pRsrc, u8 param);
//	u8 (*RegGetKValRun)		(cSPIN_Rsrc_T* pRsrc, u8 *param);
//	u8 (*RegSetKValAcc)		(cSPIN_Rsrc_T* pRsrc, u8 param);
//	u8 (*RegGetKValAcc)		(cSPIN_Rsrc_T* pRsrc, u8 *param);
//	u8 (*RegSetKValDec)		(cSPIN_Rsrc_T* pRsrc, u8 param);
//	u8 (*RegGetKValDec)		(cSPIN_Rsrc_T* pRsrc, u8 *param);

	u8 (*RegSetKVal)	(cSPIN_Rsrc_T* pRsrc, u8 kHold, u8 kRun, u8 kAcc, u8 kDec);
	u8 (*RegGetKVal)		(cSPIN_Rsrc_T* pRsrc, u8* kHold, u8* kRun, u8* kAcc, u8* kDec);

	u8 (*RegSetIntSpd_stepPerS)		(cSPIN_Rsrc_T* pRsrc, u16 param);
	u8 (*RegGetIntSpd_stepPerS)		(cSPIN_Rsrc_T* pRsrc, u16 *param);
	u8 (*RegSetStartSlp)	(cSPIN_Rsrc_T* pRsrc, u8 param);
	u8 (*RegGetStartSlp)	(cSPIN_Rsrc_T* pRsrc, u8 *param);
	u8 (*RegSetFnSlpAcc)	(cSPIN_Rsrc_T* pRsrc, u8 param);
	u8 (*RegGetFnSlpAcc)	(cSPIN_Rsrc_T* pRsrc, u8 *param);
	u8 (*RegSetFnSlpDec)	(cSPIN_Rsrc_T* pRsrc, u8 param);
	u8 (*RegGetFnSlpDec)	(cSPIN_Rsrc_T* pRsrc, u8 *param);
	u8 (*RegSetKTherm)		(cSPIN_Rsrc_T* pRsrc, u8 param);
	u8 (*RegGetKTherm)		(cSPIN_Rsrc_T* pRsrc, u8 *param);
	u8 (*RegGetAdcOut)		(cSPIN_Rsrc_T* pRsrc, u8 *param);
	u8 (*RegSetOCDTh)			(cSPIN_Rsrc_T* pRsrc, u8 param);
	u8 (*RegGetOCDTh)			(cSPIN_Rsrc_T* pRsrc, u8 *param);
	u8 (*RegSetStallTh)		(cSPIN_Rsrc_T* pRsrc, u8 param);
	u8 (*RegGetStallTh)		(cSPIN_Rsrc_T* pRsrc, u8 *param);
	u8 (*RegSetSyncSel)		(cSPIN_Rsrc_T* pRsrc, u8 param);
	u8 (*RegGetSyncSel)		(cSPIN_Rsrc_T* pRsrc, u8 *param);
	u8 (*RegSetSyncEn)		(cSPIN_Rsrc_T* pRsrc, u8 param);
	u8 (*RegGetSyncEn)		(cSPIN_Rsrc_T* pRsrc, u8 *param);
	u8 (*RegSetSwMode) 		(cSPIN_Rsrc_T* pRsrc, u8 param);
	u8 (*RegGetSwMode) 		(cSPIN_Rsrc_T* pRsrc, u8 *param);
	u8 (*RegSetVsCompEn) 	(cSPIN_Rsrc_T* pRsrc, u8 param);
	u8 (*RegGetVsCompEn) 	(cSPIN_Rsrc_T* pRsrc, u8 *param);
	u8 (*RegSetOCSDEn) 		(cSPIN_Rsrc_T* pRsrc, u8 param);
	u8 (*RegGetOCSDEn) 		(cSPIN_Rsrc_T* pRsrc, u8 *param);
	u8 (*RegSetUVLOVal)		(cSPIN_Rsrc_T* pRsrc, u8 param);
	u8 (*RegGetUVLOVal) 	(cSPIN_Rsrc_T* pRsrc, u8 *param);
	u8 (*RegSetVccVal) 		(cSPIN_Rsrc_T* pRsrc, u8 param);
	u8 (*RegGetVccVal) 		(cSPIN_Rsrc_T* pRsrc, u8 *param);
	u8 (*RegSetPwmFrq) 		(cSPIN_Rsrc_T* pRsrc, u8 param);
	u8 (*RegGetPwmFrq) 		(cSPIN_Rsrc_T* pRsrc, u8 *param);
	
	// only for l6480
	u8 (*RegSetGateTcc)		(cSPIN_Rsrc_T* pRsrc, u8 param);
	u8 (*RegGetGateTcc)		(cSPIN_Rsrc_T* pRsrc, u8 *param);
	u8 (*RegSetGateCurrent)		(cSPIN_Rsrc_T* pRsrc, u8 param);
	u8 (*RegGetGateCurrent)		(cSPIN_Rsrc_T* pRsrc, u8 *param);
	u8 (*RegSetGateTBoost)		(cSPIN_Rsrc_T* pRsrc, u8 param);
	u8 (*RegGetGateTBoost)		(cSPIN_Rsrc_T* pRsrc, u8 *param);
	u8 (*RegSetGateTDeadTime)	(cSPIN_Rsrc_T* pRsrc, u8 param);
	u8 (*RegGetGateTDeadTime)	(cSPIN_Rsrc_T* pRsrc, u8 *param);
	u8 (*RegSetGateTBlank)		(cSPIN_Rsrc_T* pRsrc, u8 param);
	u8 (*RegGetGateTBlank)		(cSPIN_Rsrc_T* pRsrc, u8 *param);
	
	u8 (*RegSetCofig)		(cSPIN_Rsrc_T* pRsrc, u16 param);
	u8 (*RegGetCofig)		(cSPIN_Rsrc_T* pRsrc, u16 *param);

	void (*Set_Param)		(cSPIN_Rsrc_T* pRsrc, cSPIN_Regs_T param, u32 value);
	u32 (*Get_Param)		(cSPIN_Rsrc_T* pRsrc, cSPIN_Regs_T param);
	
	void (*ResetDev)		(cSPIN_Rsrc_T* pRsrc);
	void (*Polling) 		(cSPIN_Rsrc_T* pRsrc, u8 tick);	
	
	void (*Run)				(cSPIN_Rsrc_T* pRsrc, cSPIN_Direction_T direction, u32 speed);
	void (*RunTimeout)		(cSPIN_Rsrc_T* pRsrc, cSPIN_Direction_T direction, u32 speed, u32 mSec);
	void (*Step_Clock)	     (cSPIN_Rsrc_T* pRsrc, cSPIN_Direction_T direction);
	void (*Move_microStep)			(cSPIN_Rsrc_T* pRsrc, cSPIN_Direction_T direction, u32 n_step);
	void (*Move_Step)			(cSPIN_Rsrc_T* pRsrc, cSPIN_Direction_T direction, u32 step);
	void (*Go_To)			(cSPIN_Rsrc_T* pRsrc, u32 abs_pos);
	void (*Go_To_Dir)		(cSPIN_Rsrc_T* pRsrc, cSPIN_Direction_T direction, u32 abs_pos);
	void (*Go_Until)		(cSPIN_Rsrc_T* pRsrc, cSPIN_Action_T action, cSPIN_Direction_T direction, u32 speed);
	void (*Release_SW)	(cSPIN_Rsrc_T* pRsrc, cSPIN_Action_T action, cSPIN_Direction_T direction);
	void (*Go_Home)			(cSPIN_Rsrc_T* pRsrc);
	void (*Go_Mark)			(cSPIN_Rsrc_T* pRsrc);
	void (*Reset_Pos)		(cSPIN_Rsrc_T* pRsrc);
	void (*Soft_Stop)		(cSPIN_Rsrc_T* pRsrc);
	void (*Hard_Stop)		(cSPIN_Rsrc_T* pRsrc);
	void (*Soft_HiZ	)		(cSPIN_Rsrc_T* pRsrc);
	void (*Hard_HiZ	)		(cSPIN_Rsrc_T* pRsrc);
	
	u16 (*Get_Status)		(cSPIN_Rsrc_T* pRsrc);
	u8 (*Busy_HW)		(cSPIN_Rsrc_T* pRsrc);
	u8 (*Busy_SW)		(cSPIN_Rsrc_T* pRsrc);
	u8 (*Flag)			(cSPIN_Rsrc_T* pRsrc);
	s8 (*SaveToROM)(cSPIN_Rsrc_T* pRsrc);
	void (*Default)(cSPIN_Rsrc_T* pRsrc);
	void (*PrintRegs)(cSPIN_Rsrc_T* pRsrc);
}cSPIN_DEV_T;

/* Exported macro ------------------------------------------------------------*/
#define Speed_Steps_to_Par(steps) 	((u32)(((steps)*67.108864)+0.5))				/* Speed conversion, range 0 to 15625 steps/s */
#define AccDec_Steps_to_Par(steps) 	((u16)(((steps)*0.068719476736)+0.5))		/* Acc/Dec rates conversion, range 14.55 to 59590 steps/s2 */
#define Par_toAccDec_Steps(par) 	((u16)((par-0.5)*14.551915))
#define MaxSpd_Steps_to_Par(steps) 	((u16)(((steps)*0.065536)+0.5))					/* Max Speed conversion, range 15.25 to 15610 steps/s */
#define MinSpd_Steps_to_Par(steps) 	((u16)(((steps)*4.194304)+0.5))					/* Min Speed conversion, range 0 to 976.3 steps/s */
#define MinSpd_Steps_to_Par_64(steps) 	((u16)(((steps)*0.065536)+0.5))			/* Min Speed conversion, range 0 to 976.3 steps/s */
#define FSSpd_Steps_to_Par(steps) 	((u16)(((steps)*0.065536)+0.5))					/* Full Step Speed conversion, range 7.63 to 15625 steps/s */
#define IntSpd_Steps_to_Par(steps) 	((u16)(((steps)*16.7773)+0.5))					/* Intersect Speed conversion, range 0 to 3906 steps/s */
#define Kval_Perc_to_Par(perc) 			((u8)(((perc)*2.56)+0.5))								/* KVAL conversions, range 0.4% to 99.6% */
#define BEMF_Slope_Perc_to_Par(perc) ((u8)(((perc)/0.00156862745098)+0.5))	/* BEMF compensation slopes, range 0 to 0.4% s/step */
#define BEMF_Slope_Perc_to_Par_u(x) ((u8)(((x)/15.6862745098)+0.5))					/* BEMF compensation slopes, range 0 to 0.4% s/step */
#define KTherm_to_Par(KTherm) 			((u8)(((KTherm - 1)/0.03125)+0.5))			/* K_THERM compensation conversion, range 1 to 1.46875 */
#define StallTh_to_Par(StallTh) 		((u8)(((StallTh - 31.25)/31.25)+0.5))		/* Stall Threshold conversion, range 31.25mV to 1000mV */

/* Exported functions ------------------------------------------------------- */
//void cSPIN_Reset_And_Standb(cSPIN_Rsrc_T* pRsrc);

void cSPIN_Setup(
	cSPIN_DEV_T *pDev, 
	const char* NAME, 
	CSPIN_CHIP CHIP,
	SPI_HandleTypeDef* SPI_HANDLE,
	const PIN_T* CS,
	
	const PIN_T* SW_L,
	const PIN_T* SW_R,
	
	const PIN_T* FLAG,
	const PIN_T* SYNC_nBSY,
	const PIN_T* STCK,
	
	void (*print)				(const char* FORMAT_ORG, ...),
  s8 (*EepromRead)   (u16 addr, u8 *pDat, u16 nBytes),
  s8 (*EepromWrite)  (u16 addr, const u8 *pDat, u16 nBytes),
	u16 romBase
);

#endif /* __CSPIN_H */

/******************* (C) COPYRIGHT 2013 STMicroelectronics *****END OF FILE****/
