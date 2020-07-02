/*
 * Copyright (C) 2020  GreenWaves Technologies, SAS
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

/* 
 * Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */

#include "../udma_impl.hpp"
#include "archi/utils.h"
#include "vp/itf/uart.hpp"
#include "udma_uart_v2.hpp"

using namespace std::placeholders;


Uart_periph::Uart_periph(udma *top, int id, int itf_id) : Udma_periph(top, id)
{
    std::string itf_name = "uart" + std::to_string(itf_id);

    top->traces.new_trace(itf_name, &trace, vp::DEBUG);

    channel0 = new Uart_rx_channel(top, this, itf_name + "_rx");
    channel1 = new Uart_tx_channel(top, this, itf_name + "_tx");

    top->new_master_port(this, itf_name, &uart_itf);

    uart_itf.set_sync_meth(&Uart_periph::rx_sync);

    this->regmap.build(top, &this->trace, itf_name);

    this->regmap.rx_dest.register_callback(std::bind(&Uart_periph::rx_dest_req, this, _1, _2, _3, _4));
    this->regmap.tx_dest.register_callback(std::bind(&Uart_periph::tx_dest_req, this, _1, _2, _3, _4));
    this->regmap.status.register_callback(std::bind(&Uart_periph::status_req, this, _1, _2, _3, _4));
}


void Uart_periph::rx_dest_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.rx_dest.update(reg_offset, size, value, is_write);

    if (is_write)
    {
        this->trace.msg(vp::trace::LEVEL_INFO, "Setting RX channel (id: %d)\n", this->regmap.rx_dest.rx_dest_get());
        this->top->channel_register(this->regmap.rx_dest.rx_dest_get(), this->channel0);
    }
}


void Uart_periph::tx_dest_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.tx_dest.update(reg_offset, size, value, is_write);

    if (is_write)
    {
        this->trace.msg(vp::trace::LEVEL_INFO, "Setting TX channel (id: %d)\n", this->regmap.tx_dest.tx_dest_get());
        this->top->channel_register(this->regmap.tx_dest.tx_dest_get(), this->channel1);
    }
}


void Uart_periph::status_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.status.tx_busy_set(this->channel1->is_busy());
    this->regmap.status.rx_busy_set(this->channel0->is_busy());

    this->regmap.status.update(reg_offset, size, value, is_write);
}


void Uart_periph::reset(bool active)
{
    Udma_periph::reset(active);
}



vp::io_req_status_e Uart_periph::custom_req(vp::io_req *req, uint64_t offset)
{
    this->regmap.access(offset, req->get_size(), req->get_data(), req->get_is_write());

    return vp::IO_REQ_OK;
}


void Uart_periph::rx_sync(void *__this, int data)
{
    Uart_periph *_this = (Uart_periph *)__this;
    (static_cast<Uart_rx_channel *>(_this->channel0))->handle_rx_bit(data);
}


Uart_tx_channel::Uart_tx_channel(udma *top, Uart_periph *periph, string name)
    : Udma_tx_channel(top, name), periph(periph)
{
    pending_word_event = top->event_new(this, Uart_tx_channel::handle_pending_word);
}


void Uart_tx_channel::handle_pending_word(void *__this, vp::clock_event *event)
{
    Uart_tx_channel *_this = (Uart_tx_channel *)__this;

    int bit = -1;
    bool end = false;

    if (_this->state == UART_TX_STATE_START)
    {
        _this->parity = 0;
        _this->state = UART_TX_STATE_DATA;
        bit = 0;
    }
    else if (_this->state == UART_TX_STATE_DATA)
    {
        bit = _this->pending_word & 1;
        _this->pending_word >>= 1;
        _this->pending_bits -= 1;
        _this->parity ^= bit;
        _this->sent_bits++;

        if (_this->pending_bits == 0)
        {
            end = true;
        }

        if (_this->sent_bits == _this->periph->regmap.setup.bit_length_get() + 5)
        {
            _this->sent_bits = 0;
            if (_this->periph->regmap.setup.parity_ena_get())
                _this->state = UART_TX_STATE_PARITY;
            else
            {
                _this->stop_bits = _this->periph->regmap.setup.stop_bits_get();
                _this->state = UART_TX_STATE_STOP;
            }
        }
    }
    else if (_this->state == UART_TX_STATE_PARITY)
    {
        bit = _this->parity;
        _this->stop_bits = _this->periph->regmap.setup.stop_bits_get();
        _this->state = UART_TX_STATE_STOP;
    }
    else if (_this->state == UART_TX_STATE_STOP)
    {
        bit = 1;
        _this->stop_bits--;
        if (_this->stop_bits == 0)
        {
            _this->state = UART_TX_STATE_START;
        }
    }

    if (bit != -1)
    {
        if (!_this->periph->uart_itf.is_bound())
        {
            _this->top->get_trace()->warning("Trying to send to UART interface while it is not connected\n");
        }
        else
        {
            if (_this->periph->regmap.setup.tx_ena_get())
            {
                _this->next_bit_cycle = _this->periph->top->get_periph_clock()->get_cycles() + _this->periph->regmap.setup.clkdiv_get() + 2;
                _this->top->get_trace()->msg(vp::trace::LEVEL_TRACE, "Sending bit (value: %d)\n", bit);
                _this->periph->uart_itf.sync(bit);
            }
        }
    }

    if (end)
    {
        _this->handle_ready_req_end(_this->pending_req);
        _this->handle_ready_reqs();
    }

    _this->check_state();
}


void Uart_tx_channel::check_state()
{
    if ((this->pending_bits != 0 || this->stop_bits) && !pending_word_event->is_enqueued())
    {
        int latency = 1;
        int64_t cycles = this->top->get_periph_clock()->get_cycles();
        if (next_bit_cycle > cycles)
            latency = next_bit_cycle - cycles;

        top->get_periph_clock()->enqueue(pending_word_event, latency);
    }
}


void Uart_tx_channel::handle_ready_reqs()
{
    if (this->pending_bits == 0 && !ready_reqs->is_empty())
    {
        vp::io_req *req = this->ready_reqs->pop();
        this->pending_req = req;
        this->pending_word = *(uint32_t *)req->get_data();
        this->pending_bits = req->get_actual_size() * 8;
        this->check_state();
    }
}


void Uart_tx_channel::reset(bool active)
{
    Udma_tx_channel::reset(active);

    if (active)
    {
        this->state = UART_TX_STATE_START;
        this->next_bit_cycle = -1;
        this->sent_bits = 0;
        this->pending_bits = 0;
        this->stop_bits = 0;
    }
}


bool Uart_tx_channel::is_busy()
{
    return this->pending_bits != 0 || !ready_reqs->is_empty();
}


Uart_rx_channel::Uart_rx_channel(udma *top, Uart_periph *periph, string name) : Udma_rx_channel(top, name), periph(periph)
{
}


void Uart_rx_channel::reset(bool active)
{
    Udma_rx_channel::reset(active);

    if (active)
    {
        this->state = UART_RX_STATE_WAIT_START;
        this->nb_received_bits = 0;
    }
}


void Uart_rx_channel::handle_rx_bit(int bit)
{
    if (this->state == UART_RX_STATE_WAIT_START)
    {
        if (bit == 0)
        {
            this->parity = 0;
            this->state = UART_RX_STATE_DATA;
        }
    }
    else if (this->state == UART_RX_STATE_DATA)
    {
        this->pending_rx_byte = (this->pending_rx_byte >> 1) | (bit << 7);
        this->nb_received_bits++;
        this->parity ^= bit;
        if (this->nb_received_bits == this->periph->regmap.setup.bit_length_get() + 5)
        {
            this->push_data((uint8_t *)&this->pending_rx_byte, 1);
            this->nb_received_bits = 0;
            if (this->periph->regmap.setup.parity_ena_get())
                this->state = UART_RX_STATE_PARITY;
            else
            {
                this->stop_bits = this->periph->regmap.setup.stop_bits_get();
                this->state = UART_RX_STATE_WAIT_STOP;
            }
        }
    }
    else if (this->state == UART_RX_STATE_PARITY)
    {
        if (bit != this->parity)
        {
        }
        this->stop_bits = this->periph->regmap.setup.stop_bits_get();
        this->state = UART_RX_STATE_WAIT_STOP;
    }
    else if (this->state == UART_RX_STATE_WAIT_STOP)
    {
        if (bit == 1)
        {
            this->stop_bits--;
            if (this->stop_bits == 0)
            {
                this->state = UART_RX_STATE_WAIT_START;
            }
        }
    }
}


bool Uart_rx_channel::is_busy()
{
    return false;
}
