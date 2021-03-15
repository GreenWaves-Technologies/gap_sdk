/*
 * Copyright (C) 2020 GreenWaves Technologies, SAS, ETH Zurich and
 *                    University of Bologna
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

/* 
 * Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */

#ifndef __DEVICES_TESTBENCH_TESTBENCH_HPP__
#define __DEVICES_TESTBENCH_TESTBENCH_HPP__

#include <vp/vp.hpp>
#include <vp/itf/io.hpp>
#include <vp/itf/uart.hpp>
#include <vp/itf/clock.hpp>
#include <vp/itf/i2c.hpp>
#include <vp/itf/i2s.hpp>
#include <vp/itf/qspim.hpp>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <regex>

extern "C" void dpi_set_status(int status);


class Testbench;
class Uart;



#define PI_TESTBENCH_CMD_GPIO_LOOPBACK 1
#define PI_TESTBENCH_CMD_UART_CHECKER  2
#define PI_TESTBENCH_CMD_SET_STATUS    3
#define PI_TESTBENCH_CMD_GPIO_PULSE_GEN 4
#define PI_TESTBENCH_CMD_GET_TIME_PS    5
#define PI_TESTBENCH_CMD_SPIM_VERIF_SETUP 6
#define PI_TESTBENCH_CMD_I2S_VERIF_SETUP 7
#define PI_TESTBENCH_CMD_I2S_VERIF_SLOT_SETUP 8
#define PI_TESTBENCH_CMD_I2S_VERIF_SLOT_START 9
#define PI_TESTBENCH_CMD_SPIM_VERIF_TRANSFER 10
#define PI_TESTBENCH_CMD_SPIM_VERIF_SPI_WAKEUP 11
#define PI_TESTBENCH_CMD_I2S_VERIF_START 12
#define PI_TESTBENCH_CMD_I2S_VERIF_SLOT_STOP 13

#define PI_TESTBENCH_MAX_REQ_SIZE 256


typedef enum {
    PI_TESTBENCH_REQ_UART_CHECKER_MODE_LOOPBACK,
    PI_TESTBENCH_REQ_UART_CHECKER_MODE_TRAFFIC_GEN
} pi_testbench_req_uart_checker_mode_e;


typedef struct {
    uint8_t input;
    uint8_t output;
    uint8_t enabled;
} pi_testbench_req_gpio_checker_t;


typedef struct {
    int64_t first_delay_ps;
    int64_t duration_ps;
    int64_t period_ps;
    uint8_t gpio;
    uint8_t enabled;
} pi_testbench_req_gpio_pulse_gen_t;


typedef struct {
    uint32_t baudrate;
    uint8_t id;
    uint8_t mode;
    uint8_t flow_control;
    uint8_t stop_bits;
    uint8_t parity;
    uint8_t word_size;
    uint8_t usart;
    uint8_t usart_is_external;
    uint8_t polarity;
    uint8_t phase;
    uint8_t enabled;
} pi_testbench_req_uart_checker_t;

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


typedef struct
{
    uint64_t delay;
    uint32_t frequency;
    uint8_t mode;
    uint8_t spi_reload;
    uint8_t itf;
    uint8_t cs;
}
__attribute__((packed)) pi_testbench_req_spim_verif_spi_wakeup_t;



typedef struct {
    uint32_t sampling_rate;
    uint8_t itf;
    uint8_t enabled;
    uint8_t word_size;
    uint8_t nb_slots;
} pi_testbench_req_i2s_verif_setup_t;


typedef struct {
    uint8_t itf;
    uint8_t is_rx;
    uint8_t slot;
    uint8_t enabled;
    uint8_t word_size;
} pi_testbench_req_i2s_verif_slot_setup_t;


typedef struct {
    uint32_t status;
} pi_testbench_req_set_status_t;



typedef struct {
    union {
        pi_testbench_req_gpio_checker_t gpio;
        pi_testbench_req_gpio_pulse_gen_t gpio_pulse_gen;
        pi_testbench_req_uart_checker_t uart;
        pi_testbench_req_set_status_t set_status;
        pi_testbench_req_i2s_verif_setup_t i2s_verif_setup;
        pi_testbench_req_i2s_verif_slot_setup_t i2s_verif_slot_setup;
    };
} pi_testbench_req_t;

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
        } tx_file_dumper;
        struct
        {
            int32_t nb_samples;
            uint32_t filepath;
            uint32_t filepath_len;
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
}
__attribute__((packed)) pi_testbench_i2s_verif_slot_stop_config_t;


typedef enum {
    STATE_WAITING_CMD,
    STATE_WAITING_REQUEST,
    STATE_SENDING_REPLY
} testbench_state_e;


class Gpio
{
public:
    Gpio(Testbench *top);
    static void pulse_handler(void *__this, vp::clock_event *event);

    Testbench *top;

    vp::wire_slave<int> itf;

    int loopback = -1;
    uint32_t value;


    vp::clock_event *pulse_event;
    int64_t pulse_duration_ps;
    int64_t pulse_period_ps;
    bool pulse_enabled = false;
    bool pulse_gen_rising_edge = false;
};


class Spim_verif;


class Spi
{
public:
    Spi(Testbench *top, int itf, int cs);

    static void sync(void *__this, int sck, int data_0, int data_1, int data_2, int data_3, int mask);
    static void cs_sync(void *_this, bool active);

    void spim_verif_setup(pi_testbench_req_spim_verif_setup_t *config);
    void spim_verif_transfer(pi_testbench_req_spim_verif_transfer_t *transfer);
    void spim_verif_spi_wakeup(pi_testbench_req_spim_verif_spi_wakeup_t *config);

    void create_loader(js::config *config);

    Testbench *top;

    Spim_verif * spim_verif;

    vp::qspim_slave itf;
    vp::wire_slave<bool> cs_itf;

    int itf_id;
    int cs;
};


class I2s_verif;

class I2s
{
public:
    I2s(Testbench *top, int itf);

    void i2s_verif_setup(pi_testbench_i2s_verif_config_t *config);
    void i2s_verif_start(pi_testbench_i2s_verif_start_config_t *config);
    void i2s_verif_slot_setup(pi_testbench_i2s_verif_slot_config_t *config);
    void i2s_verif_slot_start(pi_testbench_i2s_verif_slot_start_config_t *config);
    void i2s_verif_slot_stop(pi_testbench_i2s_verif_slot_stop_config_t *config);
    static void sync(void *__this, int sck, int ws, int sd);
    void sync_sck(int sck);
    void sync_ws(int ws);

    Testbench *top;
    I2s_verif * i2s_verif;
    vp::i2s_master itf;
    vp::trace trace;
    int itf_id;
    uint32_t clk_propagate;
    uint32_t ws_propagate;
};


class Uart_dev
{
public:
    virtual void handle_received_byte(uint8_t byte) {}
    virtual void send_byte_done() {}
};


class Uart_reply : public Uart_dev
{
public:
    Uart_reply(Testbench *top, Uart *uart);

    void send_byte_done();

    Testbench *top;
};


class Uart_flow_control_checker : public Uart_dev
{
public:
    Uart_flow_control_checker(Testbench *top, Uart *uart, pi_testbench_req_t *req);

    void handle_received_byte(uint8_t byte);
    void send_byte_done();

private:
    static void bw_limiter_handler(void *__this, vp::clock_event *event);
    void send_byte();
    void check_end_of_command();

    Testbench *top;
    Uart *uart;

    std::string current_string;
    bool waiting_command;

    int rx_start;
    int rx_incr;
    int rx_size;
    int rx_bandwidth;
    int rx_nb_iter;

    int tx_start;
    int tx_incr;
    int tx_size;
    int tx_iter_size;
    int tx_iter_size_init;
    int tx_value;
    int tx_value_init;

    int64_t rx_timestamp;
    int received_bytes;

    vp::clock_event *bw_limiter_event;

    bool send_reply = false;
    int reply_index;
    std::string reply;
    int status;

    vp::trace trace;

};


typedef enum
{
  UART_TX_STATE_START,
  UART_TX_STATE_DATA,
  UART_TX_STATE_PARITY,
  UART_TX_STATE_STOP
} uart_tx_state_e;


class Uart
{
public:
    Uart(Testbench *top, int id);

    vp::uart_slave itf;

    void start();

    void set_control(bool active, int baudrate);
    void set_dev(Uart_dev *dev) { this->dev = dev; }
    void send_byte(uint8_t byte);
    void set_cts(int cts);

    uint64_t baudrate;
    vp::clock_engine *clock;
    vp::clock_engine *tx_clock;

    int tx_parity_en = 0;
    int tx_stop_bits = 1;
    
    bool is_usart;
    int polarity;
    int phase;
    int id;
    bool is_control_active;

private:

    static void sync(void *__this, int data);
    static void sync_full(void *__this, int data, int clk, int rtr);

    void uart_tx_sampling();

    void check_send_byte();

    void uart_start_tx_sampling(int baudrate);
    void uart_stop_tx_sampling();
    static void uart_sampling_handler(void *__this, vp::clock_event *event);
    static void uart_tx_handler(void *__this, vp::clock_event *event);
    static void init_handler(void *__this, vp::clock_event *event);

    void handle_received_byte(uint8_t byte);
    void send_bit();

    static void clk_reg(vp::component *__this, vp::component *clock);
    static void tx_clk_reg(vp::component *__this, vp::component *clock);

    Testbench *top;
    Uart_dev *dev = NULL;

    vp::trace trace;

    vp::clk_slave    periph_clock_itf;

    int clk;
    int prev_clk;

    bool uart_tx_wait_start = true;
    bool uart_tx_wait_stop = false;
    int uart_current_tx;
    int uart_nb_bits;
    bool uart_sampling_tx = false;
    uint8_t uart_byte;

    vp::clock_event *uart_sampling_event;
    vp::clock_event *uart_tx_event;
    vp::clock_event *init_event;
    vp::clock_master clock_cfg;
    vp::clk_slave    clock_itf;
    vp::clock_master tx_clock_cfg;
    vp::clk_slave    tx_clock_itf;

    bool is_control = false;

    uint8_t tx_pending_byte;
    int tx_pending_bits = 0;
    int tx_parity;
    int tx_current_stop_bits;

    int tx_bit = 1;
    int tx_cts = 0;
    int rtr = 0;

    uart_tx_state_e tx_state;

};



typedef enum
{
  I2C_STATE_WAIT_START,
  I2C_STATE_WAIT_ADDRESS,
  I2C_STATE_GET_DATA,
  I2C_STATE_ACK
} I2c_state_e;


class I2C
{
public:
    void conf(Testbench *top, int id);
    void sync(int scl, int sda);
    void handle_byte();

    vp::i2c_slave itf;

    Testbench *top;
    int id;
    I2c_state_e state;
    int prev_sda;
    int pending_send_ack;
    uint32_t address;
    uint32_t pending_data;
    int pending_bits;
    int is_read;
};


class Testbench : public vp::component
{
public:
    Testbench(js::config *config);

    int build();
    void start();

    void handle_received_byte(uint8_t byte);

    void send_byte_done();

    vp::trace trace;

    std::vector<Uart *> uarts;
    std::vector<Gpio *> gpios;
    std::vector<Spi *> spis;
    std::vector<I2s *> i2ss;
    std::vector<I2C> i2cs;

    int nb_gpio;
    int nb_spi;
    int nb_uart;
    int nb_i2c;
    int nb_i2s;

private:


    void handle_gpio_loopback();
    void handle_gpio_pulse_gen();
    void handle_uart_checker();
    void handle_set_status();
    void handle_spim_verif_setup();
    void handle_spim_verif_transfer();
    void handle_spim_verif_spi_wakeup();
    void handle_i2s_verif_setup();
    void handle_i2s_verif_start();
    void handle_i2s_verif_slot_setup();
    void handle_i2s_verif_slot_start();
    void handle_i2s_verif_slot_stop();

    static void gpio_sync(void *__this, int value, int id);
    static void i2c_sync(void *__this, int scl, int sda, int id);

    string ctrl_type;
    uint64_t period;

    vp::uart_slave uart_in;

    testbench_state_e state;

    uint32_t cmd;
    int req_size;
    int current_req_size;
    uint8_t req[PI_TESTBENCH_MAX_REQ_SIZE];
    Uart *uart_ctrl;
    uint8_t *tx_buff;
    int tx_buff_size;
    int tx_buff_index;

    Spim_verif *spi_loader;
};

#endif