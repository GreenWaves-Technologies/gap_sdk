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

#pragma once

#include "pmsis.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* CLK_SEL. */
#define PI_I2S_OPT_CLK_SRC_SEL_SHIFT ( 13 )
#define PI_I2S_OPT_CLK_MST_OFFSET    ( 0 )
#define PI_I2S_OPT_CLK_SLV_OFFSET    ( 1 )

#define PI_I2S_OPT_CLK_SRC_MST ( PI_I2S_OPT_CLK_MST_OFFSET << PI_I2S_OPT_CLK_SRC_SEL_SHIFT ) /*!< Clock source is from master pin. */
#define PI_I2S_OPT_CLK_SRC_SLV ( PI_I2S_OPT_CLK_SLV_OFFSET << PI_I2S_OPT_CLK_SRC_SEL_SHIFT ) /*!< Clock source is from slave pin. */

/* DUAL PHYS CHAN. */
#define PI_I2S_OPT_DUAL_MODE_SHIFT ( 14 )
/*!< Activate dual channel mode, using both SDI0 and SDI1 or SDO0 and SDO1. */
#define PI_I2S_OPT_DUAL_MODE_ENA   ( 1 << PI_I2S_OPT_DUAL_MODE_SHIFT )


/**
 * pi_task :
 * task->data[0] = l2_buf;
 * task->data[1] = size;
 * task->data[2] = channel;
 * task->data[3] = udma_cfg;
 * task->data[4] = device_id;
 * task->data[5] = use_dbl_buf;
 */

typedef struct pi_ring_buff_s
{
    void **buffer;
    uint32_t nb_elem;
    uint32_t head;
    uint32_t tail;
} pi_ring_buff_t;

struct i2s_itf_data_s
{
    pi_udma_fifo_t udma_chan;
    //struct pi_task *fifo_head;  /* Head of fifo. */
    //struct pi_task *fifo_tail;  /* Tail of fifo. */
    //void *pingpong_buffers[2];  /* Pingpong buffers. */
    pi_ring_buff_t ring_buffer; /* Ring buffers. */
    uint32_t nb_open;
    uint32_t flags;             /*!< Options for i2s(rx/tx/fdx/clk_sel/msb). */
    uint32_t frequency;         /*!< i2s frequency(based on frame_rate, channels, wordsize). */
    uint32_t clk_setup;         /*!< Clock setup(MST, SLV, PDM). */
    uint32_t block_size;        /*!< Size of a block. */
    uint32_t nb_ready_buffer;
    uint16_t pdm_decimation;    /*!< Decimation factor, in log2. */
    uint8_t device_id;          /*!< Periph ID. */
    uint8_t i2s_id;             /*!< i2s ID(two i2s periph per device(RX and TX)). */
    uint8_t word_size;          /*!< Width of samples (8, 16, 32 bits). */
    uint8_t channels;           /*!< Number of channels used(mono/stereo). */
    uint8_t pdm;                /*!< PDM enable. */
    //uint8_t ;
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
 * \param itf_data       Pointer to driver data.
 */
void __pi_i2s_close(struct i2s_itf_data_s *itf_data);

/**
 * \brief Ioctl function.
 *
 * \param itf_data       Pointer to driver data.
 * \param cmd            Ioctl command.
 * \param arg            Ioctl argument.
 *
 * \retval -1            Uknown command.
 * \retval Value         Ioctl command return value.
 */
int32_t __pi_i2s_ioctl(struct i2s_itf_data_s *itf_data, uint32_t cmd, void *arg);

/**
 * \brief Write data into memory.
 *
 * \param itf_data       Pointer to driver data.
 * \param task           Pointer to struct pi_task.
 */
int32_t __pi_i2s_read_async(struct i2s_itf_data_s *itf_data, pi_task_t *task);

/**
 * \brief Return status of current transfer(data to memory).
 *
 * \param task           Pointer to struct pi_task.
 * \param mem_block      Pointer to data buffer.
 * \param size           Pointer to size.
 */
int32_t __pi_i2s_read_status(pi_task_t *task, void **mem_block, size_t *size);

/**
 * \brief Send data from memory.
 *
 * \param itf_data       Pointer to driver data.
 * \param buffer         Pointer to data buffer.
 * \param size           Size of data buffer.
 * \param task           Pointer to struct pi_task.
 */
int32_t __pi_i2s_write_async(struct i2s_itf_data_s *itf_data, void *buffer,
                             size_t size, struct pi_task *task);

/**
 * \brief Return status of current transfer(data to memory).
 *
 * \param task           Pointer to struct pi_task.
 *
 * \retval 0             If transfer is done.
 * \retval Value         Size of remaining data to completion.
 */
int32_t __pi_i2s_write_status(struct pi_task *task);
