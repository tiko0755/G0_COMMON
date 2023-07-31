/**********************************************************
filename: usr_tcpc.h
**********************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _USR_TCPC_H
#define _USR_TCPC_H
#include "misc.h"
#include "wizchip_conf.h"
#include "usr_net_type.h"

typedef struct{
	u8 sn;
	u8 protocol;
	u8 tx_memsz;	//send buffer size, in K-bytes
	u8 rx_memsz;	//send buffer size, in K-bytes
	uint8_t destip[4];
	uint16_t destport;	
	// callback
	void (*cb_newRcv)(u16 rcbBytes);
	void (*cb_connected)(u8* destip, u16 destport);
	void (*cb_closed)();
	// only for tcp client
	u8 isEstablished;
	u8 resumeTcpClient;
	u8 squ;
	u16 localport;
}TcpClientRsrc_t;

typedef struct{
	TcpClientRsrc_t rsrc;
	int32_t (*take_rcv)(TcpClientRsrc_t* p, u8* buff, u16 len);
	int32_t (*send)(TcpClientRsrc_t* p, u8* buff, u16 len);
	int32_t (*loop)(TcpClientRsrc_t* p, u16 tick);
	void (*closeSession)(TcpClientRsrc_t* p);
	void (*openSession)(TcpClientRsrc_t* p);
}TcpClientDev_t;

/* output variables for extern function --------------------------------------*/
void TcpClientDev_setup(
	TcpClientDev_t *dev,
	u8 sn,	// the socket no. bonding
	u8 tx_memsz,	//send buffer size, in K-bytes
	u8 rx_memsz,	//send buffer size, in K-bytes	
	uint16_t localport,	
	uint8_t *destip,
	uint16_t destport,		
	void (*cb_newRcv)(u16 rcbBytes),	// callback while there are receive data
	void (*cb_connected)(u8* destip, u16 destport),
	void (*cb_closed)()
);
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
