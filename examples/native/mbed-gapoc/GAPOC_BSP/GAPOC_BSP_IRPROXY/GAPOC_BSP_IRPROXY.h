// File GAPOC_BSP_IRPROXY.h

#ifndef  __GAPOC_BSP_IRPROXY_H
#define  __GAPOC_BSP_IRPROXY_H  // prevent multiple inclusions


// ==  Includes relied upon in this file   ========================================

#include "K80_PROXY_REGISTERS.h"
#include "GAPOC_BSP_General.h"


// ==  Custom Types  ==============================================================

// <none>

// ==  Public Functions Prototypes        ============================================


void GAPOC_IRProxy_SPI_Init(uint32_t spi_fqcy_khz);

void GAPOC_IRProxy_WriteReg12(uint8_t RegAddr, uint16_t WriteVal);     // RegAddr to fit on 4 bits and WriteVal on 12 bits


#endif  // __GAPOC_BSP_IRPROXY_H
