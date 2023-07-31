/**********************************************************
filename: usr_tcps.h
**********************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _USR_TCPS_H
#define _USR_TCPS_H
#include "misc.h"
#include "wizchip_conf.h"
#include "usr_net_type.h"

typedef struct{
	u8 sn;
	u8 protocol;
	u16 port;	
	u8 tx_memsz;	//send buffer size, in K-bytes
	u8 rx_memsz;	//send buffer size, in K-bytes
	// callback
	void (*cb_newRcv)(u16 rcbBytes);	// cb is not runing in ISR
	void (*cb_connected)(u8* destip, u16 destport);	// cb is not runing in ISR
	void (*cb_closed)();	// cb is not runing in ISR
	void (*cb_listen)(u16 port);	// cb is not runing in ISR
	u8 squ;
}TcpSeverRsrc_t;

typedef struct{
	TcpSeverRsrc_t rsrc;
	int32_t (*take_rcv)(TcpSeverRsrc_t* p, u8* buff, u16 len);
	int32_t (*send)(TcpSeverRsrc_t* p, u8* buff, u16 len);
	int32_t (*print)(TcpSeverRsrc_t* p, const char* FORMAT_ORG, ...);
	int32_t (*printS)(TcpSeverRsrc_t* p, const char*);
	int32_t (*loop)(TcpSeverRsrc_t* p, u16 tick);
}TcpSeverDev_t;

/* output variables for extern function --------------------------------------*/
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
);
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
