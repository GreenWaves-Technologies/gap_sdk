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

#include <vp/vp.hpp>
#include <vp/itf/io.hpp>
#include <stdio.h>
#include <string.h>
#include <vector>
#include "archi/utils.h"
#include "udma_v4_impl.hpp"
#include "udma_ctrl/udma_ctrl_regs.h"
#include "udma_ctrl/udma_ctrl_regfields.h"
#include "udma_ctrl/udma_ctrl_gvsoc.h"

using namespace std::placeholders;


void Udma_rx_channel::push_data(uint8_t *data, int size, int addrgen_id)
{
    this->top->rx_channels->push_data(data, size, this->addrgen, addrgen_id);
}


void Udma_rx_channel::wait_active()
{
    this->waiting_active = true;
    this->top->rx_channels->add_waiting_channel(this);
}


void Udma_rx_channel::wait_active_cancel()
{
    this->waiting_active = false;
}


void Udma_rx_channel::reset(bool active)
{
    Udma_channel::reset(active);

    if (active)
    {
        this->active = false;
        this->waiting_active = false;
    }
}


bool Udma_rx_channel::is_ready()
{
    return this->top->rx_channels->is_ready();
}


void Udma_rx_channel::set_active(bool active)
{
    bool active_done = !this->is_active() && active;
    this->active = active;

    this->top->get_trace()->msg(vp::trace::LEVEL_INFO, "Set active\n");

    if (active_done)
    {
        this->notify_wait_active_done();
    }
}


void Udma_rx_channel::notify_wait_active_done()
{
    this->waiting_active = false;
    this->wait_active_done();
}


Udma_rx_channels::Udma_rx_channels(udma *top, int fifo_size) : top(top)
{
    this->l2_free_reqs = new Udma_queue<vp::io_req>(fifo_size);
    this->l2_waiting_reqs = new Udma_queue<vp::io_req>(fifo_size);
    for (int i = 0; i < fifo_size; i++)
    {
        vp::io_req *req = new vp::io_req();
        req->set_data(new uint8_t[4]);
        req->set_is_write(true);
        req->arg_alloc(); // To store request
        this->l2_free_reqs->push(req);
    }

    this->fifo_free = new Udma_queue<Udma_request>(fifo_size);
    for (int i = 0; i < fifo_size; i++)
    {
        this->fifo_free->push(new Udma_request());
    }

    this->fifo_ready = new Udma_queue<Udma_request>(-1);

    this->send_reqs_event = top->event_new(this, Udma_rx_channels::handle_pending);
    this->waiting_reqs_event = top->event_new(this, Udma_rx_channels::handle_waiting);

    this->stream_in_channels.resize(top->nb_udma_stream_in);

    for (int i=0; i<top->nb_udma_stream_in; i++)
    {
        vp::wire_master<uint32_t> *data_itf = new vp::wire_master<uint32_t>();
        this->stream_in_data_itf.push_back(data_itf);
        top->new_master_port(this, "stream_in_data_" + std::to_string(i), data_itf);

        vp::wire_slave<bool> *ready_itf = new vp::wire_slave<bool>();
        ready_itf->set_sync_meth_muxed(&Udma_rx_channels::stream_in_ready_sync, i);
        top->new_slave_port(this, "stream_in_ready_" + std::to_string(i), ready_itf);

        this->stream_in_channels[i] = NULL;
    }
}


void Udma_rx_channels::stream_in_ready_sync(void *__this, bool ready, int id)
{
    Udma_rx_channels *_this = (Udma_rx_channels *)__this;
    Udma_rx_channel *channel = _this->stream_in_channels[id];
    if (channel != NULL && channel->is_stream && channel->stream_id == id)
    {
        channel->set_active(ready);
    }
}




void Udma_rx_channels::handle_pending(void *__this, vp::clock_event *event)
{
    Udma_rx_channels *_this = (Udma_rx_channels *)__this;

    if (!_this->fifo_ready->is_empty() && !_this->l2_free_reqs->is_empty())
    {
        Udma_request *req = _this->fifo_ready->get_first();

        if (req->addrgen == NULL)
        {
            int stream_id = req->addrgen_id - 0xe0;

            _this->fifo_ready->pop();
            _this->fifo_free->push(req);

            if (_this->stream_in_data_itf[stream_id]->is_bound())
            {
                _this->stream_in_data_itf[stream_id]->sync(req->data);
            }
        }
        else
        {
            int size = req->size;
            uint32_t addr;
            vp::io_req *l2_req = _this->l2_free_reqs->pop();

            bool err = req->addrgen->get_next_transfer(&addr, &size);

            if (!err)
            {
                l2_req->prepare();
                l2_req->set_addr(addr);
                *(uint32_t *)l2_req->get_data() = req->data;
                l2_req->set_size(size);

                _this->top->trace.msg(vp::trace::LEVEL_TRACE, "Writing to memory (value: 0x%x, addr: 0x%x, size: %d)\n", req->data, l2_req->get_addr(), size);

                int err = _this->top->l2_itf.req(l2_req);

                if (addr == 0)
                {
                    _this->top->trace.fatal("UDMA trying to access NULL\n");
                }

                if (err == vp::IO_REQ_OK)
                {
                    l2_req->set_latency(l2_req->get_latency() + _this->top->get_cycles() + 1);
                    _this->l2_waiting_reqs->push(l2_req);
                }
                else
                {
                    _this->top->trace.warning("UNIMPLEMENTED AT %s %d\n", __FILE__, __LINE__);
                }
            }

            req->size -= size;

            if (req->size == 0)
            {
                _this->fifo_ready->pop();
                *(Udma_request **)l2_req->arg_get(0) = req;
            }
            else
            {
                req->data >>= size*8;
                *(Udma_request **)l2_req->arg_get(0) = NULL;
            }
        }
    }

    _this->check_state();
}


void Udma_rx_channels::handle_waiting(void *__this, vp::clock_event *event)
{
    Udma_rx_channels *_this = (Udma_rx_channels *)__this;

    vp::io_req *req = _this->l2_waiting_reqs->get_first();
    int64_t cycles = _this->top->get_cycles();
    while (req != NULL && req->get_latency() <= cycles)
    {
        _this->top->trace.msg(vp::trace::LEVEL_TRACE, "Write request is ready, pushing to channel (req: %p)\n", req);

        _this->l2_waiting_reqs->pop();
        _this->l2_free_reqs->push(req);
        Udma_request *request = *(Udma_request **)req->arg_get(0);
        if (request)
        {
            _this->fifo_free->push(request);
        }

        req = _this->l2_waiting_reqs->get_first();
    }

    _this->check_state();
}


void Udma_rx_channels::check_waiting_channels()
{
    if (this->is_ready())
    {
        std::queue<Udma_rx_channel *> waiting_channels;
        while (!this->waiting_channels.empty())
        {
            waiting_channels.push(this->waiting_channels.front());
            this->waiting_channels.pop();
        }

        while (!waiting_channels.empty())
        {
            waiting_channels.front()->notify_wait_active_done();
            waiting_channels.pop();
        }
    }
}

bool Udma_rx_channel::is_active()
{
    return this->active && this->is_ready() && (!this->addrgen || this->addrgen->get_remaining_size() > 0);
}


void Udma_rx_channels::check_state()
{
    if (!this->send_reqs_event->is_enqueued())
    {
        if (!this->fifo_ready->is_empty() && !this->l2_free_reqs->is_empty())
        {
            this->top->event_enqueue(this->send_reqs_event, 1);
        }
    }

    if (!this->waiting_reqs_event->is_enqueued())
    {
        if (!this->l2_waiting_reqs->is_empty())
        {
            this->top->event_enqueue(this->waiting_reqs_event, this->l2_waiting_reqs->get_first()->get_latency() - this->top->get_cycles());
        }
    }

    this->check_waiting_channels();
}


void Udma_rx_channels::push_data(uint8_t *data, int size, Udma_addrgen *addrgen, int addrgen_id)
{
    Udma_request *req = this->fifo_free->pop();

    req->data = *(uint32_t *)data;
    req->size = size;
    req->addrgen = addrgen;
    req->addrgen_id = addrgen_id;
    this->fifo_ready->push(req);
    if (addrgen)
    {
        addrgen->dec_size(size);
    }

    this->check_state();
}


bool Udma_rx_channels::is_ready()
{
    return !this->fifo_free->is_empty();
}
