/* 
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#include "pmsis.h"

#ifndef __TESTBENCH_H__
#define __TESTBENCH_H__


enum pi_testbench_ctrl_type
{
    PI_TESTBENCH_CTRL_TYPE_UART
};


struct pi_testbench_conf
{
    enum pi_testbench_ctrl_type ctrl_type;
    int uart_baudrate;
    int uart_id;
};

// This structure can be used to describe what an I2S slot should do
typedef struct
{
    uint8_t itf;
    uint8_t enabled;
    uint8_t sampling_freq;
    uint8_t word_size;
    uint8_t nb_slots;
}
__attribute__((packed)) pi_testbench_i2s_verif_config_t;


// This structure can be used to describe what an I2S slot should do
typedef struct
{
    uint32_t is_rx;
    uint32_t enabled;
    uint32_t word_size;
    uint8_t itf;   // Reserved for runtime
    uint8_t slot;  // Reserved for runtime
}
__attribute__((packed)) pi_testbench_i2s_verif_slot_config_t;


typedef enum
{
    PI_TESTBENCH_I2S_VERIF_RX_ITER,
    PI_TESTBENCH_I2S_VERIF_TX_FILE_DUMPER
} pi_testbench_i2s_verif_start_config_type_e;

// This structure can be used to describe what an I2S slot should do
typedef struct
{
    uint32_t type;
    union
    {
        struct
        {
            int32_t nb_samples;   // Number of samples that should be produced
            uint32_t incr_start;   // When using incr mode, gives first value
            uint32_t incr_end;     // When using incr mode, gives last value from which it should start again from incr_start
            uint32_t incr_value;   // If different from 0, activate incr mode and give the increment after each sample
        } rx_iter;
        struct
        {
            int32_t nb_samples;
            uint32_t filepath;
            uint32_t filepath_len;
        } tx_file_dumper;
    };

    uint8_t itf;   // Reserved for runtime
    uint8_t slot;  // Reserved for runtime
}
__attribute__((packed)) pi_testbench_i2s_verif_slot_start_config_t;

pi_device_t *pi_testbench_get();

int pi_testbench_prepare_pads(struct pi_testbench_conf *conf);
int pi_testbench_open(struct pi_device *device);
void pi_testbench_set_status(pi_device_t *device, int status);
uint64_t pi_testbench_get_time_ps(pi_device_t *device);
void pi_testbench_conf_init(struct pi_testbench_conf *conf);

void pi_testbench_gpio_loopback(pi_device_t *device, int output, int input, int enabled);
void pi_testbench_gpio_pulse_gen(pi_device_t *device, int gpio, int enabled, int64_t first_delay_ps, int64_t duration_ps, int64_t period_ps);

int pi_testbench_uart_traffic_gen(pi_device_t *device, pi_device_t *uart_device, struct pi_uart_conf *conf);
void pi_testbench_uart_traffic_gen_rx(pi_device_t *device, int start, int incr, int size, int bandwidth, int nb_iter);
void pi_testbench_uart_traffic_gen_tx(pi_device_t *device, int start, int incr, int size, int iter_size);
void pi_testbench_uart_traffic_gen_start(pi_device_t *device);
int pi_testbench_uart_traffic_gen_status(pi_device_t *device);

int pi_testbench_spim_verif_setup(pi_device_t *device, int enabled, int itf, int cs, int polarity, int phase, int mem_size_log2);

int pi_testbench_i2s_verif_open(pi_device_t *device, int itf, pi_testbench_i2s_verif_config_t *config);
int pi_testbench_i2s_verif_close(pi_device_t *device, int itf);
int pi_testbench_i2s_verif_slot_open(pi_device_t *device, int itf, int slot, pi_testbench_i2s_verif_slot_config_t *config);
int pi_testbench_i2s_verif_slot_close(pi_device_t *device, int itf, int slot);
int pi_testbench_i2s_verif_slot_start(pi_device_t *device, int itf, int slot, pi_testbench_i2s_verif_slot_start_config_t *config);

#endif