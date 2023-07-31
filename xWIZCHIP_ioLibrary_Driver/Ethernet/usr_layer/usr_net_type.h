/**********************************************************
filename: usr_net_type.h
**********************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _USR_NET_TYPE_H
#define _USR_NET_TYPE_H
#include "misc.h"
//#include "wizchip_conf.h"

/**
 * @ingroup DATA_TYPE
 *  It used in setting dhcp_mode of @ref wiz_NetInfo.
 */
 
typedef struct {
	uint8_t ip[4];
	uint16_t port;
	uint8_t mac[6];
} ShortNetInfo_t;
 
typedef enum
{
	SOCKT_NONE = 0,	// NONE
	SOCKT_TCPS	= 1,	//TCP SEVERVER
	SOCKT_TCPC	= 2,	//TCP CLIENT
	SOCKT_UDP	= 3
}SOCKT_PROTOCOL;

#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
