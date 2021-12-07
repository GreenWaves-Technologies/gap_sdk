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

#include "udma_mram_v2.hpp"
#include "../udma_impl.hpp"
#include "archi/utils.h"


using namespace std::placeholders;


Mram_periph::Mram_periph(udma *top, int id, int itf_id) : Udma_periph(top, id)
{
    std::string itf_name = "mram" + std::to_string(itf_id);

    top->traces.new_trace(itf_name, &trace, vp::DEBUG);
    this->itf_name = itf_name;

    this->rx_channel = new Mram_rx_channel(top, this, itf_name + "_rx");
    this->tx_channel = new Mram_tx_channel(top, this, itf_name + "_tx");

    this->out_req.set_sync_meth(&this->sync_req);
    top->new_master_port(this, itf_name + "_req", &this->out_req);
    this->out_data.set_sync_meth(&this->sync_data);
    top->new_master_port(this, itf_name + "_data", &this->out_data);
    top->new_master_port(this, itf_name + "_conf", &this->out_conf);

    this->event = top->event_new(this, Mram_periph::handle_pending_operation);
    this->read_end_event = top->event_new(this, Mram_periph::handle_read_end);
    this->read_2d_event = top->event_new(this, Mram_periph::handle_read_2d);
    
    this->regmap.build(top, &this->trace, itf_name);

    this->regmap.clk_div.register_callback(std::bind(&Mram_periph::clk_div_req, this, _1, _2, _3, _4));
    this->regmap.trans_cfg.register_callback(std::bind(&Mram_periph::trans_cfg_req, this, _1, _2, _3, _4));
    this->regmap.tx_dest.register_callback(std::bind(&Mram_periph::tx_dest_req, this, _1, _2, _3, _4));
    this->regmap.rx_dest.register_callback(std::bind(&Mram_periph::rx_dest_req, this, _1, _2, _3, _4));

    js::config *config = this->top->get_js_config()->get("mram/events");
    this->erase_event = config->get("erase")->get_elem(itf_id)->get_int();
    this->tx_event = config->get("tx")->get_elem(itf_id)->get_int();
    this->trim_event = config->get("trim")->get_elem(itf_id)->get_int();
    this->rx_event = config->get("rx")->get_elem(itf_id)->get_int();

    this->read_event = this->top->event_new(this, Mram_periph::handle_read_req);
}


int64_t Mram_periph::get_mram_cycle_count(int64_t cycles)
{
    if (this->regmap.clk_div.data_get() == 0)
    {
        return cycles;
    }
    else
    {
        return cycles * this->regmap.clk_div.data_get();
    }
}


void Mram_periph::start()
{
    this->rcv_event = this->top->event_new(this, Mram_periph::handle_rcv_data);
    this->read_fifo = new Dc_fifo(
        4, this->top, this->itf_name,
        this->top->get_periph_clock(),
        this->top->get_clock(), this->rcv_event,
        this->top->event_new(this, Mram_periph::handle_dc_fifo_unstall)
    );
}


// Called by the mram to acknoledge the erase
void Mram_periph::sync_req(void *__this, mram_req_t *req)
{
    Mram_periph *_this = (Mram_periph *)__this;
    _this->top->trigger_event(_this->erase_event);
    _this->state = MRAM_STATE_IDLE;
}


// Called by the mram to acknoledge data sent to mram or to receive data read from mram
void Mram_periph::sync_data(void *__this, mram_data_t *req)
{
    Mram_periph *_this = (Mram_periph *)__this;

    if (_this->state == MRAM_STATE_HANDLE_TRIM_ONGOING)
    {
        // For trim, we either go on with more data or trigger soc event
        _this->pending_transfer_size -= _this->req_size;
        if (_this->pending_transfer_size == 0)
        {       
            _this->top->trigger_event(_this->trim_event);
            _this->state = MRAM_STATE_IDLE;
        }
        else
        {
            _this->handle_trim();
        }
    }
    else if (_this->state == MRAM_STATE_HANDLE_PROGRAM_ONGOING)
    {
        // For program, we either go on with more data or trigger soc event
        _this->pending_transfer_size -= _this->req_size;
        if (_this->pending_transfer_size == 0)
        {       
            _this->top->trigger_event(_this->tx_event);
            _this->state = MRAM_STATE_IDLE;
        }
        else
        {
            _this->handle_program();
        }
    }
    else
    {
        // For read, see if we can push the data, otherwise just put it on hold
        if (!_this->read_fifo->is_full())
        {
            _this->read_fifo->push(req->data, req->size);
            _this->out_data.sync(NULL);
        }
        else
        {
            _this->stalled = true;
            _this->stalled_data = req->data;
            _this->stalled_size = req->size;
        }
    }
}


void Mram_periph::clk_div_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.clk_div.update(reg_offset, size, value, is_write);
    mram_conf_t conf = { .div=this->regmap.clk_div.data_get() };
    this->out_conf.sync(&conf);
    this->read_fifo->set_snd_div(this->regmap.clk_div.data_get());
}


void Mram_periph::trans_cfg_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    if (is_write && this->state == MRAM_STATE_IDLE)
    {
        this->regmap.trans_cfg.update(reg_offset, size, value, is_write);

        this->state = MRAM_STATE_HANDLE_OPERATION;

        this->check_state();
    }
}


void Mram_periph::tx_dest_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.tx_dest.update(reg_offset, size, value, is_write);

    if (is_write)
    {
        this->top->channel_register((*(uint32_t *)value) & 0xFF, this->tx_channel);
    }
}


void Mram_periph::rx_dest_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.rx_dest.update(reg_offset, size, value, is_write);

    if (is_write)
    {
        this->top->channel_register((*(uint32_t *)value) & 0xFF, this->rx_channel);
    }
}


// Called to setut a new transfer (1d or 2d line) to be handled
void Mram_periph::enqueue_read_req()
{
    mram_req_t req = { .operation=(mram_op_e)this->regmap.mode.operation_get() };

    if (this->regmap.enable_2d.enable_get())
    {
        this->read_pending_length = this->regmap.line_2d.get();
        if (this->read_pending_length > this->pending_transfer_size)
        {
            this->read_pending_length = this->pending_transfer_size;
        }
    }
    else
    {
        this->read_pending_length = this->regmap.trans_size.get();
    }

    this->pending_trans_size = this->read_pending_length;
    this->state = MRAM_STATE_SEND_ADDR;
    this->next_state = MRAM_STATE_HANDLE_READ;

    this->check_state();
}


// Called when a read transfer is starting
void Mram_periph::handle_read_req(void *__this, vp::clock_event *event)
{
    Mram_periph *_this = (Mram_periph *)__this;
    _this->enqueue_read_req();
}


// Called when there is data in the DC fifo to be popped
void Mram_periph::handle_rcv_data(void *__this, vp::clock_event *event)
{
    Mram_periph *_this = (Mram_periph *)__this;

    int size;

    // Check if data is available in the fifo
    if (_this->read_pending_size == 0 && !_this->read_fifo->is_empty())
    {
        // If so, pop it, and store it to the line data so that we can stream it to the udma core
        uint8_t *data = _this->read_fifo->pop(&size);

        memcpy(_this->line_data, data, size);
        _this->read_pending_size = size;
        _this->read_pending_data = _this->line_data;
        _this->check_state();
    }
}


// Called when the DC fifo gets unstalled to continue pushing data
void Mram_periph::handle_dc_fifo_unstall(void *__this, vp::clock_event *event)
{
    Mram_periph *_this = (Mram_periph *)__this;
    if (_this->stalled)
    {
        _this->trace.msg(vp::trace::LEVEL_TRACE, "Unstalling\n");
        _this->stalled = false;
        _this->read_fifo->push(_this->stalled_data, _this->stalled_size);
        _this->out_data.sync(NULL);
    }
}


// Called  when a read transfer is done to trigger the soc event
void Mram_periph::handle_read_end(void *__this, vp::clock_event *event)
{
    Mram_periph *_this = (Mram_periph *)__this;
    _this->top->trigger_event(_this->rx_event);
    _this->state = MRAM_STATE_IDLE;
    _this->check_state();
}


// Called when a 2d line is done to start the next line
void Mram_periph::handle_read_2d(void *__this, vp::clock_event *event)
{
    Mram_periph *_this = (Mram_periph *)__this;
    _this->pending_addr += _this->regmap.stride_2d.get();
    _this->enqueue_read_req();
}


// FSM handler called when there is a pending operation to be handled
void Mram_periph::handle_pending_operation(void *__this, vp::clock_event *event)
{
    Mram_periph *_this = (Mram_periph *)__this;
    Udma_channel *channel = _this->regmap.trans_cfg.rxtx_get() ? (Udma_channel *)_this->rx_channel : (Udma_channel *)_this->tx_channel;
    uint32_t value;

    switch (_this->state)
    {
        case MRAM_STATE_HANDLE_OPERATION:
        {
            // A new operation has been pushed and needs to be handled
            mram_req_t req = { .operation=(mram_op_e)_this->regmap.mode.operation_get() };

            switch (req.operation)
            {
                case MRAM_OP_NORMAL_TX:
                case MRAM_OP_TRIM_CFG:
                    {
                        // Operations requiring to load data from L2 (tx and trim)
                        // Initialize the transfer and let the FSM setup the udma core channel
                        req.addr = _this->regmap.ext_addr.get();
                        req.size = _this->regmap.trans_size.get();
                        _this->out_req.sync(&req);
                        _this->pending_transfer_size = _this->regmap.trans_size.get();
                        _this->pending_trans_addr = _this->regmap.trans_addr.get();
                        _this->pending_trans_size = _this->regmap.trans_size.get();
                        _this->state = MRAM_STATE_SEND_ADDR;
                        _this->next_state = req.operation == MRAM_OP_TRIM_CFG ? MRAM_STATE_HANDLE_TRIM : MRAM_STATE_HANDLE_PROGRAM;
                        break;
                    }

                case MRAM_OP_READ_RX:
                    {
                        // Read operation
                        // Initialize the transfer and enqueue an event to delay the start of the mram operation
                        _this->pending_trans_addr = _this->regmap.trans_addr.get();
                        _this->pending_addr = _this->regmap.ext_addr.get();
                        _this->pending_transfer_size = _this->regmap.trans_size.get();
                        _this->state = MRAM_STATE_PENDING_OP;
                        _this->top->event_enqueue(_this->read_event, _this->regmap.enable_2d.enable_get() ? 20 : 1);
                        break;
                    }

                case MRAM_OP_ERASE_CHIP:
                case MRAM_OP_ERASE_SECT:
                case MRAM_OP_ERASE_WORD:
                    {
                        // Erase operation, just forward to mram
                        mram_req_t req = {
                            .operation=(mram_op_e)_this->regmap.mode.operation_get(),
                            .addr=_this->regmap.erase_addr.get(),
                            .size=_this->regmap.erase_size.get()
                        };
                        _this->out_req.sync(&req);
                        _this->state = MRAM_STATE_WAIT_SYNC_BACK;
                        break;
                    }

                default:
                    _this->next_state = MRAM_STATE_IDLE;
                    break;
            }

            break;
        }

        case MRAM_STATE_HANDLE_READ:
        {
            // UDMA core has just been setup, now send the request to mram, the data sync handler
            // will do the rest
            mram_req_t req = { .operation=(mram_op_e)_this->regmap.mode.operation_get() };
            req.addr = _this->pending_addr;
            req.size = _this->read_pending_length;
            _this->state = MRAM_STATE_PENDING_OP;
            _this->out_req.sync(&req);
            break;
        }

        case MRAM_STATE_SEND_ADDR:
            // UDMA core channel setup, addr part
            value = _this->pending_trans_addr;
            channel->access(UDMA_CORE_LIN_ADDRGEN_CFG_SA_BUF0_OFFSET, 4, (uint8_t *)&value, true);
            _this->state = MRAM_STATE_SEND_SIZE;
            break;

        case MRAM_STATE_SEND_SIZE:
            // UDMA core channel setup, size part
            value = _this->pending_trans_size;
            channel->access(UDMA_CORE_LIN_ADDRGEN_CFG_SIZE_OFFSET, 4, (uint8_t *)&value, true);
            _this->state = MRAM_STATE_SEND_CFG;
            break;

        case MRAM_STATE_SEND_CFG:
            // UDMA core channel setup, config part
            value = 1<<UDMA_CORE_LIN_ADDRGEN_CFG_CTRL_EN_BIT;
            channel->access(UDMA_CORE_LIN_ADDRGEN_CFG_CTRL_OFFSET, 4, (uint8_t *)&value, true);
            _this->state = _this->next_state;
            break;

        case MRAM_STATE_HANDLE_TRIM:
            // UDMA core channel has been setup, now start sending to mram
            _this->handle_trim();
            _this->state = MRAM_STATE_HANDLE_TRIM_ONGOING;
            break;

        case MRAM_STATE_HANDLE_PROGRAM:
            // UDMA core channel has been setup, now start sending to mram
            _this->handle_program();
            _this->state = MRAM_STATE_HANDLE_PROGRAM_ONGOING;
            break;
    }

    // Check if a read operation is on-going and data needs to be sent to udma core
    if (_this->read_pending_size > 0 && _this->rx_channel->is_ready())
    {
        // Take the max between udma core word (4 bytes), remaining trnasfer size
        // and line length
        int size = 4;
        if (size > _this->read_pending_length)
        {
            size = _this->read_pending_length;
        }
        if (size > _this->read_pending_size)
        {
            size = _this->read_pending_size;
        }

        // Push the data to UDMA
        _this->rx_channel->push_data(_this->read_pending_data, size);

        // And update the transfer
        _this->read_pending_size -= size;
        _this->read_pending_data += size;
        _this->read_pending_length -= size;
        _this->pending_transfer_size -= size;
        _this->pending_trans_addr += size;

        // If we are done with the current data line, see if we can get another data from dc fifo
        if (_this->read_pending_size == 0 && !_this->rcv_event->is_enqueued())
        {
            _this->rcv_event->exec();
        }

        // If transfer is done, schedule the soc event trigger
        if (_this->pending_transfer_size == 0)
        {
            _this->top->event_enqueue(_this->read_end_event, _this->get_mram_cycle_count(7));
        }
        // If just a line of the 2d transfer is done, schedule the next line
        else if (_this->read_pending_length == 0)
        {
            _this->top->event_enqueue(_this->read_2d_event, _this->get_mram_cycle_count(8));
        }
    }

    _this->check_state();
}


// Called to get data from the udma core to be sent to trim config
void Mram_periph::handle_trim()
{
    // Take the max size between max size (4 bytes for trim) and remaining size
    int size = this->pending_transfer_size;
    if (size > 4)
    {
        size = 4;
    }

    // And get the data from L2
    this->tx_channel->get_data(size);
}


// Called to get data from the udma core to be written to mram
void Mram_periph::handle_program()
{
    // Take the max size between max size and remaining size
    int size = this->pending_transfer_size;
    if (size > MRAM_LINE_SIZE)
    {
        size = MRAM_LINE_SIZE;
    }

    // Init all required fields to handle the write to mram once the data is read from l2
    this->req_size = size;
    this->read_pending_data = this->line_data;
    this->write_pending_size = size;

    // And get the data from L2
    this->tx_channel->get_data(size);
}



void Mram_periph::check_state()
{
    // Check if there is any operation requiring the FSM handler to be called
    if (!(this->event->is_enqueued()))
    {
        if (this->state != MRAM_STATE_PENDING_OP && 
            this->state != MRAM_STATE_HANDLE_TRIM_ONGOING && 
            this->state != MRAM_STATE_HANDLE_PROGRAM_ONGOING && 
            this->state != MRAM_STATE_IDLE || 
            (this->read_pending_size > 0 && this->rx_channel->is_ready()))
        {
            this->top->event_enqueue(this->event, 1);
        }
    }
}


vp::io_req_status_e Mram_periph::custom_req(vp::io_req *req, uint64_t offset)
{
    this->regmap.access(offset, req->get_size(), req->get_data(), req->get_is_write());

    return vp::IO_REQ_OK;
}


void Mram_periph::reset(bool active)
{
    Udma_periph::reset(active);

    this->read_fifo->reset(active);

    if (!active)
    {
        this->pending_transfer_size = 0;
        this->state = MRAM_STATE_IDLE;
        this->read_pending_size = 0;
        this->stalled = false;
        this->write_pending_size = 0;
        this->read_pending_size = 0;
    }
}


// Called by the udma core to push data to mram
void Mram_periph::push_data(uint8_t *data, int size)
{
    switch (this->state)
    {
        case MRAM_STATE_HANDLE_TRIM_ONGOING:
        {
            // For trim config, the mram can accept data of any size, so just forward 
            // the data to the mram
            mram_data_t mram_data = {.data=data, .size=size};
            this->req_size = size;
            this->out_data.sync(&mram_data);
            break;
        }

        case MRAM_STATE_HANDLE_PROGRAM_ONGOING:
        {
            // For a program operation, data must be sent line by line to the mram.
            // First gather the data into the line buffer
            memcpy(this->read_pending_data, data, size);
            this->read_pending_data += size;
            this->write_pending_size -= size;

            // And once the line has been buffered, sent it to the mram
            if (this->write_pending_size == 0)
            {
                mram_data_t mram_data = {.data=this->line_data, .size=this->req_size};
                this->out_data.sync(&mram_data);
            }
            break;
        }
    }
}

Mram_tx_channel::Mram_tx_channel(udma *top, Mram_periph *periph, string name)
    : Udma_tx_channel(top, name), periph(periph)
{
}


void Mram_tx_channel::push_data(uint8_t *data, int size)
{
    this->periph->push_data(data, size);
}


Mram_rx_channel::Mram_rx_channel(udma *top, Mram_periph *periph, string name) : Udma_rx_channel(top, name), periph(periph)
{
}
