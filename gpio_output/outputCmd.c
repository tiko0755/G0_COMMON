/******************** (C) COPYRIGHT 2015 INCUBECN *****************************
* File Name          : outputCmd.c
* Author             : Tiko Zhong
* Date First Issued  : 12/01/2015
* Description        : This file provides a set of functions needed to manage the
*                      
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "outputCmd.h"
#include "output.h"
#include "string.h"
#include "stdio.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define FUN_LEN 64
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
const char OUTPUT_HELP[] = {
    "output command:"
    "\n %brd.%dev.help()"
    "\n %brd.%dev.readPin()/(indx)/(indx0,indx1)"
    "\n %brd.%dev.writePin(status)/(indx, b)/(indx0,bit0,indx1,bit1)"
    "\n %brd.%dev.togglePin()/(indx)/(indx0,indx1)"
    "\n"
};

/* Private function prototypes -----------------------------------------------*/

/*******************************************************************************
* Function Name  : outputCmd
* Description    : gpio function commands
* Input          : - huart : using this uart to print
                                    : - cmdStr : the command string
* Output         : None
* Return         : None 

output.writepin(%pin, %state)
output.toggle(%pin)
*******************************************************************************/
u8 outputCmd(void *dev, char* CMD, u8 brdAddr, void (*xprint)(const char* FORMAT_ORG, ...)){
    s32 i,j,ii,jj;
    const char* line;
    OUTPUT_DEV_T *pDev = dev;
    OUTPUT_RSRC_T *pRsrc = &pDev->rsrc;
        
    if(strncmp(CMD, pRsrc->name, strlen(pRsrc->name)) != 0)    return 0;
    line = &CMD[strlen(pRsrc->name)];

    //.help()
    if(strncmp(line, ".help", strlen(".help")) == 0){
        xprint("+ok@%d.%s.help()\r\n%s", brdAddr, pRsrc->name, OUTPUT_HELP);
    }
    //.readPin(indx0,indx1)
    else if(sscanf(line, ".readpin %d %d", &i, &j)==2){
        if(i>=pRsrc->gpioLen || j>=pRsrc->gpioLen){    
            xprint("+err@%d.%s.readpin(\"0..%d\")\r\n", brdAddr, pRsrc->name, pRsrc->gpioLen-1);
            return 1;
        }
        ii = 0;    jj = 0;
        if(pRsrc->status & BIT(i))    ii = 1;
        if(pRsrc->status & BIT(j))    jj = 1;
        xprint("+ok@%d.%s.readpin(%d,%d,%d,%d)\r\n", brdAddr, pRsrc->name, i, ii, j, jj);
    }
    //.readPin(indx)
    else if(sscanf(line, ".readpin %d", &i)==1){
        if(i>=pRsrc->gpioLen){
            xprint("+err@%d.%s.readpin(\"0..%d\")\r\n", brdAddr, pRsrc->name, pRsrc->gpioLen-1);
            return 1;
        }
        ii = 0;    
        if(pRsrc->status & BIT(i))    ii = 1;
        xprint("+ok@%d.%s.readpin(%d,%d)\r\n", brdAddr, pRsrc->name, i, ii);
    }
    //.readPin()
    else if(strncmp(line, ".readpin", strlen(".readpin")) == 0){
        xprint("+ok@%d.%s.readpin(0x%08x)\r\n", brdAddr, pRsrc->name, pRsrc->status);
    }
    
    //.writePin(indx0,bit0,indx1,bit1)
    else if(sscanf(line, ".writepin %d %d %d %d", &i, &j, &ii, &jj)==4){
        if(i>=pRsrc->gpioLen || ii>=pRsrc->gpioLen){    
            xprint("+err@%d.%s.writepin(\"0..%d\")\r\n", brdAddr, pRsrc->name, pRsrc->gpioLen-1);
            return 1;
        }
        if(j)        pDev->WritePin(pRsrc, i, PIN_SET);
        else pDev->WritePin(pRsrc, i, PIN_RESET);
        if(jj)    pDev->WritePin(pRsrc, ii, PIN_SET);
        else pDev->WritePin(pRsrc, ii, PIN_RESET);
        xprint("+ok@%d.%s.writepin(0x%08x)\r\n", brdAddr, pRsrc->name, pRsrc->status);
    }
    //.writePin(indx, b)
    else if(sscanf(line, ".writepin %d %d", &i, &j)==2){
        if(i>=pRsrc->gpioLen){    
            xprint("+err@%d.%s.writepin(\"0..%d\")\r\n", brdAddr, pRsrc->name, pRsrc->gpioLen-1);
            return 1;
        }
        if(j)        pDev->WritePin(pRsrc, i, PIN_SET);
        else pDev->WritePin(pRsrc, i, PIN_RESET);
        xprint("+ok@%d.%s.writepin(0x%08x)\r\n", brdAddr, pRsrc->name, pRsrc->status);
    }
    //.writePin(hex)
    else if(sscanf(line, ".writepin 0x%x", &i)==1){
        pDev->WritePinHEX(pRsrc, i);
        xprint("+ok@%d.%s.writepin(0x%08x)\r\n", brdAddr, pRsrc->name, pRsrc->status);
    }
    //.writePin(dec)
    else if(sscanf(line, ".writepin %d", &i)==1){
        pDev->WritePinHEX(pRsrc, i);
        xprint("+ok@%d.%s.writepin(0x%08x)\r\n", brdAddr, pRsrc->name, pRsrc->status);
    }

    //.togglePin(indx0,indx1)
    else if(sscanf(line, ".togglepin %d %d", &i, &j)==2){
        if(i>=pRsrc->gpioLen || j>=pRsrc->gpioLen){    
            xprint("+err@%d.%s.togglepin(\"0..%d\")\r\n", brdAddr, pRsrc->name, pRsrc->gpioLen-1);
            return 1;
        }
        pDev->TogglePin(pRsrc, i);
        pDev->TogglePin(pRsrc, j);
        xprint("+ok@%d.%s.togglepin(0x%08x)\r\n", brdAddr, pRsrc->name, pRsrc->status);
    }
    //.togglePin(indx)
    else if(sscanf(line, ".togglepin %d", &i)==1){
        if(i>=pRsrc->gpioLen){    
            xprint("+err@%d.%s.togglepin(\"0..%d\")\r\n", brdAddr, pRsrc->name, pRsrc->gpioLen-1);
            return 1;
        }
        pDev->TogglePin(pRsrc, i);
        xprint("+ok@%d.%s.togglepin(0x%08x)\r\n", brdAddr, pRsrc->name, pRsrc->status);
    }
    //.togglePin()
    else if(strncmp(line, ".togglepin", strlen(".togglepin")) == 0){
        for(i=0;i<pRsrc->gpioLen;i++)    pDev->TogglePin(pRsrc, i);
        xprint("+ok@%d.%s.togglepin(0x%08x)\r\n", brdAddr, pRsrc->name, pRsrc->status);
    }
    else{
        xprint("+unknown@%s", CMD);
    }
    return 1;
}

/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
