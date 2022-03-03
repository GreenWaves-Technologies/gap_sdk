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
#include "pmsis/drivers/i2c_slave.h"

#ifndef I2C_DRIVER_DATA_IMPLEM_SPECIFC
    #define I2C_DRIVER_DATA_IMPLEM_SPECIFC
#endif

/*! @brief UART request structure. */
#define  i2c_req_t udma_req_t 

typedef uint32_t i2c_cmd_t;

struct i2c_slave_itf_data {
    uint32_t base;

    // per itf
    uint32_t rx_chan_addr;
    uint32_t tx_chan_addr;
    uint32_t cmd_chan_addr;
    
    uint8_t open_nb;
    uint8_t id;
    // --- channel event ---
    uint8_t rx_chan_id;
    uint8_t tx_chan_id;
    uint8_t cmd_chan_id;
    // -- addresses to which we answer
    uint8_t addr0_10_bit;
    uint8_t addr1_10_bit;
    uint8_t addr0_mask;
    uint8_t addr1_mask;
    uint16_t addr[4];
    // in handler callbacks
    pi_i2c_callback_t rx_callback;
    pi_i2c_callback_t tx_callback;
    // buffers
    void *rx_buffer;
    uint32_t rx_buffer_size;
    void *tx_buffer;
    uint32_t tx_buffer_size;
    I2C_DRIVER_DATA_IMPLEM_SPECIFC
};
