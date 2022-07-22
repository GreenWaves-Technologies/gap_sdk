/*
 * Copyright (c) 2021, GreenWaves Technologies, Inc.
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

#pragma once

#include "pmsis.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Stop bit at the end of transfer. */
#define PI_I3C_I2C_XFER_STOP ( 1 << 0 )


struct pi_i3c_i2c_conf_s
{
    uint8_t itf;                /*!< Device interface number. */
    uint8_t cs;                 /*!< i2c slave address (7 bits), the
                                  runtime will take care of the LSB of read and write. */
    uint32_t baudrate;          /*!< Device frequency. */
};


struct i3c_itf_data_s
{
    /* struct pi_task *fifo_head; */
    /* struct pi_task *fifo_tail; */
    pi_udma_fifo_t udma_chan;
    uint8_t device_id;
    uint8_t cs;
    uint32_t nb_open;
};

/*******************************************************************************
 * Driver data
 ******************************************************************************/

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

void pi_i3c_i2c_conf_init(struct pi_i3c_i2c_conf_s *conf);

int pi_i3c_i2c_open(pi_device_t *device);

void pi_i3c_i2c_close(pi_device_t *device);

int pi_i3c_i2c_read(pi_device_t *device, uint8_t *rx_buff, uint32_t length,
                    uint8_t flags);

int pi_i3c_i2c_read_async(pi_device_t *device, uint8_t *rx_buff, uint32_t length,
                          uint8_t flags, struct pi_task *task);

int pi_i3c_i2c_write(pi_device_t *device, uint8_t *tx_buff, uint32_t length,
                     uint8_t flags);

int pi_i3c_i2c_write_async(pi_device_t *device, uint8_t *tx_buff, uint32_t length,
                           uint8_t flags, struct pi_task *task);


void __pi_i3c_i2c_conf_init(struct pi_i3c_i2c_conf_s *conf);

int32_t __pi_i3c_i2c_open(struct pi_i3c_i2c_conf_s *conf,
                          struct i3c_itf_data_s **device_data);

void __pi_i3c_i2c_close(struct i3c_itf_data_s *device_data);

void __pi_i3c_i2c_copy(struct i3c_itf_data_s *device_data, uint32_t l2_buff,
                       uint32_t length, uint8_t flags, udma_channel_e channel,
                       struct pi_task *task);
