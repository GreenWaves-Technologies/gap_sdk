/* 
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#pragma once

#include "pmsis.h"

typedef struct
{
    int itf;
    int transfer_size;
    int sample_rate;
    int nb_slots;
    int word_size;
} testlib_i2s_trafficgen_config_t;


typedef struct
{
    pi_device_t dev;
    uint32_t frame;
    void *tx_buffers[2];
    void *rx_buffers[2];
    pi_task_t rx_tasks[2];
    pi_task_t tx_tasks[2];
    int tx_pending;
    int tx_current_task;
    pi_task_t tx_end_task;
    int rx_pending;
    int rx_current_task;
    pi_task_t rx_end_task;
    int transfer_size;
    int end;
} testlib_i2s_trafficgen_t;


void testlib_i2s_trafficgen_conf_init(testlib_i2s_trafficgen_config_t *config);
int testlib_i2s_trafficgen_init(testlib_i2s_trafficgen_t *data, testlib_i2s_trafficgen_config_t *config);
int testlib_i2s_trafficgen_start(testlib_i2s_trafficgen_t *data);
int testlib_i2s_trafficgen_stop(testlib_i2s_trafficgen_t *data);
int testlib_i2s_trafficgen_deinit(testlib_i2s_trafficgen_t *data);
