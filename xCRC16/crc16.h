#ifndef __crc16_h__
#define __crc16_h__

#include <stdint.h>
void CRC8_Init(void);
uint8_t crc8(const uint8_t *ptr, int count);

uint16_t CRC16(const uint8_t *ptr, int count, uint16_t crcInit);

#endif
