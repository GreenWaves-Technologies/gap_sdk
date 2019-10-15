/******************************************************************************************
 *
 * Copyright (c) 2018 , GreenWaves Technologies, Inc.
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
 *******************************************************************************************/


/****************************************************************************/
/* Includes. */
#include "spi_flash_io.h"

/****************************************************************************/

static void SPI_FLASH_Pin_Init( PinName mosi, PinName miso, PinName sclk, PinName ssel )
{
    /* pin out the spi pins */
    pinmap_pinout(mosi, PinMap_SPI_MOSI);
    pinmap_pinout(miso, PinMap_SPI_MISO);
    pinmap_pinout(sclk, PinMap_SPI_SCLK);
    pinmap_pinout(ssel, PinMap_SPI_SSEL);
    /* Get default Master config */
    SPI_MasterGetDefaultConfig( &master_config );

    /* determine the SPI to use */
    #if defined(__GAP8__)
    if(ssel == SPI0_CSN1 || ssel == SPI1_CSN1_B2 || ssel == SPI1_CSN1_B15)
        master_config.whichCsn = uSPI_csn1;
    else
        master_config.whichCsn = uSPI_csn0;
    #elif defined(__GAP9__)
        master_config.whichCsn = uSPI_csn0;
    #endif
}
/*-----------------------------------------------------------*/

static void SPI_FLASH_Format( uint32_t bits, uint32_t mode, uint32_t slave )
{
    /* Master config */
    master_config.bitsPerFrame = ( uint32_t ) bits;
    master_config.cpol = (mode & 0x2) ? uSPI_ClockPolarityActiveLow : uSPI_ClockPolarityActiveHigh;
    master_config.cpha = (mode & 0x1) ? uSPI_ClockPhaseSecondEdge : uSPI_ClockPhaseFirstEdge;

    SPI_MasterInit( spi_address[master_config.whichCsn], &master_config, SystemCoreClock );
}
/*-----------------------------------------------------------*/

static void SPI_FLASH_Frequency( uint32_t hz )
{
    /* Configuration only clock frequence */
    master_config.clkDiv = ( SystemCoreClock >> 1) / hz;
    SPI_MasterFrequencyConfig( spi_address[master_config.whichCsn],  &master_config );
}
/*-----------------------------------------------------------*/

static void SPI_FLASH_Conf( uint8_t qspi )
{
    master_config.qpi = qspi;
    /* Dummy cycles */
    {
        s_command_sequence[0] = SPIM_CMD_SOT( master_config.whichCsn );
        SPI_MasterTransferBlocking( spi_address[master_config.whichCsn],
                                    ( uint32_t * ) s_command_sequence,
                                    sizeof( uint32_t ), NULL, 0, 32 );
        s_command_sequence[0] = SPIM_CMD_SEND_CMD( 0x06, 8, uSPI_Single );
        SPI_MasterTransferBlocking( spi_address[master_config.whichCsn],
                                    ( uint32_t * ) s_command_sequence,
                                    sizeof( uint32_t ), NULL, 0, 32 );
        s_command_sequence[0] = SPIM_CMD_EOT( 0 );
        SPI_MasterTransferBlocking( spi_address[master_config.whichCsn],
                                    ( uint32_t * ) s_command_sequence,
                                    sizeof( uint32_t ), NULL, 0, 32 );

        memset( &s_command, 0, sizeof(spi_command_sequence_t) );
        s_command.cmd       = 0x71;
        s_command.cmd_bits  = 8;
        s_command.addr_bits = 32;
        s_command.addr      = ( 0x80000380 | DUMMY );
        s_command.cmd_mode  = uSPI_Single;
        s_command.addr_mode = uSPI_Single;
        s_command.csn = master_config.whichCsn;
        SPI_MasterTransferCommandSequence( spi_address[master_config.whichCsn], &s_command );
    }
    /* Set page size to 512 */
    {
        s_command_sequence[0] = SPIM_CMD_SOT( master_config.whichCsn );
        SPI_MasterTransferBlocking( spi_address[master_config.whichCsn],
                                    ( uint32_t * ) s_command_sequence,
                                    sizeof( uint32_t ), NULL, 0, 32 );
        s_command_sequence[0] = SPIM_CMD_SEND_CMD( 0x06, 8, uSPI_Single );
        SPI_MasterTransferBlocking( spi_address[master_config.whichCsn],
                                    ( uint32_t * ) s_command_sequence,
                                    sizeof( uint32_t ), NULL, 0, 32 );
        s_command_sequence[0] = SPIM_CMD_EOT( 0 );
        SPI_MasterTransferBlocking( spi_address[master_config.whichCsn],
                                    ( uint32_t * ) s_command_sequence,
                                    sizeof( uint32_t ), NULL, 0, 32 );
        memset(&s_command, 0, sizeof(spi_command_sequence_t));
        s_command.cmd       = 0x71;
        s_command.cmd_bits  = 8;
        s_command.addr_bits = 32;
        s_command.addr      = 0x00800004;
        s_command.cmd_mode  = uSPI_Single;
        s_command.addr_mode = uSPI_Single;
        s_command.tx_bits   = 8;
        s_command.tx_data   = 0x10 | 0x08;/* 512B/page | uniform sectors of 64kB */
        s_command.data_mode = uSPI_Single;
        s_command.csn = master_config.whichCsn;
        SPI_MasterTransferCommandSequence( spi_address[master_config.whichCsn], &s_command );
    }
    if( master_config.qpi )
    {
        /* Enable QPI mode. */
        /* Exit if not SPI0. */
        if( master_config.whichCsn )
            exit(-1);
        pinmap_pinout(SPI0_SDIO2, PinMap_QSPI_SDIO2);
        pinmap_pinout(SPI0_SDIO3, PinMap_QSPI_SDIO3);

        s_command_sequence[0] = SPIM_CMD_SOT( master_config.whichCsn );
        SPI_MasterTransferBlocking( spi_address[master_config.whichCsn],
                                    ( uint32_t * ) s_command_sequence,
                                    sizeof( uint32_t ), NULL, 0, 32 );
        s_command_sequence[0] = SPIM_CMD_SEND_CMD( 0x06, 8, uSPI_Single );
        SPI_MasterTransferBlocking( spi_address[master_config.whichCsn],
                                    ( uint32_t * ) s_command_sequence,
                                    sizeof( uint32_t ), NULL, 0, 32 );
        s_command_sequence[0] = SPIM_CMD_EOT( 0 );
        SPI_MasterTransferBlocking( spi_address[master_config.whichCsn],
                                    ( uint32_t * ) s_command_sequence,
                                    sizeof( uint32_t ), NULL, 0, 32 );
        /* QPI Enable : CR2V[6] = 1 */
        memset(&s_command, 0, sizeof(spi_command_sequence_t));
        s_command.cmd       = 0x71;
        s_command.cmd_bits  = 8;
        s_command.addr_bits = 32;
        s_command.addr      = 0x00800003;
        s_command.cmd_mode  =  uSPI_Single;
        s_command.addr_mode =  uSPI_Single;
        s_command.tx_bits   = 8;
        s_command.tx_data   = 0xC0 | DUMMY;/* QPI | 4-byte addr | latency(4rd bit)(default) */
        s_command.data_mode = uSPI_Single;
        s_command.csn = master_config.whichCsn;
        SPI_MasterTransferCommandSequence( spi_address[master_config.whichCsn], &s_command );
    }
}
/*-----------------------------------------------------------*/

static int32_t SPI_FLASH_Id( void )
{
    int32_t id = 0, spi_reg_value = 0;

    s_command_sequence[0] = SPIM_CMD_SOT( master_config.whichCsn );
    SPI_MasterTransferBlocking( spi_address[master_config.whichCsn],
                                ( uint32_t * ) s_command_sequence,
                                sizeof( uint32_t ), NULL, 0, 32 );
    s_command_sequence[0] = SPIM_CMD_SEND_CMD( 0x9f, 8, master_config.qpi );
    SPI_MasterTransferBlocking( spi_address[master_config.whichCsn],
                                ( uint32_t * ) s_command_sequence,
                                sizeof( uint32_t ), NULL, 0, 32 );

    s_command_sequence[0] = SPIM_CMD_RX_DATA(32, master_config.qpi, 0);
    SPI_MasterTransferBlocking( spi_address[master_config.whichCsn],
                                ( uint32_t * ) s_command_sequence,
                                sizeof( uint32_t ), ( uint8_t * ) &spi_reg_value,
                                4, 32);
    s_command_sequence[0] = SPIM_CMD_EOT( 0 );
    SPI_MasterTransferBlocking( spi_address[master_config.whichCsn],
                                ( uint32_t * ) s_command_sequence,
                                sizeof( uint32_t ), NULL, 0, 32 );
    if( master_config.bitsPerFrame == 8 )
        id = spi_reg_value & 0xff;
    else if( master_config.bitsPerFrame == 16 )
        id = spi_reg_value & 0xffff;
    else
        id = spi_reg_value;
    return id;
}

/*-----------------------------------------------------------*/

int32_t SPI_FLASH_Init( PinName mosi, PinName miso, PinName sclk, PinName ssel,
                  uint32_t bits, uint32_t mode, uint32_t hz, uint8_t qspi, uint8_t polling )
{
    SPI_FLASH_Pin_Init(mosi, miso, sclk, ssel );

    /* SPI bits, cpha, cpol configuration. */
    SPI_FLASH_Format( bits, mode, 0 );

    /* Set fequence. */
    SPI_FLASH_Frequency( hz );

    /* Flash Configuration. */
    SPI_FLASH_Conf( qspi );
    syncPolling = polling;

    return 0;
}
/*-----------------------------------------------------------*/

void SPI_FLASH_Deinit( void )
{
    SPI_MasterDeInit( spi_address[master_config.whichCsn] );
}
/*-----------------------------------------------------------*/

int32_t SPI_FLASH_Erase( uint32_t addr, uint32_t size )
{
    int32_t prvSize = ( size / ( 64 * 1024 ) );
    if( !prvSize )
        prvSize = 1;
    uint32_t prvAddr = addr;

    do
    {
        s_command_sequence[0] = SPIM_CMD_SOT( master_config.whichCsn );
        SPI_MasterTransferBlocking( spi_address[master_config.whichCsn],
                                    ( uint32_t * ) s_command_sequence,
                                    sizeof( uint32_t ), NULL, 0, 32 );
        s_command_sequence[0] = SPIM_CMD_SEND_CMD( 0x06, 8, master_config.qpi );
        SPI_MasterTransferBlocking( spi_address[master_config.whichCsn],
                                    ( uint32_t * ) s_command_sequence,
                                    sizeof( uint32_t ), NULL, 0, 32 );
        s_command_sequence[0] = SPIM_CMD_EOT( 0 );
        SPI_MasterTransferBlocking( spi_address[master_config.whichCsn],
                                    ( uint32_t * ) s_command_sequence,
                                    sizeof( uint32_t ), NULL, 0, 32 );

        memset(&s_command, 0, sizeof(spi_command_sequence_t));
        s_command.cmd       = 0xD8;/* 0xD8 : 64kb or 256kB erase | 0x20 : 4kB erase. */
        s_command.cmd_bits  = 8;
        s_command.addr_bits = 32;
        s_command.addr      = prvAddr;
        s_command.csn = master_config.whichCsn;
        if( !master_config.qpi )
        {
            s_command.cmd_mode  = uSPI_Single;
            s_command.addr_mode = uSPI_Single;
        }
        else
        {
            s_command.cmd_mode  = uSPI_Quad;
            s_command.addr_mode = uSPI_Quad;
        }
        SPI_MasterTransferCommandSequence( spi_address[master_config.whichCsn], &s_command );

        prvAddr = ( prvAddr + 512 * 128 );/* next sector address */
        prvSize -= 1; /* 1 sector deleted */

        if( syncPolling )
            SPI_FLASH_Sync_Polling();
        else
            SPI_FLASH_Sync();
    }
    while( prvSize > 0 );
    return 0;
}
/*-----------------------------------------------------------*/

int32_t SPI_FLASH_Write( uint32_t addr, uint32_t size, void *buf )
{
    int32_t prvSize = ( size / 512 );
    if( !prvSize )
        prvSize = 1;
    uint32_t prvAddr = addr;
    uint8_t *prvBuffer = ( uint8_t * ) buf;
    do
    {
        s_command_sequence[0] = SPIM_CMD_SOT( master_config.whichCsn );
        SPI_MasterTransferBlocking( spi_address[master_config.whichCsn],
                                    ( uint32_t * ) s_command_sequence,
                                    sizeof( uint32_t ), NULL, 0, 32 );
        s_command_sequence[0] = SPIM_CMD_SEND_CMD( 0x06, 8, master_config.qpi );
        SPI_MasterTransferBlocking( spi_address[master_config.whichCsn],
                                    ( uint32_t * ) s_command_sequence,
                                    sizeof( uint32_t ), NULL, 0, 32 );
        s_command_sequence[0] = SPIM_CMD_EOT( 0 );
        SPI_MasterTransferBlocking( spi_address[master_config.whichCsn],
                                    ( uint32_t * ) s_command_sequence,
                                    sizeof( uint32_t ), NULL, 0, 32 );

        memset(&s_command, 0, sizeof(spi_command_sequence_t));
        s_command.cmd_bits  = 8;
        s_command.addr_bits = 32;
        s_command.addr      = prvAddr;
        s_command.tx_bits   = 512*8;/* 1 page of 512B */
        s_command.tx_data   = 0;
        s_command.tx_buffer = prvBuffer;
        s_command.csn = master_config.whichCsn;
        if( !master_config.qpi )
        {
            s_command.cmd       = 0x02;
            s_command.cmd_mode  = uSPI_Single;
            s_command.addr_mode = uSPI_Single;
            s_command.data_mode = uSPI_Single;
        }
        else
        {
            s_command.cmd       = 0x12;
            s_command.cmd_mode  = uSPI_Quad;
            s_command.addr_mode = uSPI_Quad;
            s_command.data_mode = uSPI_Quad;
        }
        SPI_MasterTransferCommandSequence( spi_address[master_config.whichCsn], &s_command );

        prvBuffer = ( prvBuffer + 512 );/* next buffer data for next page data */
        prvAddr = prvAddr + 512;/* next page address */
        prvSize--;

        if( syncPolling )
            SPI_FLASH_Sync_Polling();
        else
            SPI_FLASH_Sync();
    }
    while( prvSize > 0 );
    return 0;
}
/*-----------------------------------------------------------*/

int32_t SPI_FLASH_Read( uint32_t addr, uint32_t size, void *buf )
{
    memset(&s_command, 0, sizeof(spi_command_sequence_t));
    s_command.cmd_bits  = 8;
    s_command.addr_bits = 32;
    s_command.addr      = addr;
    s_command.rx_bits   = size*8;
    s_command.dummy     = DUMMY;
    s_command.rx_buffer = buf;
    s_command.csn = master_config.whichCsn;
    if( !master_config.qpi )
    {
        s_command.cmd       = 0x0C;
        s_command.cmd_mode  = uSPI_Single;
        s_command.addr_mode = uSPI_Single;
        s_command.data_mode = uSPI_Single;
    }
    else
    {
        s_command.cmd       = 0xEB;
        s_command.cmd_mode  = uSPI_Quad;
        s_command.addr_mode = uSPI_Quad;
        s_command.data_mode = uSPI_Quad;
        s_command.alter_data_bits = 8;
        s_command.alter_data      = 0x0A;
        s_command.alter_data_mode = uSPI_Quad;
    }
    SPI_MasterTransferCommandSequence( spi_address[master_config.whichCsn], &s_command );
    return 0;
}
/*-----------------------------------------------------------*/

int32_t SPI_FLASH_Sync( void )
{
    int16_t read_status;
    do {
        {
            s_command_sequence[0] = SPIM_CMD_SOT( master_config.whichCsn );
            SPI_MasterTransferBlocking( spi_address[master_config.whichCsn],
                                        ( uint32_t * ) s_command_sequence,
                                        sizeof( uint32_t ), NULL, 0, 32 );
            s_command_sequence[0] = SPIM_CMD_SEND_CMD( 0x05, 8, master_config.qpi );
            SPI_MasterTransferBlocking( spi_address[master_config.whichCsn],
                                        ( uint32_t * ) s_command_sequence,
                                        sizeof( uint32_t ), NULL, 0, 32 );
            s_command_sequence[0] = SPIM_CMD_RX_DATA(32, master_config.qpi, 0);
            SPI_MasterTransferBlocking( spi_address[master_config.whichCsn],
                                        ( uint32_t * ) s_command_sequence,
                                        sizeof( uint32_t ), ( uint8_t * ) &read_status,
                                        4, 32);
            s_command_sequence[0] = SPIM_CMD_EOT( 0 );
            SPI_MasterTransferBlocking( spi_address[master_config.whichCsn],
                                        ( uint32_t * ) s_command_sequence,
                                        sizeof( uint32_t ), NULL, 0, 32 );
            if( master_config.bitsPerFrame == 8 )
                read_status = read_status & 0xff;
            else if( master_config.bitsPerFrame == 16 )
                read_status = read_status & 0xffff;
            else
                read_status = read_status;
        }
    } while( read_status & ( 0x1 << S25FS256_SR_WIP ) );
    return 0;
}
/*-----------------------------------------------------------*/

int32_t SPI_FLASH_Sync_Polling( void )
{
    spi_polling_config_t config;
    config.csn         = 0;
    config.cmd         = 0x05;
    config.cmd_mode    = uSPI_Quad;
    config.match_type  = uSPI_Match_Bits_Zero;
    config.match_bits  = 8;
    config.match_value = 0xffff & ~( 0x1 << S25FS256_SR_WIP );
    config.data_mode   = uSPI_Quad;
    config.time_out    = 0xf000;

    SPI_Master_AutoPolling( spi_address[master_config.whichCsn], &config );
    return 0;
}
/*-----------------------------------------------------------*/
