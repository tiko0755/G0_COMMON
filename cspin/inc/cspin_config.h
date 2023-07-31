/**
  ******************************************************************************
  * @file    cspin_config.h 
  * @author  IPC Rennes
  * @version V2.1
  * @date    October 15, 2013
  * @brief   Header with configuration parameters for cspin.c module
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
#ifndef __CSPIN_CONFIG_H
#define __CSPIN_CONFIG_H

/* Includes ------------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

  /****************************************************************************/
  /***************** CSPIN chip type ******************************************/
  /****************************************************************************/
//  #define motor (1)
//	#define USING_L6480	(1)
//	#define USING_L6470 (0)	
  /****************************************************************************/
  /***************** Operation Mode Choice ************************************/
  /***************** 1 if daisy chain for one device or more, else 0 **********/
  /****************************************************************************/
//  #define DAISY_CHAIN              (0)

//  /****************************************************************************/
//  /******************Daisy Chain Mode *****************************************/
//  /****************************************************************************/

///* Exported constants --------------------------------------------------------*/

//  /**************************** Slaves numbering ******************************/
//        /* Number of dPIN slaves */
//        #define NUMBER_OF_SLAVES         (1)
//        /* Devices */
//        /* The first device of the chain receives the last byte transmitted by the master */
//        /* The last device of the chain receives the first byte transmitted by the master */
//        #define DEVICE_1                 (NUMBER_OF_SLAVES-1)
//#if (DEVICE_1>0)
//        #define DEVICE_2                 (NUMBER_OF_SLAVES-2)
//#else
//        #define DEVICE_2                 (DEVICE_1)
//#endif
//#if (DEVICE_2>0)
//        #define DEVICE_3                 (NUMBER_OF_SLAVES-3)
//#else
//        #define DEVICE_3                 (DEVICE_2)
//#endif
//#if (DEVICE_3>0)
//        #define DEVICE_4                 (NUMBER_OF_SLAVES-4)
//#else
//        #define DEVICE_4                 (DEVICE_3)
//#endif
//#if (DEVICE_4>0)
//        #define DEVICE_5                 (NUMBER_OF_SLAVES-5)
//#else
//        #define DEVICE_5                 (DEVICE_4)
//#endif
//#if (DEVICE_5>0)
//        #define DEVICE_6                 (NUMBER_OF_SLAVES-6)
//#else
//        #define DEVICE_6                 (DEVICE_5)
//#endif
//#if (DEVICE_6>0)
//        #define DEVICE_7                 (NUMBER_OF_SLAVES-7)
//#else
//        #define DEVICE_7                 (DEVICE_6)
//#endif
//#if (DEVICE_7>0)
//        #define DEVICE_8                 (NUMBER_OF_SLAVES-8)
//#else
//        #define DEVICE_8                 (DEVICE_7)
//#endif

  /****************************************************************************/
  /***** #define cSPIN_CONF_PARAM_XXX (DEVICE_N, DEVICE_N-1, ..., DEVICE_1) ***/
  /****************************************************************************/


  /**************************** Speed Profile *********************************/
	/* Register : ACC */
        /* Acceleration rate in step/s2, range 14.55 to 59590 steps/s2 */
    #define cSPIN_DC_CONF_PARAM_ACC {2008.16}

	/* Register : DEC */
        /* Deceleration rate in step/s2, range 14.55 to 59590 steps/s2 */
    #define cSPIN_DC_CONF_PARAM_DEC {2008.16}

	/* Register : MAX_SPEED */
        /* Maximum speed in step/s, range 15.25 to 15610 steps/s */
    #define cSPIN_DC_CONF_PARAM_MAX_SPEED {991.82}

	/* Register : MIN_SPEED */
	/* Minimum speed in step/s, range 0 to 976.3 steps/s */
    #define cSPIN_DC_CONF_PARAM_MIN_SPEED {0}

	/* Register : FS_SPD */
        /* Full step speed in step/s, range 7.63 to 15625 steps/s */
    #define cSPIN_DC_CONF_PARAM_FS_SPD {595.09}
  
  /************************ Phase Current Control *****************************/
  /* Register : KVAL_HOLD */
	/* Hold duty cycle (torque) in %, range 0 to 99.6% */
    #define cSPIN_DC_CONF_PARAM_KVAL_HOLD {2}

	/* Register : KVAL_RUN */
	/* Run duty cycle (torque) in %, range 0 to 99.6% */
    #define cSPIN_DC_CONF_PARAM_KVAL_RUN {27}

	/* Register : KVAL_ACC */
  /* Acceleration duty cycle (torque) in %, range 0 to 99.6% */
    #define cSPIN_DC_CONF_PARAM_KVAL_ACC {27}

	/* Register : KVAL_DEC */
	/* Deceleration duty cycle (torque) in %, range 0 to 99.6% */
    #define cSPIN_DC_CONF_PARAM_KVAL_DEC {27}

	/* Register : CONFIG - field : EN_VSCOMP */
	/* Motor Supply Voltage Compensation enabling , enum cSPIN_CONFIG_EN_VSCOMP_TypeDef */
    #define cSPIN_DC_CONF_PARAM_VS_COMP {cSPIN_CONFIG_VS_COMP_DISABLE}

	/* Register : MIN_SPEED - field : LSPD_OPT */
	/* Low speed optimization bit, enum cSPIN_LSPD_OPT_TypeDef */
    #define cSPIN_DC_CONF_PARAM_LSPD_BIT {cSPIN_LSPD_OPT_OFF}

	/* Register : K_THERM */
	/* Thermal compensation param, range 1 to 1.46875 */
    #define cSPIN_DC_CONF_PARAM_K_THERM {1}

	/* Register : INT_SPEED */
	/* Intersect speed settings for BEMF compensation in steps/s, range 0 to 3906 steps/s */
    #define cSPIN_DC_CONF_PARAM_INT_SPD {61.512}

	/* Register : ST_SLP */
	/* BEMF start slope settings for BEMF compensation in % step/s, range 0 to 0.4% s/step */
    #define cSPIN_DC_CONF_PARAM_ST_SLP {0.03815}

	/* Register : FN_SLP_ACC */
	/* BEMF final acc slope settings for BEMF compensation in % step/s, range 0 to 0.4% s/step  */
    #define cSPIN_DC_CONF_PARAM_FN_SLP_ACC {0.06256}

	/* Register : FN_SLP_DEC */
	/* BEMF final dec slope settings for BEMF compensation in % step/s, range 0 to 0.4% s/step */
    #define cSPIN_DC_CONF_PARAM_FN_SLP_DEC {0.06256}

	/* Register : CONFIG - field : F_PWM_INT */
	/* PWM Frequency Integer division, enum cSPIN_CONFIG_F_PWM_INT_TypeDef */
    #define cSPIN_DC_CONF_PARAM_PWM_DIV {cSPIN_CONFIG_PWM_DIV_2}

	/* Register : CONFIG - field : F_PWM_DEC */
	/* PWM Frequency Integer Multiplier, enum cSPIN_CONFIG_F_PWM_INT_TypeDef */
    #define cSPIN_DC_CONF_PARAM_PWM_MUL {cSPIN_CONFIG_PWM_MUL_1}

/******************************* Gate Driving **********************************/
	/* Register : GATECFG1 - field : TBOOST */
        /* Duration of the overboost phase during gate turn-off via enum cSPIN_TBOOST_TypeDef */
    #define cSPIN_DC_CONF_PARAM_TBOOST {cSPIN_TBOOST_0ns}

	/* Register : GATECFG1 - field : IGATE */
        /* Gate sink/source current via enum cSPIN_IGATE_TypeDef */
    #define cSPIN_DC_CONF_PARAM_IGATE {cSPIN_IGATE_4mA}

	/* Register : GATECFG1 - field : TCC */
        /* Controlled current time via enum cSPIN_TCC_TypeDef */
    #define cSPIN_DC_CONF_PARAM_TCC {cSPIN_TCC_125ns}

	/* Register : GATECFG2 - field : TBLANK */
        /* Duration of the blanking time via enum cSPIN_TBLANK_TypeDef */
    #define cSPIN_DC_CONF_PARAM_TBLANK {cSPIN_TBLANK_125ns}

	/* Register : GATECFG2 - field : TDT */
        /* Duration of the dead time via enum cSPIN_TDT_TypeDef */
    #define cSPIN_DC_CONF_PARAM_TDT {cSPIN_TDT_125ns}

	/* Register : CONFIG - field : VCCVAL */
        /* VCC Val, enum cSPIN_CONFIG_VCCVAL_TypeDef */
    #define cSPIN_DC_CONF_PARAM_VCCVAL {cSPIN_CONFIG_VCCVAL_7_5V}

	/* Register : CONFIG - field : UVLOVAL */	
        /* UVLO Threshold via cSPIN_CONFIG_UVLOVAL_TypeDef */
    #define cSPIN_DC_CONF_PARAM_UVLOVAL {cSPIN_CONFIG_UVLOVAL_LOW}

/******************************* Others *************************************/
	/* Register : OCD_TH */
        /* Overcurrent threshold settings via enum cSPIN_OCD_TH_TypeDef*/
    #define cSPIN_DC_CONF_PARAM_OCD_TH {cSPIN_OCD_TH_281_25mV}

	/* Register : STALL_TH */
	/* Stall threshold settings in mV, range 31.25mV to 1000mV */
    #define cSPIN_DC_CONF_PARAM_STALL_TH {531.25}

	/* Register : ALARM_EN */
	/* Alarm settings via bitmap enum cSPIN_ALARM_EN_TypeDef */
    #define cSPIN_DC_CONF_PARAM_ALARM_EN {cSPIN_ALARM_EN_OVERCURRENT | cSPIN_ALARM_EN_THERMAL_SHUTDOWN | cSPIN_ALARM_EN_THERMAL_WARNING | cSPIN_ALARM_EN_UVLO | cSPIN_ALARM_EN_ADC_UVLO | cSPIN_ALARM_EN_STALL_DETECTION | cSPIN_ALARM_EN_SW_TURN_ON | cSPIN_ALARM_EN_WRONG_NPERF_CMD}

	/* Register : STEP_MODE - field : STEP_MODE */
        /* Step mode settings via enum cSPIN_STEP_SEL_TypeDef */
    #define cSPIN_DC_CONF_PARAM_STEP_MODE {cSPIN_STEP_SEL_1_128}

	/* Register : STEP_MODE - Field : SYNC_MODE and SYNC_EN */
    /* Synch. Mode settings via enum cSPIN_SYNC_SEL_TypeDef */
    #define cSPIN_DC_CONF_PARAM_SYNC_MODE {cSPIN_SYNC_SEL_DISABLED}

	/* Register : GATECFG1 - field : WD_EN */
	/* External clock watchdog, enum cSPIN_WD_EN_TypeDef */
    #define cSPIN_DC_CONF_PARAM_WD_EN {cSPIN_WD_EN_DISABLE}

	/* Register : CONFIG - field : OC_SD */
        /* Over current shutwdown enabling, enum cSPIN_CONFIG_OC_SD_TypeDef */
    #define cSPIN_DC_CONF_PARAM_OC_SD {cSPIN_CONFIG_OC_SD_DISABLE}

	/* Register : CONFIG - field : SW_MODE */
        /* External switch hard stop interrupt mode, enum cSPIN_CONFIG_SW_MODE_TypeDef */
    #define cSPIN_DC_CONF_PARAM_SW_MODE {cSPIN_CONFIG_SW_HARD_STOP}

	/* Register : CONFIG - field : OSC_CLK_SEL */
        /* Clock setting , enum cSPIN_CONFIG_OSC_MGMT_TypeDef */
    #define cSPIN_DC_CONF_PARAM_CLOCK_SETTING {cSPIN_CONFIG_INT_16MHZ_OSCOUT_2MHZ}

  
/* Exported types ------------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */

  /****************************************************************************/
  /******************No Daisy Chain Mode **************************************/
  /****************************************************************************/

/* Exported constants --------------------------------------------------------*/

  /**************************** Speed Profile *********************************/
	/* Register : ACC */
        /* Acceleration rate in step/s2, range 14.55 to 59590 steps/s2 */
    #define cSPIN_CONF_PARAM_ACC (2008.16)

	/* Register : DEC */
        /* Deceleration rate in step/s2, range 14.55 to 59590 steps/s2 */
    #define cSPIN_CONF_PARAM_DEC (2008.16)

	/* Register : MAX_SPEED */
        /* Maximum speed in step/s, range 15.25 to 15610 steps/s */
    #define cSPIN_CONF_PARAM_MAX_SPEED (991.82)			/*991.82*/

	/* Register : MIN_SPEED */
        /* Minimum speed in step/s, range 0 to 976.3 steps/s */
    #define cSPIN_CONF_PARAM_MIN_SPEED (0)

	/* Register : FS_SPD */
        /* Full step speed in step/s, range 7.63 to 15625 steps/s */
    #define cSPIN_CONF_PARAM_FS_SPD (595.09)		/*595.09*/
  
  /************************ Phase Current Control *****************************/
  /* Register : KVAL_HOLD */
	/* Hold duty cycle (torque) in %, range 0 to 99.6% */
    #define cSPIN_CONF_PARAM_KVAL_HOLD (8)

	/* Register : KVAL_RUN */
	/* Run duty cycle (torque) in %, range 0 to 99.6% */
    #define cSPIN_CONF_PARAM_KVAL_RUN (8)

	/* Register : KVAL_ACC */
  /* Acceleration duty cycle (torque) in %, range 0 to 99.6% */
    #define cSPIN_CONF_PARAM_KVAL_ACC (8)

	/* Register : KVAL_DEC */
	/* Deceleration duty cycle (torque) in %, range 0 to 99.6% */
    #define cSPIN_CONF_PARAM_KVAL_DEC (8)		/*16.02*/

	/* Register : CONFIG - field : EN_VSCOMP */
	/* Motor Supply Voltage Compensation enabling , enum cSPIN_CONFIG_EN_VSCOMP_TypeDef */
    #define cSPIN_CONF_PARAM_VS_COMP (cSPIN_CONFIG_VS_COMP_DISABLE)

	/* Register : MIN_SPEED - field : LSPD_OPT */
	/* Low speed optimization bit, enum cSPIN_LSPD_OPT_TypeDef */
    #define cSPIN_CONF_PARAM_LSPD_BIT (cSPIN_LSPD_OPT_OFF)

	/* Register : K_THERM */
	/* Thermal compensation param, range 1 to 1.46875 */
    #define cSPIN_CONF_PARAM_K_THERM (1)

	/* Register : INT_SPEED */
	/* Intersect speed settings for BEMF compensation in steps/s, range 0 to 3906 steps/s */
    #define cSPIN_CONF_PARAM_INT_SPD (61.512)

	/* Register : ST_SLP */
	/* BEMF start slope settings for BEMF compensation in % step/s, range 0 to 0.4% s/step */
    #define cSPIN_CONF_PARAM_ST_SLP (0.03815)

	/* Register : FN_SLP_ACC */
	/* BEMF final acc slope settings for BEMF compensation in % step/s, range 0 to 0.4% s/step  */
    #define cSPIN_CONF_PARAM_FN_SLP_ACC (0.06256)

	/* Register : FN_SLP_DEC */
	/* BEMF final dec slope settings for BEMF compensation in % step/s, range 0 to 0.4% s/step */
    #define cSPIN_CONF_PARAM_FN_SLP_DEC (0.06256)

	/* Register : CONFIG - field : F_PWM_INT */
	/* PWM Frequency Integer division, enum cSPIN_CONFIG_F_PWM_INT_TypeDef */
    #define cSPIN_CONF_PARAM_PWM_DIV (cSPIN_CONFIG_PWM_DIV_2)

	/* Register : CONFIG - field : F_PWM_DEC */
	/* PWM Frequency Integer Multiplier, enum cSPIN_CONFIG_F_PWM_INT_TypeDef */
    #define cSPIN_CONF_PARAM_PWM_MUL (cSPIN_CONFIG_PWM_MUL_1)

/******************************* Gate Driving **********************************/
	/* Register : GATECFG1 - field : TBOOST */
        /* Duration of the overboost phase during gate turn-off via enum cSPIN_TBOOST_TypeDef */
    #define cSPIN_CONF_PARAM_TBOOST     (cSPIN_TBOOST_0ns)		//

	/* Register : GATECFG1 - field : IGATE */
        /* Gate sink/source current via enum cSPIN_IGATE_TypeDef */
    #define cSPIN_CONF_PARAM_IGATE      (cSPIN_IGATE_4mA)

	/* Register : GATECFG1 - field : TCC */
        /* Controlled current time via enum cSPIN_TCC_TypeDef */
    #define cSPIN_CONF_PARAM_TCC        (cSPIN_TCC_125ns)

	/* Register : GATECFG2 - field : TBLANK */
        /* Duration of the blanking time via enum cSPIN_TBLANK_TypeDef */
    #define cSPIN_CONF_PARAM_TBLANK     (cSPIN_TBLANK_125ns)

	/* Register : GATECFG2 - field : TDT */
        /* Duration of the dead time via enum cSPIN_TDT_TypeDef */
    #define cSPIN_CONF_PARAM_TDT        (cSPIN_TDT_125ns)

	/* Register : CONFIG - field : VCCVAL */
        /* VCC Val, enum cSPIN_CONFIG_VCCVAL_TypeDef */
    #define cSPIN_CONF_PARAM_VCCVAL (cSPIN_CONFIG_VCCVAL_7_5V)

	/* Register : CONFIG - field : UVLOVAL */
        /* UVLO Threshold via cSPIN_CONFIG_UVLOVAL_TypeDef */
    #define cSPIN_CONF_PARAM_UVLOVAL    (cSPIN_CONFIG_UVLOVAL_LOW)

  /******************************* Others *************************************/
	/* Register : OCD_TH */
        /* Overcurrent threshold settings via enum cSPIN_OCD_TH_TypeDef*/
    #define cSPIN_CONF_PARAM_OCD_TH (cSPIN_OCD_TH_281_25mV)		//cSPIN_OCD_TH_281_25mV

	/* Register : STALL_TH */
	/* Stall threshold settings in mV, range 31.25mV to 1000mV */
    #define cSPIN_CONF_PARAM_STALL_TH (800)	//531.25 

	/* Register : ALARM_EN */
	/* Alarm settings via bitmap enum cSPIN_ALARM_EN_TypeDef */
    #define cSPIN_CONF_PARAM_ALARM_EN (cSPIN_ALARM_EN_OVERCURRENT | cSPIN_ALARM_EN_THERMAL_SHUTDOWN | cSPIN_ALARM_EN_THERMAL_WARNING | cSPIN_ALARM_EN_UVLO | cSPIN_ALARM_EN_ADC_UVLO | cSPIN_ALARM_EN_STALL_DETECTION | cSPIN_ALARM_EN_SW_TURN_ON | cSPIN_ALARM_EN_WRONG_NPERF_CMD)

	/* Register : STEP_MODE - field : STEP_MODE */
        /* Step mode settings via enum cSPIN_STEP_SEL_TypeDef */
    #define cSPIN_CONF_PARAM_STEP_MODE (cSPIN_STEP_SEL_1_32)

	/* Register : STEP_MODE - Field : SYNC_MODE and SYNC_EN */
    /* Synch. Mode settings via enum cSPIN_SYNC_SEL_TypeDef */
    #define cSPIN_CONF_PARAM_SYNC_MODE (cSPIN_SYNC_SEL_DISABLED)

	/* Register : GATECFG1 - field : WD_EN */
    /* External clock watchdog, enum cSPIN_WD_EN_TypeDef */
    #define cSPIN_CONF_PARAM_WD_EN (cSPIN_WD_EN_DISABLE)

	/* Register : CONFIG - field : OC_SD */
        /* Over current shutwdown enabling, enum cSPIN_CONFIG_OC_SD_TypeDef */
    #define cSPIN_CONF_PARAM_OC_SD (cSPIN_CONFIG_OC_SD_DISABLE)

	/* Register : CONFIG - field : SW_MODE */
        /* External switch hard stop interrupt mode, enum cSPIN_CONFIG_SW_MODE_TypeDef */
    #define cSPIN_CONF_PARAM_SW_MODE (cSPIN_CONFIG_SW_HARD_STOP)

	/* Register : CONFIG - field : OSC_CLK_SEL */
        /* Clock setting , enum cSPIN_CONFIG_OSC_MGMT_TypeDef */
    #define cSPIN_CONF_PARAM_CLOCK_SETTING (cSPIN_CONFIG_INT_16MHZ_OSCOUT_16MHZ)

  
/* Exported types ------------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */
#endif /* __CSPIN_CONFIG_H */

/******************* (C) COPYRIGHT 2013 STMicroelectronics *****END OF FILE****/
