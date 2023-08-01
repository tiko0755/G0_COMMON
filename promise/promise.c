/******************** (C) COPYRIGHT 2022 INCUBECN *****************************
* File Name          : promise.c
* Author             : Tiko Zhong
* Date First Issued  : 26/oct,2022
* Description        : This file provides a set of functions needed to manage the
*                      stepper ramp generator
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "promise.h"
#include <string.h>
#include <stdio.h>
#include "board.h"
/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#define PROMISE_SYMBOL_END		("\r\n")

#define PROMISE_SQU_IDEL	1
#define PROMISE_SQU_DELAY	20
#define PROMISE_SQU_REQ		30

/* Private function prototypes -----------------------------------------------*/
// take "cmd.reg 'ss aad ad ' errt;"
// if(sscanf(line, "cmd.reg%*[^']'%[^']'%*[^$a-z]%[^;\r\n]", format, label)==2)
static s32 promise_Start(promiseRsrc_t*, SCRIPT_T* SCRIPT);
static void promise_PeriodJOb(promiseRsrc_t*, u8 tick);
//static void promise_Resp(promiseRsrc_t*, const char* );
static s32 promise_Cmd(promiseRsrc_t*, const char* ORG);

static void promise_requestCB(void* x, const char* RCV_ORG);
static void promise_requestSendCB(void* x, const char* MSG);

static s32 script_isLabel(promiseRsrc_t* r, const char* ORG);

//static const char* promiseReadLine(char* buff, u16 len, const char* ORG);
static s32* script_takeREG(promiseRsrc_t* r, const char* REG);
static s32 script_print(promiseRsrc_t* r, const char* ORG);
static s32 script_scanf(promiseRsrc_t* r, const char* ORG);
static s32 script_move(promiseRsrc_t* r, const char* ORG);
static s32 script_req(promiseRsrc_t* r, const char* ORG);
static s32 script_switch(promiseRsrc_t* r, const char* ORG);
static s32 script_call(promiseRsrc_t* r, const char* ORG);
static s32 script_ret(promiseRsrc_t* r, const char* ORG);
static s32 script_reti(promiseRsrc_t* r, const char* ORG);
static s32 script_kill_itselt(promiseRsrc_t* r, const char* ORG);
static s32 script_jmp(promiseRsrc_t* r, const char* ORG);
static s32 script_delay(promiseRsrc_t* r, const char* ORG);
static s32 script_add(promiseRsrc_t* r, const char* ORG);
static s32 script_sub(promiseRsrc_t* r, const char* ORG);

/*******************************************************************************
* Function Name  : aplentalQT_periodJob
* Description    : rampCmd function commands
* Input          : - CMD, command line
				 : - brdAddr, board address
				 : - print function
* Output         : None
* Return         : None
*******************************************************************************/
void promiseSetup(promiseDev_t* dev, requestDev_t* req, void (*xPrint)(const char* FORMAT_ORG, ...))
{
	promiseRsrc_t* r = &dev->rsrc;
	memset(r, 0, sizeof(promiseRsrc_t));

	r->req = req;
	r->xPrint = xPrint;
	r->debugEn = 0;

	r->STARTS = STARTS;
	r->curRun = NULL;

	dev->PeriodJOb = promise_PeriodJOb;
	dev->Start = promise_Start;
	dev->Cmd = promise_Cmd;
}

static s32 promise_Start(promiseRsrc_t* r, SCRIPT_T* start){
	memset(r->pos, 0, 8*sizeof(const char*));
	r->curRun = start;
	r->curRun->id = r;
	r->pos[0] = r->curRun->SCRIPT;
	r->squ = 1;
	r->squCpy = 1;
	r->tick = 0;
	// find ISR pos once
	r->isr_cmd = strstr(r->pos[0], "isr_cmd:");
	r->isr_t0 = strstr(r->pos[0], "isr_t0:");
	r->isr_t1 = strstr(r->pos[0], "isr_t1:");
	r->isr_50ms = strstr(r->pos[0], "isr_50ms:");
	return 0;
}

// @return: next line beginning
const char* promiseReadLine(char* buff, u16 buffLen, const char* ORG){
	char xx[64] = {0};
	const char *x;
	s32 len;

	for(x = ORG; 1; x++){
		if(x[0] == ' ')	continue;
		else if(x[0] == '\t')	continue;
		else if(x[0] == '\r')	continue;
		else if(x[0] == '\n')	continue;
		else {	break;	}
	}

	if(sscanf(x, "%64[^':;\r\n]", xx) <= 0){	return NULL;	}
	len = strlen(xx);

	switch(x[len]){
	case ':':
	case ';':
	case '\r':
	case '\n':
		len += 1;
		memcpy(buff, x, len);
		return &x[len];
	case '\'':
		len += 1;	// take first '

		// take until the second '
		memset(xx,0,64);
		if(sscanf(&x[len], "%64[^']", xx) <= 0){	return NULL;	}
		len += strlen(xx) + 1;

		// take until ; or \r\n
		memset(xx,0,64);
		if( sscanf(&x[len], "%64[^;\r\n]", xx) > 0){
			len += strlen(xx) + 1;
			memcpy(buff, x, len);
			return &x[len];
		}
		else if((x[len] == ';') || (x[len] == '\r') || (x[len] == '\n')){
			len += 1;
			memcpy(buff, x, len);
			return &x[len];
		}
		return NULL;
	}

	return NULL;
}

static s32* script_takeREG(promiseRsrc_t* r, const char* REG){
	if(strncmp(REG, "r07", 3)==0){	return &r->r[7];	}
	else if(strncmp(REG, "r06", 3)==0){	return &r->r[6];	}
	else if(strncmp(REG, "r05", 3)==0){	return &r->r[5];	}
	else if(strncmp(REG, "r04", 3)==0){	return &r->r[4];	}
	else if(strncmp(REG, "r03", 3)==0){	return &r->r[3];	}
	else if(strncmp(REG, "r02", 3)==0){	return &r->r[2];	}
	else if(strncmp(REG, "r01", 3)==0){	return &r->r[1];	}
	else if(strncmp(REG, "r00", 3)==0){	return &r->r[0];	}
	else if(strncmp(REG, "r7", 2)==0){	return &r->r[7];	}
	else if(strncmp(REG, "r6", 2)==0){	return &r->r[6];	}
	else if(strncmp(REG, "r5", 2)==0){	return &r->r[5];	}
	else if(strncmp(REG, "r4", 2)==0){	return &r->r[4];	}
	else if(strncmp(REG, "r3", 2)==0){	return &r->r[3];	}
	else if(strncmp(REG, "r2", 2)==0){	return &r->r[2];	}
	else if(strncmp(REG, "r1", 2)==0){	return &r->r[1];	}
	else if(strncmp(REG, "r0", 2)==0){	return &r->r[0];	}
	return NULL;
}

static s32 script_print(promiseRsrc_t* r, const char* ORG){
	s32 i,j,z=0xffffffff;
	char format[PROMISE_CMD_LEN] = {0};
	char s[8][8] = {0};
	s32* p[8] = {&z};
// 	"req%*[^']'%[^']'"	// only take string between ' and '
	if(strncmp(ORG, "print res", strlen("print res")) == 0){
		r->xPrint(r->busResp);
		return 1;
	}

	i = sscanf(ORG, "print%*[^']'%[^']' %s %s %s %s %s %s %s %s", format,s[0],s[1],s[2],s[3],s[4],s[5],s[6],s[7]);
	if( i <= 0){	return 0;	}
	else if(i==1){
		r->xPrint(format);
		return 1;
	}
	for(j=0; j<i-1; j++){	p[j] = script_takeREG(r,s[j]);	}
	r->xPrint(format,*p[0],*p[1],*p[2],*p[3],*p[4],*p[5],*p[6],*p[7]);
	return 1;
}

static s32 script_scanf(promiseRsrc_t* r, const char* ORG){
	s32 i,j,z=0xffffffff;
	char obj[16] = {0};
	char format[PROMISE_CMD_LEN] = {0};
	char s[8][8] = {0};
	s32* p[8] = {&z};
// 	"req%*[^']'%[^']'"	// only take string between ' and '
	i = sscanf(ORG, "%16[^.].scanf%*[^']'%64[^']' %s %s %s %s %s %s %s %s", obj, format, s[0],s[1],s[2],s[3],s[4],s[5],s[6],s[7]);
	if(i>=3){
		for(j=0; j<i-2; j++){
			p[j] = script_takeREG(r,s[j]);
		}
		// take from 485 response
		if(strncmp(obj,"res",3) == 0){
			i = sscanf(r->busResp, format, p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7]);
		}
		else if(strncmp(obj,"cmd",3) == 0){
			i = sscanf(r->busResp, format, p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7]);
		}
	}
	else {	return 0;	}

	return 1;
}

static s32 script_move(promiseRsrc_t* r, const char* ORG){
	char s[2][8] = {0};
	s32 *p0, *p1;
	s32 xInt;

	p0 = NULL;
	p1 = NULL;
	if(sscanf(ORG, "move %s %s", s[0], s[1])==2){
		p0 = script_takeREG(r,s[0]);
		p1 = script_takeREG(r,s[1]);
		if(p0 == NULL){
			r->squ = 200;	// goto error
		}
		else{
			if(p1){	*p0 = *p1;	}
			else if(sscanf(s[1], "%d", &xInt)==1){	*p0 = xInt;	}
			else{
				r->squ = 200;	// goto error
			}
		}
		return 1;
	}
	return 0;
}

// "req '1.input.readpin 0' 100"
static s32 script_req(promiseRsrc_t* r, const char* ORG){
	char strTmp[PROMISE_CMD_LEN] = {0};
	s32 v0;
	if(sscanf(ORG, "req%*[^']'%[^']'%d", strTmp, &v0)==2){
		memset(r->busResp, 0, PROMISE_CMD_LEN);
		memset(r->busOrg, 0, PROMISE_CMD_LEN);
		strcpy(r->busOrg, strTmp);
		r->req->Add(&r->req->rsrc, strTmp, v0, r, promise_requestSendCB, promise_requestCB);
		r->retryTz = 3;
		r->tTimeout = v0;
		r->squ = PROMISE_SQU_REQ;	// just wait
		return 1;
	}
	return 0;
}

// res.switch "format" label;
static s32 script_switch(promiseRsrc_t* r, const char* ORG){
	char format[PROMISE_CMD_LEN] = {0};
	char obj[16] = {0};
	char label[16] = {0};
	char* src = NULL;
	s32 x=0,y,z;
	r->pos[0] = r->nextPos;	// default
	if(sscanf(ORG, "%16[^.].switch%*[^']'%64[^']'%*[^$a-z]%16[^;\r\n]", obj, format, label)==3){
		if(strncmp(obj, "res", 3)==0){	src = r->busResp; }
		else if(strncmp(obj, "cmd", 3)==0){	src = r->cmdHost; }
		if(src){
			if(strstr(src, format)){
				strcat(label, ":");
				if(label[0] == '$'){	r->pos[0] = strstr(r->pos[0], label);	}
				else{	r->pos[0] = strstr(r->curRun->SCRIPT, label);	}
			}
		}
		else{
			if(strncmp(obj, "r07", 3)==0){	x = r->r[7];	}
			else if(strncmp(obj, "r06", 3)==0){	x = r->r[6];	}
			else if(strncmp(obj, "r05", 3)==0){	x = r->r[5];	}
			else if(strncmp(obj, "r04", 3)==0){	x = r->r[4];	}
			else if(strncmp(obj, "r03", 3)==0){	x = r->r[3];	}
			else if(strncmp(obj, "r02", 3)==0){	x = r->r[2];	}
			else if(strncmp(obj, "r01", 3)==0){	x = r->r[1];	}
			else if(strncmp(obj, "r00", 3)==0){	x = r->r[0];	}
			else if(strncmp(obj, "r7", 2)==0){	x = r->r[7];	}
			else if(strncmp(obj, "r6", 2)==0){	x = r->r[6];	}
			else if(strncmp(obj, "r5", 2)==0){	x = r->r[5];	}
			else if(strncmp(obj, "r4", 2)==0){	x = r->r[4];	}
			else if(strncmp(obj, "r3", 2)==0){	x = r->r[3];	}
			else if(strncmp(obj, "r2", 2)==0){	x = r->r[2];	}
			else if(strncmp(obj, "r1", 2)==0){	x = r->r[1];	}
			else if(strncmp(obj, "r0", 2)==0){	x = r->r[0];	}

			if(sscanf(format,"![%d,%d]",&y,&z)==2){
				if((x<y) || (x>z)){
					strcat(label, ":");
					if(label[0] == '$'){	r->pos[0] = strstr(r->pos[0], label);	}
					else{	r->pos[0] = strstr(r->curRun->SCRIPT, label);	}
				}
			}
			else if(sscanf(format,"[%d,%d]",&y,&z)==2){
				// print("[%d,%d]\n",y,z);
				if((x>=y) && (x<=z)){
					strcat(label, ":");
					if(label[0] == '$'){	r->pos[0] = strstr(r->pos[0], label);	}
					else{	r->pos[0] = strstr(r->curRun->SCRIPT, label);	}
				}
			}
		}
		return 1;
	}
	return 0;
}

static s32 script_call(promiseRsrc_t* r, const char* ORG){
	char s[16] = {0};
	u8 i;
	if(sscanf(ORG, "call%*[^$a-z]%[^;\r\n]", s)==1){
		for(i=7;i>=1;i--){	r->pos[i] = r->pos[i-1];	}
		strcat(s, ":");
		r->pos[0] = strstr(r->curRun->SCRIPT, s);
		return 1;
	}
	return 0;
}

static s32 script_reti(promiseRsrc_t* r, const char* ORG){
	u8 i;
	if(strncmp(ORG, "reti", 4)==0){
		for(i=0;i<7;i++){	r->pos[i] = r->pos[i+1];	}
		r->pos[7] = NULL;
		if(r->pos[0] == NULL){
			r->squ = 0;	// goto error
			r->curRun->id = NULL;
			r->curRun = NULL;
			print("error@reti\n");
		}
		r->squ = r->squCpy;
		return 1;
	}
	return 0;
}

static s32 script_kill_itselt(promiseRsrc_t* r, const char* ORG){
	if(strncmp(ORG, "kill_itselt", 4)==0){
		r->squ = 0;
		r->curRun->id = NULL;
		r->curRun = NULL;
	}
}

static s32 script_ret(promiseRsrc_t* r, const char* ORG){
	char line[PROMISE_CMD_LEN] = {0};
	const char* p;
	u8 i;
	if(strncmp(ORG, "ret", 3)==0){
		for(i=0;i<7;i++){	r->pos[i] = r->pos[i+1];	}
		r->pos[7] = NULL;
		if(r->pos[0] == NULL){
			r->squ = 0;	// goto error
			r->curRun->id = NULL;
			r->curRun = NULL;
			return 1;
		}
		p = promiseReadLine(line, PROMISE_CMD_LEN, r->pos[0]);
		r->pos[0] = p;
		return 1;
	}
	return 0;
}

// delay 123;
// delay r0;
static s32 script_jmp(promiseRsrc_t* r, const char* ORG){
	char str[PROMISE_CMD_LEN] = {0};
	if(sscanf(ORG, "jmp%*[^$a-z]%[^;\r\n]", str)==1){
		strcat(str, ":");
		if(str[0] == '$'){
			r->pos[0] = strstr(r->nextPos, str);
		}
		else{
			r->pos[0] = strstr(r->curRun->SCRIPT, str);
		}
		return 1;
	}
	return 0;
}

static s32 script_delay(promiseRsrc_t* r, const char* ORG){
	s32 v0;
	s32 i = 0;

	if(sscanf(ORG, "delay %d", &v0) == 1){
		i =1;
	}
	else if(sscanf(ORG, "delay r%d", &v0)==1){
		if(v0 >= PROMISE_REG_SZ){	return 0;}
		i = 2;
		v0 = r->r[v0];
		print("delay %d\n", v0);
	}

	if(i){
		r->squ = PROMISE_SQU_DELAY;
		r->tick = 0;
		r->tDelay = v0;
	}
	return i;
}

static s32 script_add(promiseRsrc_t* r, const char* ORG){
	char s[2][8] = {0};
	s32 *p0, *p1;
	s32 xInt;

	p0 = NULL;
	p1 = NULL;
	if(sscanf(ORG, "add %s %s", s[0], s[1])==2){
		p0 = script_takeREG(r,s[0]);
		p1 = script_takeREG(r,s[1]);
		if(p0 == NULL){
			r->squ = 200;	// goto error
		}
		else{
			if(p1){	*p0 += *p1;	}
			else if(sscanf(s[1], "%d", &xInt)==1){	*p0 += xInt;	}
			else{
				r->squ = 200;	// goto error
			}
		}
		return 1;
	}
	return 0;
}

static s32 script_sub(promiseRsrc_t* r, const char* ORG){
	char s[2][8] = {0};
	s32 *p0, *p1;
	s32 xInt;

	p0 = NULL;
	p1 = NULL;
	if(sscanf(ORG, "sub %s %s", s[0], s[1])==2){
		p0 = script_takeREG(r,s[0]);
		p1 = script_takeREG(r,s[1]);
		if(p0 == NULL){
			r->squ = 200;	// goto error
		}
		else{
			if(p1){	*p0 -= *p1;	}
			else if(sscanf(s[1], "%d", &xInt)==1){	*p0 -= xInt;	}
			else{
				r->squ = 200;	// goto error
			}
		}
		return 1;
	}
	return 0;
}

static s32 script_isLabel(promiseRsrc_t* r, const char* ORG){
	if(strlen(ORG) < 1){	return 0;	}
	if(ORG[strlen(ORG)-1] == ':'){	return 1;	}
	return 0;
}

static void promise_PeriodJOb(promiseRsrc_t* r, u8 tick){
	s32 i;
	char line[PROMISE_CMD_LEN] = {0};
	r->tick += tick;
	switch(r->squ){
	case 0:
		break;
	case PROMISE_SQU_IDEL:	// read line and send
		r->nextPos = promiseReadLine(line, PROMISE_CMD_LEN, r->pos[0]);
		//print("::<%s>\r\n", line);
		if(strncmp(line, "cmd.reg ", strlen("cmd.reg ")) == 0){	r->pos[0] = r->nextPos;	}
		else if(strncmp(line, "start ", strlen("start ")) == 0){	r->pos[0] = r->nextPos;	}
		else if(script_isLabel(r, line)){	r->pos[0] = r->nextPos;	}
		else if(script_move(r, line)){	r->pos[0] = r->nextPos;	}
		else if(script_add(r, line)){	r->pos[0] = r->nextPos;	}
		else if(script_sub(r, line)){	r->pos[0] = r->nextPos;	}
		else if(script_req(r, line)){	r->pos[0] = r->nextPos;	}
		else if(script_switch(r, line)){	}
		else if(script_print(r, line)){	r->pos[0] = r->nextPos;	}
		else if(script_scanf(r, line)){	r->pos[0] = r->nextPos;	}
		else if(script_call(r, line)){	}
		else if(script_reti(r, line)){	}	// muset before 'script_ret'
		else if(script_ret(r, line)){	}
		else if(script_kill_itselt(r, line)){	}
		else if(script_jmp(r, line)){	}
		else if(script_delay(r, line)){	r->pos[0] = r->nextPos;	}
		else{	r->squ = 0;	}
		break;

	// system delay
	case PROMISE_SQU_DELAY:
		if(r->tick >= r->tDelay){	r->squ = 1;	}
		break;

	// wait send out asynchrous
	case PROMISE_SQU_REQ:
		break;

	// wait response
	case PROMISE_SQU_REQ + 1:
		if(strncmp(r->busResp, "+ok@", strlen("+ok@")) == 0){
			r->squ = 1;
		}
		else if(strncmp(r->busResp, "+err@", strlen("+err@")) == 0){
			r->xPrint("+err@%s\r\n", r->busOrg);
			r->squ = 0;
		}
		else if(strncmp(r->busResp, "+$timeout@req\r\n", strlen("+$timeout@req\r\n")) == 0){
			if(r->retryTz == 0){
				r->xPrint("+timeout1@%s\r\n", r->busResp);
				r->squ = 0;
			}
			else{
				r->retryTz--;
				memset(r->busResp, 0, PROMISE_CMD_LEN);
				r->req->Add(&r->req->rsrc, r->busOrg, r->tTimeout, r, promise_requestSendCB, promise_requestCB);
				r->squ = PROMISE_SQU_REQ;	// just wait
			}
		}
		else if(r->tick >= r->tTimeout*4){
			r->xPrint("+timeout2@%s\r\n", r->busOrg);
			r->squ = 0;
		}
		break;

	case 200:
		r->xPrint("+err@%s\r\n", r->busOrg);
		r->squ = 0;
		break;

	case 255:
		break;
	}

}

static s32 promise_Cmd(promiseRsrc_t* r, const char* ORG){
	s32 i,j,k,z=0xffffffff;
	char line[PROMISE_CMD_LEN] = {0};
	char format[PROMISE_CMD_LEN] = {0};
	char param[32];
	char s[8][8] = {0};
	s32* p[8] = {&z}, *ix;
	s32 v[8] = {0};
	u8 matched=0;

	for(i=0; i<SCRIPT_COUNT && (matched==0); i++){
		memset(line, 0, PROMISE_CMD_LEN);
		promiseReadLine(line, PROMISE_CMD_LEN, STARTS[i].SCRIPT);
		if(line[strlen(line)-1] == ';'){	line[strlen(line)-1] = ' ';	}
		j = sscanf(line, "start%*[^']'%64[^']' %s %s %s %s %s %s %s %s", format, s[0],s[1],s[2],s[3],s[4],s[5],s[6],s[7]);
		if(j <= 0){	continue;	}
		else if(j == 1){
			if(strncmp(ORG, format, strlen(format)) == 0){	matched = 1;	}
		}
		else{
			k = sscanf(ORG, format, &v[0],&v[1],&v[2],&v[3],&v[4],&v[5],&v[6],&v[7]);
			if(k >= (j-1)){	matched = 2;	}
		}

		if(matched>0){
			if(STARTS[i].id){
				print("+busy@%s", ORG);
				return 1;
			}
			else if((STARTS[i].id==NULL) && (r->curRun == NULL)){
				// load r0..7
				memset(r->r, 0, sizeof(s32)*8);
				if(matched==2){
					for(k=0;k<(j-1);k++){
						ix = script_takeREG(r,s[k]);
						if(ix)	*ix = v[k];
					}
				}
				// start script
				promise_Start(r, &STARTS[i]);
				return 1;
			}
		}
	}

//	if((matched==0) && (r->squ > 0)){
//		memset(r->cmdHost, 0, PROMISE_CMD_LEN);
//		strcpy(r->cmdHost, ORG);
//		if(r->isr_cmd){
//			for(i=7;i>=1;i--){	r->pos[i] = r->pos[i-1];	}
//			r->pos[0] = r->isr_cmd;
//			r->squCpy = r->squ;
//			//r->squ = 1;////////////////////////////////////////////////////////////////////////////////////////
//		}
//	}

	return 0;
}

static void promise_requestCB(void* x, const char* RCV_ORG){
	promiseRsrc_t* r = (promiseRsrc_t*)x;
	if(r->squ == PROMISE_SQU_REQ + 1){
		strcpy(r->busResp, RCV_ORG);
		if(r->debugEn)	r->xPrint("::%s", RCV_ORG);
	}
//	else{	print("::%s", RCV_ORG);	}
}

static void promise_requestSendCB(void* x, const char* MSG){
	promiseRsrc_t* r = (promiseRsrc_t*)x;
	r->tick = 0;
	r->squ = PROMISE_SQU_REQ + 1;
}

/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
