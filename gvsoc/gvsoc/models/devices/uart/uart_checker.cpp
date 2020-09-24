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
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>
//#include <common/telnet_proxy.hpp>

class Uart_checker : public vp::component
{
public:
    Uart_checker(js::config *config);

    int build();

    void tx_edge(int64_t timestamp, int tx);
    void rx_edge(int64_t timestamp, int rx);

    void tx_sampling();
    void rx_sampling();

private:
    void dpi_task(void);
    bool rx_is_sampling(void);
    void stdin_task(void);

    bool open_telnet_socket(int);
    void telnet_listener(void);
    void telnet_loop(int);

    static void dpi_uart_task_stub(Uart_checker *);
    static void dpi_uart_stdin_task_stub(Uart_checker *);

    void start_tx_sampling(int baudrate);
    void stop_tx_sampling();

    void start_rx_sampling(int baudrate);
    void stop_rx_sampling();

    static void sync(void *__this, int data);

    static void event_handler(void *__this, vp::clock_event *event);

    uint64_t period;
    bool tx_wait_start = true;
    bool tx_wait_stop = false;
    int current_tx;
    int current_rx;
    uint64_t baudrate;
    int nb_bits;
    uint32_t rx_bit_buffer = 0;
    int rx_nb_bits = 0;
    bool loopback;
    bool stdout;
    bool stdin;
    bool telnet;
    bool sampling_rx = false;
    bool sampling_tx = false;
    bool telnet_error = false;
    uint8_t byte;
    FILE *tx_file = NULL;

    vp::uart_slave in;

    std::thread *stdin_thread;

    std::thread *telnet_loop_thread;
    std::thread *telnet_listener_thread;
    int telnet_socket;
    int telnet_port;

    //Telnet_proxy *telnet_proxy;
    std::mutex rx_mutex;
    vp::trace trace;

    vp::clock_event *event;
    vp::clock_master clock_cfg;
};

Uart_checker::Uart_checker(js::config *config)
    : vp::component(config)
{
}

int Uart_checker::build()
{
    traces.new_trace("trace", &trace, vp::DEBUG);

    this->new_master_port("clock_cfg", &clock_cfg);


    baudrate = get_js_config()->get("baudrate")->get_int();
    loopback = get_js_config()->get("loopback")->get_bool();
    stdout = get_js_config()->get("stdout")->get_bool();
    stdin = get_js_config()->get("stdin")->get_bool();
    telnet = get_js_config()->get("telnet")->get_bool();

    this->in.set_sync_meth(&Uart_checker::sync);
    new_slave_port("input", &in);

    this->event = event_new(Uart_checker::event_handler);

    if (telnet)
    {
        telnet_port = get_js_config()->get("telnet_port")->get_int();
    }
    std::string tx_filename = get_js_config()->get("tx_file")->get_str();
    period = 1000000000000UL / baudrate;
    if (tx_filename != "")
    {
        tx_file = fopen(tx_filename.c_str(), (char *)"w");
        if (tx_file == NULL)
        {
            //print("Unable to open TX log file: %s", strerror(errno));
        }
    }
    if (this->telnet)
    {
        //this->telnet_proxy = new Telnet_proxy(this->telnet_port);
    }
    if (this->stdin || this->telnet)
    {
        stdin_thread = new std::thread(&Uart_checker::stdin_task, this);
    }

    return 0;
}

void Uart_checker::tx_sampling()
{
    this->trace.msg(vp::trace::LEVEL_INFO, "Sampling bit (value: %d)\n", current_tx);

    if (tx_wait_stop)
    {
        if (current_tx == 1)
        {
            this->trace.msg(vp::trace::LEVEL_INFO, "Received stop bit\n", current_tx);
            tx_wait_start = true;
            tx_wait_stop = false;
            this->stop_tx_sampling();
        }
    }
    else
    {
        this->trace.msg(vp::trace::LEVEL_INFO, "Received data bit (data: %d)\n", current_tx);
        byte = (byte >> 1) | (current_tx << 7);
        nb_bits++;
        if (nb_bits == 8)
        {
            this->trace.msg(vp::trace::LEVEL_INFO, "Sampled TX byte (value: 0x%x)\n", byte);
            if (this->telnet)
            {
                //this->telnet_proxy->push_byte(&byte);
            }
            else if (this->stdout)
            {
                std::cout << byte;
            }
            else if (tx_file)
            {
                fwrite((void *)&byte, 1, 1, tx_file);
            }
            this->trace.msg(vp::trace::LEVEL_INFO, "Waiting for stop bit\n");
            tx_wait_stop = true;
        }
    }
}


void Uart_checker::event_handler(void *__this, vp::clock_event *event)
{
    Uart_checker *_this = (Uart_checker *)__this;

    _this->trace.msg(vp::trace::LEVEL_INFO, "EVENT HANDLER\n");

    _this->tx_sampling();

    if (_this->sampling_tx)
    {
        _this->event_enqueue(event, 2);
    }
}


void Uart_checker::sync(void *__this, int data)
{
    Uart_checker *_this = (Uart_checker *)__this;

    _this->trace.msg(vp::trace::LEVEL_TRACE, "Sync (value: %d, waiting_start: %d, loopback: %d)\n", data, _this->tx_wait_start, _this->loopback);

    if (_this->loopback)
    {
        _this->in.sync(data);
    }

    _this->current_tx = data;

    if (_this->tx_wait_start && data == 0)
    {
        _this->trace.msg(vp::trace::LEVEL_DEBUG, "Received start bit\n");

        _this->start_tx_sampling(_this->baudrate);
        _this->tx_wait_start = false;
        _this->nb_bits = 0;
    }
}

void Uart_checker::rx_sampling()
{
    rx_mutex.lock();
    this->current_rx = this->rx_bit_buffer & 0x1;
    this->rx_bit_buffer = this->rx_bit_buffer >> 1;
    //std::cerr << "Sampling bit " << current_rx << std::endl;

    //uart->rx_edge(this->current_rx);
    rx_nb_bits++;
    if (rx_nb_bits == 10)
    {
        this->stop_rx_sampling();
    }
    rx_mutex.unlock();
}

void Uart_checker::start_tx_sampling(int baudrate)
{
    // We set the frequency to twice the baudrate to be able sampling in the
    // middle of the cycle
    this->clock_cfg.set_frequency(baudrate*2);

    this->sampling_tx = 1;

    this->event_reenqueue(this->event, 3);
}

void Uart_checker::stop_tx_sampling(void)
{
    this->sampling_tx = 0;
    
    if (this->event->is_enqueued())
    {
        this->event_cancel(this->event);
    }
}

// will be called protected by a mutex
void Uart_checker::start_rx_sampling(int baudrate)
{
    this->sampling_rx = 1;
}

void Uart_checker::stop_rx_sampling(void)
{
    this->sampling_rx = 0;
    rx_nb_bits = 0;
}

void Uart_checker::dpi_task(void)
{
    while (1)
    {
        while (!(this->rx_is_sampling() || this->sampling_tx))
        {
            //this->wait_event();
        }

        //wait_ps(period/2);

        while (this->rx_is_sampling() || this->sampling_tx)
        {
            //this->wait_ps(period);
            if (this->sampling_tx)
            {
                this->tx_sampling();
            }
            if (this->rx_is_sampling())
            {
                this->rx_sampling();
            }
        }
    }
}

bool Uart_checker::rx_is_sampling(void)
{
    rx_mutex.lock();
    bool ret = this->sampling_rx;
    rx_mutex.unlock();
    return ret;
}

void Uart_checker::stdin_task(void)
{
    while (1)
    {
        //print("stdin task sampling\n");
        uint8_t c = 0;
        if (this->stdin)
        {
            c = getchar();
        }
        else if (this->telnet)
        {
            //this->telnet_proxy->pop_byte(&c);
        }
        //print("got char:%c\n",c);
        while (this->rx_is_sampling())
        { // TODO: use cond instead
            usleep(5);
        }
        // TODO: use once atomic function for this instead
        this->rx_mutex.lock();
        rx_bit_buffer = 0;
        rx_bit_buffer |= ((uint32_t)c) << 1;
        rx_bit_buffer |= 1 << 9;
        rx_nb_bits = 0;
        this->start_rx_sampling(baudrate);
        this->rx_mutex.unlock();
        //raise_event_from_ext();
        printf("raised_event\n");
    }
}

extern "C" vp::component *vp_constructor(js::config *config)
{
    return new Uart_checker(config);
}
