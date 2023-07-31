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
#include "usr_udp.h"
#include "socket.h"

static int32_t TcpUdpLoop(UdpRsrc_t* pRsrc, u16 tick);
static  int32_t TcpUdpTakeRcv(UdpRsrc_t* p, u8* buff, u16 len); 
static  int32_t TcpUdpSend(UdpRsrc_t* p, u8* buff, u16 len);

void UdpDev_setup(
	UdpDev_t *dev,
	u8 sn,	// the socket no. bonding
	u8 tx_memsz,	//send buffer size, in K-bytes
	u8 rx_memsz,	//send buffer size, in K-bytes	
	u16 localport,
	u8* rxRBPool,
	u16 rxRBPoolLen,
	void (*cb_newRcv)(u16 rcbBytes),	// callback while there are receive data
	void (*cb_closed)()
){
	UdpRsrc_t *pRsrc = &dev->rsrc;
	pRsrc->sn = sn;
	pRsrc->tx_memsz = tx_memsz;
	pRsrc->rx_memsz = rx_memsz;
	pRsrc->localport = localport;
	pRsrc->cb_newRcv = cb_newRcv;
	pRsrc->cb_closed = cb_closed;
	pRsrc->rxRBPool = rxRBPool;
	pRsrc->rxRBPoolLen = rxRBPoolLen;

	RingBuffer_Init(&pRsrc->rxRB, rxRBPool, 1, rxRBPoolLen);

	// register function
	dev->loop = TcpUdpLoop;
	dev->send = TcpUdpSend;
	dev->take_rcv = TcpUdpTakeRcv;
}	

static int32_t TcpUdpLoop(UdpRsrc_t* pRsrc, u16 tick){
	int32_t  ret;
	int32_t size;
	u8 sn = pRsrc->sn;

	switch(getSn_SR(sn))
	{
	  case SOCK_UDP :
		 if((size = getSn_RX_RSR(sn)) > 0)
		 {
			 if(pRsrc->cb_newRcv)	pRsrc->cb_newRcv(size);
		 }
		 break;
	  case SOCK_CLOSED:
		if(pRsrc->cb_closed)	pRsrc->cb_closed();
		if((ret = socket(sn, Sn_MR_UDP, pRsrc->localport, 0x00)) != sn)
			return ret;
	#ifdef _LOOPBACK_DEBUG_
		 print("%d:Opened, UDP loopback, port [%d]\r\n", sn, port);
	#endif
		 break;
	  default :
		 break;
	}
   return 1;
}

static  int32_t TcpUdpTakeRcv(UdpRsrc_t* p, u8* buff, u16 len){
	// will update destinate Net info
	return recvfrom(p->sn, buff, len, p->destNetInfo.ip, &p->destNetInfo.port);
}

static  int32_t TcpUdpSend(UdpRsrc_t* p, u8* buff, u16 size){
	u16 sentsize = 0;
	int32_t ret;
	while(size != sentsize)
	{
		ret = sendto(p->sn, buff+sentsize, size-sentsize, p->destNetInfo.ip, p->destNetInfo.port);
		if(ret < 0)
		{
			close(p->sn);
			return ret;
		}
		sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
	}
	return sentsize;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
