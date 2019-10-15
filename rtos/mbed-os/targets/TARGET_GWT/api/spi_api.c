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

#include <math.h>
#include "mbed_assert.h"

#if DEVICE_SPI

#include "cmsis.h"
#include "pinmap.h"
#include "mbed_error.h"
#include "PeripheralPins.h"
#include "spi_multi_api.h"


#if DEVICE_SPI_ASYNCH
#define SPI_S(obj)    (( struct spi_s *)(&(obj->spi)))
#else
#define SPI_S(obj)    (( struct spi_s *)(obj))
#endif


/* Array of SPIM peripheral base address. */
static SPIM_Type *const spi_address[] = SPIM_BASE_PTRS;

/* SPI read return value */
static uint32_t spi_reg_value;

/* SPI transfer command sequence array */
static uint32_t s_command_sequence[6];

/* SPI master global configuration */
static spi_master_config_t master_config;

void spi_init(spi_t *obj, PinName mosi, PinName miso, PinName sclk, PinName ssel)
{
    struct spi_s *spi_obj = SPI_S(obj);
    uint32_t spi_mosi = pinmap_peripheral(mosi, PinMap_SPI_MOSI);
    uint32_t spi_miso = pinmap_peripheral(miso, PinMap_SPI_MISO);
    uint32_t spi_sclk = pinmap_peripheral(sclk, PinMap_SPI_SCLK);
    uint32_t spi_data = pinmap_merge(spi_mosi, spi_miso);
    spi_obj->instance = pinmap_merge(spi_sclk, spi_data);

    MBED_ASSERT((int)spi_obj->instance != NC);

    /* pin out the spi pins */
    pinmap_pinout(mosi, PinMap_SPI_MOSI);
    pinmap_pinout(miso, PinMap_SPI_MISO);
    pinmap_pinout(sclk, PinMap_SPI_SCLK);

    if (ssel != NC)
        pinmap_pinout(ssel & 0xFF, PinMap_SPI_SSEL);
    else
        pinmap_pinout(SPI0_CSN0 & 0xFF, PinMap_SPI_SSEL);

    /* Get default Master config */
    SPI_MasterGetDefaultConfig(&master_config);

    /* determine the SPI to use */
    #if defined(__GAP8__)
    if(ssel == SPI0_CSN1 || ssel == SPI1_CSN1_B2 || ssel == SPI1_CSN1_B15)
        master_config.whichCsn = uSPI_csn1;
    else
        master_config.whichCsn = uSPI_csn0;
    #elif defined(__GAP9__)
        master_config.whichCsn = uSPI_csn0;
    #endif

    /* Set the transfer status to idle */
    spi_obj->status = uSPI_Idle;
}

void spi_free(spi_t *obj)
{
    struct spi_s *spi_obj = SPI_S(obj);
    SPI_MasterDeInit(spi_address[spi_obj->instance]);
}

void spi_format(spi_t *obj, int bits, int mode, int slave)
{
    struct spi_s *spi_obj = SPI_S(obj);

    /* Bits: values between 8 and 32 are valid */
    MBED_ASSERT(bits == 8 || bits == 16 || bits == 32);
    /* Only valid for RX*/
    spi_obj->bits = bits;

    /* Master config */
    master_config.bitsPerFrame = (uint32_t)bits;
    master_config.cpol = (mode & 0x2) ? uSPI_ClockPolarityActiveLow : uSPI_ClockPolarityActiveHigh;
    master_config.cpha = (mode & 0x1) ? uSPI_ClockPhaseSecondEdge : uSPI_ClockPhaseFirstEdge;

    SPI_MasterInit(spi_address[spi_obj->instance], &master_config, SystemCoreClock);
}

void spi_frequency(spi_t *obj, int hz)
{
    struct spi_s *spi_obj = SPI_S(obj);
    uint16_t clk_div = (SystemCoreClock >> 1) / hz;

    /* Configuration only clock frequence */
    master_config.clkDiv = clk_div;
    master_config.baudRate = hz;

    SPI_MasterFrequencyConfig(spi_address[spi_obj->instance],  &master_config);
}

int spi_master_qspi(spi_t *obj, spi_qpi_t qpi)
{
    struct spi_s *spi_obj = SPI_S(obj);
    master_config.qpi= qpi;

    if (qpi == uSPI_Quad) {
        /* Only SPIM0 support QPI mode */
        if (spi_obj->instance)
            return -1;

        uint32_t spi_sdio2 = pinmap_peripheral(SPI0_SDIO2, PinMap_QSPI_SDIO2);
        uint32_t spi_sdio3 = pinmap_peripheral(SPI0_SDIO3, PinMap_QSPI_SDIO3);
        uint32_t spi_data  = pinmap_merge(spi_sdio2, spi_sdio3);

        MBED_ASSERT(spi_obj->instance == spi_data);

        /* SPI pin binding */
        pinmap_pinout(SPI0_SDIO2, PinMap_QSPI_SDIO2);
        pinmap_pinout(SPI0_SDIO3, PinMap_QSPI_SDIO3);
    }
    return 0;
}

int spi_master_cs(spi_t *obj, int status)
{
    struct spi_s *spi_obj = SPI_S(obj);

    SPI_Master_CS(spi_address[spi_obj->instance], master_config.whichCsn, status);

    return 0;
}

int spi_master_write(spi_t *obj, int value)
{
    struct spi_s *spi_obj = SPI_S(obj);

    SPI_Master_DulpexTransfer(spi_address[spi_obj->instance], value, (void *)&spi_reg_value, spi_obj->bits);

    if(spi_obj->bits == 8)
        return spi_reg_value & 0xFF;
    else if(spi_obj->bits == 16)
        return spi_reg_value & 0xFFFF;
    else
        return spi_reg_value;
}

int spi_master_auto_polling(spi_t *obj, spi_polling_config_t *conf)
{
    struct spi_s *spi_obj = SPI_S(obj);
    return SPI_Master_AutoPolling(spi_address[spi_obj->instance], conf);
}

int spi_master_transfer_command_sequence(spi_t *obj, spi_command_sequence_t* seq)
{
    struct spi_s *spi_obj = SPI_S(obj);

    seq->csn = master_config.whichCsn;

    SPI_MasterTransferCommandSequence(spi_address[spi_obj->instance], seq);

    return 0;
}

int spi_master_block_write(spi_t *obj, const char *tx_buffer, int tx_length,
                           char *rx_buffer, int rx_length, char write_fill) {
    int total = (tx_length > rx_length) ? tx_length : rx_length;

    for (int i = 0; i < total; i++) {
        char out = (i < tx_length) ? tx_buffer[i] : write_fill;
        spi_master_cs(obj, 0);
        char in = spi_master_write(obj, out);
        spi_master_cs(obj, 1);

        if (i < rx_length) {
            rx_buffer[i] = in;
        }
    }

    return total;
}

#if DEVICE_SPI_ASYNCH

static int spi_master_tx_transfer_prepare(spi_t *obj, int tx_bits)
{
    int index = 0;

    s_command_sequence[index++] = SPIM_CMD_TX_DATA(tx_bits, master_config.qpi, 0);

    /* Blocking transfer */
    SPI_MasterTransferBlocking(spi_address[obj->spi.instance],
                               (uint32_t*)s_command_sequence,
                               (index * sizeof(uint32_t)),
                               NULL, 0, 32);
    return 0;
}

static int spi_master_rx_transfer_prepare(spi_t *obj, int rx_bits)
{
    int index = 0;

    s_command_sequence[index++] = SPIM_CMD_RX_DATA(rx_bits, master_config.qpi, 0);

    /* Blocking transfer */
    SPI_MasterTransferBlocking(spi_address[obj->spi.instance],
                               (uint32_t*)s_command_sequence,
                               index * sizeof(uint32_t),
                               NULL, 0, 32);
    return 0;
}

static int32_t spi_master_transfer_asynch(spi_t *obj, UDMAHint hint)
{
    spi_transfer_t masterXfer;
    int32_t status = 0;

    /*Start master transfer*/
    masterXfer.txData = obj->tx_buff.buffer;
    masterXfer.txDataSize = obj->tx_buff.length;
    masterXfer.rxData = obj->rx_buff.buffer;
    masterXfer.rxDataSize = obj->rx_buff.length;
    masterXfer.configFlags = obj->rx_buff.width;

    /* Busy transferring */
    obj->spi.status = uSPI_Busy;

    if(masterXfer.txDataSize)
        spi_master_tx_transfer_prepare(obj, masterXfer.txDataSize * obj->tx_buff.width);
    else if(masterXfer.rxDataSize)
        spi_master_rx_transfer_prepare(obj, masterXfer.rxDataSize * obj->rx_buff.width);

    status = SPI_MasterTransferNonBlocking(spi_address[obj->spi.instance], &obj->spi.spi_master_handle, &masterXfer);

    return status;
}

static void spi_buffer_set(spi_t *obj, const void *tx, uint32_t tx_length, void *rx, uint32_t rx_length, uint8_t bit_width)
{
    obj->tx_buff.buffer = (void *)tx;
    obj->rx_buff.buffer = rx;
    obj->tx_buff.length = tx_length;
    obj->rx_buff.length = rx_length;
    obj->tx_buff.pos = 0;
    obj->rx_buff.pos = 0;
    obj->tx_buff.width = bit_width;
    obj->rx_buff.width = bit_width;
}

void spi_master_transfer(spi_t *obj, const void *tx, size_t tx_length, void *rx, size_t rx_length, uint8_t bit_width, uint32_t handler, uint32_t event, DMAUsage hint)
{
    /* Asynchronious UDMA transfer */
    if(spi_active(obj)) {
        return;
    }

    /* check corner case */
    if(tx_length == 0) {
        tx = (void*) 0;
    }

    if(rx_length == 0) {
        rx = (void*) 0;
    }

    /* First, set the buffer */
    spi_buffer_set(obj, tx, tx_length, rx, rx_length, bit_width);

    /* Save handler */
    SPI_MasterTransferCreateHandle(spi_address[obj->spi.instance], &obj->spi.spi_master_handle, (spi_master_transfer_callback_t)handler, NULL);

    /* Start the transfer */
    /*
     * Remove hint, unused in spi_master_transfer_asynch
     */
    if (spi_master_transfer_asynch(obj, hint) != uStatus_Success) {
        obj->spi.status = uSPI_Idle;
    }
}

uint32_t spi_irq_handler_asynch(spi_t *obj)
{
    obj->spi.status = uSPI_Idle;

    return SPI_EVENT_COMPLETE;
}

void spi_abort_asynch(spi_t *obj)
{
    // If we're not currently transferring, then there's nothing to do here
    if(spi_active(obj) == 0) {
        return;
    }

    obj->spi.status = uSPI_Idle;
}

uint8_t spi_active(spi_t *obj)
{
    return obj->spi.status;
}
#endif

void spis_init(spi_t *obj, PinName mosi, PinName miso, PinName sclk, PinName ssel, PinName sdio2, PinName sdio3) {
    struct spi_s *spi_obj = SPI_S(obj);
    uint32_t spi_mosi = pinmap_peripheral(mosi, PinMap_SPIS_SDIO1);
    uint32_t spi_miso = pinmap_peripheral(miso, PinMap_SPIS_SDIO0);
    uint32_t spi_sclk = pinmap_peripheral(sclk, PinMap_SPIS_CLK);
    uint32_t spi_ssel = pinmap_peripheral(ssel, PinMap_SPIS_CSN);
    uint32_t spi_sdio2 = pinmap_peripheral(sdio2, PinMap_SPIS_SDIO2);
    uint32_t spi_sdio3 = pinmap_peripheral(sdio3, PinMap_SPIS_SDIO3);

    uint32_t spi_data0 = pinmap_merge(spi_mosi, spi_miso);
    uint32_t spi_data1 = pinmap_merge(spi_sdio2, spi_sdio3);
    uint32_t spi_data = pinmap_merge(spi_data0, spi_data1);
    uint32_t spi_cntl = pinmap_merge(spi_sclk, spi_ssel);

    spi_obj->instance = pinmap_merge(spi_data, spi_cntl);
    MBED_ASSERT(ssel != NC);

    /* pin out the spi pins */
    pinmap_pinout(mosi, PinMap_SPIS_SDIO1);
    pinmap_pinout(miso, PinMap_SPIS_SDIO0);
    pinmap_pinout(sclk, PinMap_SPIS_CLK);
    pinmap_pinout(ssel, PinMap_SPIS_CSN);
    pinmap_pinout(sdio2, PinMap_SPIS_SDIO2);
    pinmap_pinout(sdio3, PinMap_SPIS_SDIO3);
}

int spi_busy(spi_t *obj)
{
    struct spi_s *spi_obj = SPI_S(obj);

    return spi_obj->status;
}
#endif
