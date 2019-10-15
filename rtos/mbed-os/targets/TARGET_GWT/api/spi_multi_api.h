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

#ifndef MBED_SPI_MULTI_H
#define MBED_SPI_MULTI_H

#include "gap_spi.h"
#include "spi_api.h"


#ifdef __cplusplus
extern "C" {
#endif

void spis_init(spi_t *obj, PinName mosi, PinName miso, PinName sclk, PinName ssel, PinName sdio2, PinName sdio3);

/** Control spi master chip select high or low
 *
 * @param[in] obj   The SPI peripheral to use for sending
 * @param[in] status   Indicate spi chip select low or high
 * @return Returns the transfer status
 */
int spi_master_cs(spi_t *obj, int status);

/** Specify I/O width of SPI transaction (Quad SPI or not)
 *
 * @param[in] obj   The SPI peripheral to use for sending
 * @param[in] qpi   Choose Quad SPI or normal SPI
 */
int spi_master_qspi(spi_t *obj, spi_qpi_t qpi);

/** SPI auto polling
 *
 * @param[in] obj   The SPI peripheral to use for sending
 * @param[in] conf  The configuration of auto polling
 */
int spi_master_auto_polling(spi_t *obj, spi_polling_config_t *conf);

/** SPI blocking sequence transaction
 *
 * This function will create sequence according to sequence configuration data
 * And then do blocking transaction read or write
 *
 * @param[in] obj   The SPI peripheral to use for sending
 * @param[in] seq   The command sequence configuration data
 */
int spi_master_transfer_command_sequence(spi_t *obj, spi_command_sequence_t* seq);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // MBED_SPI_MULTI_H
