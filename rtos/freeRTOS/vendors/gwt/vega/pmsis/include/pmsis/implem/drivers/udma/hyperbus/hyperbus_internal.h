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

/**
 * pi_task:
 * data[0] = l2_buf
 * data[1] = size
 * data[2] = channel
 * data[3] = hyper_addr
 * data[4] = stride
 * data[5] = length
 * data[6] = is_emulated ?
 * data[7] = repeat_size
 */

struct hyper_itf_data_s
{
    pi_udma_fifo_t udma_chan;   /* HW-SW event tasks. */
    uint32_t nb_open;
    uint32_t pending_emu_buffer;
    uint32_t pending_emu_hyper_addr;
    uint32_t pending_emu_size;
    uint32_t pending_emu_size_2d;
    udma_channel_e pending_emu_channel;
    uint8_t pending_emu_do_memcpy;
    uint8_t device_id;          /* Device ID. */
};

struct hyper_itf_conf_s
{
    uint8_t device_id;
    uint8_t cs;
    uint8_t clk_div;
    //uint8_t latency;
    pi_hyper_type_e hyper_type;
};

struct hyper_transfer_s
{
    uint32_t hyper_addr;
    uint32_t buffer;
    uint32_t size;
    uint32_t stride;
    uint32_t length;
    udma_channel_e channel;
};


/*******************************************************************************
 * Driver data
 ******************************************************************************/

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

/**
 * \brief Init hyperbus configuration structure.
 *
 * \param conf           Pointer to hyperbus conf.
 */
void __pi_hyper_conf_init(struct pi_hyper_conf *conf);

/**
 * \brief Open hyperbus device.
 *
 * \param conf           Pointer to hyperbus conf.
 * \param driver_conf    Pointer to internal hyperbus conf pointer.
 *
 * \retval 0             Operation is successful.
 * \retval ERRNO         Error code.
 */
int32_t __pi_hyper_open(struct pi_hyper_conf *conf,
                        struct hyper_itf_conf_s **driver_conf);

/**
 * \brief Close hyperbus device.
 *
 * \param conf           Pointer to internal hyperbus conf.
 */
void __pi_hyper_close(struct hyper_itf_conf_s *conf);

/**
 * \brief Enqueue 1D transfer.
 *
 * \param transfer       Pointer to transfer struct.
 * \param task           Event task attached to the transfer.
 */
void __pi_hyper_copy(struct hyper_itf_conf_s *conf,
                     struct hyper_transfer_s *transfer, struct pi_task *task);

/**
 * \brief Enqueue 2D transfer.
 *
 * \param conf           Pointer to driver config struct.
 * \param transfer       Pointer to transfer struct.
 * \param task           Event task attached to the transfer.
 */
void __pi_hyper_copy_2d(struct hyper_itf_conf_s *conf,
                        struct hyper_transfer_s *transfer, struct pi_task *task);

int32_t __pi_hyper_ioctl(struct hyper_itf_conf_s *conf, uint32_t cmd, void *arg);
