#include "crc.h"
#include "stdint.h"

uint16_t crc;
int crcBits;
uint8_t crcByte;

/* Calculate CRC16 value */
uint16_t CRC16_Calc (uint8_t* data, uint16_t length, uint16_t initVal){
	crc = initVal;
	while(length--){
		crcBits = 8;
		crcByte = *data++;
		while(crcBits--){
			if((crcByte & 1) ^ (crc & 1)){
				crc = (crc >> 1) ^ CRC16_POLYNOM;
			}else{
				crc >>= 1;
			}
			crcByte >>= 1;
		}
	}
	crc = 65535-crc;
	return crc;
}
