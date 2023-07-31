/******************** (C) COPYRIGHT 2015 INCUBECN *****************************
* File Name          : usr_tcpc.c
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
#include "usr_tcpc.h"
#include "socket.h"

extern void print(const char* FORMAT_ORG, ...);
extern void printS(const char* MSG);

static int32_t TcpClientLoop(TcpClientRsrc_t* pRsrc, u16 tick);
static  int32_t TcpClientTakeRcv(TcpClientRsrc_t* p, u8* buff, u16 len); 
static  int32_t TcpClientSend(TcpClientRsrc_t* p, u8* buff, u16 len);
static void TcpClientClose(TcpClientRsrc_t* p);
static void TcpClientOpen(TcpClientRsrc_t* p);

void TcpClientDev_setup(
	TcpClientDev_t *dev,
	u8 sn,	// the socket no. bonding
	u8 tx_memsz,	//send buffer size, in K-bytes
	u8 rx_memsz,	//send buffer size, in K-bytes	
	u16 localport,	
	uint8_t* destip,
	uint16_t destport,		
	void (*cb_newRcv)(u16 rcbBytes),	// callback while there are receive data
	void (*cb_connected)(u8* destip, u16 destport),
	void (*cb_closed)()
){
	TcpClientRsrc_t *pRsrc = &dev->rsrc;
	memset(pRsrc, 0, sizeof(TcpClientRsrc_t));
	pRsrc->sn = sn;
	pRsrc->tx_memsz = tx_memsz;
	pRsrc->rx_memsz = rx_memsz;
	pRsrc->destip[0] = destip[0];
	pRsrc->destip[1] = destip[1];
	pRsrc->destip[2] = destip[2];
	pRsrc->destip[3] = destip[3];
	pRsrc->destport = destport;
	pRsrc->cb_newRcv = cb_newRcv;
	pRsrc->cb_connected = cb_connected;
	pRsrc->cb_closed = cb_closed;
	pRsrc->squ = 0;
	pRsrc->localport = localport;
	pRsrc->isEstablished = 0;
	pRsrc->resumeTcpClient = 0;
	// register function
	dev->loop = TcpClientLoop;
	dev->send = TcpClientSend;
	dev->take_rcv = TcpClientTakeRcv;
	dev->closeSession = TcpClientClose;
	dev->openSession = TcpClientOpen;
}	

static int32_t TcpClientLoop(TcpClientRsrc_t* pRsrc, u16 tick){
   int32_t ret; // return value for SOCK_ERRORs
   uint16_t size = 0;
   
   u8 sn = pRsrc->sn;
   // Socket Status Transitions
   // Check the W5500 Socket n status register (Sn_SR, The 'Sn_SR' controlled by Sn_CR command or Packet send/recv status)
   switch(getSn_SR(sn))
   {
      case SOCK_ESTABLISHED :
         if(getSn_IR(sn) & Sn_IR_CON)	// Socket n interrupt register mask; TCP CON interrupt = connection with peer is successful
         {
			setSn_IR(sn, Sn_IR_CON);  // this interrupt should be write the bit cleared to '1'
			pRsrc->isEstablished = 1;
			if(pRsrc->cb_connected) 	 pRsrc->cb_connected(pRsrc->destip, pRsrc->destport);
         }
         //////////////////////////////////////////////////////////////////////////////////////////////
         // Data Transaction Parts; Handle the [data receive and send] process
         //////////////////////////////////////////////////////////////////////////////////////////////
		 if((size = getSn_RX_RSR(sn)) > 0) // Sn_RX_RSR: Socket n Received Size Register, Receiving data length
         {
			 if(pRsrc->cb_newRcv)	pRsrc->cb_newRcv(size);
         }
         break;

      case SOCK_CLOSE_WAIT :
         if((ret=disconnect(sn)) != SOCK_OK) return ret;
         break;

      case SOCK_INIT:
    	 if( (ret = connect(sn, pRsrc->destip, pRsrc->destport)) != SOCK_OK) return ret;	//	Try to TCP connect to the TCP server (destination)
		break;

      case SOCK_CLOSED:
		if(pRsrc->cb_closed)	pRsrc->cb_closed();
		pRsrc->isEstablished = 0;
		if(pRsrc->resumeTcpClient == 0)	break;
		close(sn);
		if((ret = socket(sn, Sn_MR_TCP, pRsrc->localport, Sn_MR_ND)) != sn){
			//if(pRsrc->anyport == 0xffff) pRsrc->anyport = 0xffff-100;
			return ret; // TCP socket open with 'any_port' port number
        }
        break;
      default:
         break;
   }
   return 1;
}

static  int32_t TcpClientTakeRcv(TcpClientRsrc_t* p, u8* buff, u16 len){
	return recv(p->sn, buff, len);
}

static  int32_t TcpClientSend(TcpClientRsrc_t* p, u8* buff, u16 size){
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

static void TcpClientClose(TcpClientRsrc_t* p){
	 close(p->sn);
	 p->resumeTcpClient = 0;
}

static void TcpClientOpen(TcpClientRsrc_t* p){
	 p->resumeTcpClient = 1;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
