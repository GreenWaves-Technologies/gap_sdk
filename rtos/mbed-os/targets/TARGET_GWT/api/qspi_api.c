/* mbed Microcontroller Library
 * Copyright (c) 2018, ARM Limited
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of STMicroelectronics nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#if DEVICE_QSPI

#include "qspi_api.h"
#include "mbed_error.h"
#include "cmsis.h"
#include "pinmap.h"
#include "PeripheralPins.h"
#include "spi_multi_api.h"

qspi_status_t qspi_init(qspi_t *obj, PinName io0, PinName io1, PinName io2, PinName io3, PinName sclk, PinName ssel, uint32_t hz, uint8_t mode)
{
    spi_init(obj, io0, io1, sclk, ssel);
    spi_format(obj, 8, 0, 0);
    spi_frequency(obj, hz);
    spi_master_qspi(obj, uSPI_Quad);

    return QSPI_STATUS_OK;
}

qspi_status_t qspi_free(qspi_t *obj)
{
    spi_free(obj);

    return QSPI_STATUS_OK;
}

qspi_status_t qspi_frequency(qspi_t *obj, int hz)
{
    spi_frequency(obj, hz);

    return QSPI_STATUS_OK;
}

static void qspi_prepare_command(qspi_t *obj, const qspi_command_t *command, const void *tx_data, size_t tx_size, void *rx_data, size_t rx_size)
{
    spi_command_sequence_t seq;

    memset(&seq, 0, sizeof(spi_command_sequence_t));

    /* instruction */
    assert(command->instruction.bus_width != QSPI_CFG_BUS_DUAL);

    if (command->instruction.disabled)
        seq.cmd_bits = 0;
    else
        seq.cmd_bits = 8;
    seq.cmd      = command->instruction.value;
    seq.cmd_mode = ((command->instruction.bus_width == QSPI_CFG_BUS_SINGLE) ? uSPI_Single : uSPI_Quad);

    /* address */
    assert(command->address.bus_width != QSPI_CFG_BUS_DUAL);

    if (command->address.disabled)
        seq.addr_bits = 0;
    else
        seq.addr_bits = (command->address.size + 1) << 3;
    seq.addr      = command->address.value;
    seq.addr_mode = ((command->address.bus_width == QSPI_CFG_BUS_SINGLE) ? uSPI_Single : uSPI_Quad);

    /* alt */
    assert(command->alt.bus_width != QSPI_CFG_BUS_DUAL);

    if (command->alt.disabled)
        seq.alter_data_bits = 0;
    else
        seq.alter_data_bits = (command->alt.size + 1) << 3;
    seq.alter_data =  command->alt.value;
    seq.alter_data_mode = ((command->alt.bus_width == QSPI_CFG_BUS_SINGLE) ? uSPI_Single : uSPI_Quad);

    seq.dummy = command->dummy_count;

    assert(command->data.bus_width != QSPI_CFG_BUS_DUAL);
    seq.data_mode = ((command->data.bus_width == QSPI_CFG_BUS_SINGLE) ? uSPI_Single : uSPI_Quad);

    if (tx_size) {
        seq.tx_bits = tx_size << 3;

        if (seq.tx_bits <= 32)
        {
            seq.tx_data = *((int *)tx_data);
        } else {
            seq.tx_buffer = (uint8_t*)tx_data;
        }
    } else {
        seq.tx_bits = 0;
    }

    if (rx_size) {
        seq.rx_bits   = rx_size << 3;
        seq.rx_buffer = (uint8_t*)rx_data;
    } else {
        seq.rx_bits = 0;
    }

    spi_master_transfer_command_sequence(obj, &seq);
}

qspi_status_t qspi_write(qspi_t *obj, const qspi_command_t *command, const void *data, size_t *length)
{
    uint32_t to_write = *length;
    uint8_t *data_send = (uint8_t *)data;

    /* Enforce word-sized access */
    if ((to_write & 0x3) != 0) {
        return QSPI_STATUS_INVALID_PARAMETER;
    }

    /* Prepare the write command */
    qspi_prepare_command(obj, command, data, to_write, NULL, 0);

    return QSPI_STATUS_OK;
}

qspi_status_t qspi_read(qspi_t *obj, const qspi_command_t *command, void *data, size_t *length)
{
    uint32_t to_read = *length;

    /* Prepare for read command */
    qspi_prepare_command(obj, command, NULL, 0, data, to_read);

    return QSPI_STATUS_OK;
}

qspi_status_t qspi_command_transfer(qspi_t *obj, const qspi_command_t *command, const void *tx_data, size_t tx_size, void *rx_data, size_t rx_size)
{
    qspi_prepare_command(obj, command, tx_data, tx_size, rx_data, rx_size);

    return QSPI_STATUS_OK;
}

const PinMap *qspi_master_sclk_pinmap()
{
    return PinMap_SPI_SCLK;
}

const PinMap *qspi_master_ssel_pinmap()
{
    return PinMap_SPI_SSEL;
}

const PinMap *qspi_master_data0_pinmap()
{
    return PinMap_QSPI_SDIO0;
}

const PinMap *qspi_master_data1_pinmap()
{
    return PinMap_QSPI_SDIO1;
}

const PinMap *qspi_master_data2_pinmap()
{
    return PinMap_QSPI_SDIO2;
}

const PinMap *qspi_master_data3_pinmap()
{
    return PinMap_QSPI_SDIO3;
}

#endif
