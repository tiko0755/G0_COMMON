/******************** (C) COPYRIGHT 2015 INCUBECN *****************************
* File Name          : stm_flash_cmd.c
* Author             : Tiko Zhong
* Description        : This file provides a set of functions needed to manage the
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm_flash_cmd.h"
#include "stm_flash.h"
#include "string.h"
#include "stdio.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
const char STM_FLSH_HELP[] = {
    "STM_FLAH command: "
    "\n %B.flsh.help()"
    "\n %B.flsh.print"
    "\n %B.flsh.read addr len"
    "\n %B.flsh.write addr x0 x1 x2 x3 x4 x5 x6 x7"
    "\n %B.flsh.write addr x0 x1 x2 x3 x4 x5 x6"
    "\n %B.flsh.write addr x0"
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
u8 stmFlsh_Cmd(void *p, u8* cmd, u8 len, void (*xprint)(const char* FORMAT_ORG, ...)){
    char* CMD = (char*)cmd;
    u8 brdAddr = g_boardAddr;
    u8 buff[32] = {0};
    
    s32 i,j,ii, x[32];
    const char* line = CMD;

    if(strncmp(CMD, "flsh.help", strlen("flsh.help")) == 0){
        xprint(STM_FLSH_HELP);
        return 1;
    }
    else if(strncmp(CMD, "flsh.print", strlen("flsh.print")) == 0){
        stmFlsh_print(xprint);
        return 1;
    }
    else if(strncmp(CMD, "flsh.format", strlen("flsh.format")) == 0){
        stmFlsh_format();
        xprint("+ok@flsh.format\r\n");
        return 1;
    }
    else if(sscanf(line, "flsh.read %d %d", &i, &j)==2){
        if(stmFlsh_read(i, buff, (j<=32?j:32)) == 0){
            xprint("+ok@flsh.read %d %d\r\n",i,j);
            for(i=0;i<j;i++){
                xprint("%02x ", buff[i]);
                if(i%16==15){
                    xprint("\n");
                }
            }
            xprint("\n");
        }
        else{
            xprint("+err@flsh.read %d %d\r\n", i,j);
        }
        return 1;
    }
    
    else if(sscanf(line, "flsh.write %d %d %d %d %d %d %d %d %d", &i,&x[0],&x[1],&x[2],&x[3],&x[4],&x[5],&x[6],&x[7])==9){
        for(j=0;j<8;j++){
            buff[j] = x[j];
        }
        if(stmFlsh_write(i, buff, 8) == 0){
            xprint("+ok@flsh.write %d %d %d %d %d %d %d %d %d\r\n", i,x[0],x[1],x[2],x[3],x[4],x[5],x[6],x[7]);
        }
        else{
            xprint("+err@flsh.write %d %d %d %d %d %d %d %d %d\r\n", i,x[0],x[1],x[2],x[3],x[4],x[5],x[6],x[7]);
        }
        return 1;
    }
    else if(sscanf(line, "flsh.write %d %d %d %d %d %d %d %d", &i,&x[0],&x[1],&x[2],&x[3],&x[4],&x[5],&x[6])==8){
        for(j=0;j<7;j++){
            buff[j] = x[j];
        }
        if(stmFlsh_write(i, buff, 7) == 0){
            xprint("+ok@flsh.write %d %d %d %d %d %d %d %d\r\n", i,x[0],x[1],x[2],x[3],x[4],x[5],x[6]);
        }
        else{
            xprint("+err@flsh.write %d %d %d %d %d %d %d %d\r\n", i,x[0],x[1],x[2],x[3],x[4],x[5],x[6]);
        }
        return 1;
    }
    else if(sscanf(line, "flsh.write %d %d %d %d %d %d %d", &i,&x[0],&x[1],&x[2],&x[3],&x[4],&x[5])==7){
        for(j=0;j<6;j++){
            buff[j] = x[j];
        }
        if(stmFlsh_write(i, buff, 6) == 0){
            xprint("+ok@flsh.write %d %d %d %d %d %d %d\r\n", i,x[0],x[1],x[2],x[3],x[4],x[5]);
        }
        else{
            xprint("+err@flsh.write %d %d %d %d %d %d %d\r\n", i,x[0],x[1],x[2],x[3],x[4],x[5]);
        }
        return 1;
    }
    else if(sscanf(line, "flsh.write %d %d %d %d %d %d", &i,&x[0],&x[1],&x[2],&x[3],&x[4])==6){
        for(j=0;j<5;j++){
            buff[j] = x[j];
        }
        if(stmFlsh_write(i, buff, 5) == 0){
            xprint("+ok@flsh.write %d %d %d %d %d %d\r\n", i,x[0],x[1],x[2],x[3],x[4]);
        }
        else{
            xprint("+err@flsh.write %d %d %d %d %d %d\r\n", i,x[0],x[1],x[2],x[3],x[4]);
        }
        return 1;
    }
    else if(sscanf(line, "flsh.write %d %d %d %d %d", &i,&x[0],&x[1],&x[2],&x[3])==5){
        for(j=0;j<4;j++){
            buff[j] = x[j];
        }
        if(stmFlsh_write(i, buff, 4) == 0){
            xprint("+ok@flsh.write %d %d %d %d %d\r\n", i,x[0],x[1],x[2],x[3]);
        }
        else{
            xprint("+err@flsh.write %d %d %d %d %d\r\n", i,x[0],x[1],x[2],x[3]);
        }
        return 1;
    }
    else if(sscanf(line, "flsh.write %d %d %d %d", &i,&x[0],&x[1],&x[2])==4){
        for(j=0;j<3;j++){
            buff[j] = x[j];
        }
        if(stmFlsh_write(i, buff, 3) == 0){
            xprint("+ok@flsh.write %d %d %d %d\r\n", i,x[0],x[1],x[2]);
        }
        else{
            xprint("+err@flsh.write %d %d %d %d\r\n", i,x[0],x[1],x[2]);
        }
        return 1;
    }
    else if(sscanf(line, "flsh.write %d %d %d", &i,&x[0],&x[1])==3){
        for(j=0;j<2;j++){
            buff[j] = x[j];
        }
        if(stmFlsh_write(i, buff, 2) == 0){
            xprint("+ok@flsh.write %d %d %d\r\n", i,x[0],x[1]);
        }
        else{
            xprint("+err@flsh.write %d %d %d\r\n", i,x[0],x[1]);
        }
        return 1;
    }
    else if(sscanf(line, "flsh.write %d %d", &i,&x[0])==2){
        for(j=0;j<1;j++){
            buff[j] = x[j];
        }
        if(stmFlsh_write(i, buff, 1) == 0){
            xprint("+ok@flsh.write %d %d\r\n", i,x[0]);
        }
        else{
            xprint("+err@flsh.write %d %d\r\n", i,x[0]);
        }
        return 1;
    }

//    else{
//        xprint("+unknown@%s", CMD);
//        return 1;
//    }
    return 0;
}

/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
