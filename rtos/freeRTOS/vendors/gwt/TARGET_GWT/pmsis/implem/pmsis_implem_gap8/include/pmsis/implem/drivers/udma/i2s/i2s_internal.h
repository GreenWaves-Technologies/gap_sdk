/*
 * Copyright (c) 2019, GreenWaves Technologies, Inc.
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

#ifndef __I2S_INTERNAL_H__
#define __I2S_INTERNAL_H__

#include "pmsis.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

struct i2s_itf_data_s
{
    /* Best to use only one queue since both RX & TX can be used at the same time. */
    struct pi_task *fifo_head;  /* Head of SW fifo waiting transfers. */
    struct pi_task *fifo_tail;  /* Tail of SW fifo waiting transfers. */
    uint32_t device_id;
    uint32_t nb_open;
    uint32_t frequency;
    uint32_t frame_clk_freq;
    uint32_t udma_cfg;
    uint32_t pending_size;
    size_t block_size;
    uint8_t i2s_id;
    uint16_t pdm_decimation;
    int8_t pdm_shift;
    uint8_t pdm_filter_ena;
    //uint8_t shift;
    uint8_t word_size;
    uint8_t channels;
    uint8_t clk;
    uint8_t cur_buffer;
    uint8_t cur_read_buffer;
    uint8_t nb_ready_buffer;
    uint8_t reenqueue;
    pi_i2s_fmt_t format;
    pi_i2s_opt_t options;
    void *pending_buffer;
    void *pingpong_buffers[2];
    void **ring_buffer;
    uint32_t ring_buffer_nb_elem;
    uint32_t ring_buffer_head;
    uint32_t ring_buffer_tail;
    pi_mem_slab_t *mem_slab;
};

/*******************************************************************************
 * Driver data
 *****************************************************************************/

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

/**
 * \brief Init i2s configuration structure.
 *
 * \param conf           Pointer to i2s conf.
 */
void __pi_i2s_conf_init(struct pi_i2s_conf *conf);

/**
 * \brief Open i2s device.
 *
 * \param conf           Pointer to i2s conf.
 * \param device_data    Pointer to device data.
 *
 * \retval 0             Operation is successful.
 * \retval ERRNO         Error code.
 */
int32_t __pi_i2s_open(struct pi_i2s_conf *conf, struct i2s_itf_data_s **device_data);

/**
 * \brief Close i2s device.
 *
 * \param i2s_id         ID of i2s interface.
 */
void __pi_i2s_close(struct i2s_itf_data_s *itf_data);

/**
 * \brief Ioctl function.
 *
 * \param i2s_id         ID of i2s interface.
 * \param cmd            Ioctl command.
 * \param arg            Ioctl argument.
 *
 * \retval -1            Uknown command.
 * \retval Value         Ioctl command return value.
 */
int32_t __pi_i2s_ioctl(struct i2s_itf_data_s *itf_data, uint32_t cmd, void *arg);

/**
 * \brief Load data from micro.
 *
 * \param itf_data       Pointer to driver data.
 * \param task           Pointer to struct pi_task.
 */
int32_t __pi_i2s_read_async(struct i2s_itf_data_s *itf_data, pi_task_t *task);

/**
 * \brief Return status.
 *
 * \param task           Pointer to struct pi_task.
 * \param mem_block      Pointer to data buffer.
 * \param size           Pointer to size.
 */
int32_t __pi_i2s_read_status(pi_task_t *task, void **mem_block, size_t *size);

/**
 * \brief Setup misc flags.
 *
 * \param flags          Flags to set.
 */
void __pi_i2s_setup(uint32_t flags);

#endif  /* __I2S_INTERNAL_H__ */
