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



#include "nina_b112.hpp"

#include <random>
#include <regex>

Nina_b112::Nina_b112(js::config *config)
    : vp::component(config)
{
    /* Behavior configuration */
    this->behavior.loopback_size = 200;
    js::config* behavior = config->get("behavior");
    if (NULL != behavior)
    {
        /* configure loopback size */
        js::config* loopback_size_elt = behavior->get("loopback_size");
        int loopback_size = 200;
        if (NULL != loopback_size_elt)
        {
            loopback_size = loopback_size_elt->get_int();
            if (loopback_size <= 0)
            {
                loopback_size = 200;
            }
        }

        this->behavior.loopback_size = loopback_size;
    }
    /* RTS generation configuration */
    this->rts_gen.enabled = false;
    js::config* rts = config->get("rts");
    if (NULL != rts)
    {
        /* configure whether rts generation is enabled */
        js::config* enabled_elt = rts->get("enabled");
        bool enabled = false;
        if (NULL != enabled_elt)
        {
            enabled = enabled_elt->get_bool();
        }

        /* configure how many words the model can receive before triggering a rts event */
        js::config* buffer_limit_elt = rts->get("buffer_limit");
        int buffer_limit = 0;
        if (NULL != buffer_limit_elt)
        {
            buffer_limit = buffer_limit_elt->get_int();
            if (buffer_limit <= 0)
            {
                buffer_limit = 1;
            }
        }

        /* configure how long the rts will last (in uart cycles) */
        js::config* duration_elt = rts->get("duration");
        int duration = 0;
        if (NULL != duration_elt)
        {
            duration = duration_elt->get_int();
            if (duration <= 0)
            {
                duration = 1;
            }
        }

        /* configure random seed */
        js::config* random_seed_elt = rts->get("random_seed");
        int random_seed = 1234;
        if (NULL != random_seed_elt)
        {
            random_seed = random_seed_elt->get_int();
        }

        /* configure random high limit */
        js::config* random_high_elt = rts->get("random_high");
        int random_high = 0;
        if (NULL != random_high_elt)
        {
            random_high = random_high_elt->get_int();
            if (random_high <= 0)
            {
                random_high = 10000;
            }
        }

        /* configure random threshold */
        /* Below this threshold, RTS will trigger */
        js::config* random_threshold_elt = rts->get("random_threshold");
        int random_threshold = 0;
        if (NULL != random_threshold_elt)
        {
            random_threshold = random_threshold_elt->get_int();
            if (!(random_threshold >= 0 && random_threshold < random_high))
            {
                random_threshold = 0;
            }
        }

        std::mt19937 rd_gen(random_seed);
        std::uniform_int_distribution<> rd_distrib(0, random_high);

        this->rts_gen.enabled = enabled;
        this->rts_gen.trigger =false;
        this->rts_gen.bit_trigger = 0;
        this->rts_gen.buffer_limit = buffer_limit;
        this->rts_gen.duration = duration;
        this->rts_gen.random_threshold = random_threshold;
        this->rts_gen.random_generator = rd_gen;
        this->rts_gen.random_dist = rd_distrib;
    }
}


void Nina_b112::sync_full(void *__this, int data, int clk, int rts)
{
    Nina_b112 *_this = (Nina_b112 *)__this;

    _this->tx_cts = rts;

    if (_this->rx_state == UART_RX_STATE_WAIT_START && _this->rx_prev_data == 1 && data == 0)
    {
        _this->trace.msg(vp::trace::LEVEL_TRACE, "Received start bit\n");

        _this->rx_start_sampling();
        _this->rx_state = UART_RX_STATE_GET_DATA;
        _this->rx_nb_bits = 0;
    }

    _this->rx_prev_data = data;
}


void Nina_b112::rx_start_sampling()
{
    this->trace.msg(vp::trace::LEVEL_TRACE, "Start RX sampling (baudrate: %d)\n", this->uart_cfg.baudrate);

    // We set the frequency to twice the baudrate to be able sampling in the
    // middle of the cycle
    this->rx_clock_cfg.set_frequency(this->uart_cfg.baudrate * 2);

    this->rx_sampling = 1;

    this->rx_clock->reenqueue(this->rx_sampling_event, 3);
}


void Nina_b112::rx_stop_sampling()
{
    this->rx_sampling = 0;

    if (this->rx_sampling_event->is_enqueued())
    {
        this->rx_clock->cancel(this->rx_sampling_event);
    }
}


void Nina_b112::rx_handle_byte(uint8_t byte)
{
    static std::string input_buffer("");

    this->trace.msg(vp::trace::LEVEL_TRACE, "Received byte 0x%x\n", byte);

    /* add incoming byte to input buffer */
    input_buffer.push_back((char)byte);
    //this->trace.msg(vp::trace::LEVEL_TRACE, "Input_buffer (size: %d): %s\n", input_buffer.size(), input_buffer.c_str());

    switch(this->behavior.operating_mode)
    {
        case NINA_B112_OPERATING_MODE_COMMAND:
            {
                /* only seach for a command when byte is '\r' */
                if (byte == '\r')
                {
                    /* search for a command in the input_buffer */
                    std::regex command_regexp("AT.*\r");
                    std::smatch match;

                    if(std::regex_match(input_buffer, match, command_regexp))
                    {
                        std::string received_command("");
                        received_command.append(match[0]);
                        this->trace.msg(vp::trace::LEVEL_TRACE, "Received command(size: %d) %s\n",
                                received_command.size(),
                                received_command.c_str());

                        std::string input_cmd = input_buffer;
                        input_cmd.erase(0, 2);
                        input_cmd.pop_back();

                        /* act depending on received command */
                        this->tx_send_buffer(this->parse_cmd(input_cmd));

                        /* reset input buffer */
                        input_buffer.clear();
                    }
                }
                break;
            }
        case NINA_B112_OPERATING_MODE_DATA:
            {
                /* store a buffer, and then send it back */
                //this->trace.msg(vp::trace::LEVEL_TRACE, "Data operating mode\n");
                if (input_buffer.size() >= this->behavior.loopback_size)
                {
                    this->trace.msg(vp::trace::LEVEL_INFO, "Sending back data received\n");
                    this->tx_send_buffer(input_buffer);
                    input_buffer.clear();
                }
                break;
            }
        default:
            {
                this->trace.msg(vp::trace::LEVEL_TRACE, "Unexpected operating mode\n");
                break;
            }

    }
}


void Nina_b112::rx_handle_sampling()
{
    //this->trace.msg(vp::trace::LEVEL_TRACE, "Sampling bit (value: %d)\n", this->rx_prev_data);

    static int bytes_counter = 0;

    switch (this->rx_state)
    {
        case UART_RX_STATE_GET_DATA:
        {
            this->trace.msg(vp::trace::LEVEL_TRACE, "Received data bit (data: %d)\n", this->rx_prev_data);
            this->rx_byte = (this->rx_byte >> 1) | (this->rx_prev_data << 7);
            this->rx_nb_bits++;
            if (this->rx_nb_bits == this->uart_cfg.data_bits)
            {
                /* a whole word has been received */
                this->trace.msg(vp::trace::LEVEL_DEBUG, "Sampled RX byte (value: 0x%x)\n", this->rx_byte);
                this->trace.msg(vp::trace::LEVEL_TRACE, "Waiting for stop bit\n");
                if (this->uart_cfg.parity != NINA_B112_UART_PARITY_NONE)
                {
                    this->rx_state = UART_RX_STATE_PARITY;
                }
                else
                {
                    this->rx_state = UART_RX_STATE_WAIT_STOP;
                }
                this->rx_handle_byte(this->rx_byte);
            }

            if (this->uart_cfg.flow_control && this->rts_gen.trigger &&
                (this->rts_gen.bit_trigger == this->rx_nb_bits) &&
                !this->rts_event->is_enqueued())
            {
                this->trace.msg(vp::trace::LEVEL_TRACE, "triggering cts\n", this->rx_prev_data);
                this->rx_clock->enqueue(this->rts_event, this->rts_gen.duration);
                this->set_rts(1);
            }

            break;
        }

        case UART_RX_STATE_PARITY:
        {
            this->trace.msg(vp::trace::LEVEL_TRACE, "Received parity bit (data: %d)\n", this->rx_prev_data);
            this->rx_state = UART_RX_STATE_WAIT_STOP;

            break;
        }

        case UART_RX_STATE_WAIT_STOP:
        {
            if (this->rx_prev_data == 1)
            {
                bytes_counter++;
                this->trace.msg(vp::trace::LEVEL_TRACE, "Received stop bit\n");
                this->rx_state = UART_RX_STATE_WAIT_START;
                this->rx_stop_sampling();

                /* decide if next byte will trigger cts */
                if (this->rts_gen.enabled &&
                        ((bytes_counter > this->rts_gen.buffer_limit) ||
                         this->rts_gen.random_dist(this->rts_gen.random_generator) < this->rts_gen.random_threshold
                        )
                   )
                {
                    this->trace.msg(vp::trace::LEVEL_INFO, "triggering cts on next byte\n");
                    bytes_counter = 0;
                    this->rts_gen.trigger = true;
                    this->rts_gen.bit_trigger = ((this->rts_gen.bit_trigger + 1) % this->uart_cfg.data_bits) + 1;
                }

            }
            break;
        }
    }
}


void Nina_b112::rx_sampling_handler(void *__this, vp::clock_event *event)
{
    Nina_b112 *_this = (Nina_b112 *)__this;

    _this->rx_handle_sampling();

    if (_this->rx_sampling)
    {
        _this->rx_clock->enqueue(_this->rx_sampling_event, 2);
    }
}


void Nina_b112::set_rts(int rts)
{
    this->rx_rts = rts;
    //this->trace.msg(vp::trace::LEVEL_TRACE, "SET CTS\n");
    this->uart_itf.sync_full(this->tx_bit, 2, this->rx_rts);
}


void Nina_b112::tx_sampling_handler(void *__this, vp::clock_event *event)
{
    Nina_b112 *_this = (Nina_b112 *)__this;
    _this->tx_send_bit();
}


void Nina_b112::tx_send_bit()
{
    // TODO move static variables to an object or struct
    static nina_b112_uart_cfg_t tx_uart_cfg = this->uart_cfg;
    static int bits_sent;

    static int tx_parity;
    static int tx_current_stop_bits;
    static int tx_pending_bits;
    static int tx_current_pending_byte;

    int bit = 1;

    switch (this->tx_state)
    {
        case UART_TX_STATE_IDLE:
        {
            /* update local baudrate parameters */
            /* parameters should only be updated after sending a response */
            this->tx_clock_cfg.set_frequency(tx_uart_cfg.baudrate * 2);

            if(!this->tx_pending_bytes.empty())
            {
                this->trace.msg(vp::trace::LEVEL_TRACE, "Initiating new byte transfer\n");
                this->tx_state = UART_TX_STATE_START;
            }
            break;
        }

        case UART_TX_STATE_START:
        {
            if ((tx_uart_cfg.flow_control == false) || 0 == this->tx_cts)
            {
                if(!this->tx_pending_bytes.empty())
                {
                    this->trace.msg(vp::trace::LEVEL_TRACE, "Sending start bit\n");
                    tx_parity = 0;
                    this->tx_state = UART_TX_STATE_DATA;
                    tx_current_stop_bits = 1;

                    tx_current_pending_byte = this->tx_pending_bytes.front();
                    this->tx_pending_bytes.pop();
                    tx_pending_bits = tx_uart_cfg.data_bits;

                    bit = 0;
                    bits_sent = 0;
                }
            }
            else
            {
                this->trace.msg(vp::trace::LEVEL_TRACE, "Waiting for CTS to clear\n");
            }
            break;
        }

        case UART_TX_STATE_DATA:
        {
            if (tx_pending_bits > 0)
            {
                bits_sent++;
                bit = tx_current_pending_byte & 1;
                this->trace.msg(vp::trace::LEVEL_TRACE, "Sending data bit #%d (value: %d)\n", bits_sent, bit);
                tx_current_pending_byte >>= 1;
                tx_pending_bits -= 1;
                tx_parity ^= bit;

            }
            else
            {
                if (tx_uart_cfg.parity != NINA_B112_UART_PARITY_NONE)
                {
                    this->tx_state = UART_TX_STATE_PARITY;
                }
                else
                {
                    this->tx_state = UART_TX_STATE_STOP;
                }
            }
            break;
        }

        case UART_TX_STATE_PARITY:
        {
            bit = tx_parity;
            this->trace.msg(vp::trace::LEVEL_TRACE, "Sending parity bit (value: %d)\n", bit);
            this->tx_state = UART_TX_STATE_STOP;
            break;
        }

        case UART_TX_STATE_STOP:
        {
            this->trace.msg(vp::trace::LEVEL_TRACE, "Sending stop bit\n", this->rx_prev_data);
            bit = 1;
            tx_current_stop_bits--;

            if (tx_current_stop_bits == 0)
            {
                this->tx_state = UART_TX_STATE_IDLE;
            }

            break;
        }
    }

    /* send data bit */
    this->tx_bit = bit;
    this->uart_itf.sync_full(this->tx_bit, 2, this->rx_rts);

    /* always reenqueue */
    if(!(this->tx_pending_bytes.empty() && this->tx_state == UART_TX_STATE_IDLE))
    {
        this->tx_clock->reenqueue(this->tx_sampling_event, 2);
    }
    else
    {
        tx_uart_cfg = this->uart_cfg;
    }
}

void Nina_b112::tx_send_buffer(std::string buffer)
{
    for (size_t i = 0; i < buffer.size(); i++)
    {
        this->tx_send_byte(buffer[i]);
    }
}

void Nina_b112::tx_check_pending_byte()
{
    if (!this->tx_pending_bytes.empty() && !this->tx_sampling_event->is_enqueued())
    {
        this->tx_clock->reenqueue(this->tx_sampling_event, 2);
    }
}


void Nina_b112::tx_send_byte(uint8_t byte)
{
    this->trace.msg(vp::trace::LEVEL_DEBUG, "Sending byte (value: 0x%x)\n", byte);
    this->tx_pending_bytes.push(byte);
    this->tx_check_pending_byte();
}



void Nina_b112::rx_clk_reg(vp::component *__this, vp::component *clock)
{
    Nina_b112 *_this = (Nina_b112 *)__this;
    _this->rx_clock = (vp::clock_engine *)clock;
}


void Nina_b112::tx_clk_reg(vp::component *__this, vp::component *clock)
{
    Nina_b112 *_this = (Nina_b112 *)__this;
    _this->tx_clock = (vp::clock_engine *)clock;
}


int Nina_b112::build()
{
    traces.new_trace("trace", &trace, vp::DEBUG);

    /* Initialize clocks and ports */
    this->rx_clock_itf.set_reg_meth(&Nina_b112::rx_clk_reg);
    this->new_slave_port(this, "rx_clock", &this->rx_clock_itf);

    this->tx_clock_itf.set_reg_meth(&Nina_b112::tx_clk_reg);
    this->new_slave_port(this, "tx_clock", &this->tx_clock_itf);

    this->new_master_port(this, "tx_clock_cfg", &this->tx_clock_cfg);

    this->new_master_port(this, "rx_clock_cfg", &this->rx_clock_cfg);

    this->uart_itf.set_sync_full_meth(&Nina_b112::sync_full);
    this->new_slave_port(this, "uart", &this->uart_itf);

    /* Initialize events */
    this->init_event = this->event_new(Nina_b112::init_handler);
    this->rts_event = this->event_new(Nina_b112::rts_end_handler);
    this->rx_sampling_event = this->event_new(Nina_b112::rx_sampling_handler);
    this->tx_sampling_event = this->event_new(Nina_b112::tx_sampling_handler);

    this->rx_state = UART_RX_STATE_WAIT_START;
    this->rx_prev_data = 1;

    this->tx_state = UART_TX_STATE_IDLE;
    this->tx_bit = 1;

    this->rx_rts = 0; /* we are ready to receive */

    /* Initialize UART configuration */
    this->uart_cfg.baudrate = 115200;
    this->uart_cfg.data_bits = 8;
    this->uart_cfg.stop_bits = 1;
    this->uart_cfg.parity = NINA_B112_UART_PARITY_NONE;
    this->uart_cfg.flow_control = true;

    /* Initialize behavior */
    this->behavior.operating_mode = NINA_B112_OPERATING_MODE_COMMAND;
    this->behavior.local_name = std::string("Bluetooth Device");

    return 0;
}

void Nina_b112::start()
{
    this->trace.msg(vp::trace::LEVEL_TRACE, "starting nina b112 model\n");

    /* Initialize tx with 1 */
    this->uart_itf.sync_full(1, 2, 2);
    this->rx_clock->enqueue(this->init_event, 1);
}


void Nina_b112::init_handler(void *__this, vp::clock_event *event)
{
    Nina_b112 *_this = (Nina_b112 *)__this;

    _this->uart_itf.sync_full(1, 2, 0);
}

void Nina_b112::rts_end_handler(void *__this, vp::clock_event *event)
{
    Nina_b112 *_this = (Nina_b112 *)__this;

    _this->trace.msg(vp::trace::LEVEL_TRACE, "rts_end_handler\n");
    _this->rts_gen.trigger = false;
    _this->set_rts(0);
}


extern "C" vp::component *vp_constructor(js::config *config)
{
    return new Nina_b112(config);
}
