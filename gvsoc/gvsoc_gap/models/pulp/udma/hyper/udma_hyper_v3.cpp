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
#include "vp/itf/hyper.hpp"


using namespace std::placeholders;


Hyper_periph::Hyper_periph(udma *top, int id, int itf_id) : Udma_periph(top, id)
{
    std::string itf_name = "hyper" + std::to_string(itf_id);

    top->traces.new_trace(itf_name, &trace, vp::DEBUG);

    channel0 = new Hyper_rx_channel(top, this, itf_name + "_rx");
    channel1 = new Hyper_tx_channel(top, this, itf_name + "_tx");

    this->hyper_itf.set_sync_cycle_meth(&Hyper_periph::rx_sync);
    top->new_master_port(this, itf_name, &this->hyper_itf);

    this->pending_word_event = top->event_new(this, Hyper_periph::handle_pending_word);
    this->pending_channel_event = top->event_new(this, Hyper_periph::handle_pending_channel);

    this->pending_bytes = 0;
    this->next_bit_cycle = -1;
    this->state = HYPER_STATE_IDLE;
    this->channel_state = HYPER_CHANNEL_STATE_IDLE;

    this->rx_channel = static_cast<Hyper_rx_channel *>(this->channel0);
    this->tx_channel = static_cast<Hyper_tx_channel *>(this->channel1);

    js::config *config = this->top->get_js_config()->get("hyper/eot_events");
    if (config)
        this->eot_event = config->get_elem(itf_id)->get_int();
    else
        this->eot_event = -1;

    this->regmap.build(top, &this->trace, itf_name);

    this->top->new_master_port(itf_name + "_irq", &this->irq_itf);

    this->regmap.trans_cfg.register_callback(std::bind(&Hyper_periph::trans_cfg_req, this, _1, _2, _3, _4));
}


void Hyper_periph::trans_cfg_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.trans_cfg.update(reg_offset, size, value, is_write);

    if (this->regmap.trans_cfg.valid_get())
    {
        this->regmap.trans_cfg.valid_set(0);

        if (this->channel_state == HYPER_CHANNEL_STATE_IDLE)
        {
            if (this->regmap.burst_enable.burst_2d_enable_get())
            {
                this->trace.msg(vp::trace::LEVEL_DEBUG, "Starting 2D transfer (addr: 0x%x, size: 0x%x, length: 0x%x, stride: 0x%x, is_tx: %d)\n",
                    this->regmap.trans_addr.addr_get(), this->regmap.trans_size.size_get(),
                    this->regmap.line_2d.line_get(), this->regmap.stride_2d.stride_get(), !this->regmap.trans_cfg.rxtx_get());

                this->pending_length = this->regmap.line_2d.line_get();
                this->pending_2d_addr = this->regmap.ext_addr.saddr_get();
            }
            else
            {
                this->trace.msg(vp::trace::LEVEL_DEBUG, "Starting 1D transfer (addr: 0x%x, size: 0x%x, is_tx: %d)\n",
                    this->regmap.trans_addr.addr_get(), this->regmap.trans_size.size_get(), !this->regmap.trans_cfg.rxtx_get());

                this->pending_length = 0;
            }

            this->channel_state = HYPER_CHANNEL_STATE_SEND_ADDR;
            this->top->event_enqueue(this->pending_channel_event, 1);
        }
        else
        {
            this->trace.force_warning("Trying to start transfer while one is already pending\n");
        }
    }
    else
    {
        if (this->channel_state != HYPER_CHANNEL_STATE_IDLE)
        {
            this->channel_state = HYPER_CHANNEL_STATE_STOP;
        }
    }
}


vp::io_req_status_e Hyper_periph::custom_req(vp::io_req *req, uint64_t offset)
{
    this->regmap.access(offset, req->get_size(), req->get_data(), req->get_is_write());

    return vp::IO_REQ_OK;
}


void Hyper_periph::reset(bool active)
{
    Udma_periph::reset(active);

    if (active)
    {
        this->pending_tx = false;
        this->pending_rx = false;
        this->current_cmd = NULL;
    }
}


void Hyper_periph::rx_sync(void *__this, int data)
{
    Hyper_periph *_this = (Hyper_periph *)__this;
    (static_cast<Hyper_rx_channel *>(_this->channel0))->handle_rx_data(data);
}


void Hyper_periph::handle_pending_channel(void *__this, vp::clock_event *event)
{
    Hyper_periph *_this = (Hyper_periph *)__this;
    Udma_channel *channel = _this->regmap.trans_cfg.rxtx_get() ? (Udma_channel *)_this->rx_channel : (Udma_channel *)_this->tx_channel;
    uint32_t value;

    switch (_this->channel_state)
    {
        case HYPER_CHANNEL_STATE_SEND_ADDR:
            value = _this->regmap.trans_addr.addr_get();
            channel->access(UDMA_CORE_LIN_ADDRGEN_CFG_SA_BUF0_OFFSET, 4, (uint8_t *)&value, true);
            _this->channel_state = HYPER_CHANNEL_STATE_SEND_SIZE;
            break;

        case HYPER_CHANNEL_STATE_SEND_SIZE:
            value = _this->regmap.trans_size.size_get();
            channel->access(UDMA_CORE_LIN_ADDRGEN_CFG_SIZE_OFFSET, 4, (uint8_t *)&value, true);
            _this->channel_state = HYPER_CHANNEL_STATE_SEND_CFG;
            break;

        case HYPER_CHANNEL_STATE_SEND_CFG:
            value = 1<<UDMA_CORE_LIN_ADDRGEN_CFG_CTRL_EN_BIT;
            channel->access(UDMA_CORE_LIN_ADDRGEN_CFG_CTRL_OFFSET, 4, (uint8_t *)&value, true);
            _this->channel_state = HYPER_CHANNEL_STATE_IDLE;
            break;

        case HYPER_CHANNEL_STATE_STOP:
            value = 1<<UDMA_CORE_LIN_ADDRGEN_CFG_CTRL_STOP_BIT;
            channel->access(UDMA_CORE_LIN_ADDRGEN_CFG_CTRL_OFFSET, 4, (uint8_t *)&value, true);
            _this->channel_state = HYPER_CHANNEL_STATE_IDLE;
            break;
    }

    if (_this->channel_state != HYPER_CHANNEL_STATE_IDLE)
    {
        _this->top->event_enqueue(_this->pending_channel_event, 1);
    }
}


void Hyper_periph::handle_pending_word(void *__this, vp::clock_event *event)
{
    Hyper_periph *_this = (Hyper_periph *)__this;
    uint8_t byte;
    int cs_value;
    bool send_byte = false;
    bool send_cs = false;
    bool end = false;
    uint32_t mba0 = _this->regmap.mba0.mba0_get() << UDMA_HYPER_MBA0_MBA0_BIT;
    uint32_t mba1 = _this->regmap.mba1.mba1_get() << UDMA_HYPER_MBA1_MBA1_BIT;
    uint32_t addr = _this->regmap.ext_addr.saddr_get();
    int cs;

    if (mba1 >= mba0)
    {
        if (addr >= mba1)
        {
            cs = 1;
            addr -= mba1;
        }
        else
        {
            cs = 0;
        }
    }
    else
    {
        if (addr >= mba0)
        {
            cs = 0;
            addr -= mba0;
        }
        else
        {
            cs = 1;
        }
    }

    if (_this->state == HYPER_STATE_IDLE)
    {
        if (_this->pending_bytes > 0)
        {
            _this->state = HYPER_STATE_DELAY;
            _this->delay = 72;
            _this->ca_count = 6;
            _this->ca.low_addr = ARCHI_REG_FIELD_GET(addr, 0, 3);
            _this->ca.high_addr = ARCHI_REG_FIELD_GET(addr, 3, 29);

            _this->ca.burst_type = 0;
            _this->ca.address_space = _this->regmap.ext_addr.reg_access_get();
            _this->ca.read = _this->pending_rx ? 1 : 0;

            bool burst_enable = cs == 0 ? _this->regmap.burst_enable.cs0_auto_burst_enable_get() : _this->regmap.burst_enable.cs1_auto_burst_enable_get();

            if (burst_enable)
            {
                _this->pending_burst = _this->regmap.timing_cfg.cs_max_get();
            }
            else
            {
                _this->pending_burst = 0;
            }

            if (_this->transfer_size == 0)
            {
                if (_this->ca.read)
                    _this->transfer_size = _this->rx_channel->current_cmd->size;
                else
                    _this->transfer_size = _this->tx_channel->current_cmd->size;
            }
        }
    }
    else if (_this->state == HYPER_STATE_DELAY)
    {
        _this->delay--;
        if (_this->delay == 0)
            _this->state = HYPER_STATE_CS;
    }
    else if (_this->state == HYPER_STATE_CS)
    {
        _this->state = HYPER_STATE_CA;
        send_cs = true;
        cs_value = 1;
    }
    else if (_this->state == HYPER_STATE_CA)
    {
        send_byte = true;
        _this->ca_count--;
        byte = _this->ca.raw[_this->ca_count];

        if (_this->ca_count == 0)
        {
            _this->state = HYPER_STATE_DATA;
        }
    }
    else if (_this->state == HYPER_STATE_DATA && _this->pending_bytes > 0)
    {
        send_byte = true;
        byte = _this->pending_word & 0xff;
        _this->pending_word >>= 8;
        _this->pending_bytes--;
        _this->transfer_size--;

        if (_this->transfer_size == 0)
        {
            _this->pending_bytes = 0;
            _this->state = HYPER_STATE_CS_OFF;
        }
        else
        {
            if (_this->pending_length != 0)
            {
                _this->pending_length--;
                if (_this->pending_length == 0)
                {
                    _this->regmap.ext_addr.saddr_set(_this->pending_2d_addr + _this->regmap.stride_2d.stride_get());
                    _this->pending_2d_addr = _this->regmap.ext_addr.saddr_get();
                    _this->pending_length = _this->regmap.line_2d.line_get();
                    _this->state = HYPER_STATE_CS_OFF;
                }
            }

            if (_this->state != HYPER_STATE_CS_OFF)
            {
                if (_this->pending_burst > 0)
                {
                    _this->pending_burst--;
                    if (_this->pending_burst == 0)
                    {
                        _this->regmap.ext_addr.saddr_set(_this->regmap.ext_addr.saddr_get() + _this->regmap.timing_cfg.cs_max_get());
                        _this->pending_burst = _this->regmap.timing_cfg.cs_max_get();
                        _this->state = HYPER_STATE_CS_OFF;
                    }
                }
            }
        }

        if (_this->pending_bytes == 0)
        {
            end = true;
        }
    }
    else if (_this->state == HYPER_STATE_CS_OFF)
    {
        _this->state = HYPER_STATE_IDLE;
        send_cs = true;
        cs_value = 0;

        if (_this->transfer_size == 0 && _this->regmap.irq_en.en_get())
        {
            _this->trace.msg(vp::trace::LEVEL_DEBUG, "Triggering event (event: %d)\n", _this->eot_event);
            _this->top->trigger_event(_this->eot_event);
            if (_this->irq_itf.is_bound())
            {
                _this->trace.msg(vp::trace::LEVEL_DEBUG, "Triggering interrupt\n");
                _this->irq_itf.sync(1);
            }
        }

    }

    if (send_byte || send_cs)
    {
        if (!_this->hyper_itf.is_bound())
        {
            _this->top->get_trace()->warning("Trying to send to HYPER interface while it is not connected\n");
        }
        else
        {
            int div = _this->regmap.clk_div.data_get() * 2;

            _this->next_bit_cycle = _this->top->get_periph_clock()->get_cycles() + div;
            if (send_byte)
            {
                _this->top->get_trace()->msg("Sending byte (value: 0x%x)\n", byte);
                _this->hyper_itf.sync_cycle(byte);
            }
            else
            {
                _this->top->get_trace()->msg("Updating CS (cs: %d, value: %d)\n", cs, cs_value);
                _this->hyper_itf.cs_sync(cs, cs_value);
            }
        }
    }

    if (end)
    {
        if (!_this->ca.read)
        {
            _this->pending_tx = false;
            _this->tx_channel->handle_ready_req_end(_this->pending_req);
            _this->tx_channel->handle_ready_reqs();
        }
        else
            _this->pending_rx = false;
    }

    _this->check_state();
}


void Hyper_periph::check_state()
{
    if (this->pending_bytes == 0)
    {
        if (!this->tx_channel->ready_reqs->is_empty() && (this->pending_tx || !this->pending_rx))
        {
            this->pending_tx = true;
            vp::io_req *req = this->tx_channel->ready_reqs->pop();
            this->pending_req = req;
            this->pending_word = *(uint32_t *)req->get_data();
            this->pending_bytes = req->get_size();
        }
        else if (this->rx_channel->current_cmd && (this->pending_rx || !this->pending_tx))
        {
            this->pending_rx = true;
            this->pending_bytes = rx_channel->current_cmd->size;
        }
    }

    if (this->pending_bytes != 0 || this->state == HYPER_STATE_CS_OFF)
    {
        if (!this->pending_word_event->is_enqueued())
        {
            int latency = 1;
            int64_t cycles = this->top->get_periph_clock()->get_cycles();
            if (this->next_bit_cycle > cycles)
                latency = this->next_bit_cycle - cycles;

            this->top->get_periph_clock()->enqueue_ext(this->pending_word_event, latency);
        }
    }
}


void Hyper_periph::handle_ready_reqs()
{
    this->check_state();
}


Hyper_tx_channel::Hyper_tx_channel(udma *top, Hyper_periph *periph, string name)
    : Udma_tx_channel(top, name), periph(periph)
{
}


void Hyper_tx_channel::reset(bool active)
{
    Udma_tx_channel::reset(active);
}


void Hyper_tx_channel::handle_ready_reqs()
{
    this->periph->handle_ready_reqs();
}


Hyper_rx_channel::Hyper_rx_channel(udma *top, Hyper_periph *periph, string name) : Udma_rx_channel(top, name), periph(periph)
{
}


void Hyper_rx_channel::reset(bool active)
{
    Udma_rx_channel::reset(active);
}


void Hyper_rx_channel::handle_rx_data(int data)
{
    this->push_data((uint8_t *)&data, 1);
}


void Hyper_rx_channel::handle_ready()
{
    this->periph->handle_ready_reqs();
}


void Hyper_rx_channel::handle_transfer_end()
{
    Udma_rx_channel::handle_transfer_end();
}


void Hyper_tx_channel::handle_transfer_end()
{
    Udma_tx_channel::handle_transfer_end();
}