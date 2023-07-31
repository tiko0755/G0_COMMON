/******************** (C) COPYRIGHT 2015 INCUBECN *****************************
* File Name          : net_device.c
* Author             : Tiko Zhong
* Date First Issued  : 20210703
* Description        : w5500 socket device
*                      
********************************************************************************
* History:
* 20210703: V0.0	
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "misc.h"
#include <string.h>
#include "stdarg.h"
#include "stdio.h"
#include "usr_tcps.h"
#include "socket.h"

static int32_t TcpSeverLoop(TcpSeverRsrc_t* pRsrc, u16 tick);
static  int32_t TcpSeverTakeRcv(TcpSeverRsrc_t* p, u8* buff, u16 len); 
static  int32_t TcpSeverSend(TcpSeverRsrc_t* p, u8* buff, u16 len);

static int32_t TcpSeverPrint(TcpSeverRsrc_t* p, const char* FORMAT_ORG, ...);
static int32_t TcpSeverPrintS(TcpSeverRsrc_t* p, const char*);


void TcpSeverDev_setup(
	TcpSeverDev_t *dev,
	u8 sn,	// the socket no. bonding
	u8 tx_memsz,	//send buffer size, in K-bytes
	u8 rx_memsz,	//send buffer size, in K-bytes	
	u16 port,		
	void (*cb_newRcv)(u16 rcbBytes),	// callback while there are receive data
	void (*cb_connected)(u8* destip, u16 destport),
	void (*cb_closed)(),
	void (*cb_listen)(u16 port)
){
	u8 i;
	TcpSeverRsrc_t *pRsrc = &dev->rsrc;
	pRsrc->sn = sn;
	pRsrc->tx_memsz = tx_memsz;
	pRsrc->rx_memsz = rx_memsz;
	pRsrc->port = port;
	pRsrc->cb_newRcv = cb_newRcv;
	pRsrc->cb_connected = cb_connected;
	pRsrc->cb_closed = cb_closed;
	pRsrc->cb_listen = cb_listen;
	pRsrc->squ = 0;
	// register function
	dev->loop = TcpSeverLoop;
	dev->send = TcpSeverSend;
	dev->take_rcv = TcpSeverTakeRcv;
	dev->print = TcpSeverPrint;
	dev->printS = TcpSeverPrintS;
}	

static int32_t TcpSeverLoop(TcpSeverRsrc_t* pRsrc, u16 tick){
   int32_t ret;
   uint16_t size = 0;
   uint8_t destip[4];
   uint16_t destport;
	u8 sn = pRsrc->sn;
	
   switch(getSn_SR(sn))
   {
      case SOCK_ESTABLISHED :
         if(getSn_IR(sn) & Sn_IR_CON)
         {
			 if(pRsrc->cb_connected){	
				getSn_DIPR(sn, destip);
				destport = getSn_DPORT(sn);				 
				pRsrc->cb_connected(destip, destport);
			 }
			setSn_IR(sn, Sn_IR_CON);
         }
		 if((size = getSn_RX_RSR(sn)) > 0) // Don't need to check SOCKERR_BUSY because it doesn't not occur.
         {
			 if(pRsrc->cb_newRcv)	pRsrc->cb_newRcv(size);
         }
         break;
      case SOCK_CLOSE_WAIT :
        if((ret = disconnect(sn)) != SOCK_OK) return ret;
		if(pRsrc->cb_closed)	pRsrc->cb_closed();
        break;
      case SOCK_INIT :
		if(pRsrc->cb_listen)	pRsrc->cb_listen(pRsrc->port);
         if( (ret = listen(sn)) != SOCK_OK) return ret;
         break;
      case SOCK_CLOSED:
         if((ret = socket(sn, Sn_MR_TCP, pRsrc->port, 0x00)) != sn) return ret;
         break;
      default:
         break;
   }
   return 1;
}

static  int32_t TcpSeverTakeRcv(TcpSeverRsrc_t* p, u8* buff, u16 len){
	return recv(p->sn, buff, len);
}

static  int32_t TcpSeverSend(TcpSeverRsrc_t* p, u8* buff, u16 size){
	u16 sentsize = 0;
	int32_t ret;
	while(size != sentsize)
	{
		ret = send(p->sn, buff+sentsize, size-sentsize);
		if(ret < 0)
		{
			close(p->sn);
			return ret;
		}
		sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
	}
	return 0;
}

static int32_t TcpSeverPrint(TcpSeverRsrc_t* p, const char* FORMAT_ORG, ...){
	char buff[MAX_CMD_LEN] = {0};
	va_list ap;
	s16 bytes;
	//take string
	va_start(ap, FORMAT_ORG);
	bytes = vsnprintf(buff, MAX_CMD_LEN, FORMAT_ORG, ap);
	va_end(ap);
	//send out
	if(bytes>0)	TcpSeverSend(p, (u8*)buff, strlen(buff));
	return 0;
}

static int32_t TcpSeverPrintS(TcpSeverRsrc_t* p, const char* S){
	char buff[MAX_CMD_LEN] = {0};
	strcpy(buff, S);
	TcpSeverSend(p, (u8*)buff, strlen(buff));
	return 0;
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
