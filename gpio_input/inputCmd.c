/******************** (C) COPYRIGHT 2015 INCUBECN *****************************
* File Name          : inputCmd.c
* Author             : Tiko Zhong
* Date First Issued  : 12/01/2015
* Description        : This file provides a set of functions needed to manage the
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "inputCmd.h"
#include "input.h"
#include "string.h"
#include "stdio.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

typedef enum{
    INPUT_EVENT_FALLING    =    0,
    INPUT_EVENT_RAISING    =    1,
} INPUT_EVENT_T;


/* Private variables ---------------------------------------------------------*/
const char INPUT_HELP[] = {
    "input command: "
    "\n %brd.%dev.help()"
    "\n %brd.%dev.readPin()/(indx)/(indx0,indx1)"
    "\n %brd.%dev.falling 0x%x"
    "\n %brd.%dev.raising 0x%x"
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
u8 inputCmdU8(void *dev, u8* CMD, u8 len, void (*xprint)(const char* FORMAT_ORG, ...)){
    return(inputCmd(dev,(char*)CMD,g_boardAddr,xprint));
}

u8 inputCmd(void *p, char* CMD, u8 brdAddr, void (*xprint)(const char* FORMAT_ORG, ...)){
    s32 i,j,ii;
    const char* line;
    INPUT_DEV_T* dev = p;
    INPUT_RSRC_T *pRsrc = &dev->rsrc;

    if(strncmp(CMD, pRsrc->name, strlen(pRsrc->name)) != 0)    return 0;
    line = &CMD[strlen(pRsrc->name)];

    //.help()
    if(strncmp(line, ".help", strlen(".help")) == 0){
        xprint("+ok@%d.%s.help()\r\n%s", brdAddr, pRsrc->name, INPUT_HELP);
        return 1;
    }

    //.readPin(indx0,indx1)
    else if(sscanf(line, ".readpin %d %d", &i, &j)==2){
        if(i>=dev->rsrc.gpioLen || j>=dev->rsrc.gpioLen){
            xprint("+err@%d.%s.readpin(\"0..%d\")\r\n", brdAddr, pRsrc->name, pRsrc->gpioLen-1);
            return 1;
        }
        ii = pRsrc->status[0];
        xprint("+ok@%d.%s.readpin(%d,%d,%d,%d)\r\n", brdAddr, pRsrc->name, i, ((ii&BIT(i))?1:0), j, ((ii&BIT(j))?1:0));
        return 1;
    }
    //.readPin(indx)
    else if(sscanf(line, ".readpin %d", &i)==1){
        if(i>=dev->rsrc.gpioLen){
            xprint("+err@%d.%s.readpin(\"0..%d\")\r\n", brdAddr, pRsrc->name, pRsrc->gpioLen-1);
            return 1;
        }
        ii = pRsrc->status[0];
        xprint("+ok@%d.%s.readpin(%d,%d)\r\n", brdAddr, pRsrc->name, i, ((ii&BIT(i))?1:0));
        return 1;
    }
    //.readPin()
    else if(strncmp(line, ".readpin", strlen("readpin")) == 0){
        xprint("+ok@%d.%s.readpin(0x%08x)\r\n", brdAddr, pRsrc->name, pRsrc->status[0]);
        return 1;
    }

    //.enableFalling(indx)"
    else if(sscanf(line, ".falling 0x%x", &i)==1){
        pRsrc->enableFalling = i;
        xprint("+ok@%d.%s.falling(0x%08x)\r\n", brdAddr, pRsrc->name, pRsrc->enableFalling);
        return 1;
    }
    //.enableFalling()"
//    else if(strncmp(line, ".enablefalling", strlen(".enablefalling")) == 0){
//        pRsrc->enableFalling = 0xffffffff;
//        xprint("+ok@%d.%s.enablefalling(0x%08x)\r\n", brdAddr, pRsrc->name, pRsrc->enableFalling);
//        return 1;
//    }
    //.disableFalling(indx)
//    else if(sscanf(line, ".disablefalling %d", &i)==1){
//        if(i>=dev->rsrc.gpioLen){
//            xprint("+err@%d.%s.disablefalling(\"0..%d\")\r\n", brdAddr, pRsrc->name, pRsrc->gpioLen);
//            return 1;
//        }
//        pRsrc->enableFalling &= (0xffffffff ^ BIT(i));
//        xprint("+ok@%d.%s.disablefalling(0x%08x)\r\n", brdAddr, pRsrc->name, pRsrc->enableFalling);
//        return 1;
//    }
//    //.disableFalling()
//    else if(strncmp(line, ".disablefalling", strlen(".disablefalling")) == 0){
//        pRsrc->enableFalling = 0;
//        xprint("+ok@%d.%s.disablefalling(0x%08x)\r\n", brdAddr, pRsrc->name, pRsrc->enableFalling);
//        return 1;
//    }    
    //.enableRaising(indx)
    else if(sscanf(line, ".raising 0x%x", &i)==1){
        pRsrc->enableRaising = i;
        xprint("+ok@%d.%s.raising(0x%08x)\r\n", brdAddr, pRsrc->name, pRsrc->enableRaising);
        return 1;
    }
    //.enableRaising()
//    else if(strncmp(line, ".enableraising", strlen(".enableraising")) == 0){
//        pRsrc->enableRaising = 0xffffffff;
//        xprint("+ok@%d.%s.enableraising(0x%08x)\r\n", brdAddr, pRsrc->name, pRsrc->enableRaising);
//        return 1;
//    }    
//    //.disableRaising(indx)
//    else if(sscanf(line, ".disablefalling %d", &i)==1){
//        if(i>=dev->rsrc.gpioLen){
//            xprint("+err@%d.%s.disablefalling(\"0..%d\")\r\n", brdAddr, pRsrc->name, pRsrc->gpioLen);
//            return 1;
//        }
//        pRsrc->enableRaising &= (0xffffffff ^ BIT(i));
//        xprint("+ok@%d.%s.disableraising(0x%08x)\r\n", brdAddr, pRsrc->name, pRsrc->enableRaising);
//        return 1;
//    }
    //.disableRaising()
//    else if(strncmp(line, ".disableraising", strlen(".disableraising")) == 0){
//        pRsrc->enableRaising = 0;
//        xprint("+ok@%d.%s.disableraising(0x%08x)\r\n", brdAddr, pRsrc->name, pRsrc->enableRaising);
//        return 1;
//    }    
    else{
        xprint("+unknown@%s", CMD);
        return 1;
    }
//    return 0;
}

/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
