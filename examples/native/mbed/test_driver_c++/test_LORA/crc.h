#ifndef CRC_H
#define CRC_H
#ifdef __cplusplus
extern "C"{
#endif 
#include <stdint.h>
#include <string.h>
/* Constant Variabled */
static const uint16_t CRC16_INIT_VALUE=0xFFFF;
static const uint16_t CRC16_GOOD_VALUE=0x0F47;
static const uint16_t CRC16_POLYNOM=0x8408;

/* Declare Functions */
uint16_t CRC16_Calc (uint8_t* data, uint16_t length, uint16_t initVal);
#ifdef __cplusplus
}
#endif
#endif

