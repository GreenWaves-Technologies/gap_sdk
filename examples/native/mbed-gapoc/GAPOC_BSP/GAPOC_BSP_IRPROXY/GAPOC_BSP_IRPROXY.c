// File: GAPOC_BSP_IRPROXY.c

/*
 * Copyright (c) 2019, GreenWaves Technologies
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of GreenWaves Technologies nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 
 
// ====  Handy "defines" for application, used locally   =======================

// <none>


// ====  Includes    ===========================================================

#include "GAPOC_BSP_IRPROXY.h"


 
// ====  Imported Global Variables  ============================================

// <none>

// ====  Exported Global Variables  ============================================

// <none>


// ====  File-wide Variables  ===================================================

static GAP_L2_DATA   spi_t   spim1;      
static GAP_L2_DATA  uint16_t  spi_word16;



// #############################################################################
// ##### FUNCTION DEFINITIONS        ###########################################
 
 
 /** Function : GAPOC_IRProxy_SPI_Init 
    Action : Initializes SPI1 to be able to talk with IR Proxy with specifed baudrate
*/

void GAPOC_IRProxy_SPI_Init(uint32_t spi_fqcy_khz)
{
        // SPI pins init, SPI udma channel init 
        spi_init(&spim1, SPI1_MOSI, NC, SPI1_SCLK, SPI1_CSN0_A5);
//        spi_init(&spim1, B3, SPI1_MISO, B4, SPI1_CSN0_A5);


        // SPI bits, cpha, cpol configuration 
        spi_format(&spim1, 16, 0, 0);  // 16-bit words, idle level =low
            // BEWARE - For some reason when doing 16-bit SPI transfers, SPI send MSB first in ech byte and LSByte first 
            
        // Set SPI fequency *
        spi_frequency(&spim1, spi_fqcy_khz*1000);

        DBG_PRINT("\nSPI1 initalized\n");
           
}


// -----------------------------------------------------------------

/** Function : GAPOC_IRProxy_WriteReg12 
    Action : Using SPI1, Writes value WriteVal into 12-bit register of the IR Proxy specified by its 4-bit address RegAddr
*/
     
void GAPOC_IRProxy_WriteReg12(uint8_t RegAddr, uint16_t WriteVal)  // RegAddr to fit on 4 bits and WriteVal on 12 bits
{

#if GAPOC_DEBUG == 1
    if ( (RegAddr >>4) != 0x0)
    {
        DBG_PRINT("Reg. Address must fit on 4 bits !  -- Stopping Here\n");
        while(1);
    }
    if ( (WriteVal >>12) != 0x0)
    {
        DBG_PRINT("Reg. Value must fit on 12 bits !  -- Stopping Here\n");
        while(1);
    }
#endif

    spi_word16 =  (RegAddr <<12) | (WriteVal );  // Addr = 4 MSB, Value = 12 LSB

    // Ulis Proxy wants 16-bit with MSB first : A3.A2.A1.A0.D11.D10.......D1.D0
    // while (when doing 16-bit SPI transfers) SPI send MSB first in ech byte and LSByte first (strange!?)
    // So swap MSBbyte and LSBbyte
    spi_word16 =  (spi_word16 & 0xFF) <<8 | spi_word16 >>8 ; 
                
    spi_master_cs(&spim1, 0);            
    spi_master_write(&spim1, spi_word16);
        // NB - When doing 16-bit SPI transfers, it appears SPI send MSB first in ech byte and LSByte first ??!!??       
    spi_master_cs(&spim1, 1);    
}


// ###   END OF FILE  ##################################################################################################################
