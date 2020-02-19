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

struct i2s_driver_fifo_s
{
    /* Best to use only one queue since both RX & TX can be used at the same time. */
    struct pi_task *fifo_head;  /* Head of SW fifo waiting transfers. */
    struct pi_task *fifo_tail;  /* Tail of SW fifo waiting transfers. */
    uint32_t device_id;
    uint32_t frequency;
    uint32_t frame_clk_freq;
    uint32_t udma_cfg;
    uint32_t nb_open;
    uint32_t pending_size;
    size_t block_size;
    uint8_t i2s_id;
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

void __pi_i2s_conf_init(struct pi_i2s_conf *conf);

int32_t __pi_i2s_open(struct pi_i2s_conf *conf);

void __pi_i2s_close(uint8_t i2s_id);

int32_t __pi_i2s_ioctl(uint8_t i2s_id, uint32_t cmd, void *arg);

int32_t __pi_i2s_read(uint8_t i2s_id, void **mem_block, size_t *size);

int32_t __pi_i2s_read_async(uint8_t i2s_id, pi_task_t *task);

int32_t __pi_i2s_read_status(pi_task_t *task, void **mem_block, size_t *size);

int32_t __pi_i2s_write(uint8_t i2s_id, void *mem_block, size_t size);

#endif  /* __I2S_INTERNAL_H__ */
