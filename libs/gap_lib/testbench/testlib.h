/* 
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */


#ifndef __TESTLIB_H__
#define __TESTLIB_H__

#include "pmsis.h"
#include "testbench.h"

#define I2S_SLOT_STATIC_INIT {0}

typedef struct
{
    int itf;
    int sampling_freq;
    int word_size;
    int nb_slots;
    int full_duplex;
    uint8_t is_ext_clk;
    uint8_t is_ext_ws;
    uint8_t is_sai0_clk;
    uint8_t is_sai0_ws;
    uint8_t clk_polarity;
    uint8_t ws_polarity;
    int options;
    uint64_t rx_slots_width;
    uint64_t tx_slots_width;
    uint64_t rx_slots_format;
    uint64_t tx_slots_format;
    uint8_t  use_fast_clk;
    uint64_t fast_clk_freq;
} i2s_config_t;


typedef struct
{
    int itf;
    int slot;
    int is_rx;
    int word_size;
    int nb_elem;
    int elem_size;
    unsigned int format;
    int mute_delay;
    int mute_delay_start;
    int mute_delay_incr;
    int mute_delay_end;
    int random_mute;
    int ts_evt_en;
    int slot_disable;
    union
    {
        struct
        {
        } rx;
        struct
        {
        } tx;
    };
}
i2s_slot_config_t;


typedef enum
{
    I2S_VERIF_RX_ITER,
    I2S_VERIF_TX_ITER
} i2s_slot_type_e;


typedef struct
{
    uint32_t type;
    uint32_t nb_samples;

    pi_testbench_i2s_verif_slot_start_config_t testbench_config;

    union
    {
        struct
        {
            int32_t nb_samples;   // Number of samples that should be produced
            uint32_t incr_start;   // When using incr mode, gives first value
            uint32_t incr_end;     // When using incr mode, gives last value from which it should start again from incr_start
            uint32_t incr_value;   // If different from 0, activate incr mode and give the increment after each sample
        } tx_iter;
    };
}
i2s_slot_start_config_t;

typedef struct 
{
    struct pi_device *i2s;
    pi_task_t end_task;
    unsigned int format;
    int retval;
    int itf;
    int slot;
    int word_size;
    int elem_size;
    pi_task_t task;
    uint32_t incr_start;
    uint32_t incr_end;
    uint32_t incr_value;
    uint32_t current_value;
    int nb_sample;
    int is_rx;
    int buffer_size;
    int nb_elem;
    int tx_buffer;
    void *tx_buffers[2];
    int is_first_rx;
    pi_task_t mute_task;
    int mute_delay_start;
    int mute_delay_incr;
    int mute_delay_end;
    int mute_delay_current;
    int mute;
    int random_mute;
    int mute_ack;
    void *buffers[2];
}
i2s_slot_test_t;


typedef struct
{
    uint8_t itf;
    uint32_t sampling_freq;
    uint8_t full_duplex;
    uint8_t word_size;
    uint8_t nb_slots;
    uint32_t tx_slots;
    uint32_t rx_slots;
    uint32_t buffer_nb_elem;
    uint8_t elem_size;
    uint32_t nb_samples;
    uint8_t is_ext_clk;
    uint8_t is_ext_ws;
    uint8_t is_sai0_clk;
    uint8_t is_sai0_ws;
    uint8_t clk_polarity;
    uint8_t ws_polarity;
    uint64_t rx_slots_width;
    uint64_t tx_slots_width;
    uint64_t rx_slots_format;
    uint64_t tx_slots_format;
    uint8_t  use_fast_clk;
    uint64_t fast_clk_freq;
    uint32_t test_id;
    int random_mute;
    uint32_t ts_evt;
    pi_device_t * timestamp;
    uint32_t fifo_id;
}
i2s_test_config_t;


typedef struct 
{
    struct pi_device i2s;
    i2s_slot_test_t slot_test_rx[16];
    i2s_slot_test_t slot_test_tx[16];
    uint32_t tx_slots;
    uint32_t rx_slots;
    uint8_t is_ext_clk;
    uint8_t itf;
    int random_mute;
}
i2s_test_t;


pi_device_t *get_testbench();

pi_device_t *i2s_init(struct pi_device *i2s, i2s_config_t *config);
int i2s_deinit(struct pi_device *i2s, int itf);

int i2s_slot_init(i2s_slot_test_t *i2s_slot, struct pi_device *i2s, i2s_slot_config_t *config);
int i2s_slot_start(i2s_slot_test_t *i2s_slot, i2s_slot_start_config_t *config);
int i2s_slot_stop(i2s_slot_test_t *i2s_slot);
void i2s_slot_end(i2s_slot_test_t *i2s_slot);
int i2s_slot_wait(i2s_slot_test_t *i2s_slot);

int i2s_test_init(i2s_test_t *test, i2s_test_config_t *config);
int i2s_test_deinit(i2s_test_t *test);
int i2s_test_start(i2s_test_t *i2s_slot);
int i2s_test_wait(i2s_test_t *i2s_slot);
int i2s_test_stop(i2s_test_t *i2s_slot);



typedef struct
{
    int itf;
    int cs;
    int transfer_size;
    int frequency;
    int nb_2d_lines;
} testlib_hyperram_trafficgen_config_t;


typedef struct
{
    pi_device_t dev;
    void *buffer;
    uint32_t hyper_addr;
    int transfer_size;
    pi_task_t read_task;
    pi_task_t write_task;
    int is_read;
    int end;
    pi_task_t end_task;
    int pending;
    int nb_2d_lines;
} testlib_hyperram_trafficgen_t;


void testlib_hyperram_trafficgen_conf_init(testlib_hyperram_trafficgen_config_t *config);
int testlib_hyperram_trafficgen_init(testlib_hyperram_trafficgen_t *data, testlib_hyperram_trafficgen_config_t *config);
int testlib_hyperram_trafficgen_start(testlib_hyperram_trafficgen_t *data);
int testlib_hyperram_trafficgen_stop(testlib_hyperram_trafficgen_t *data);
int testlib_hyperram_trafficgen_deinit(testlib_hyperram_trafficgen_t *data);


#endif
