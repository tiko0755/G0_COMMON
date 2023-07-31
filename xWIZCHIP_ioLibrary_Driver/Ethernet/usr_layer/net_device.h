/**********************************************************
filename: net_device.h
**********************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _NET_DEVICE_H
#define _NET_DEVICE_H
#include "misc.h"
#include "wizchip_conf.h"
#include "usr_net_type.h"
#include "usr_tcps.h"
#include "usr_tcpc.h"
#include "usr_udp.h"

typedef struct{
	SOCKT_PROTOCOL protocol;
	void* socketDev;
} xSOCKET_t;

typedef struct{
	//config
	wiz_NetInfo gWIZNETINFO;
	
	// loop task squence
	u32 chkLinkTmr,chkLinkTick;
	u8 squ;
	u8 isLinked;
	u8 onFirstLinked;
	void (*print)(const char* FORMAT_ORG, ...);
	void (*printS)(const char* FORMAT_ORG);
	
	TcpSeverDev_t tcpS[_WIZCHIP_SOCK_NUM_];
	TcpClientDev_t tcpC[_WIZCHIP_SOCK_NUM_];
	UdpDev_t udp[_WIZCHIP_SOCK_NUM_];
	
	// finally, got 8 socket-x
	xSOCKET_t sckt[_WIZCHIP_SOCK_NUM_];
}WizDevRsrc;

typedef struct{
	WizDevRsrc rsrc;
	// functiion
	void (*initial)(WizDevRsrc *);
	void (*loop)(WizDevRsrc *, u8 tick);
	void (*printInfo)(WizDevRsrc *);
	
	TcpSeverDev_t* (*newTcpServer)(
		WizDevRsrc *pRsrc, 	
		u8 tx_memsz,	//send buffer size, in K-bytes
		u8 rx_memsz,	//send buffer size, in K-bytes
		u16 port,	
		void (*cb_newRcv)(u16 rcbBytes),	// callback while there are receive data
		void (*cb_connected)(u8* destip, u16 destport),
		void (*cb_closed)(),
		void (*cb_listen)(u16 port)
	);
		
	TcpClientDev_t* (*newTcpClient)(
		WizDevRsrc *pRsrc, 
		u8 tx_memsz,	//send buffer size, in K-bytes
		u8 rx_memsz,	//send buffer size, in K-bytes
		u16 localport,
		u8* destip,
		u16 destiport,
		void (*cb_newRcv)(u16 rcbBytes),	// callback while there are receive data
		void (*cb_connected)(u8* destip, u16 destport),
		void (*cb_closed)()
	);
		
	UdpDev_t* (*newUdp)(
		WizDevRsrc *pRsrc, 	
		u8 tx_memsz,	//send buffer size, in K-bytes
		u8 rx_memsz,	//send buffer size, in K-bytes
		u16 localport,
		u8* rxRBPool,
		u16 rxRBPoolLen,
		void (*cb_newRcv)(u16 rcbBytes),	// callback while there are receive data
		void (*cb_closed)()
	);
		
	void (*reLink)(WizDevRsrc* pRsrc, wiz_NetInfo info);
		
}WizDevDev;

/* output variables for extern function --------------------------------------*/
s8 wizDev_setup(
	WizDevDev* dev, 
	wiz_NetInfo info,
	void (*print)(const char* FORMAT_ORG, ...),
	void (*printS)(const char* FORMAT_ORG)
);
	
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
