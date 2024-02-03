/**********************************************************
filename: vRegulator.c
**********************************************************/

#include "vRegulator.h"
#include "misc.h"
#include "string.h"
#include "user_log.h"
#include "crc16.h"

// #include "board.h"

static s32 dcvTurnOn(dcvRsrc_t* p);
static s32 dcvTurnOff(dcvRsrc_t* p);
static s8 dcvConfigRead(dcvRsrc_t* p);
static s8 dcvConfigWrite(dcvRsrc_t* p);

static u16 dcvVolt2Duty(dcvRsrc_t* r, u16 mv);
static u16 dcvVolt2Adc(dcvRsrc_t* r, u16 mv);
static u16 dcvAdc2Volt(dcvRsrc_t* r, u16 adc);
static u16 dcvGetADC(dcvRsrc_t* r);
static u16 dcvGetVolt(dcvRsrc_t* r);
static s32 dcvSetVolt(dcvRsrc_t* r, u16 x_mv);
static s32 dcvIncVolt(dcvRsrc_t* r, u16 mv);
static s32 dcvDecVolt(dcvRsrc_t* r, u16 mv);
static void dcvCallibrate(dcvRsrc_t* r, u32 true_uv);
static void dcvDefault(dcvRsrc_t* r);
static void dcvConfig(dcvRsrc_t* r, u16 limit, u16 poweron, u8 ispoweron);

void dcvDevSetup(dcvDev_t* pDev,
	const char* NAME,
	TIM_HandleTypeDef* vTimHandle,
	u32 vTimChannel,
	ADC_HandleTypeDef* vAdcHandle,
	u32 vAdcChannel,
    TIM_HandleTypeDef* iTimHandle,
	u32 iTimChannel,
    ADC_HandleTypeDef* iAdcHandle,
	u32 iAdcChannel,
	u16 ioBase,
    s32 (*ioWrite)(u16 addr, const u8 *pDat, u16 nBytes),
	s32 (*ioRead)(u16 addr, u8 *pDat, u16 nBytes)
){
	dcvRsrc_t* r = &pDev->rsrc;

	memset(r,0,sizeof(dcvRsrc_t));
	strcpy(r->name, NAME);

	r->vTimHandle = vTimHandle;
	r->vTimChannel = vTimChannel;
    r->vAdcHandle = vAdcHandle;
    r->vAdcChannel = vAdcChannel;
    
	r->iTimHandle = iTimHandle;
	r->iTimChannel = iTimChannel;
    r->iAdcHandle = iAdcHandle;
    r->iAdcChannel = iAdcChannel;
   
	r->ioBase = ioBase;
	r->ioWrite = ioWrite;
	r->ioRead = ioRead;
    
	if(dcvConfigRead(r) <0 ){
		r->calVolt_uv = 22960000;	//in uv
		r->calAdc = 3600;
		r->calDuty = 3600;			//while Duty = 3600, vout = 22.96V
		r->limit_mv = 24000;
		r->powerOn_mv = 5000;
		r->isPowerOn = 0;
		dcvConfigWrite(r);
		// print("%s.config error\r\n", r->name);
	}

	//register op
	pDev->decVoltBy = dcvDecVolt;
	pDev->incVoltBy = dcvIncVolt;
	pDev->setVolt = dcvSetVolt;
	pDev->getVolt = dcvGetVolt;
	pDev->getADC = dcvGetADC;
	pDev->callibrate_uv = dcvCallibrate;
	pDev->defaultx = dcvDefault;
	pDev->turnOn = dcvTurnOn;
	pDev->turnOff = dcvTurnOff;
	pDev->config = dcvConfig;

	__HAL_TIM_SET_COMPARE(vTimHandle, vTimChannel, 0);
	HAL_TIM_PWM_Start(vTimHandle, vTimChannel);
    
	__HAL_TIM_SET_COMPARE(iTimHandle, iTimChannel, 2047);
	HAL_TIM_PWM_Start(iTimHandle, iTimChannel);

	r->prvVolt = 0;
	if(r->isPowerOn){
		r->prvVolt = r->powerOn_mv;
		dcvSetVolt(r, r->powerOn_mv);
	}
	else{	dcvTurnOff(r);	}

}

static s32 dcvTurnOn(dcvRsrc_t* p){
	// print("prvVolt: %d\n", p->prvVolt);
	return (dcvSetVolt(p, p->prvVolt));
}

static s32 dcvTurnOff(dcvRsrc_t* p){
	if(p->curVolt){
		p->prvVolt = p->curVolt;
	}
	return (dcvSetVolt(p, 0));
}

static s8 dcvConfigWrite(dcvRsrc_t* r){
    s32 ret = 0;
    u16 computedCRC;
    u8* buff = (u8*)r->cal;
    computedCRC = CRC16(buff, sizeof(CAL32_T)*2*4, CRC_INIT);
    ret += r->ioWrite(r->ioBase+0, (u8*)&computedCRC, 2);
    ret += r->ioWrite(r->ioBase+2, buff, sizeof(CAL32_T)*2*4);
    return ret;
}

static s8 dcvConfigRead(dcvRsrc_t* r){
    s32 i,j;
    u16 romCRC = 1, computedCRC = 2;
    u8* buff;
    
    // read from EEPROM
    if((r->ioRead!=NULL) && (r->ioWrite!=NULL)){
        r->ioRead(r->ioBase+0, (u8*)&romCRC, 2);
        buff = (u8*)r->cal;
        r->ioRead(r->ioBase+2, buff, sizeof(CAL32_T)*2*4);
        computedCRC = CRC16(buff, sizeof(CAL32_T)*2*4, CRC_INIT);
        if(computedCRC != romCRC){
            log("<%s romCRC:0x%02x >", __func__, romCRC);
            log("<%s computedCRC:0x%02x >", __func__, computedCRC);
            for(i=0;i<2;i++){
                for(j=0;j<4;j++){
                    r->cal[i][j].offset = 0;
                    r->cal[i][j].gainDiv = 1;
                    r->cal[i][j].gainMul = 1;
                }
            }
            dcvConfigWrite(r);
        }
    }
    else{
        // initial cal
        for(i=0;i<2;i++){
            for(j=0;j<4;j++){
                r->cal[i][j].offset = 0;
                r->cal[i][j].gainDiv = 1;
                r->cal[i][j].gainMul = 1;
            }
        }
    }
    return -1;
}

static u16 dcvVolt2Duty(dcvRsrc_t* r, u16 mv){
	return (mv*1000/(r->calVolt_uv/r->calDuty));
}

static u16 dcvVolt2Adc(dcvRsrc_t* r, u16 mv){
	return (mv*1000/(r->calVolt_uv/r->calAdc));
}

static u16 dcvAdc2Volt(dcvRsrc_t* r, u16 adc){
	u32 uv = adc;
	uv = uv*(r->calVolt_uv/r->calAdc);
	return uv/1000;
}

static u16 dcvGetADC(dcvRsrc_t* r){
	u16 rslt=0, i;
	ADC_ChannelConfTypeDef sConfig = {0};

	sConfig.Channel = r->vAdcChannel;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLETIME_79CYCLES_5;
	if (HAL_ADC_ConfigChannel(r->vAdcHandle, &sConfig) != HAL_OK){
        log("err@dcrGetADC 001 %s", r->name);	
        return -1;
    }

	for(i=0;i<4;i++){
		HAL_ADC_Start(r->vAdcHandle);
		HAL_ADC_PollForConversion(r->vAdcHandle, 2);
		rslt += HAL_ADC_GetValue(r->vAdcHandle);
		HAL_ADC_Stop(r->vAdcHandle);
	}
	rslt >>= 2;

	return rslt;
}

static u16 dcvGetVolt(dcvRsrc_t* r){
	u32 x = dcvGetADC(r);
	x = dcvAdc2Volt(r, x);
	return(x);
}

static s32 dcvSetVolt(dcvRsrc_t* r, u16 x_mv){
	u16 duty;
	if(r->limit_mv>0 && x_mv>r->limit_mv){	return -1;	}
	duty = dcvVolt2Duty(r, x_mv);
	__HAL_TIM_SET_COMPARE(r->vTimHandle, r->vTimChannel, duty);
	r->pwmDuty = duty;
	r->prvVolt = r->curVolt;
	r->curVolt = x_mv;
	return x_mv;
}

static s32 dcvIncVolt(dcvRsrc_t* r, u16 mv){
	return (dcvSetVolt(r, r->curVolt+mv));
}

static s32 dcvDecVolt(dcvRsrc_t* r, u16 mv){
	if(r->curVolt <= mv){	return (dcvSetVolt(r, 0));	}
	else	return (dcvSetVolt(r, r->curVolt-mv));
}

static void dcvCallibrate(dcvRsrc_t* r, u32 true_uv){
	r->calDuty = r->pwmDuty;
	r->calVolt_uv = true_uv;
	r->calAdc = dcvGetADC(r);
	dcvConfigWrite(r);
}

static void dcvDefault(dcvRsrc_t* r){
	r->calDuty = 3600;
	r->calVolt_uv = 22960000;
	r->calAdc = 3600;
	r->limit_mv = 24000;
	r->powerOn_mv = 5000;
	r->isPowerOn = 0;
	dcvConfigWrite(r);
}

static void dcvConfig(dcvRsrc_t* r, u16 limit, u16 poweron, u8 ispoweron){
	r->limit_mv = limit;
	r->powerOn_mv = poweron;
	r->isPowerOn = ispoweron;
	dcvConfigWrite(r);
}


/**********************************************************
 == THE END ==
**********************************************************/
