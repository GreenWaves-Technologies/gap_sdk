/*
 * Copyright (C) 2022 GreenWaves Technologies
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "stdlib.h"
#include "pmsis.h"
#include "pmsis/drivers/i2c.h"
#include "udma_i2c.h"
#include <string.h>

#if !defined(__FREERTOS__)
#if !defined(__TRACE_ALL__) && !defined(__TRACE_I2C__)
#define I2C_TRACE(x...)
#define I2C_TRACE_ERR(...)         ((void) 0)
#else
#define I2C_TRACE(level, x...) POS_TRACE(level, "[I2C] " x)
#define I2C_TRACE_ERR(...)         PI_LOG_ERR(__func__, __VA_ARGS__)
#endif
#endif

/*! @brief UART request structure. */
#define  i2c_req_t udma_req_t

#define I2C_READ 0
#define I2C_WRITE 1
#define I2C_WRITE_READ 2
#define I2C_WRITE_DUAL 3

#define I2C_CMD_BUF_SIZE   14

#define I2C_BUF_START_POS   6
#define I2C_BUF_WRITE_RPT_POS 8

#define I2C_W_R_BUF_WR_POS 2
#define I2C_W_R_BUF_RESTART_POS 4
#define RD_BUF_RPT(bits) ((bits) ? 10 : 8)
#define W_R_BUF_RD(bits) ((bits) ? 8  : 6)

#define RD_BUF_STOP_ID(bits) (RD_BUF_RPT((bits)) + 3)
#define WR_BUF_STOP_ID 10

#define I2C_SLAVE_GET_ITF(slave) (slave->itf_data)


typedef uint32_t i2c_cmd_t;

typedef struct i2c_slave_data
{
    struct i2c_itf_data *itf_data;
    uint32_t cfg;
    // a slave might answer to up to two addresses, 7 or 10 bits
    int16_t wait_cycles;
    uint16_t slave_addr;
    uint16_t slave_addrh;
    int8_t is_10_bits;
} i2c_slave_data_t;

typedef struct i2c_itf_data {
    uint32_t base;
    pi_task_t *fifo_head;
    pi_task_t *fifo_tail;
    pi_task_t *end_task;
    i2c_cmd_t cmd_buf[I2C_CMD_BUF_SIZE];
    uint32_t rx_chan_addr;
    uint32_t tx_chan_addr;
    uint32_t cmd_chan_addr;
    // per itf
    uint8_t open_nb;
    uint8_t id;
    // --- channel event ---
    int8_t rx_chan_id;
    int8_t tx_chan_id;
    int8_t cmd_chan_id;
    // --- timeout channel ---
    uint8_t rx_timeout_id;
    uint8_t tx_timeout_id;
} i2c_itf_data_t;


// Has to be synchronized with irq_disabled since irq handler might pop at the same time
static inline void __pi_i2c_drv_fifo_enqueue(struct i2c_itf_data *data,
        pi_task_t *pi_task)
{
    if (data->fifo_head)
    {
        data->fifo_tail->next = pi_task;
    }
    else
    {
        data->fifo_head = pi_task;
    }

    pi_task->next = NULL;
    data->fifo_tail = pi_task;
}

static inline pi_task_t *__pi_i2c_drv_fifo_pop(struct i2c_itf_data *data)
{
    pi_task_t *ret_task = data->fifo_head;
    if (ret_task)
    {
        data->fifo_head = ret_task->next;
    }
    return ret_task;
}

/*
 * @brief compute input divisor for i2c ip
 * Input divisor is not the same as hw/real divisor, so compute an input
 * divisor as near as possible to satisfy constraint
 */
static inline uint32_t __i2c_prepare_timing(uint32_t max_baudrate,
        uint32_t periph_clock)
{
    pi_i2c_mode_e mode;
    // TODO: HW, discuss plain divisor, this makes no sense
    uint32_t input_div = 0, hw_div=0, targetL, targetH, divH, divL, cmd;

    // second, determine mode:
    if(max_baudrate < 200000)
    {
        mode = PI_I2C_STD_MODE;
    }
    else if(max_baudrate >= 200000 && max_baudrate < 750000)
    {
        mode = PI_I2C_FAST_MODE;
    }
    else if(max_baudrate >= 750000)
    {
        mode = PI_I2C_FAST_MODE_PLUS;
    }

    // choose target L and H (depend on std = 100KHz, fast mode=400KHz)
    // we try to get the best frequency, without going faster than what standard
    // allows
    switch(mode)
    {
        case PI_I2C_STD_MODE: // 100KHz - up to 200
            targetL = 4700; // 4.7 µs
            targetH = 4000; // 4.0 µs
            break;
        case PI_I2C_FAST_MODE: // 400KHz
            targetL = 1300; // 1.3 µs
            targetH = 600;  // 0.6 µs
            break;
        case PI_I2C_FAST_MODE_PLUS: // 1MHz
            targetL = 500; // 0.5 µs
            targetH = 260; // 0.260 µs
            break;
        default:
            return PI_FAIL;
    }

    /* timing method extracted from IP designer python script */
    input_div = 0;
    divL = 15;
    divH = 15;

    uint32_t dio = 3;
    uint32_t f_pclk_mega = periph_clock / 1000000;
    input_div = targetL * f_pclk_mega / (1000 * (15 + 1 + dio));
    divL = targetL * f_pclk_mega / (1000 * (input_div + 1)) - dio;
    divH = targetH * f_pclk_mega / (1000 * (input_div + 1)) - dio;

    if (divL > 15)
    {
        input_div = input_div + 1;
        divL = 1000 * targetL * f_pclk_mega / (input_div + 1) - dio;
        divH = 1000 * targetH * f_pclk_mega / (input_div + 1) - dio;
    }

    cmd = I2C_CMD_TIMING(((input_div&0xFF) << 8) | ((divH&0xF) << 4)
            | (divL&0xF));
    return cmd;
}

void __pi_i2c_timeout_abort(void* arg);

static inline void __pi_i2c_timeout_config_set(pi_task_t *task, uint8_t timeout_id,
                                               uint8_t udma_chan_id, uint32_t timeout_us,
                                               pi_callback_func_t abort_func,
                                               void *arg)
{
#if defined(__FREERTOS__)
    pi_udma_timeout_config_set(task, timeout_id, udma_chan_id, timeout_us);
    pi_task_timeout_callback_set(task, abort_func, arg);
#endif
}
