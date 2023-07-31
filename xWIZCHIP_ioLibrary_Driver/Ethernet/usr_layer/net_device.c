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
#include "net_device.h"
#include "socket.h"

//#include "board.h"
static void net_dev_loop(WizDevRsrc *, u8 tick);
static void net_printInfo(WizDevRsrc *pRsrc);
static void net_initial(WizDevRsrc *);
static void net_reLink(WizDevRsrc* pRsrc, wiz_NetInfo info);

static int32_t net_loop_tcpClient(TcpSeverRsrc_t* pRsrc);
static void net_resumeTcpClient(TcpSeverRsrc_t* pRsrc);

/*
	spi CONFIG
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
	
*/

static UdpDev_t* net_newUdp(
	WizDevRsrc *pRsrc, 
	u8 tx_memsz,	//send buffer size, in K-bytes
	u8 rx_memsz,	//send buffer size, in K-bytes
	u16 localport,	
	u8* rxRBPool,
	u16 rxRBPoolLen,
	void (*cb_newRcv)(u16 rcbBytes),	// callback while there are receive data
	void (*cb_closed)()
);

static TcpSeverDev_t* net_newTcpServer(
	WizDevRsrc *pRsrc, 
	u8 tx_memsz,	//send buffer size, in K-bytes
	u8 rx_memsz,	//send buffer size, in K-bytes
	u16 port,		
	void (*cb_newRcv)(u16 rcbBytes),	// callback while there are receive data
	void (*cb_connected)(u8* destip, u16 destport),
	void (*cb_closed)(),
	void (*cb_listen)(u16 port)
);
	
static TcpClientDev_t* net_newTcpClient(
	WizDevRsrc *pRsrc, 
	u8 tx_memsz,	//send buffer size, in K-bytes
	u8 rx_memsz,	//send buffer size, in K-bytes
	u16 localport,
	u8* destip,
	u16 destport,		
	void (*cb_newRcv)(u16 rcbBytes),	// callback while there are receive data
	void (*cb_connected)(u8* destip, u16 destport),
	void (*cb_closed)()
);

s8 wizDev_setup(
	WizDevDev* dev, 
	wiz_NetInfo info,
	void (*print)(const char* FORMAT_ORG, ...),
	void (*printS)(const char* FORMAT_ORG)
){
	WizDevRsrc* pRsrc = &dev->rsrc;
	pRsrc->gWIZNETINFO = info;
	pRsrc->print = print;
	pRsrc->printS = printS;
	pRsrc->chkLinkTmr = 3000;	//per 3 seconds check once

	// register device's function
	dev->reLink = net_reLink;
	dev->initial = net_initial;
	dev->loop = net_dev_loop;
	dev->printInfo = net_printInfo;
	dev->newTcpServer = net_newTcpServer;
	dev->newTcpClient = net_newTcpClient;
	dev->newUdp = net_newUdp;
	
	// start net work
	wizchip_init(NULL, NULL);
	ctlnetwork(CN_SET_NETINFO, (void*)&pRsrc->gWIZNETINFO);
	return 0;
}

static void net_initial(WizDevRsrc *pRsrc){
	u8 tx_memsz[8];
	u8 rx_memsz[8];
	u8 i;
	TcpSeverRsrc_t *pTcpS;
	TcpClientRsrc_t *pTcpC;
	UdpRsrc_t *pUdp;
	for(i=0;i<_WIZCHIP_SOCK_NUM_;i++){
		if(pRsrc->sckt[i].protocol == SOCKT_NONE){	
			tx_memsz[i] = 0;
			rx_memsz[i] = 0;
		}
		else if(pRsrc->sckt[i].protocol == SOCKT_TCPS){
			pTcpS = &pRsrc->tcpS[i].rsrc;
			tx_memsz[i] = pTcpS->tx_memsz;
			rx_memsz[i] = pTcpS->rx_memsz;
		}
		else if(pRsrc->sckt[i].protocol == SOCKT_TCPC){
			pTcpC = &pRsrc->tcpC[i].rsrc;
			tx_memsz[i] = pTcpC->tx_memsz;
			rx_memsz[i] = pTcpC->rx_memsz;
		}
		else if(pRsrc->sckt[i].protocol == SOCKT_UDP){
			pUdp = &pRsrc->udp[i].rsrc;
			tx_memsz[i] = pUdp->tx_memsz;
			rx_memsz[i] = pUdp->rx_memsz;
		}
	}
	wizchip_init(tx_memsz, rx_memsz);
}

static void net_printInfo(WizDevRsrc *pRsrc){
	uint8_t tmpstr[6];
	wiz_NetInfo gWIZNETINFO = {0};
	if(pRsrc->print == NULL)	return;
	ctlnetwork(CN_GET_NETINFO, (void*)&gWIZNETINFO);
	// Display Network Information
	ctlwizchip(CW_GET_ID,(void*)tmpstr);
//	if(pRsrc->gWIZNETINFO.dhcp == NETINFO_DHCP) pRsrc->print("\r\n===== %s NET CONF : DHCP =====\r\n",(char*)tmpstr);
//	else pRsrc->print("\r\n===== %s NET CONF : Static =====\r\n",(char*)tmpstr);
	//pRsrc->printS("======================\r\n");
	pRsrc->print("MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n",gWIZNETINFO.mac[0],gWIZNETINFO.mac[1],gWIZNETINFO.mac[2],
	  gWIZNETINFO.mac[3],gWIZNETINFO.mac[4],gWIZNETINFO.mac[5]);
	pRsrc->print("SIP: %d.%d.%d.%d\r\n", gWIZNETINFO.ip[0],gWIZNETINFO.ip[1],gWIZNETINFO.ip[2],gWIZNETINFO.ip[3]);
	pRsrc->print("SUB: %d.%d.%d.%d\r\n", gWIZNETINFO.sn[0],gWIZNETINFO.sn[1],gWIZNETINFO.sn[2],gWIZNETINFO.sn[3]);
	pRsrc->print("GAR: %d.%d.%d.%d\r\n", gWIZNETINFO.gw[0],gWIZNETINFO.gw[1],gWIZNETINFO.gw[2],gWIZNETINFO.gw[3]);
	//pRsrc->print("DNS: %d.%d.%d.%d\r\n", gWIZNETINFO.dns[0],gWIZNETINFO.dns[1],gWIZNETINFO.dns[2],gWIZNETINFO.dns[3]);
	//pRsrc->printS("======================\r\n");
}

static void net_reLink(WizDevRsrc* pRsrc, wiz_NetInfo info){
	/* Network initialization */
	pRsrc->onFirstLinked = 0;
	pRsrc->gWIZNETINFO = info;
	ctlnetwork(CN_SET_NETINFO, (void*)&pRsrc->gWIZNETINFO);
	net_initial(pRsrc);
}

static void net_dev_loop(WizDevRsrc* pRsrc, u8 tick){
	u8 tmp,i;
//	u8 socketBuf[300] = {0};
	TcpSeverDev_t *tcpS;
	TcpClientDev_t *tcpC;
	UdpDev_t *udp;

	ctlwizchip(CW_GET_PHYLINK, (void*)&tmp);
	if(tmp == PHY_LINK_OFF){
		pRsrc->chkLinkTick += tick;
		// per chkLinkTmr retry
		if(pRsrc->chkLinkTick > pRsrc->chkLinkTmr){
			// if(pRsrc->printS)	pRsrc->printS("Net disconnect, retry\n");
			net_reLink(pRsrc, pRsrc->gWIZNETINFO);
			pRsrc->chkLinkTick = 0;
		}
		pRsrc->isLinked = 0;
		return;
	}
	else {
		if(pRsrc->isLinked == 0){	// print info
			if(pRsrc->onFirstLinked == 0){
				net_printInfo(pRsrc);
				pRsrc->onFirstLinked = 1;
			}
			pRsrc->isLinked = 1;
		}
	}
	
	// loop all socket
	for(i=0;i<_WIZCHIP_SOCK_NUM_;i++){
		if(pRsrc->sckt[i].protocol == SOCKT_NONE){	continue;	}
		else if(pRsrc->sckt[i].protocol == SOCKT_TCPS){
			tcpS = pRsrc->sckt[i].socketDev;
			tcpS->loop(&tcpS->rsrc, tick);
		}
		else if(pRsrc->sckt[i].protocol == SOCKT_TCPC){
			tcpC = pRsrc->sckt[i].socketDev;
			tcpC->loop(&tcpC->rsrc, tick);
		}
		else if(pRsrc->sckt[i].protocol == SOCKT_UDP){
			udp = pRsrc->sckt[i].socketDev;
			udp->loop(&udp->rsrc, tick);
		}
	}
}

static UdpDev_t* net_newUdp(
	WizDevRsrc *pRsrc, 
	u8 tx_memsz,	//send buffer size, in K-bytes
	u8 rx_memsz,	//send buffer size, in K-bytes
	u16 localport,	
	u8* rxRBPool,
	u16 rxRBPoolLen,
	void (*cb_newRcv)(u16 rcbBytes),	// callback while there are receive data
	void (*cb_closed)()
){
	u8 i;
	UdpDev_t *dev = NULL;
	for(i=0;i<_WIZCHIP_SOCK_NUM_;i++){
		if(pRsrc->sckt[i].protocol == SOCKT_NONE){
			dev = &pRsrc->udp[i];
			// new socket alloc in this record
			pRsrc->sckt[i].protocol = SOCKT_UDP;
			pRsrc->sckt[i].socketDev = dev;
			// initial socket
			UdpDev_setup(
				dev,
				i,	// the socket no. bonding
				tx_memsz,	//send buffer size, in K-bytes
				rx_memsz,	//send buffer size, in K-bytes	
				localport,	// local port
				rxRBPool, 	// receive buffer for ring buffer
				rxRBPoolLen,	// receive buffer length
				cb_newRcv,	// callback while there are receive data
				cb_closed
			);
			break;
		}
	}
	return dev;
}

static TcpSeverDev_t* net_newTcpServer(
	WizDevRsrc *pRsrc, 
	u8 tx_memsz,	//send buffer size, in K-bytes
	u8 rx_memsz,	//send buffer size, in K-bytes
	u16 port,		
	void (*cb_newRcv)(u16 rcbBytes),	// callback while there are receive data
	void (*cb_connected)(u8* destip, u16 destport),
	void (*cb_closed)(),
	void (*cb_listen)(u16 port)
){
	u8 i;
	TcpSeverDev_t *dev = NULL;
	for(i=0;i<_WIZCHIP_SOCK_NUM_;i++){
		if(pRsrc->sckt[i].protocol == SOCKT_NONE){
			dev = &pRsrc->tcpS[i];
			// new socket alloc in this record
			pRsrc->sckt[i].protocol = SOCKT_TCPS;
			pRsrc->sckt[i].socketDev = dev;
			// initial socket
			TcpSeverDev_setup(
				dev,
				i,	// the socket no. bonding
				tx_memsz,	//send buffer size, in K-bytes
				rx_memsz,	//send buffer size, in K-bytes	
				port,		
				cb_newRcv,	// callback while there are receive data
				cb_connected,
				cb_closed,
				cb_listen
			);
			break;
		}
	}
	return dev;
}

static TcpClientDev_t* net_newTcpClient(
	WizDevRsrc *pRsrc, 
	u8 tx_memsz,	//send buffer size, in K-bytes
	u8 rx_memsz,	//send buffer size, in K-bytes
	u16 localport,
	u8* destip,
	u16 destport,		
	void (*cb_newRcv)(u16 rcbBytes),	// callback while there are receive data
	void (*cb_connected)(u8* destip, u16 destport),
	void (*cb_closed)()
){
	u8 i;
	TcpClientDev_t *dev = NULL;
	for(i=0;i<_WIZCHIP_SOCK_NUM_;i++){
		if(pRsrc->sckt[i].protocol == SOCKT_NONE){
			dev = &pRsrc->tcpC[i];
			// new socket alloc in this record
			pRsrc->sckt[i].protocol = SOCKT_TCPC;
			pRsrc->sckt[i].socketDev = dev;
			// initial socket
			TcpClientDev_setup(
				dev,
				i,	// the socket no. bonding
				tx_memsz,	//send buffer size, in K-bytes
				rx_memsz,	//send buffer size, in K-bytes	
				localport,
				destip,		
				destport,
				cb_newRcv,	// callback while there are receive data
				cb_connected,
				cb_closed
			);
			break;
		}
	}
	return dev;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
