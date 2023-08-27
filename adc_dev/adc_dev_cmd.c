/******************** (C) COPYRIGHT 2015 INCUBECN *****************************
* File Name          : adc_dev_cmd.c
* Author             : Tiko Zhong
* Description        : This file provides a set of functions needed to manage the
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "adc_dev_cmd.h"
#include "adc_dev.h"
#include "string.h"
#include "stdio.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
const char ADC_DEV_HELP[] = {
    "adc command: "
    "\n %B.%D.help()"
    "\n %B.%D.read()/(ch)"
    "\n %B.%D.read_detail(ch)"
    "\n %B.%D.start(ms)"
    "\n %B.%D.stop"
    "\n %B.%D.cal_offset(ch)"
    "\n %B.%D.cal_gain(ch,scale)"
    "\n %B.%D.cal_save()"
    "\n"
};

/* Private function prototypes -----------------------------------------------*/

/*******************************************************************************
* Function Name  : inputCmd
* Description    : gpio function commands
* Input          : 
                                    : 
* Output         : None
* Return         : None 
*******************************************************************************/
u8 adc_dev_cmd(void *p, u8* cmd, u8 len, void (*xprint)(const char* FORMAT_ORG, ...)){
    char* CMD = (char*)cmd;
    adcDev_T* d = (adcDev_T*)p;
    u8 brdAddr = g_boardAddr;
    u8 buff[32] = {0};
    
    s32 i,j,ii, x[32];
    s16 val;
    const char* line = CMD;

    if(strncmp(CMD, "adc.help", strlen("adc.help")) == 0){
        xprint(ADC_DEV_HELP);
        return 1;
    }
    else if(strncmp(CMD, "flsh.print", strlen("flsh.print")) == 0){
        return 1;
    }
    else if(sscanf(line, "adc.read %d", &i)==1){
        if(d->read(&d->rsrc, i, &val)<0){
            xprint("+err@adc.read %d\r\n",i);
        }
        else{
            xprint("+ok@adc.read %d %d\r\n",i,val);
        }
        return 1;
    }
    else if(sscanf(line, "adc.readraw %d", &i)==1){
        if(d->readRaw(&d->rsrc, i, &val)<0){
            xprint("+err@adc.readraw %d\r\n",i);
        }
        else{
            xprint("+ok@adc.readraw %d %d\r\n",i,val);
        }
        return 1;
    }    
    
    else if(sscanf(line, "adc.start %d", &i)==1){
        d->start(&d->rsrc,i);
        xprint("+ok@adc.start %d\r\n",i);
        return 1;
    }
    else if(strncmp(CMD, "adc.stop", strlen("adc.stop")) == 0){
        d->stop(&d->rsrc);
        xprint("+ok@adc.stop\r\n");
        return 1;
    }
    else if(sscanf(line, "adc.cal_offset %d", &i)==1){
        d->cal_offset(&d->rsrc,i);
        xprint("+ok@adc.cal_offset %d\r\n", i);
        return 1;
    }
    else if(sscanf(line, "adc.cal_gain %d %d", &i, &j)==2){
        d->cal_gain(&d->rsrc,i,j);
         xprint("+ok@adc.cal_gain %d %j\r\n", i,j);
        return 1;
    }
    else if(strncmp(CMD, "adc.cal_save", strlen("adc.cal_save")) == 0){
        d->cal_save(&d->rsrc);
         xprint("+ok@adc.cal_save\r\n");
        return 1;
    }
    else if(strncmp(CMD, "adc.cal_reset", strlen("adc.cal_reset")) == 0){
        d->cal_reset(&d->rsrc);
         xprint("+ok@adc.cal_reset\r\n");
        return 1;
    }
    else if(strncmp(CMD, "adc.cal_info", strlen("adc.cal_info")) == 0){
        for(i=0; i<ADC_CH_COUNT; i++){
            xprint("ch[%d]\tofs:%d mul:%d div:%d\n",i,
                d->rsrc.cal[i].offset,
                d->rsrc.cal[i].gainMul,
                d->rsrc.cal[i].gainDiv
            );
        }
        xprint("+ok@adc.cal_info\r\n");  
        return 1;
    }
    else if(strncmp(CMD, "adc.info", strlen("adc.info")) == 0){
        for(i=0; i<ADC_CH_COUNT; i++){
            xprint("ch[%d]:",i);
            for(j=0;j<ADC_BUFF_LEN;j++){
                xprint("%d ",d->rsrc.valSeries[i][j]);
            }
            xprint("\n",i);
        }
        xprint("+ok@adc.info\r\n");
        return 1;
    }
//    else{
//        xprint("+unknown@%s", CMD);
//        return 1;
//    }
    return 0;
}

/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
