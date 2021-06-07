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


typedef struct {
    uint16_t cmd;
    uint16_t size;
} pi_testbench_req_header_t;


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


typedef struct {
    uint8_t enabled;
    uint8_t itf;
    uint8_t cs;
    uint8_t is_master;
    uint8_t polarity;
    uint8_t phase;
    uint16_t mem_size_log2;
} pi_testbench_req_spim_verif_setup_t;

typedef struct {
    pi_testbench_req_header_t header;
    uint32_t frequency;
    uint32_t address;
    uint32_t size;
    uint8_t is_master;
    uint8_t is_rx;
    uint8_t is_duplex;
    uint8_t itf;
    uint8_t cs;
    uint8_t is_boot_protocol;
} pi_testbench_req_spim_verif_transfer_t;


// This structure can be used to describe what an I2S slot should do
typedef struct
{
    uint8_t itf;
    uint8_t enabled;
    uint32_t sampling_freq;
    uint8_t word_size;
    uint8_t nb_slots;
    uint8_t is_pdm;
    uint8_t is_full_duplex;
    uint8_t is_ext_clk;
    uint8_t is_ext_ws;
    uint8_t is_sai0_clk;
    uint8_t is_sai0_ws;
    uint8_t clk_polarity;
    uint8_t ws_polarity;
    uint8_t ws_delay;
}
__attribute__((packed)) pi_testbench_i2s_verif_config_t;


typedef struct
{
    uint8_t itf;
    uint8_t start;
}
__attribute__((packed)) pi_testbench_i2s_verif_start_config_t;


// This structure can be used to describe what an I2S slot should do
typedef struct
{
    uint32_t is_rx;
    uint32_t enabled;
    uint32_t word_size;
    uint8_t format;
    uint8_t itf;   // Reserved for runtime
    uint8_t slot;  // Reserved for runtime
}
__attribute__((packed)) pi_testbench_i2s_verif_slot_config_t;


typedef enum
{
    PI_TESTBENCH_I2S_VERIF_RX_ITER,
    PI_TESTBENCH_I2S_VERIF_TX_FILE_DUMPER,
    PI_TESTBENCH_I2S_VERIF_RX_FILE_READER
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
            uint32_t type;
        } tx_file_dumper;
        struct
        {
            int32_t nb_samples;
            uint32_t filepath;
            uint32_t filepath_len;
            uint32_t type;
        } rx_file_reader;
    };

    uint8_t itf;   // Reserved for runtime
    uint8_t slot;  // Reserved for runtime
}
__attribute__((packed)) pi_testbench_i2s_verif_slot_start_config_t;

// This structure can be used to describe what an I2S slot should do
typedef struct
{
    uint8_t itf;   // Reserved for runtime
    uint8_t slot;  // Reserved for runtime
    uint8_t stop_rx;
    uint8_t stop_tx;
}
__attribute__((packed)) pi_testbench_i2s_verif_slot_stop_config_t;

pi_device_t *pi_testbench_get();
pi_device_t *pi_testbench_reopen();

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
void pi_testbench_uart_traffic_gen_rx_usart(pi_device_t *device, int start, int incr, int size, int bandwidth, int nb_iter);
void pi_testbench_uart_traffic_gen_tx_usart(pi_device_t *device, int start, int incr, int size, int iter_size);
void pi_testbench_uart_traffic_gen_start_usart(pi_device_t *device);
int pi_testbench_uart_traffic_gen_status_usart(pi_device_t *device);


/*
 * SPIM verif
 */

typedef enum
{
    PI_TESTBENCH_REQ_SPIM_VERIF_SPI_WAKEUP_MODE_CMD,
    PI_TESTBENCH_REQ_SPIM_VERIF_SPI_WAKEUP_MODE_CS,
    PI_TESTBENCH_REQ_SPIM_VERIF_SPI_WAKEUP_MODE_SDO
}
pi_testbench_req_spim_verif_spi_wakeup_mode_e;


typedef struct
{
    pi_testbench_req_header_t header;
    uint64_t delay;
    uint32_t frequency;
    uint8_t mode;
    uint8_t spi_reload;
    uint8_t itf;
    uint8_t cs;
}
__attribute__((packed)) pi_testbench_req_spim_verif_spi_wakeup_t;


int pi_testbench_spim_verif_setup(pi_device_t *device, pi_testbench_req_spim_verif_setup_t *config);
int pi_testbench_spim_verif_transfer(pi_device_t *device, int itf, int cs, pi_testbench_req_spim_verif_transfer_t *config);
int pi_testbench_spim_verif_transfer_async(pi_device_t *device, int itf, int cs, pi_testbench_req_spim_verif_transfer_t *config, pi_task_t *task);
int pi_testbench_spim_verif_spi_wakeup(pi_device_t *device, int itf, int cs, pi_testbench_req_spim_verif_spi_wakeup_t *config);


/*
 * I2S verif
 */

void pi_testbench_i2s_verif_init(pi_testbench_i2s_verif_config_t *config);
int pi_testbench_i2s_verif_open(pi_device_t *device, int itf, pi_testbench_i2s_verif_config_t *config);
int pi_testbench_i2s_verif_start(pi_device_t *device, int itf);
int pi_testbench_i2s_verif_close(pi_device_t *device, int itf);
int pi_testbench_i2s_verif_slot_open(pi_device_t *device, int itf, int slot, pi_testbench_i2s_verif_slot_config_t *config);
int pi_testbench_i2s_verif_slot_close(pi_device_t *device, int itf, int slot);
int pi_testbench_i2s_verif_slot_start(pi_device_t *device, int itf, int slot, pi_testbench_i2s_verif_slot_start_config_t *config);
int pi_testbench_i2s_verif_slot_stop(pi_device_t *device, int itf, int slot);

#endif