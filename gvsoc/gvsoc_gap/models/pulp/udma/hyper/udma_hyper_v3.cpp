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

#include "udma_hyper_v3.hpp"
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

    this->refill_itf.set_sync_meth(&Hyper_periph::refill_req);
    top->new_slave_port(this, "refill_" + itf_name, &this->refill_itf);
  
    this->top->new_reg(itf_name + "/state", &this->state, 32);
    this->top->new_reg(itf_name + "/active", &this->active, 8);

    this->pending_word_event = top->event_new(this, Hyper_periph::handle_pending_word);
    this->check_state_event = top->event_new(this, Hyper_periph::handle_check_state);
    this->pending_channel_event = top->event_new(this, Hyper_periph::handle_pending_channel);
    this->push_data_event = top->event_new(this, Hyper_periph::handle_push_data);

    this->pending_bytes = 0;
    this->next_bit_cycle = -1;
    this->state.set(HYPER_STATE_IDLE);
    this->active.release();
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

    // HW is having 4 entries for outstanding requests and 8 entries for dc fifo, just model it as 12 entries
    // however timing model gives better result with only 8
    int fifo_size = 8;
    this->read_req_free = new Udma_queue<Hyper_read_request>(fifo_size);
    for (int i=0; i<fifo_size; i++)
    {
         this->read_req_free->push(new Hyper_read_request());
    }
    this->read_req_waiting = new Udma_queue<Hyper_read_request>(-1);
    this->read_req_ready = new Udma_queue<Hyper_read_request>(-1);

}


void Hyper_periph::refill_req(void *__this, udma_refill_req_t *req)
{
    Hyper_periph *_this = (Hyper_periph *)__this;
    _this->pending_refill_req = req;
    _this->check_state();
}


void Hyper_periph::enqueue_transfer(uint32_t ext_addr, uint32_t l2_addr, uint32_t transfer_size, uint32_t length, uint32_t stride, bool is_write, int address_space)
{
    this->trace.msg(vp::trace::LEVEL_DEBUG, "Enqueuing transfer (ext_addr: 0x%x, l2_addr: 0x%x, size: 0x%x, length: 0x%x, stride: 0x%x, is_write: %d, address_space: %d)\n",
        ext_addr, l2_addr, transfer_size, length, stride, is_write, address_space);

    this->channel_state = HYPER_CHANNEL_STATE_SEND_ADDR;
    this->pending_ext_addr = ext_addr;
    this->ext_addr = ext_addr;
    this->l2_addr = l2_addr;
    this->pending_is_write = is_write;
    this->transfer_size = transfer_size;
    this->address_space = address_space;
    this->iter_2d = false;
    if (!this->pending_is_write)
    {
        this->pending_bytes = this->transfer_size;
    }
    else
    {
        this->nb_bytes_to_read = this->transfer_size;
    }
    this->pending_length = length;
    this->length = length;
    this->stride = stride;
    this->top->event_enqueue(this->pending_channel_event, 1);
}


void Hyper_periph::trans_cfg_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.trans_cfg.update(reg_offset, size, value, is_write);

    if (this->regmap.trans_cfg.valid_get())
    {
        this->regmap.trans_cfg.valid_set(0);

        if (this->channel_state == HYPER_CHANNEL_STATE_IDLE)
        {
            int length = 0;
            if (this->regmap.burst_enable.qspi_2d_enable_get() & 1)
            {
                this->trace.msg(vp::trace::LEVEL_DEBUG, "Starting 2D transfer (addr: 0x%x, size: 0x%x, length: 0x%x, stride: 0x%x, is_tx: %d)\n",
                    this->regmap.trans_addr.addr_get(), this->regmap.trans_size.size_get(),
                    this->regmap.line_2d.line_get(), this->regmap.stride_2d.stride_get(), !this->regmap.trans_cfg.rxtx_get());

                length = this->regmap.line_2d.line_get();
            }
            else
            {
                this->trace.msg(vp::trace::LEVEL_DEBUG, "Starting 1D transfer (addr: 0x%x, size: 0x%x, is_tx: %d)\n",
                    this->regmap.trans_addr.addr_get(), this->regmap.trans_size.size_get(), !this->regmap.trans_cfg.rxtx_get());
            }

            this->enqueue_transfer(
                this->regmap.ext_addr.saddr_get(), this->regmap.trans_addr.addr_get(), this->regmap.trans_size.size_get(),
                length, this->regmap.stride_2d.stride_get(), !this->regmap.trans_cfg.rxtx_get(), this->regmap.ext_addr.reg_access_get()
            );
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

    // TODO properly clean-up the mask once other fields of these registers are modeled
    if (offset == UDMA_HYPER_RX_DEST_OFFSET)
    {
        if (req->get_is_write())
            this->top->channel_register((*(uint32_t *)req->get_data()) & 0xFF, this->channel0);
    }
    else if (offset == UDMA_HYPER_TX_DEST_OFFSET)
    {
        if (req->get_is_write())
            this->top->channel_register((*(uint32_t *)req->get_data()) & 0xFF, this->channel1);
    }

    return vp::IO_REQ_OK;
}


void Hyper_periph::reset(bool active)
{
    Udma_periph::reset(active);

    if (!active)
    {
        this->pending_tx = false;
        this->pending_rx = false;
        this->transfer_size = 0;
        this->pending_word_ready = false;
        this->pending_word_size = 0;
        this->nb_bytes_to_read = 0;
        this->pending_refill_req = NULL;
        this->is_refill_req = false;
        this->pending_bytes = 0;
        this->state.set(HYPER_STATE_IDLE);
        this->active.release();
        this->pending_is_write = false;
    }
}


bool Hyper_periph::push_to_udma()
{
    if (this->rx_channel->is_ready())
    {    
        this->trace.msg(vp::trace::LEVEL_TRACE, "Pushing word to udma channel (value: 0x%x)\n", this->pending_word);
        this->rx_channel->push_data((uint8_t *)&this->pending_word, this->pending_word_size);
        this->pending_word_size = 0;
        this->pending_word_ready = false;
        return true;
    }

    return false;
}

void Hyper_periph::rx_sync(void *__this, int data)
{
    Hyper_periph *_this = (Hyper_periph *)__this;

    _this->trace.msg(vp::trace::LEVEL_TRACE, "Received byte (value: 0x%x)\n", data);

    _this->pending_word = (_this->pending_word & ((1 << (_this->pending_word_size * 8)) - 1)) | (data << (_this->pending_word_size * 8));

    _this->pending_word_size++;

    if (_this->pending_word_size == 4 || _this->transfer_size == 0)
    {
        if (!_this->push_to_udma())
        {
            _this->pending_word_ready = true;
        }
    }
}


void Hyper_periph::handle_pending_channel(void *__this, vp::clock_event *event)
{
    Hyper_periph *_this = (Hyper_periph *)__this;
    Udma_channel *channel = !_this->pending_is_write ? (Udma_channel *)_this->rx_channel : (Udma_channel *)_this->tx_channel;
    uint32_t value;

    switch (_this->channel_state)
    {
        case HYPER_CHANNEL_STATE_SEND_ADDR:
            value = _this->l2_addr;
            channel->access(UDMA_CORE_LIN_ADDRGEN_CFG_SA_BUF0_OFFSET, 4, (uint8_t *)&value, true);
            _this->channel_state = HYPER_CHANNEL_STATE_SEND_SIZE;
            break;

        case HYPER_CHANNEL_STATE_SEND_SIZE:
            value = _this->transfer_size;
            channel->access(UDMA_CORE_LIN_ADDRGEN_CFG_SIZE_OFFSET, 4, (uint8_t *)&value, true);
            _this->channel_state = HYPER_CHANNEL_STATE_SEND_LENGTH;
            break;

        case HYPER_CHANNEL_STATE_SEND_LENGTH:
            value = _this->transfer_size;
            channel->access(UDMA_CORE_2D_CFG_ROW_LEN_OFFSET, 4, (uint8_t *)&value, true);
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

    _this->check_state();
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
    uint32_t addr = _this->pending_ext_addr;
    int cs;

        _this->top->get_trace()->msg(vp::trace::LEVEL_INFO, "Handle pending word (state: %d)\n", _this->state.get());
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

    if (_this->state.get() == HYPER_STATE_IDLE)
    {
        if (_this->pending_bytes > 0)
        {
            _this->state.set(HYPER_STATE_DELAY);
            _this->active.set(1);
            if (_this->iter_2d)
            {
                _this->delay = 24;
            }
            else
            {
                _this->delay = 8;
            }
            _this->ca_count = 6;

            _this->ca.low_addr = ARCHI_REG_FIELD_GET(addr, 0, 3);
            _this->ca.high_addr = ARCHI_REG_FIELD_GET(addr, 3, 29);

            _this->ca.burst_type = 0;
            _this->ca.address_space = _this->address_space;
            _this->ca.read = !_this->pending_is_write;

            bool burst_enable = cs == 0 ? _this->regmap.burst_enable.cs0_auto_burst_enable_get() : _this->regmap.burst_enable.cs1_auto_burst_enable_get();

            if (burst_enable)
            {
                _this->pending_burst = _this->regmap.timing_cfg.cs_max_get();
            }
            else
            {
                _this->pending_burst = 0;
            }
        }
    }
    else if (_this->state.get() == HYPER_STATE_DELAY)
    {
        _this->delay--;
        if (_this->delay == 0)
            _this->state.set(HYPER_STATE_CS);
    }
    else if (_this->state.get() == HYPER_STATE_CS)
    {
        _this->state.set(HYPER_STATE_CA);
        send_cs = true;
        cs_value = 1;
    }
    else if (_this->state.get() == HYPER_STATE_CA)
    {
        send_byte = true;
        _this->ca_count--;
        byte = _this->ca.raw[_this->ca_count];

        if (_this->ca_count == 0)
        {
            _this->state.set(HYPER_STATE_DATA);
        }
    }
    else if (_this->state.get() == HYPER_STATE_DATA && _this->pending_bytes > 0)
    {
        send_byte = true;
        if (_this->pending_is_write)
        {
            byte = _this->pending_word & 0xff;
            _this->pending_word >>= 8;
        }
        else
        {
            byte = 0;
        }
        _this->pending_bytes--;
        _this->transfer_size--;

        _this->check_read_req_ready();

        if (_this->transfer_size == 0)
        {
            _this->pending_bytes = 0;
            _this->state.set(HYPER_STATE_CS_OFF);
        }
        else
        {
            if (_this->pending_length != 0)
            {
                _this->pending_length--;
                if (_this->pending_length == 0)
                {
                    _this->ext_addr += _this->stride;
                    _this->pending_ext_addr = _this->ext_addr;
                    _this->pending_length = _this->length;
                    _this->state.set(HYPER_STATE_CS_OFF);
                    _this->iter_2d = true;
                }
            }

            if (_this->state.get() != HYPER_STATE_CS_OFF)
            {
                if (_this->pending_burst > 0)
                {
                    _this->pending_burst--;
                    if (_this->pending_burst == 0)
                    {
                        _this->pending_ext_addr += _this->regmap.timing_cfg.cs_max_get();
                        _this->pending_burst = _this->regmap.timing_cfg.cs_max_get();
                        _this->state.set(HYPER_STATE_CS_OFF);
                    }
                }
            }
        }

        if (_this->pending_bytes == 0)
        {
            end = true;
        }
    }
    else if (_this->state.get() == HYPER_STATE_CS_OFF)
    {
        _this->state.set(HYPER_STATE_IDLE);
        _this->active.release();
        send_cs = true;
        cs_value = 0;

        if (_this->transfer_size == 0 && _this->regmap.irq_en.en_get())
        {
            if (_this->is_refill_req)
            {
                udma_refill_req_t *req = _this->pending_refill_req;
                _this->pending_refill_req = NULL;
                _this->is_refill_req = false;
                _this->refill_itf.sync(req);
            }
            else
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
                _this->top->get_trace()->msg(vp::trace::LEVEL_INFO, "Sending byte (value: 0x%x)\n", byte);
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
        }
        else
            _this->pending_rx = false;
    }

    _this->check_state();
}


void Hyper_periph::check_read_req_ready()
{
    if (this->pending_is_write && this->pending_bytes == 0 && !this->read_req_ready->is_empty())
    {
        Hyper_read_request *req = this->read_req_ready->pop();
        this->pending_word = req->data;
        this->pending_bytes = req->size;
        this->read_req_free->push(req);
    }
}


void Hyper_periph::handle_check_state(void *__this, vp::clock_event *event)
{
    Hyper_periph *_this = (Hyper_periph *)__this;

    if (_this->channel_state != HYPER_CHANNEL_STATE_IDLE)
        return;

    _this->check_read_req_ready();

    if (!_this->pending_is_write && _this->pending_word_ready)
    {
        _this->push_to_udma();
    }

    if (_this->nb_bytes_to_read > 0 && !_this->read_req_free->is_empty() && _this->tx_channel->is_ready())
    {
        int size = _this->nb_bytes_to_read > 4 ? 4 : _this->nb_bytes_to_read;
        Hyper_read_request *req = _this->read_req_free->pop();
        _this->nb_bytes_to_read -= size;
        _this->read_req_waiting->push(req);
        req->requested_size = size;
        req->size = 0;
        req->data = 0;
        _this->tx_channel->get_data(size);
    }

    _this->check_state();
}


void Hyper_periph::check_state()
{
    if (this->channel_state == HYPER_CHANNEL_STATE_IDLE && ((this->pending_bytes > 0 && (this->pending_is_write || !this->pending_is_write && !this->pending_word_ready)) || this->state.get() == HYPER_STATE_CS_OFF))
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

    if (!this->pending_is_write && this->pending_word_ready || this->nb_bytes_to_read > 0 && !this->read_req_free->is_empty() || this->pending_is_write && this->pending_bytes == 0 && !this->read_req_ready->is_empty())
    {
        if (!this->check_state_event->is_enqueued())
        {
            this->top->event_enqueue(this->check_state_event, 1);
        }
    }


    if (this->pending_refill_req && !this->is_refill_req && this->channel_state == HYPER_CHANNEL_STATE_IDLE)
    {
        this->is_refill_req = true;

        this->enqueue_transfer(
            this->pending_refill_req->ext_addr, this->pending_refill_req->l2_addr, this->pending_refill_req->size, this->pending_refill_req->size,
            0, this->pending_refill_req->is_write, 0
        );
    }
}


void Hyper_periph::push_data(uint8_t *data, int size)
{
    // TX transfers do not have full bandwidth when soc frequency is low because of this sequence
    //   Cycle 0: hyper request data
    //   Cycle 2: udma core requests data to L2
    //   Cycle 3: udma core receives response from L2
    //   Cycle 4: hyper receives response
    //   Cycle 6: hyper request new data
    // Hyper can do 4 outstanding requests so there is a hole of 2 cycles.
    // To model that, just delay a bit the incoming data
    int delay = 4;
    this->push_data_fifo_data.push(*(uint32_t *)data);
    this->push_data_fifo_size.push(size);
    this->push_data_fifo_cycles.push(this->top->get_cycles() + delay);

    if (!this->push_data_event->is_enqueued())
    {
        this->top->event_enqueue(this->push_data_event, delay);
    }
}


void Hyper_periph::handle_push_data(void *__this, vp::clock_event *event)
{
    Hyper_periph *_this = (Hyper_periph *)__this;

    while(!_this->push_data_fifo_cycles.empty() && _this->push_data_fifo_cycles.front() <= _this->top->get_cycles())
    {
        uint32_t data = _this->push_data_fifo_data.front();
        int size = _this->push_data_fifo_size.front();

        _this->push_data_fifo_data.pop();
        _this->push_data_fifo_size.pop();
        _this->push_data_fifo_cycles.pop();

        while (size)
        {
            Hyper_read_request *req = _this->read_req_waiting->get_first();
            int iter_size = size > req->requested_size ? req->requested_size : size;
            req->data |= (data) <<  (req->size * 8);
            req->size += iter_size;
            req->requested_size -= iter_size;
            size -= iter_size;
            if (req->requested_size == 0)
            {
                _this->read_req_waiting->pop();
                _this->read_req_ready->push(req);
                _this->check_state();
            }
        }
    }

    if (!_this->push_data_fifo_cycles.empty())
    {
        _this->top->event_enqueue(_this->push_data_event, _this->push_data_fifo_cycles.front() -  _this->top->get_cycles());
    }
}


Hyper_tx_channel::Hyper_tx_channel(udma *top, Hyper_periph *periph, string name)
    : Udma_tx_channel(top, name), periph(periph)
{
}


void Hyper_tx_channel::push_data(uint8_t *data, int size)
{
    this->periph->push_data(data, size);
}


Hyper_rx_channel::Hyper_rx_channel(udma *top, Hyper_periph *periph, string name) : Udma_rx_channel(top, name), periph(periph)
{
}
