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
#include "pmsis/implem/hal/hal.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*
 * pi_task:
 * data[0] = l2_buf
 * data[1] = size
 * data[2] = flags
 * data[3] = channel
 * data[4] = p_cs_data
 * data[5] = repeat_size
 */

/* Length of i2c cmd buffer. */
#define __PI_I2C_CMD_BUFF_SIZE (16)
/* Lenght of i2c stop command sequence. */
#define __PI_I2C_STOP_CMD_SIZE (3)

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

struct i2c_cs_data_s
{
    uint8_t device_id;          /*!< I2C interface ID. */
    uint8_t cs;                 /*!< Chip select i2c device. */
    uint16_t clk_div;           /*!< Clock divider for the selected i2c chip. */
    uint32_t max_baudrate;      /*!< Max baudrate for the selected i2c chip. */
    struct i2c_cs_data_s *next; /*!< Pointer to next i2c cs data struct. */
};

struct i2c_itf_data_s
{
    /* Best to use only one queue since both RX & TX can be used at the same time. */
    struct pi_task *hw_buffer[2];                 /*!< RX + TX */
    struct pi_task *fifo_head;                    /*!< Head of SW fifo waiting transfers. */
    struct pi_task *fifo_tail;                    /*!< Tail of SW fifo waiting transfers. */
    struct i2c_pending_transfer_s *pending;       /*!< RX + TX. */
    uint32_t nb_open;                             /*!< Number of devices opened. */
    uint32_t i2c_cmd_index;                       /*!< Number of commands in i2c_cmd_seq. */
    pi_freq_cb_t i2c_freq_cb;                     /*!< Callback associated to frequency changes. */
    struct i2c_cs_data_s *cs_list;                /*!< List of i2c associated to this itf. */
    uint8_t i2c_cmd_seq[__PI_I2C_CMD_BUFF_SIZE];  /*!< Command sequence. */
    uint8_t i2c_stop_send;                        /*!< Set if a stop cmd seq should be sent. */
    uint8_t i2c_stop_seq[__PI_I2C_STOP_CMD_SIZE]; /*!< Command STOP sequence. */
    uint8_t device_id;                            /*!< I2C interface ID. */
    /* This variable is used to count number of events received to handle EoT sequence. */
    uint8_t nb_events;                            /*!< Number of events received. */
};

/*******************************************************************************
 * Driver data
 *****************************************************************************/

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

/* Init i2c conf struct. */
void __pi_i2c_conf_init(pi_i2c_conf_t *conf);

/* Open i2c device. */
int32_t __pi_i2c_open(struct pi_i2c_conf *conf, struct i2c_cs_data_s **device_data);

/* Close i2c device. */
void __pi_i2c_close(struct i2c_cs_data_s *device_data);

/* Ioctl function. */
void __pi_i2c_ioctl(struct i2c_cs_data_s *device_data, uint32_t cmd, void *arg);

/* Copy in UDMA. */
void __pi_i2c_copy(struct i2c_cs_data_s *cs_data, uint32_t l2_buff, uint32_t length,
                   pi_i2c_xfer_flags_e flags, udma_channel_e channel,
                   struct pi_task *task);

/* Scan i2c bus to detect connected devices. */
int32_t __pi_i2c_detect(struct i2c_cs_data_s *cs_data, struct pi_i2c_conf *conf,
                        uint8_t *rx_data, struct pi_task *task);

#endif  /* __I2C_INTERNAL_H__ */
