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

#include "testbench.hpp"
#include "spim_verif.hpp"
#include "i2s_verif.hpp"
#include <stdio.h>

Uart_flow_control_checker::Uart_flow_control_checker(Testbench *top, Uart *uart, pi_testbench_req_t *req)
: top(top), uart(uart)
{

    top->traces.new_trace("uart_" + std::to_string(uart->id) + "/flow_control", &this->trace, vp::DEBUG);

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
            this->trace.msg(vp::trace::LEVEL_DEBUG, "UART flow control received command (command: %s)\n", this->current_string.c_str());

            std::regex regex{R"([\s]+)"};
            std::sregex_token_iterator it{this->current_string.begin(), this->current_string.end(), regex, -1};
            std::vector<std::string> words{it, {}};

            if (words[0] == "START")
            {
                this->trace.msg(vp::trace::LEVEL_INFO, "UART flow control received start command\n");
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
                this->trace.msg(vp::trace::LEVEL_INFO, "UART flow control received traffic rx command (start: %d, incr: %d, size: %d, bandwidth: %d, nb_iter: %d)\n", this->rx_start, this->rx_incr, this->rx_size, this->rx_bandwidth, this->rx_nb_iter);
            }
            else if (words[0] == "TRAFFIC_TX")
            {
                this->tx_start = std::stoi(words[1]);
                this->tx_value = this->tx_start;
                this->tx_value_init = this->tx_start;
                this->tx_incr = std::stoi(words[2]);
                this->tx_size = std::stoi(words[3]);
                this->tx_iter_size = std::stoi(words[4]);
                this->tx_iter_size_init = this->tx_iter_size;
                this->trace.msg(vp::trace::LEVEL_INFO, "UART flow control received traffic tx command (start: %d, incr: %d, size: %d, iter_size: %d)\n", this->tx_start, this->tx_incr, this->tx_size, this->tx_iter_size);
            }
            else if (words[0] == "STATUS")
            {
                this->trace.msg(vp::trace::LEVEL_INFO, "UART flow control received status command\n");
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

            if (byte != (this->tx_value & 0xFF))
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

                    // Randomize a bit
                    cycles = cycles * (rand() % 100) / 100;

                    this->uart->clock->enqueue(this->bw_limiter_event, cycles);
                }

            }

            this->tx_size--;
            this->tx_iter_size--;
            if (this->tx_iter_size == 0)
            {
                this->tx_value = this->tx_value_init;
                this->tx_iter_size = this->tx_iter_size_init;
            }
            this->check_end_of_command();
        }
    
    }
}



Testbench::Testbench(js::config *config)
    : vp::component(config)
{
    this->state = STATE_WAITING_CMD;
    this->current_req_size = 0;

}


int Testbench::build()
{
    traces.new_trace("trace", &trace, vp::DEBUG);

    this->ctrl_type = get_js_config()->get("ctrl_type")->get_str();
    this->nb_gpio = get_js_config()->get("nb_gpio")->get_int();
    this->nb_spi = get_js_config()->get("nb_spi")->get_int();
    this->nb_i2s = get_js_config()->get("nb_i2s")->get_int();
    this->nb_i2c = get_js_config()->get("nb_i2c")->get_int();
    this->nb_uart = get_js_config()->get("nb_uart")->get_int();

    for (int i=0; i<this->nb_uart; i++)
    {
        this->uarts.push_back(new Uart(this, i));
    }

    this->gpios.resize(this->nb_gpio);
    
    for (int i=0; i<this->nb_gpio; i++)
    {
        this->gpios[i] = new Gpio(this);
        this->gpios[i]->itf.set_sync_meth_muxed(&Testbench::gpio_sync, i);
        this->new_slave_port("gpio" + std::to_string(i), &this->gpios[i]->itf);
    }
    
    for (int i=0; i<this->nb_spi; i++)
    {
        for (int j=0; j<4; j++)
        {
            Spi *spi = new Spi(this, i, j);
            this->spis.push_back(spi);
        }
    }

    for (int i=0; i<this->nb_i2s; i++)
    {
        I2s *i2s = new I2s(this, i);
        this->i2ss.push_back(i2s);
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
        this->uarts[uart_id]->set_dev(new Uart_reply(this, this->uarts[uart_id]));
        this->uart_ctrl = this->uarts[uart_id];
    }

    return 0;
}


void Testbench::start()
{
    if (get_js_config()->get_child_bool("spislave_boot/enabled"))
    {
        int itf = get_js_config()->get_child_int("spislave_boot/itf");
        this->spis[itf*4]->create_loader(get_js_config()->get("spislave_boot"));
    }

    for (int i=0; i<this->nb_uart; i++)
    {
        this->uarts[i]->start();
    }
}

void Uart::set_control(bool active, int baudrate)
{
    this->is_control_active = active;
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
    this->trace.msg(vp::trace::LEVEL_TRACE, "Sampling bit (value: %d)\n", uart_current_tx);

    if (uart_tx_wait_stop)
    {
        if (uart_current_tx == 1)
        {
            this->trace.msg(vp::trace::LEVEL_TRACE, "Received stop bit\n", uart_current_tx);
            uart_tx_wait_start = true;
            uart_tx_wait_stop = false;
            this->uart_stop_tx_sampling();
        }
    }
    else
    {
        this->trace.msg(vp::trace::LEVEL_TRACE, "Received data bit (data: %d)\n", uart_current_tx);
        uart_byte = (uart_byte >> 1) | (uart_current_tx << 7);
        uart_nb_bits++;
        if (uart_nb_bits == 8)
        {
            this->trace.msg(vp::trace::LEVEL_DEBUG, "Sampled TX byte (value: 0x%x)\n", uart_byte);

            if (!this->is_usart)
            {
                this->trace.msg(vp::trace::LEVEL_TRACE, "Waiting for stop bit\n");
                uart_tx_wait_stop = true;
            }
            else
            {
                uart_tx_wait_start = true;
            }
            this->handle_received_byte(uart_byte);
        }
    }
}


void Uart::check_send_byte()
{
    if (!this->is_usart && this->tx_pending_bits && !this->uart_tx_event->is_enqueued())
    {
        if (this->rtr == 0)
        {
            this->tx_clock->reenqueue(this->uart_tx_event, 2);
        }
    }
}


void Uart::send_byte(uint8_t byte)
{
    this->trace.msg(vp::trace::LEVEL_TRACE, "Send byte (value: 0x%x)\n", byte);

    this->tx_pending_byte = byte;
    this->tx_pending_bits = 8;

    if (this->is_usart)
    {
        this->tx_state = UART_TX_STATE_DATA;
        //this->send_bit();
    }
    else
    {
        this->tx_state = UART_TX_STATE_START;
        this->tx_clock_cfg.set_frequency(this->baudrate*2);
    
        this->check_send_byte();
    }
}


Uart::Uart(Testbench *top, int id)
: top(top), id(id)
{
    this->uart_sampling_event = top->event_new(this, Uart::uart_sampling_handler);
    this->uart_tx_event = top->event_new(this, Uart::uart_tx_handler);
    this->init_event = top->event_new(this, Uart::init_handler);
    this->itf.set_sync_meth(&Uart::sync);
    this->itf.set_sync_full_meth(&Uart::sync_full);
    this->top->new_slave_port(this, "uart" + std::to_string(this->id), &this->itf);
    this->top->new_master_port(this, "uart" + std::to_string(this->id) + "_clock_cfg", &clock_cfg);
    this->top->new_master_port(this, "uart" + std::to_string(this->id) + "_tx_clock_cfg", &tx_clock_cfg);

    this->clock_itf.set_reg_meth(&Uart::clk_reg);
    this->top->new_slave_port(this, "uart" + std::to_string(this->id) + "_clock", &this->clock_itf);

    this->tx_clock_itf.set_reg_meth(&Uart::tx_clk_reg);
    this->top->new_slave_port(this, "uart" + std::to_string(this->id) + "_tx_clock", &this->tx_clock_itf);

    top->traces.new_trace("uart_" + std::to_string(id), &trace, vp::DEBUG);

    this->uart_current_tx = 0;
    this->is_usart = 0;

    this->is_control_active = false;
}


void Uart::start()
{
    if (this->is_control_active)
    {
        this->clock->enqueue(this->init_event, 1);
    }
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


void Uart::init_handler(void *__this, vp::clock_event *event)
{
    Uart *_this = (Uart *)__this;
    _this->itf.sync_full(1, 2, 2);
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

    _this->trace.msg(vp::trace::LEVEL_TRACE, "UART sync (data: %d, clk: %d, rtr: %d)\n", data, clk, rtr);
    _this->rtr = rtr;
    _this->clk = clk ^ _this->polarity;

    _this->check_send_byte();

    Uart::sync(__this, data);
}


void Uart::sync(void *__this, int data)
{
    Uart *_this = (Uart *)__this;

    if (!_this->is_control && !_this->dev)
        return;

    _this->trace.msg(vp::trace::LEVEL_TRACE, "UART control sync (value: %d, waiting_start: %d)\n", data, _this->uart_tx_wait_start);

    int prev_data = _this->uart_current_tx;
    _this->uart_current_tx = data;

    if (_this->uart_tx_wait_start && prev_data == 1 && data == 0)
    {
        _this->trace.msg(vp::trace::LEVEL_TRACE, "Received start bit\n");

        if (!_this->is_usart)
        {
            _this->uart_start_tx_sampling(_this->baudrate);
        }
        _this->uart_tx_wait_start = false;
        _this->uart_nb_bits = 0;
    }
    else if (_this->is_usart)
    {
        if (_this->prev_clk != _this->clk)
        {
            int high = _this->clk ^ _this->phase;

            if (high)
            {
                _this->uart_tx_sampling();
            }
            else
            {
                _this->send_bit();
            }
        }
    }

    _this->prev_clk = _this->clk;
}


void Uart::set_cts(int cts)
{
    this->tx_cts = cts;
    this->itf.sync_full(this->tx_bit, 2, this->tx_cts);
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
            if (this->tx_pending_bits > 0)
            {
                bit = this->tx_pending_byte & 1;
                this->tx_pending_byte >>= 1;
                this->tx_pending_bits -= 1;
                this->tx_parity ^= bit;

                if (this->tx_pending_bits == 0)
                {
                    if (this->is_usart)
                    {
                        this->dev->send_byte_done();
                    }
                    else
                    {
                        if (this->tx_parity_en)
                            this->tx_state = UART_TX_STATE_PARITY;
                        else
                        {
                            this->tx_state = UART_TX_STATE_STOP;
                        }
                    }
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
    this->trace.msg(vp::trace::LEVEL_TRACE, "Sending bit (bit: %d)\n", bit);
    this->itf.sync_full(this->tx_bit, 2, this->tx_cts);

    if (!this->is_usart && this->tx_state != UART_TX_STATE_START)
    {
        this->tx_clock->reenqueue(this->uart_tx_event, 2);
    }
}


void Uart::uart_start_tx_sampling(int baudrate)
{
    this->trace.msg(vp::trace::LEVEL_TRACE, "Start TX sampling (baudrate: %d)\n", this->baudrate);

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


Uart_reply::Uart_reply(Testbench *top, Uart *uart) : top(top)
{
}


void Uart_reply::send_byte_done()
{
    this->top->send_byte_done();
}


void Testbench::send_byte_done()
{
    this->tx_buff_index++;
    if (this->tx_buff_index == this->tx_buff_size)
    {
        this->state = STATE_WAITING_CMD;
        this->current_req_size = 0;
    }
    else
    {
        this->uart_ctrl->send_byte(this->tx_buff[this->tx_buff_index]);
    }
}


void Testbench::handle_received_byte(uint8_t byte)
{
    if (this->state == STATE_WAITING_CMD)
    {
        if (this->current_req_size == 0)
        {
            this->cmd = byte;
        }
        else
        {
            this->cmd |= (byte << this->current_req_size);
        }

        this->current_req_size += 8;
        if (this->current_req_size < 32)
            return;

        switch (this->cmd & 0xffff) {
            case PI_TESTBENCH_CMD_GPIO_LOOPBACK:
            case PI_TESTBENCH_CMD_UART_CHECKER:
            case PI_TESTBENCH_CMD_SET_STATUS:
            case PI_TESTBENCH_CMD_GPIO_PULSE_GEN:
            case PI_TESTBENCH_CMD_SPIM_VERIF_SETUP:
            case PI_TESTBENCH_CMD_SPIM_VERIF_TRANSFER:
            case PI_TESTBENCH_CMD_SPIM_VERIF_SPI_WAKEUP:
            case PI_TESTBENCH_CMD_I2S_VERIF_SETUP:
            case PI_TESTBENCH_CMD_I2S_VERIF_START:
            case PI_TESTBENCH_CMD_I2S_VERIF_SLOT_START:
            case PI_TESTBENCH_CMD_I2S_VERIF_SLOT_STOP:
            case PI_TESTBENCH_CMD_I2S_VERIF_SLOT_SETUP:
                this->state = STATE_WAITING_REQUEST;
                this->req_size = cmd >> 16;
                this->current_req_size = 0;
                break;


            case PI_TESTBENCH_CMD_GET_TIME_PS:
                this->state = STATE_SENDING_REPLY;
                this->tx_buff = new uint8_t[8];
                *(uint64_t *)this->tx_buff = this->get_time();
                this->tx_buff_size = 8;
                this->tx_buff_index = 0;
                this->uart_ctrl->itf.sync_full(1, 2, 0);
                this->uart_ctrl->send_byte(this->tx_buff[0]);
                break;
        }
    }
    else if (this->state == STATE_WAITING_REQUEST)
    {
        this->req[this->current_req_size++] = byte;
        if (this->current_req_size == this->req_size)
        {
            this->state = STATE_WAITING_CMD;
            this->current_req_size = 0;

            switch (this->cmd & 0xffff) {
                case PI_TESTBENCH_CMD_GPIO_LOOPBACK:
                    this->handle_gpio_loopback();
                    break;

                case PI_TESTBENCH_CMD_UART_CHECKER:
                    this->handle_uart_checker();
                    break;

                case PI_TESTBENCH_CMD_SET_STATUS:
                    this->handle_set_status();
                    break;

                case PI_TESTBENCH_CMD_GPIO_PULSE_GEN:
                    this->handle_gpio_pulse_gen();
                    break;

                case PI_TESTBENCH_CMD_SPIM_VERIF_SETUP:
                    this->handle_spim_verif_setup();
                    break;

                case PI_TESTBENCH_CMD_SPIM_VERIF_TRANSFER:
                    this->handle_spim_verif_transfer();
                    break;

                case PI_TESTBENCH_CMD_SPIM_VERIF_SPI_WAKEUP:
                    this->handle_spim_verif_spi_wakeup();
                    break;

                case PI_TESTBENCH_CMD_I2S_VERIF_SETUP:
                    this->handle_i2s_verif_setup();
                    break;

                case PI_TESTBENCH_CMD_I2S_VERIF_START:
                    this->handle_i2s_verif_start();
                    break;

                case PI_TESTBENCH_CMD_I2S_VERIF_SLOT_SETUP:
                    this->handle_i2s_verif_slot_setup();
                    break;

                case PI_TESTBENCH_CMD_I2S_VERIF_SLOT_START:
                {
                    this->handle_i2s_verif_slot_start();
                    break;
                }

                case PI_TESTBENCH_CMD_I2S_VERIF_SLOT_STOP:
                {
                    this->handle_i2s_verif_slot_stop();
                    break;
                }
            }
        }
    }
}


void Spi::create_loader(js::config *load_config)
{
    uint8_t enabled;
    uint8_t itf;
    uint8_t cs;
    uint8_t is_master;
    uint16_t mem_size_log2;
    pi_testbench_req_spim_verif_setup_t config;
    config.enabled = 1;
    config.itf = this->itf_id;
    config.cs = 0;
    config.is_master = 1;
    config.mem_size_log2 = 20;

    this->spim_verif = new Spim_verif(this->top, this, &this->itf, &config);

    this->spim_verif->enqueue_spi_load(load_config);
}


void Spi::sync(void *__this, int sck, int data_0, int data_1, int data_2, int data_3, int mask)
{
    Spi *_this = (Spi *)__this;
    if (_this->spim_verif)
    {
        _this->spim_verif->sync(sck, data_0, data_1, data_2, data_3, mask);
    }
}


void Spi::cs_sync(void *__this, bool active)
{
    Spi *_this = (Spi *)__this;
    if (_this->spim_verif)
    {
        _this->spim_verif->cs_sync(!active);
    }
}



void Testbench::gpio_sync(void *__this, int value, int id)
{
    Testbench *_this = (Testbench *)__this;
    Gpio *gpio = _this->gpios[id];

    _this->trace.msg(vp::trace::LEVEL_DEBUG, "Received GPIO sync (id: %d)\n", id);

    gpio->value = value;

    if (gpio->loopback != -1)
    {
        _this->trace.msg(vp::trace::LEVEL_DEBUG, "Generating gpio on loopback (id: %d)\n", gpio->loopback);
        _this->gpios[gpio->loopback]->itf.sync(value);
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
        Uart_dev *dev = new Uart_flow_control_checker(this, uart, req);
        uart->set_dev(dev);
        uart->is_usart = req->uart.usart;
        uart->polarity = req->uart.polarity;
        uart->phase = req->uart.phase;
    }
}


void Testbench::handle_set_status()
{
    pi_testbench_req_t *req = (pi_testbench_req_t *)this->req;

    dpi_set_status(req->set_status.status);
}


void Testbench::handle_gpio_loopback()
{
    pi_testbench_req_t *req = (pi_testbench_req_t *)this->req;

    this->trace.msg(vp::trace::LEVEL_INFO, "Handling GPIO loopback (enabled: %d, output: %d, intput: %d)\n", req->gpio.enabled, req->gpio.output, req->gpio.input);

    if (req->gpio.enabled)
    {
        this->gpios[req->gpio.output]->loopback = req->gpio.input;
        this->gpios[req->gpio.input]->itf.sync(this->gpios[req->gpio.output]->value);
    }
    else
    {
        this->gpios[req->gpio.output]->loopback = -1;
    }
}


void Testbench::handle_gpio_pulse_gen()
{
    pi_testbench_req_t *req = (pi_testbench_req_t *)this->req;

    bool enabled = req->gpio_pulse_gen.enabled;
    int gpio_id = req->gpio_pulse_gen.gpio;
    int64_t duration_ps = req->gpio_pulse_gen.duration_ps;
    int64_t period_ps = req->gpio_pulse_gen.period_ps;
    Gpio *gpio = this->gpios[gpio_id];
    
    this->trace.msg(vp::trace::LEVEL_INFO, "Handling GPIO pulse generator (gpio: %d, enabled: %d, duration_ps: %ld, period_ps: %ld)\n",
        gpio_id, enabled, duration_ps, period_ps);

    gpio->pulse_enabled = enabled;

    gpio->itf.sync(0);

    if (enabled)
    {
        gpio->pulse_duration_ps = duration_ps;
        gpio->pulse_period_ps = period_ps;
        gpio->pulse_gen_rising_edge = true;
        this->event_enqueue(gpio->pulse_event, req->gpio_pulse_gen.first_delay_ps);
    }
}

void Testbench::handle_spim_verif_setup()
{
    pi_testbench_req_spim_verif_setup_t *req = (pi_testbench_req_spim_verif_setup_t *)this->req;

    int itf = req->itf;
    int cs = req->cs;

    this->trace.msg(vp::trace::LEVEL_INFO, "Handling Spim verif setup (itf: %d, cs: %d, mem_size: %d)\n",
        itf, cs, 1<<req->mem_size_log2);

    this->spis[cs + itf*4]->spim_verif_setup(req);
}


void Testbench::handle_spim_verif_transfer()
{
    pi_testbench_req_spim_verif_transfer_t *req = (pi_testbench_req_spim_verif_transfer_t *)this->req;

    int itf = req->itf;
    int cs = req->cs;

    this->trace.msg(vp::trace::LEVEL_INFO, "Handling Spim verif transfer (itf: %d, cs: %d)\n",
        itf, cs);

    this->spis[cs + itf*4]->spim_verif_transfer((pi_testbench_req_spim_verif_transfer_t *)req);
}


void Testbench::handle_spim_verif_spi_wakeup()
{
    pi_testbench_req_spim_verif_spi_wakeup_t *req = (pi_testbench_req_spim_verif_spi_wakeup_t *)this->req;

    int itf = req->itf;
    int cs = req->cs;

    this->trace.msg(vp::trace::LEVEL_INFO, "Handling Spim verif spi wakeup (itf: %d, cs: %d)\n",
        itf, cs);

    this->spis[cs + itf*4]->spim_verif_spi_wakeup((pi_testbench_req_spim_verif_spi_wakeup_t *)req);
}


void Testbench::handle_i2s_verif_setup()
{
    pi_testbench_i2s_verif_config_t *req = (pi_testbench_i2s_verif_config_t *)this->req;
    int itf = req->itf;

    if (itf >= this->nb_i2s)
    {
        this->trace.fatal("Invalid I2S interface (id: %d, nb_interfaces: %d)", itf, this->nb_i2s);
        return;
    }

    this->i2ss[itf]->i2s_verif_setup(req);
}

void Testbench::handle_i2s_verif_start()
{
    pi_testbench_i2s_verif_start_config_t *req = (pi_testbench_i2s_verif_start_config_t *)this->req;
    int itf = req->itf;

    if (itf >= this->nb_i2s)
    {
        this->trace.fatal("Invalid I2S interface (id: %d, nb_interfaces: %d)", itf, this->nb_i2s);
        return;
    }

    this->i2ss[itf]->i2s_verif_start(req);
}


void Testbench::handle_i2s_verif_slot_setup()
{
    pi_testbench_i2s_verif_slot_config_t *req = (pi_testbench_i2s_verif_slot_config_t *)this->req;
    int itf = req->itf;

    if (itf >= this->nb_i2s)
    {
        this->trace.fatal("Invalid I2S interface (id: %d, nb_interfaces: %d)", itf, this->nb_i2s);
        return;
    }

    this->i2ss[itf]->i2s_verif_slot_setup(req);
}


void Testbench::handle_i2s_verif_slot_start()
{
    pi_testbench_i2s_verif_slot_start_config_t *req = (pi_testbench_i2s_verif_slot_start_config_t *)this->req;
    int itf = req->itf;

    if (itf >= this->nb_i2s)
    {
        this->trace.fatal("Invalid I2S interface (id: %d, nb_interfaces: %d)", itf, this->nb_i2s);
        return;
    }

    this->i2ss[itf]->i2s_verif_slot_start(req);
}


void Testbench::handle_i2s_verif_slot_stop()
{
    pi_testbench_i2s_verif_slot_stop_config_t *req = (pi_testbench_i2s_verif_slot_stop_config_t *)this->req;
    int itf = req->itf;

    if (itf >= this->nb_i2s)
    {
        this->trace.fatal("Invalid I2S interface (id: %d, nb_interfaces: %d)", itf, this->nb_i2s);
        return;
    }

    this->i2ss[itf]->i2s_verif_slot_stop(req);
}


void Gpio::pulse_handler(void *__this, vp::clock_event *event)
{
    Gpio *_this = (Gpio *)__this;

    _this->itf.sync(_this->pulse_gen_rising_edge);

    if (_this->pulse_gen_rising_edge)
    {
        _this->top->event_enqueue(_this->pulse_event, _this->pulse_duration_ps);
    }
    else if (_this->pulse_period_ps > 0)
    {
        _this->top->event_enqueue(_this->pulse_event, _this->pulse_period_ps - _this->pulse_duration_ps);
    }
    
    _this->pulse_gen_rising_edge ^= 1;
}


Gpio::Gpio(Testbench *top) : top(top)
{
    this->pulse_event = top->event_new(this, Gpio::pulse_handler);
}


Spi::Spi(Testbench *top, int itf, int cs) : top(top)
{
    this->itf.set_sync_meth(&Spi::sync);
    this->top->new_slave_port(this, "spi" + std::to_string(itf) + "_cs" + std::to_string(cs) + "_data", &this->itf);
    this->cs_itf.set_sync_meth(&Spi::cs_sync);
    this->top->new_slave_port(this, "spi" + std::to_string(itf) + "_cs" + std::to_string(cs), &this->cs_itf);
    
    this->spim_verif = NULL;
    this->itf_id = itf;
    this->cs = cs;
}


void Spi::spim_verif_setup(pi_testbench_req_spim_verif_setup_t *config)
{
    if (this->spim_verif)
    {
        delete this->spim_verif;
        this->spim_verif = NULL;
    }

    if (config->enabled)
    {
        this->spim_verif = new Spim_verif(this->top, this, &this->itf, config);
    }
}


void Spi::spim_verif_transfer(pi_testbench_req_spim_verif_transfer_t *config)
{
    if (this->spim_verif)
    {
        this->spim_verif->transfer(config);
    }

}


void Spi::spim_verif_spi_wakeup(pi_testbench_req_spim_verif_spi_wakeup_t *config)
{
    if (this->spim_verif)
    {
        this->spim_verif->spi_wakeup(config);
    }
}



I2s::I2s(Testbench *top, int itf) : top(top)
{
    this->i2s_verif = NULL;
    this->itf_id = itf;
    this->clk_propagate = 0;
    this->ws_propagate = 0;

    this->itf.set_sync_meth(&I2s::sync);
    top->new_master_port(this, "i2s" + std::to_string(itf), &this->itf);

    top->traces.new_trace("i2s_itf" + std::to_string(itf), &trace, vp::DEBUG);
}


void I2s::i2s_verif_slot_start(pi_testbench_i2s_verif_slot_start_config_t *config)
{
    if (!this->i2s_verif)
    {
        this->trace.fatal("Trying to start slot in inactive interface (itf: %d)", this->itf_id);
        return;
    }

    this->i2s_verif->slot_start(config);
}


void I2s::i2s_verif_slot_stop(pi_testbench_i2s_verif_slot_stop_config_t *config)
{
    if (!this->i2s_verif)
    {
        this->trace.fatal("Trying to start slot in inactive interface (itf: %d)", this->itf_id);
        return;
    }

    this->i2s_verif->slot_stop(config);
}


void I2s::sync(void *__this, int sck, int ws, int sd)
{
    I2s *_this = (I2s *)__this;

    if (_this->i2s_verif)
    {
        _this->i2s_verif->sync(sck, ws, sd);
    }

    if (_this->clk_propagate)
    {
        for (int i=0; i<_this->top->nb_i2s; i++)
        {
            if ((_this->clk_propagate >> i) & 1)
            {
                _this->top->i2ss[i]->sync_sck(sck);
            }
        }
    }

    if (_this->ws_propagate)
    {
        for (int i=0; i<_this->top->nb_i2s; i++)
        {
            if ((_this->ws_propagate >> i) & 1)
            {
                _this->top->i2ss[i]->sync_ws(ws);
            }
        }
    }
}

void I2s::sync_sck(int sck)
{
    if (this->i2s_verif)
    {
        this->i2s_verif->sync_sck(sck);
    }
}

void I2s::sync_ws(int ws)
{
    if (this->i2s_verif)
    {
        this->i2s_verif->sync_ws(ws);
    }
}


void I2s::i2s_verif_setup(pi_testbench_i2s_verif_config_t *config)
{
    this->trace.msg(vp::trace::LEVEL_INFO, "I2S verif setup (enabled: %d, sampling_rate: %d, word_size: %d, nb_slots: %d)\n",
        config->enabled, config->sampling_freq, config->word_size, config->nb_slots);

    if (this->i2s_verif)
    {
        delete this->i2s_verif;
        this->i2s_verif = NULL;
    }

    if (config->enabled)
    {
        this->i2s_verif = new I2s_verif(this->top, &this->itf, this->itf_id, config);
    }
}


void I2s::i2s_verif_start(pi_testbench_i2s_verif_start_config_t *config)
{
    this->trace.msg(vp::trace::LEVEL_INFO, "I2S verif start (start: %d)\n",
        config->start);

    if (!this->i2s_verif)
    {
        this->trace.fatal("Trying to start inactive interface (itf: %d)", this->itf_id);
        return;
    }

    this->i2s_verif->start(config);
}


void I2s::i2s_verif_slot_setup(pi_testbench_i2s_verif_slot_config_t *config)
{
    this->trace.msg(vp::trace::LEVEL_INFO, "I2S verif slot setup (slot: %d, is_rx: %d, enabled: %d, word_size: %d)\n",
        config->slot, config->is_rx, config->enabled, config->word_size);

    if (!this->i2s_verif)
    {
        this->trace.fatal("Trying to configure slot in inactive interface (itf: %d, slot: %d)", this->itf_id, config->slot);
        return;
    }

    this->i2s_verif->slot_setup(config);
}


extern "C" vp::component *vp_constructor(js::config *config)
{
    return new Testbench(config);
}
