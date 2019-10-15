/*
 * Copyright (c) 2018, GreenWaves Technologies, Inc.
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
 * o Neither the name of GreenWaves Technologies, Inc. nor the names of its
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

#include "hyperbus_io.h"

/*******************************************************************************
 * Variables, macros, structures,... definitions
 ******************************************************************************/

static malloc_t __hyperram_malloc; /* HyperRam memory allocator. */

/* HyperFlash status register device ready offset. */
#define DEVICE_READY_OFFSET     7
/* Write and read address */
#define SA                      0x0000

/* Sector erase sequence */
static cmdSeq Erase_Seq[6] = {{0xAA, 0x555}, {0x55, 0x2AA}, {0x80, 0x555},
                              {0xAA, 0x555}, {0x55, 0x2AA}, {0x30, SA}};

/* Configure register0 sequence */
static cmdSeq VCR_Seq[4]   = {{0xAA, 0x555}, {0x55, 0x2AA}, {0x38, 0x555}, {0x8e0b, 0x0}};

/* Read status register sequence */
static cmdSeq Reg_Seq      = {0x70, 0x555};

/* Write 512/4 = 128 word to Sector addr 0x4xxx */
static cmdSeq WP_Seq[3]    = {{0xAA, 0x555}, {0x55, 0x2AA}, {0xA0, 0x555}};

/* Variables used to set/read values in/from HyperFlash registers. */
static uint32_t read_val = 0, write_val = 0;

/* Structure holding information about HyperFlash transfers. */
static hyperbus_transfer_t xfer;

/*******************************************************************************
 * Function definition
 ******************************************************************************/

static void HYPERBUS_IO_Pin_Init( uint32_t nbArgs, ... )
{
    PORT_Type *const xPort_addrs[] = PORT_BASE_PTRS;
    port_pin_config_t config = { uPORT_PullUpEnable, uPORT_HighDriveStrength, uPORT_MuxAlt3 };
    PinName pin = 0;
    uint32_t port = 0, pin_nb = 0;

    /* pin out the hyperbus pins */
    va_list list;
    va_start( list, nbArgs );
    for( uint32_t i = 0; i< nbArgs; i++)
    {
        pin = va_arg( list, PinName );
        port = GET_GPIO_PORT( pin );
        pin_nb = GET_GPIO_PIN_NUM( pin );
        PORT_SetPinConfig( xPort_addrs[port], pin_nb, &config );
    }
    va_end( list );
    hyperbus_config_t  hyper_config;

    HYPERBUS_GetDefaultConfig( &hyper_config );
    HYPERBUS_Init( ( HYPERBUS_Type * ) HYPERBUS_BASE_PTRS, &hyper_config, SystemCoreClock );
}

static void HYPERBUS_IO_Flash_Config( void )
{
    /* Set VCR to 5 delay cycles */
    for ( uint32_t i = 0; i < 4; i++ )
    {
        xfer.txData = &VCR_Seq[i].data;
        xfer.txDataSize = 2;
        xfer.rxData = 0;
        xfer.rxDataSize = 0;
        xfer.configFlags = 32;
        xfer.addr = ( VCR_Seq[i].addr << 1 );
        xfer.device = uHYPERBUS_Flash;
        xfer.reg_access = uHYPERBUS_Mem_Access;

        HYPERBUS_TransferBlocking( ( HYPERBUS_Type * ) HYPERBUS_BASE_PTRS, &xfer );
    }
}

int32_t HYPERBUS_IO_Init( uint8_t device )
{
    uint32_t latency = 0;

    HYPERBUS_IO_Pin_Init( 13, HYPERBUS_DQ0, HYPERBUS_DQ1, HYPERBUS_DQ2, HYPERBUS_DQ3,
                   HYPERBUS_DQ4, HYPERBUS_DQ5, HYPERBUS_DQ6, HYPERBUS_DQ7,
                   HYPERBUS_CLK, HYPERBUS_CLKN, HYPERBUS_RWDS, HYPERBUS_CSN0, HYPERBUS_CSN1 );
    if( device == uHYPERBUS_Flash )
    {
        latency = 0;
        #if defined(__GAP8__)
        /* Config memory maximum transfer data length for TX and RX*/
        HYPERBUS_SetMaxLength( ( HYPERBUS_Type * ) HYPERBUS_BASE_PTRS, 1, 0x1ff, 0, uHYPERBUS_Flash );
        HYPERBUS_SetMaxLength( ( HYPERBUS_Type * ) HYPERBUS_BASE_PTRS, 1, 0x1ff, 1, uHYPERBUS_Flash );

        /* Config memory access timing for TX and RX*/
        HYPERBUS_SetTiming( ( HYPERBUS_Type * ) HYPERBUS_BASE_PTRS, 4, 4, 4, latency, 0, uHYPERBUS_Flash );
        HYPERBUS_SetTiming( ( HYPERBUS_Type * ) HYPERBUS_BASE_PTRS, 4, 4, 4, latency, 1, uHYPERBUS_Flash );
        #else
        HYPERBUS_SetTiming( ( HYPERBUS_Type * ) HYPERBUS_BASE_PTRS, 5, 5, 6, 1, 1, 665);
        #endif
        HYPERBUS_IO_Flash_Config();
    }
    else
    {
        latency = 1;
        #if defined(__GAP8__)
        /* Config memory maximum transfer data length for TX and RX*/
        HYPERBUS_SetMaxLength( ( HYPERBUS_Type * ) HYPERBUS_BASE_PTRS, 1, 0x1ff, 0, uHYPERBUS_Ram );
        HYPERBUS_SetMaxLength( ( HYPERBUS_Type * ) HYPERBUS_BASE_PTRS, 1, 0x1ff, 1, uHYPERBUS_Ram );
        /* Config memory access timing for TX and RX*/
        HYPERBUS_SetTiming( ( HYPERBUS_Type * ) HYPERBUS_BASE_PTRS, 4, 4, 4, latency, 0, uHYPERBUS_Ram );
        HYPERBUS_SetTiming( ( HYPERBUS_Type * ) HYPERBUS_BASE_PTRS, 4, 4, 4, latency, 1, uHYPERBUS_Ram );
        #else
        HYPERBUS_SetTiming( ( HYPERBUS_Type * ) HYPERBUS_BASE_PTRS, 6, 6, 6, 1, 1, 665);
        #endif
    }

    return 0;
}

void HYPERBUS_IO_Deinit( void )
{
    HYPERBUS_DeInit( (HYPERBUS_Type *) HYPERBUS_BASE_PTRS );
}

int32_t HYPERBUS_IO_Erase( uint32_t addr )
{
    for ( uint32_t i = 0; i < 5; i++ )
    {
        xfer.txData = &Erase_Seq[i].data;
        xfer.txDataSize = 2;
        xfer.rxData = 0;
        xfer.rxDataSize = 0;
        xfer.configFlags = 32;
        xfer.addr = Erase_Seq[i].addr << 1;
        xfer.device = uHYPERBUS_Flash;
        xfer.reg_access = uHYPERBUS_Mem_Access;

        HYPERBUS_TransferBlocking( ( HYPERBUS_Type * ) HYPERBUS_BASE_PTRS, &xfer );
    }

    xfer.txData = &Erase_Seq[5].data;
    xfer.txDataSize = sizeof( uint16_t );
    xfer.rxData = 0;
    xfer.rxDataSize = 0;
    xfer.configFlags = 32;
    xfer.addr = addr;
    xfer.device = uHYPERBUS_Flash;
    xfer.reg_access = uHYPERBUS_Mem_Access;

    HYPERBUS_TransferBlocking( ( HYPERBUS_Type * ) HYPERBUS_BASE_PTRS, &xfer );

    return 0;
}

int32_t HYPERBUS_IO_Write( uint32_t addr, uint32_t size, void *buf, uint8_t device, uint8_t device_access )
{
    if( device == uHYPERBUS_Flash )
    {
        /* Write to Buffer command sequence */
        for( uint32_t i = 0; i < 3; i++ )
        {
            xfer.txData = &WP_Seq[i].data;
            xfer.txDataSize = 2;
            xfer.rxData = 0;
            xfer.rxDataSize = 0;
            xfer.configFlags = 32;
            xfer.addr = WP_Seq[i].addr << 1;
            xfer.device = uHYPERBUS_Flash;
            xfer.reg_access = uHYPERBUS_Mem_Access;

            HYPERBUS_TransferBlocking( ( HYPERBUS_Type * ) HYPERBUS_BASE_PTRS, &xfer );
        }
    }
    /* Word Program */
    xfer.txData = buf;
    xfer.txDataSize = size;
    xfer.rxData = 0;
    xfer.rxDataSize = 0;
    xfer.configFlags = 32;
    xfer.addr = addr;
    xfer.device = device;
    xfer.reg_access = device_access;

    HYPERBUS_TransferBlocking( ( HYPERBUS_Type * ) HYPERBUS_BASE_PTRS, &xfer );
    return 0;
}

int32_t HYPERBUS_IO_Read( uint32_t addr, uint32_t size, void *buf, uint8_t device, uint8_t device_access )
{
    xfer.txData = 0;
    xfer.txDataSize = 0;
    xfer.rxData = buf;
    xfer.rxDataSize = size;
    xfer.configFlags = 32;
    xfer.addr = addr;
    xfer.device = device;
    xfer.reg_access = device_access;

    HYPERBUS_TransferBlocking( ( HYPERBUS_Type * ) HYPERBUS_BASE_PTRS, &xfer );

    return 0;
}

int32_t HYPERBUS_IO_Sync( void )
{
    uint16_t reg;
    hyperbus_transfer_t xferWrite, xferRead;

    write_val = Reg_Seq.data;
    xferWrite.txData = ( uint16_t * ) &write_val;
    xferWrite.txDataSize = 2;
    xferWrite.rxData = 0;
    xferWrite.rxDataSize = 0;
    xferWrite.configFlags = 32;
    xferWrite.addr = Reg_Seq.addr << 1;
    xferWrite.device = uHYPERBUS_Flash;
    xferWrite.reg_access = uHYPERBUS_Mem_Access;

    xferRead.txData = 0;
    xferRead.txDataSize = 0;
    xferRead.rxData = ( uint16_t * ) &read_val;
    xferRead.rxDataSize = 2;
    xferRead.configFlags = 32;
    xferRead.addr = 0;
    xferRead.device = uHYPERBUS_Flash;
    xferRead.reg_access = uHYPERBUS_Mem_Access;
    /* Wait the end of process
     * Status Register (SR)
     * bit 4 -> program status bit, 0-success ; 1-failure
     * bit 5 -> erase status bit,   0-success ; 1-failure
     * bit 7 -> device ready bit,   0-busy    ; 1-ready
     */
    do
    {
        HYPERBUS_TransferBlocking( ( HYPERBUS_Type * ) HYPERBUS_BASE_PTRS, &xferWrite );
        HYPERBUS_TransferBlocking( ( HYPERBUS_Type * ) HYPERBUS_BASE_PTRS, &xferRead );
        reg = ( ( read_val >> 16 ) & 0xffff );
    } while( !( reg & ( 1 << DEVICE_READY_OFFSET ) ) );

    return 0;
}

uint32_t HYPERBUS_IO_Malloc_Init( void *addr, uint32_t size )
{
    __hyperram_malloc.first_free = NULL;
    __hyperram_malloc.type = EXTERNAL_MALLOC;
    return __malloc_extern_init( &__hyperram_malloc, addr, size );
}

void HYPERBUS_IO_Malloc_Deinit()
{
    __malloc_extern_deinit( &__hyperram_malloc );
}

void *HYPERBUS_IO_Malloc( uint32_t size )
{
    return __malloc_extern( &__hyperram_malloc, size );
}

uint32_t HYPERBUS_IO_MallocFree( void *chunk, uint32_t size )
{
    return __malloc_extern_free( &__hyperram_malloc, chunk, size );
}
