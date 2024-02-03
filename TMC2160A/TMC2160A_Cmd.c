/******************** (C) COPYRIGHT 2022 tiko *****************************
* File Name          : tmc2160a_cmd.c
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "tmc2160a_cmd.h"
#include "tmc2160a_dev.h"
#include "string.h"
#include "stdio.h"

#include "board.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define FUN_LEN 64
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
const char TMC2160A_HELP[] = {
    "ramp command:"
    "\n %brd.%dev.help()"
    "\n %brd.%dev.info()"
    "\n %brd.%dev.default()"
    "\n %brd.%dev.spd(min,max)/()"
    "\n"
};

/* Private function prototypes -----------------------------------------------*/

/*******************************************************************************
* Function Name  : rampCmd
* Description    : rampCmd function commands
* Input          : - CMD, command line
                 : - brdAddr, board address
                 : - print function
* Output         : None
* Return         : None 
*******************************************************************************/
u8 tmc2160aCmdU8(void *dev, u8* cmd, u8 len, void (*xprint)(const char* FORMAT_ORG, ...)){
    return(tmc2160aCmd(dev,(char*)cmd,g_boardAddr,xprint));
}

u8 tmc2160aCmd(void *dev, char* CMD, u8 brdAddr, void (*xprint)(const char* FORMAT_ORG, ...)){
    s32 i,j;
    const char* line;
    TMC2160A_dev_t *d = dev;
    TMC2160A_rsrc_t *r = &d->rsrc;
        u8 buff[5];

    if(strncmp(CMD, r->name, strlen(r->name)) != 0)    return 0;
    line = &CMD[strlen(r->name)+1];

    //    .stops()
    if(sscanf(line, "regread 0x%x", &i)==1){
            memset(buff,0,5);
            buff[0] = i&0xff;
            d->readWriteArray(r, buff, 5);
            memset(buff,0,5);
            buff[0] = i&0xff;
            d->readWriteArray(r, buff, 5);
            xprint("+ok@%d.%s.regread(0x%02x,0x%02x,0x%02x%02x%02x%02x)\r\n", brdAddr, r->name, i&0xff, 
                buff[0],buff[1],buff[2],buff[3],buff[4]);
    }
    else if(sscanf(line, "regwrite 0x%x 0x%x", &i,&j)==2){
            memset(buff,0,5);
            buff[0] = 0x80|(i&0xff);
            buff[4] = j&0xff;    j>>=8;
            buff[3] = j&0xff;    j>>=8;
            buff[2] = j&0xff;    j>>=8;
            buff[1] = j&0xff;
            d->readWriteArray(r, buff, 5);
            xprint("+ok@%d.%s.regwrite(0x%02x,0x%02x,0x%02x%02x%02x%02x)\r\n", brdAddr, r->name, i&0xff,
                buff[0],buff[1],buff[2],buff[3],buff[4]);
    }
    else if(sscanf(line, "regwrite 0x%x %d", &i,&j)==2){
            memset(buff,0,5);
            buff[0] = 0x80|(i&0xff);
            buff[4] = j&0xff;    j>>=8;
            buff[3] = j&0xff;    j>>=8;
            buff[2] = j&0xff;    j>>=8;
            buff[1] = j&0xff;
            d->readWriteArray(r, buff, 5);
            xprint("+ok@%d.%s.regwrite(0x%02x,0x%02x,0x%02x%02x%02x%02x)\r\n", brdAddr, r->name, i&0xff,
                buff[0],buff[1],buff[2],buff[3],buff[4]);
    }
        
    //.help()
    else if(strncmp(line, "help", strlen("help")) == 0){
        xprint("%s", TMC2160A_HELP);
        xprint("+ok@%d.%s.help()\r\n%s", brdAddr, r->name);
    }

    else{
        xprint("+unknown@%s", CMD);
    }
    return 1;
}

/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
