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

#include <vp/vp.hpp>
#include <vp/itf/io.hpp>
#include <vp/itf/uart.hpp>
#include <vp/itf/clock.hpp>
#include <vp/itf/i2c.hpp>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <regex>


class Testbench;
class Uart;


#define PI_TESTBENCH_CMD_GPIO_LOOPBACK 1
#define PI_TESTBENCH_CMD_UART_CHECKER 2
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
    uint32_t baudrate;
    uint8_t id;
    uint8_t mode;
    uint8_t flow_control;
    uint8_t stop_bits;
    uint8_t parity;
    uint8_t word_size;
    uint8_t usart;
    uint8_t usart_is_external;
    uint8_t enabled;
} pi_testbench_req_uart_checker_t;


typedef struct {
    union {
        pi_testbench_req_gpio_checker_t gpio;
        pi_testbench_req_uart_checker_t uart;
    };
} pi_testbench_req_t;


typedef enum {
    STATE_WAITING_CMD,
    STATE_WAITING_REQUEST
} testbench_state_e;


class Gpio
{
public:
    vp::wire_slave<int> itf;

    int loopback = -1;
    uint32_t value;
};


class Uart_dev
{
public:
    virtual void handle_received_byte(uint8_t byte) {}
    virtual void send_byte_done() {}
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
    int tx_value;

    int64_t rx_timestamp;
    int received_bytes;

    vp::clock_event *bw_limiter_event;

    bool send_reply = false;
    int reply_index;
    std::string reply;
    int status;
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

    void set_control(bool active, int baudrate);
    void set_dev(Uart_dev *dev) { this->dev = dev; }
    void send_byte(uint8_t byte);
    void set_cts(int cts);

    uint64_t baudrate;
    vp::clock_engine *clock;
    vp::clock_engine *tx_clock;

    int tx_parity_en = 0;
    int tx_stop_bits = 1;
    
private:

    static void sync(void *__this, int data);
    static void sync_full(void *__this, int data, int clk, int rtr);

    void uart_tx_sampling();

    void check_send_byte();

    void uart_start_tx_sampling(int baudrate);
    void uart_stop_tx_sampling();
    static void uart_sampling_handler(void *__this, vp::clock_event *event);
    static void uart_tx_handler(void *__this, vp::clock_event *event);

    void handle_received_byte(uint8_t byte);
    void send_bit();

    static void clk_reg(vp::component *__this, vp::component *clock);
    static void tx_clk_reg(vp::component *__this, vp::component *clock);

    Testbench *top;
    int id;
    Uart_dev *dev = NULL;

    vp::clk_slave    periph_clock_itf;

    bool uart_tx_wait_start = true;
    bool uart_tx_wait_stop = false;
    int uart_current_tx;
    int uart_nb_bits;
    bool uart_sampling_tx = false;
    uint8_t uart_byte;

    vp::clock_event *uart_sampling_event;
    vp::clock_event *uart_tx_event;
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
    void handle_received_byte(uint8_t byte);

    vp::trace trace;

private:


    void handle_gpio_loopback();

    void handle_uart_checker();

    static void gpio_sync(void *__this, int value, int id);
    static void i2c_sync(void *__this, int scl, int sda, int id);

    string ctrl_type;
    uint64_t period;
    int nb_gpio;
    int nb_uart;
    int nb_i2c;

    std::vector<Uart *> uarts;
    std::vector<Gpio> gpios;
    std::vector<I2C> i2cs;
    vp::uart_slave uart_in;

    testbench_state_e state;

    uint8_t cmd;
    int req_size;
    int current_req_size;
    uint8_t req[PI_TESTBENCH_MAX_REQ_SIZE];
};


Uart_flow_control_checker::Uart_flow_control_checker(Testbench *top, Uart *uart, pi_testbench_req_t *req)
: top(top), uart(uart)
{
    this->uart->set_cts(0);

    uart->baudrate = req->uart.baudrate;
    this->current_string = "";
    this->waiting_command = true;
    this->rx_size = 0;
    this->tx_size = 0;
    this->rx_timestamp = -1;

    this->bw_limiter_event = top->event_new(this, Uart_flow_control_checker::bw_limiter_handler);
}


void Uart_flow_control_checker::bw_limiter_handler(void *__this, vp::clock_event *event)
{
    Uart_flow_control_checker *_this = (Uart_flow_control_checker *)__this;
    _this->uart->set_cts(0);
}


void Uart_flow_control_checker::send_byte()
{
    if (this->send_reply)
    {
        this->uart->send_byte(this->reply[this->reply_index]);
        this->reply_index++;
        if (this->reply_index == this->reply.size() + 1)
        {
            this->send_reply = false;
        }
    }
    else
    {    
        this->uart->send_byte(this->rx_start);
        this->rx_start += this->rx_incr;
        this->rx_size--;
    }
}


void Uart_flow_control_checker::send_byte_done()
{
    if (this->rx_size > 0 || this->send_reply)
        this->send_byte();
}


void Uart_flow_control_checker::check_end_of_command()
{
    if (this->tx_size == 0 && this->rx_size == 0)
    {
        this->waiting_command = true;
    }
}

void Uart_flow_control_checker::handle_received_byte(uint8_t byte)
{
    if (waiting_command)
    {
        if (byte != 0)
        {
            this->current_string += byte;
        }
        else
        {
            this->top->trace.msg(vp::trace::LEVEL_DEBUG, "UART flow control received command (command: %s)\n", this->current_string.c_str());

            std::regex regex{R"([\s]+)"};
            std::sregex_token_iterator it{this->current_string.begin(), this->current_string.end(), regex, -1};
            std::vector<std::string> words{it, {}};

            if (words[0] == "START")
            {
                this->waiting_command = false;
                this->status = 0;
                if (this->rx_size > 0)
                    this->send_byte();
            }
            else if (words[0] == "TRAFFIC_RX")
            {
                this->received_bytes = 0;
                this->rx_start = std::stoi(words[1]);
                this->rx_incr = std::stoi(words[2]);
                this->rx_size = std::stoi(words[3]);
                this->rx_bandwidth = std::stoi(words[4]);
                this->rx_nb_iter = std::stoi(words[5]);
                this->top->trace.msg(vp::trace::LEVEL_INFO, "UART flow control received traffic rx command (start: %d, incr: %d, size: %d, bandwidth: %d, nb_iter: %d)\n", this->rx_start, this->rx_incr, this->rx_size, this->rx_bandwidth, this->rx_nb_iter);
            }
            else if (words[0] == "TRAFFIC_TX")
            {
                this->tx_start = std::stoi(words[1]);
                this->tx_value = this->tx_start;
                this->tx_incr = std::stoi(words[2]);
                this->tx_size = std::stoi(words[3]);
                this->top->trace.msg(vp::trace::LEVEL_INFO, "UART flow control received traffic tx command (start: %d, incr: %d, size: %d)\n", this->tx_start, this->tx_incr, this->tx_size);
            }
            else if (words[0] == "STATUS")
            {
                this->send_reply = true;
                this->reply_index = 0;
                if (this->status == 0)
                {
                    this->reply = "OK";
                }
                else
                {
                    this->reply = "KO";
                }
                this->send_byte();
            }

            this->current_string = "";
        }
    }
    else
    {
        if (this->tx_size > 0)
        {
            int64_t current_time = this->top->get_time();

            if (byte != this->tx_value)
            {
                this->status = 1;
            }
            this->tx_value += this->tx_incr;

            if (this->rx_timestamp == -1)
            {
                this->rx_timestamp = current_time - 1000000000000ULL*8/this->uart->baudrate;
            }

            this->received_bytes++;

            if (current_time > this->rx_timestamp)
            {
                if ((float)this->received_bytes * 1000000000000ULL  / (current_time - this->rx_timestamp) > this->rx_bandwidth)
                {
                    this->uart->set_cts(1);

                    int64_t next_time = (float)this->received_bytes * 1000000000000ULL / this->rx_bandwidth + this->rx_timestamp;
                    int64_t period = this->uart->clock->get_period();
                    int64_t cycles = (next_time - current_time + period - 1) / period;

                    this->uart->clock->enqueue(this->bw_limiter_event, cycles);
                }

            }

            this->tx_size--;
            this->check_end_of_command();
        }
    
    }
}



Testbench::Testbench(js::config *config)
    : vp::component(config)
{
    this->state = STATE_WAITING_CMD;
}


int Testbench::build()
{
    traces.new_trace("trace", &trace, vp::DEBUG);

    this->ctrl_type = get_js_config()->get("ctrl_type")->get_str();
    this->nb_gpio = get_js_config()->get("nb_gpio")->get_int();
    this->nb_i2c = get_js_config()->get("nb_i2c")->get_int();
    this->nb_uart = get_js_config()->get("nb_uart")->get_int();

    for (int i=0; i<this->nb_uart; i++)
    {
        this->uarts.push_back(new Uart(this, i));
    }

    this->gpios.resize(this->nb_gpio);
    
    for (int i=0; i<this->nb_gpio; i++)
    {
        this->gpios[i].itf.set_sync_meth_muxed(&Testbench::gpio_sync, i);
        this->new_slave_port("gpio" + std::to_string(i), &this->gpios[i].itf);
    }

    this->i2cs.resize(this->nb_i2c);
    
    for (int i=0; i<this->nb_i2c; i++)
    {
        this->i2cs[i].conf(this, i);
        this->i2cs[i].itf.set_sync_meth_muxed(&Testbench::i2c_sync, i);
        this->new_slave_port("i2c" + std::to_string(i), &this->i2cs[i].itf);
    }

    if (this->ctrl_type == "uart")
    {
        int uart_id = get_js_config()->get("uart_id")->get_int();
        int uart_baudrate = get_js_config()->get("uart_baudrate")->get_int();

        this->uarts[uart_id]->set_control(true, uart_baudrate);
    }

    return 0;
}


void Uart::set_control(bool active, int baudrate)
{
    this->baudrate = baudrate;
    this->is_control = active;
}


void Uart::handle_received_byte(uint8_t byte)
{
    if (this->is_control)
    {
        this->top->handle_received_byte(uart_byte);
    }
    else if (this->dev)
    {
        dev->handle_received_byte(uart_byte);
    }
}


void Uart::uart_tx_sampling()
{
    this->top->trace.msg(vp::trace::LEVEL_TRACE, "Sampling bit (value: %d)\n", uart_current_tx);

    if (uart_tx_wait_stop)
    {
        if (uart_current_tx == 1)
        {
            this->top->trace.msg(vp::trace::LEVEL_TRACE, "Received stop bit\n", uart_current_tx);
            uart_tx_wait_start = true;
            uart_tx_wait_stop = false;
            this->uart_stop_tx_sampling();
        }
    }
    else
    {
        this->top->trace.msg(vp::trace::LEVEL_TRACE, "Received data bit (data: %d)\n", uart_current_tx);
        uart_byte = (uart_byte >> 1) | (uart_current_tx << 7);
        uart_nb_bits++;
        if (uart_nb_bits == 8)
        {
            this->top->trace.msg(vp::trace::LEVEL_DEBUG, "Sampled TX byte (value: 0x%x)\n", uart_byte);
            this->top->trace.msg(vp::trace::LEVEL_TRACE, "Waiting for stop bit\n");
            uart_tx_wait_stop = true;
            this->handle_received_byte(uart_byte);
        }
    }
}


void Uart::check_send_byte()
{
    if (this->tx_pending_bits && !this->uart_tx_event->is_enqueued())
    {
        if (this->rtr == 0)
        {
            this->tx_clock->reenqueue(this->uart_tx_event, 2);
        }
    }
}


void Uart::send_byte(uint8_t byte)
{
    this->tx_pending_byte = byte;
    this->tx_pending_bits = 8;
    this->tx_state = UART_TX_STATE_START;
    this->tx_clock_cfg.set_frequency(this->baudrate*2);

    this->check_send_byte();
}


Uart::Uart(Testbench *top, int id)
: top(top), id(id)
{
    this->uart_sampling_event = top->event_new(this, Uart::uart_sampling_handler);
    this->uart_tx_event = top->event_new(this, Uart::uart_tx_handler);
    this->itf.set_sync_meth(&Uart::sync);
    this->itf.set_sync_full_meth(&Uart::sync_full);
    this->top->new_slave_port(this, "uart" + std::to_string(this->id), &this->itf);
    this->top->new_master_port(this, "uart" + std::to_string(this->id) + "_clock_cfg", &clock_cfg);
    this->top->new_master_port(this, "uart" + std::to_string(this->id) + "_tx_clock_cfg", &tx_clock_cfg);

    this->clock_itf.set_reg_meth(&Uart::clk_reg);
    this->top->new_slave_port(this, "uart" + std::to_string(this->id) + "_clock", &this->clock_itf);

    this->tx_clock_itf.set_reg_meth(&Uart::tx_clk_reg);
    this->top->new_slave_port(this, "uart" + std::to_string(this->id) + "_tx_clock", &this->tx_clock_itf);
}


void Uart::tx_clk_reg(vp::component *__this, vp::component *clock)
{
    Uart *_this = (Uart *)__this;
    _this->tx_clock = (vp::clock_engine *)clock;
}


void Uart::clk_reg(vp::component *__this, vp::component *clock)
{
    Uart *_this = (Uart *)__this;
    _this->clock = (vp::clock_engine *)clock;
}


void Uart::uart_sampling_handler(void *__this, vp::clock_event *event)
{
    Uart *_this = (Uart *)__this;

    _this->uart_tx_sampling();

    if (_this->uart_sampling_tx)
    {
        _this->clock->enqueue(_this->uart_sampling_event, 2);
    }
}


void Uart::sync_full(void *__this, int data, int clk, int rtr)
{
    Uart *_this = (Uart *)__this;

    _this->top->trace.msg(vp::trace::LEVEL_TRACE, "UART sync (data: %d, clk: %d, rtr: %d)\n", data, clk, rtr);
    _this->rtr = rtr;

    _this->check_send_byte();

    Uart::sync(__this, data);
}


void Uart::sync(void *__this, int data)
{
    Uart *_this = (Uart *)__this;

    if (!_this->is_control && !_this->dev)
        return;

    _this->top->trace.msg(vp::trace::LEVEL_TRACE, "UART control sync (value: %d, waiting_start: %d)\n", data, _this->uart_tx_wait_start);

    _this->uart_current_tx = data;

    if (_this->uart_tx_wait_start && data == 0)
    {
        _this->top->trace.msg(vp::trace::LEVEL_TRACE, "Received start bit\n");

        _this->uart_start_tx_sampling(_this->baudrate);
        _this->uart_tx_wait_start = false;
        _this->uart_nb_bits = 0;
    }
}


void Uart::set_cts(int cts)
{
    this->tx_cts = cts;
    this->itf.sync_full(this->tx_bit, 0, this->tx_cts);
}


void Uart::uart_tx_handler(void *__this, vp::clock_event *event)
{
    Uart *_this = (Uart *)__this;
    _this->send_bit();
}


void Uart::send_bit()
{
    int bit;

    switch (this->tx_state)
    {
        case UART_TX_STATE_START:
        {
            this->tx_parity = 0;
            this->tx_state = UART_TX_STATE_DATA;
            this->tx_current_stop_bits = this->tx_stop_bits;
            bit = 0;
            break;
        }
        case UART_TX_STATE_DATA:
        {
            bit = this->tx_pending_byte & 1;
            this->tx_pending_byte >>= 1;
            this->tx_pending_bits -= 1;
            this->tx_parity ^= bit;

            if (this->tx_pending_bits == 0)
            {
                if (this->tx_parity_en)
                    this->tx_state = UART_TX_STATE_PARITY;
                else
                {
                    this->tx_state = UART_TX_STATE_STOP;
                }
            }
            break;
        }
        case UART_TX_STATE_PARITY:
        {
            bit = this->tx_parity;
            this->tx_state = UART_TX_STATE_STOP;
            break;
        }
        case UART_TX_STATE_STOP:
        {
            bit = 1;
            this->tx_current_stop_bits--;
            if (this->tx_current_stop_bits == 0)
            {
                this->tx_state = UART_TX_STATE_START;
                if (this->dev)
                {
                    this->dev->send_byte_done();
                }
            }
            break;
        }
    }

    this->tx_bit = bit;
    this->itf.sync_full(this->tx_bit, 0, this->tx_cts);

    if (this->tx_state != UART_TX_STATE_START)
    {
        this->tx_clock->reenqueue(this->uart_tx_event, 2);
    }
}


void Uart::uart_start_tx_sampling(int baudrate)
{
    this->top->trace.msg(vp::trace::LEVEL_TRACE, "Start TX sampling (baudrate: %d)\n", this->baudrate);

    // We set the frequency to twice the baudrate to be able sampling in the
    // middle of the cycle
    this->clock_cfg.set_frequency(this->baudrate*2);

    this->uart_sampling_tx = 1;

    this->clock->reenqueue(this->uart_sampling_event, 3);
}


void Uart::uart_stop_tx_sampling(void)
{
    this->uart_sampling_tx = 0;
    
    if (this->uart_sampling_event->is_enqueued())
    {
        this->clock->cancel(this->uart_sampling_event);
    }
}


void Testbench::handle_received_byte(uint8_t byte)
{
    if (this->state == STATE_WAITING_CMD)
    {
        this->cmd = byte;

        switch (byte) {
            case PI_TESTBENCH_CMD_GPIO_LOOPBACK:
            case PI_TESTBENCH_CMD_UART_CHECKER:
                this->state = STATE_WAITING_REQUEST;
                this->req_size = sizeof(pi_testbench_req_t);
                this->current_req_size = 0;
                break;
        }
    }
    else if (this->state == STATE_WAITING_REQUEST)
    {
        this->req[this->current_req_size++] = byte;
        if (this->current_req_size == this->req_size)
        {
            this->state = STATE_WAITING_CMD;

            switch (this->cmd) {
                case PI_TESTBENCH_CMD_GPIO_LOOPBACK:
                    this->handle_gpio_loopback();
                    break;
            }

            switch (this->cmd) {
                case PI_TESTBENCH_CMD_UART_CHECKER:
                    this->handle_uart_checker();
                    break;
            }

        }
    }
}


void Testbench::gpio_sync(void *__this, int value, int id)
{
    Testbench *_this = (Testbench *)__this;
    Gpio *gpio = &_this->gpios[id];

    _this->trace.msg(vp::trace::LEVEL_DEBUG, "Received GPIO sync (id: %d)\n", id);

    gpio->value = value;

    if (gpio->loopback != -1)
    {
        _this->trace.msg(vp::trace::LEVEL_DEBUG, "Generating gpio on loopback (id: %d)\n", gpio->loopback);
        _this->gpios[gpio->loopback].itf.sync(value);
    }
}


void I2C::conf(Testbench *top, int id)
{
    this->top = top;
    this->id = id;
    this->state = I2C_STATE_WAIT_START;
    this->prev_sda = 1;
    this->pending_send_ack = false;
}


void I2C::handle_byte()
{
    this->top->trace.msg(vp::trace::LEVEL_DEBUG, "Received I2C byte (id: %d, value: 0x%x)\n", this->id, this->pending_data & 0xff);
}


void I2C::sync(int scl, int sda)
{
    this->top->trace.msg(vp::trace::LEVEL_TRACE, "Received I2C sync (id: %d, scl: %d, sda: %d)\n", this->id, scl, sda);

    if (scl == 1 && this->prev_sda != sda)
    {
        if (this->prev_sda == 1)
        {
            this->top->trace.msg(vp::trace::LEVEL_TRACE, "Received I2C start bit (id: %d)\n", id);

            this->state = I2C_STATE_WAIT_ADDRESS;
            this->address = 0;
            this->pending_bits = 8;
        }
        else
        {
            this->top->trace.msg(vp::trace::LEVEL_TRACE, "Received I2C stop bit (id: %d)\n", id);
            this->state = I2C_STATE_WAIT_START;
        }
    }
    else
    {
        if (scl == 0)
        {
            if (this->pending_send_ack)
            {
                this->pending_send_ack = false;
                this->itf.sync(1);
            }
        }
        else
        {
            switch (this->state)
            {
                case I2C_STATE_WAIT_ADDRESS:
                {
                    if (this->pending_bits > 1)
                    {
                        this->address = (this->address << 1) | sda;
                    }
                    else
                    {
                        this->is_read = sda;
                    }
                    this->pending_bits--;
                    if (this->pending_bits == 0)
                    {
                        this->state = I2C_STATE_ACK;
                        this->pending_bits = 8;
                    }
                    break;
                }

                case I2C_STATE_GET_DATA:
                {
                    //if (sda_out)
                    //{
                    //    *sda_out = (this->pending_send_byte >> 7) & 1;
                    //    this->pending_send_byte <<= 1;
                    //}
                    this->top->trace.msg(vp::trace::LEVEL_TRACE, "Got I2C data (id: %d, sda: %d)\n", id, this->pending_bits);
                    this->pending_data = (this->pending_data << 1) | sda;
                    this->pending_bits--;
                    if (this->pending_bits == 0)
                    {
                        this->pending_bits = 8;
                        this->handle_byte();
                        this->state = I2C_STATE_ACK;
                    }
                    break;
                }
                
                case I2C_STATE_ACK:
                {
                    this->top->trace.msg(vp::trace::LEVEL_TRACE, "Generate I2C ack (id: %d)\n", id);

                    this->itf.sync(0);
                    this->state = I2C_STATE_GET_DATA;
                    break;
                }
            }
        }
    }

    this->prev_sda = sda;
}


void Testbench::i2c_sync(void *__this, int scl, int sda, int id)
{
    Testbench *_this = (Testbench *)__this;

    _this->i2cs[id].sync(scl, sda);
}



void Testbench::handle_uart_checker()
{
    pi_testbench_req_t *req = (pi_testbench_req_t *)this->req;

    if (req->uart.enabled)
    {
        Uart *uart = this->uarts[req->uart.id];

        uart->set_dev(new Uart_flow_control_checker(this, uart, req));
    }
}



void Testbench::handle_gpio_loopback()
{
    pi_testbench_req_t *req = (pi_testbench_req_t *)this->req;

    this->trace.msg(vp::trace::LEVEL_INFO, "Handling GPIO loopback (enabled: %d, output: %d, intput: %d)\n", req->gpio.enabled, req->gpio.output, req->gpio.input);

    if (req->gpio.enabled)
    {
        this->gpios[req->gpio.output].loopback = req->gpio.input;
        this->gpios[req->gpio.input].itf.sync(this->gpios[req->gpio.output].value);
    }
    else
    {
        this->gpios[req->gpio.output].loopback = -1;
    }
}


extern "C" vp::component *vp_constructor(js::config *config)
{
    return new Testbench(config);
}
