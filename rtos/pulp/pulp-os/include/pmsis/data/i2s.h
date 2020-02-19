/*
 * Copyright (C) 2018 GreenWaves Technologies
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

#ifndef __PMSIS_DATA_I2S_H__
#define __PMSIS_DATA_I2S_H__

#ifndef LANGUAGE_ASSEMBLY

#include "pmsis/task.h"

typedef struct {
    uint32_t pending_size;
    char * pending_buffer;
    uint8_t reenqueue;
    uint8_t clk;
    uint8_t open_count;
    uint8_t current_buffer;
    uint8_t current_read_buffer;
    uint8_t nb_ready_buffer;
    uint8_t channel;
    uint8_t is_pingpong;
    struct pi_i2s_conf conf;
    pi_task_t *waiting_first;
    pi_task_t *waiting_last;
    int i2s_freq;
    uint32_t udma_cfg;
    void **ring_buffer;
    int ring_buffer_nb_elem;
    int ring_buffer_head;
    int ring_buffer_tail;
} pos_i2s_t;

#endif

#define POS_I2S_T_OPEN_COUNT      0

#endif
