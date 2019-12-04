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

#ifndef __I2C_INTERNAL_H__
#define __I2C_INTERNAL_H__

#include "pmsis.h"
#include "pmsis/task.h"
#include "pmsis/drivers/i2c.h"
#include "pmsis_hal/pmsis_hal.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Max length of a i2c request/data buffer. */
#define MAX_SIZE               (0xFF)
/* Length of i2c cmd buffer. */
#define __PI_I2C_CMD_BUFF_SIZE (16)
/* Lenght of i2c stop command sequence. */
#define __PI_I2C_STOP_CMD_SIZE (3)

struct i2c_transfer_s
{
    uint8_t *buffer;
    uint32_t size;
    pi_i2c_xfer_flags_e flags;
    int8_t device_id;
    udma_channel_e channel;
};

struct i2c_pending_transfer_s
{
    uint32_t pending_buffer;
    uint32_t pending_size;
    uint32_t pending_repeat;
    uint32_t pending_repeat_size;
    pi_i2c_xfer_flags_e flags;
    int8_t device_id;
    udma_channel_e channel;
};

struct i2c_cb_args_s
{
    struct pi_task *cb;
    struct i2c_transfer_s transfer;
    struct i2c_cb_args_s *next;
};

struct i2c_driver_fifo_s
{
    /* Best to use only one queue since both RX & TX can be used at the same time. */
    struct pi_task *hw_buffer[2];                 /* RX + TX */
    struct i2c_cb_args_s *fifo_head;              /* Head of SW fifo waiting transfers. */
    struct i2c_cb_args_s *fifo_tail;              /* Tail of SW fifo waiting transfers. */
    struct i2c_pending_transfer_s *pending;       /* RX + TX. */
    uint32_t cs;                                  /* Chip select i2c device. */
    uint32_t max_baudrate;                        /* Max baudrate for the selected i2c chip. */
    uint32_t div;                                 /* Clock divider for the selected i2c chip. */
    uint32_t i2c_cmd_index;                       /* Number of commands in i2c_cmd_seq. */
    uint8_t i2c_cmd_seq[__PI_I2C_CMD_BUFF_SIZE];  /* Command sequence. */
    uint8_t i2c_stop_send;                        /* Set if a stop command sequence should be sent. */
    uint8_t i2c_stop_seq[__PI_I2C_STOP_CMD_SIZE]; /* Command STOP sequence. */
};

/*******************************************************************************
 * Driver data
 *****************************************************************************/

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

/* Copy in UDMA. */
void __pi_i2c_copy(struct i2c_driver_fifo_s *fifo, struct i2c_transfer_s *transfer, struct pi_task *task);

/* Handler. */
void i2c_handler(void *arg);

/* Clock divider. */
uint32_t __pi_i2c_get_clk_div(uint32_t baudrate);

#endif  /* __I2C_INTERNAL_H__ */
