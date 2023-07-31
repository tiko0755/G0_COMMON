/**********************************************************
filename: usr_udp.h
**********************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _USR_UDP_H
#define _USR_UDP_H
#include "misc.h"
#include "wizchip_conf.h"
#include "usr_net_type.h"
#include "ring_buffer.h"

typedef struct{
	u8 sn;
	u8 protocol;
	u16 localport;
	ShortNetInfo_t destNetInfo;
	u8 tx_memsz;	//wiznet chip's send buffer size, in K-bytes
	u8 rx_memsz;	//wiznet chip's send buffer size, in K-bytes
	RINGBUFF_T rxRB;
	u8* rxRBPool;
	u16 rxRBPoolLen;
	// callback
	void (*cb_newRcv)(u16 rcbBytes);
	void (*cb_closed)();
}UdpRsrc_t;

typedef struct{
	UdpRsrc_t rsrc;
	int32_t (*take_rcv)(UdpRsrc_t* p, u8* buff, u16 len);
	int32_t (*send)(UdpRsrc_t* p, u8* buff, u16 len);
	int32_t (*loop)(UdpRsrc_t* p, u16 tick);
}UdpDev_t;

/* output variables for extern function --------------------------------------*/
/*
	u8 sn,			// the socket no. bonding
	u8 tx_memsz,	// send buffer size, in K-bytes
	u8 rx_memsz,	// send buffer size, in K-bytes	
	u16 localport,	// the port bonding
	void (*cb_newRcv)(u16 rcbBytes),	// callback while there are receive data
	void (*cb_closed)()					// callback after close
*/
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
);
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
