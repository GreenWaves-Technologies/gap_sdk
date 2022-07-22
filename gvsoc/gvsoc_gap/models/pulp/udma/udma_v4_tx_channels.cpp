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


void Udma_tx_channel::reset(bool active)
{
    Udma_channel::reset(active);

    if (active)
    {

        while(!this->requested_size_queue.empty())
        {
            this->requested_size_queue.pop_front();
        }
        this->requested_size = 0;
        this->enqueued = false;
    }
}


void Udma_tx_channel::set_active(bool active)
{
    if (active)
    {
        if (!this->requested_size_queue.empty() && !this->enqueued)
        {
            this->top->tx_channels->push_ready_channel(this);
        }

    }
    else
    {
        if (this->enqueued)
        {
            this->top->tx_channels->pop_ready_channel(this);
        }
    }

    this->active = active;

    this->check_state();
}

void Udma_tx_channel::check_state()
{
    this->top->tx_channels->check_state();
}


void Udma_tx_channel::get_data(int size, int channel)
{
    this->requested_size_queue.push_back(size);
    this->requested_size += size;
    if (this->requested_size_queue.size() > 0 && this->is_active() && !this->enqueued)
    {
        this->top->tx_channels->push_ready_channel(this);
        this->check_state();
    }

    if (channel != -1 && channel >= 0xe0)
    {
        int stream_id = channel - 0xe0;
        uint32_t data = this->top->tx_channels->stream_out_data_pop(stream_id);
        this->push_data((uint8_t *)&data, 4);
    }
}

bool Udma_tx_channel::is_ready()
{
    return this->top->tx_channels->is_ready();
}


Udma_tx_channels::Udma_tx_channels(udma *top, int fifo_size) : top(top)
{
    int nb_outstanding_l2_reqs = 6;
    this->l2_free_reqs = new Udma_queue<vp::io_req>(nb_outstanding_l2_reqs);
    this->l2_waiting_reqs = new Udma_queue<vp::io_req>(nb_outstanding_l2_reqs);
    for (int i = 0; i < nb_outstanding_l2_reqs; i++)
    {
        vp::io_req *req = new vp::io_req();
        req->set_data(new uint8_t[4]);
        req->set_is_write(false);
        req->arg_alloc();
        this->l2_free_reqs->push(req);
    }

    this->send_reqs_event = top->event_new(this, Udma_tx_channels::handle_pending);
    this->waiting_reqs_event = top->event_new(this, Udma_tx_channels::handle_waiting);

    this->stream_out_channels.resize(top->nb_udma_stream_in);
    this->stream_out_data.resize(top->nb_udma_stream_in);
    this->stream_out_data_ready.resize(top->nb_udma_stream_in);

    for (int i=0; i<top->nb_udma_stream_out; i++)
    {
        vp::wire_slave<uint32_t> *data_itf = new vp::wire_slave<uint32_t>();
        data_itf->set_sync_meth_muxed(&Udma_tx_channels::stream_out_data_sync, i);
        top->new_slave_port(this, "stream_out_data_" + std::to_string(i), data_itf);

        vp::wire_master<bool> *ready_itf = new vp::wire_master<bool>();
        this->stream_out_ready_itf.push_back(ready_itf);
        top->new_master_port(this, "stream_out_ready_" + std::to_string(i), ready_itf);

        this->stream_out_channels[i] = NULL;
    }
}


uint32_t Udma_tx_channels::stream_out_data_pop(int id)
{
    if (!this->stream_out_data_ready[id])
    {
        this->stream_out_ready_set(id, true);
        return this->stream_out_data[id];
    }
    else
    {
        return 0;
    }
}


void Udma_tx_channels::stream_out_data_sync(void *__this, uint32_t data, int id)
{
    Udma_tx_channels *_this = (Udma_tx_channels *)__this;

    _this->stream_out_data[id] = data;
    _this->stream_out_ready_set(id, false);
}

bool Udma_tx_channels::is_ready()
{
    return true;
}


void Udma_tx_channels::push_ready_channel(Udma_tx_channel *channel)
{
    channel->enqueued = true;
    this->pending_channels.push(channel);
}


void Udma_tx_channels::pop_ready_channel(Udma_tx_channel *channel)
{
    std::queue<Udma_tx_channel *> new_queue;
    channel->enqueued = false;
    while(!this->pending_channels.empty())
    {
        Udma_tx_channel *channel_head = this->pending_channels.front();
        this->pending_channels.pop();
        if (channel_head != channel)
        {
            new_queue.push(channel_head);
        }
    }

    this->pending_channels.swap(new_queue);
}

void Udma_tx_channels::reset(bool active)
{
    if (active)
    {
        for (int i=0; i<top->nb_udma_stream_out; i++)
        {
            this->stream_out_ready_set(i, true);
        }
    }
}


void Udma_tx_channels::stream_out_ready_set(int id, bool ready)
{
    this->stream_out_data_ready[id] = ready;
    this->stream_out_ready_itf[id]->sync(ready);
}


void Udma_tx_channels::handle_pending(void *__this, vp::clock_event *event)
{
    Udma_tx_channels *_this = (Udma_tx_channels *)__this;

    if (!_this->pending_channels.empty() && !_this->l2_free_reqs->is_empty() && _this->pending_channels.front()->is_active())
    {
        // Get the next request from the next channel
        Udma_tx_channel *channel = _this->pending_channels.front();

        int size = 4;
        if (size > channel->requested_size_queue.front())
        {
            size = channel->requested_size_queue.front();
        }
        uint32_t addr;

        channel->get_addrgen()->get_next_transfer(&addr, &size);

        _this->pop_ready_channel(channel);

        // Decrease next channel pending size and put it back at the tail if there is still something to send
        channel->requested_size -= size;
        if (size == channel->requested_size_queue.front())
        {
            channel->requested_size_queue.pop_front();
        }
        else
        {
            int requested_size_queue = channel->requested_size_queue.front();
            channel->requested_size_queue.pop_front();
            channel->requested_size_queue.push_front(requested_size_queue - size);
        }

        if (channel->requested_size_queue.size() && !channel->enqueued && channel->is_active())
        {
            _this->push_ready_channel(channel);
        }

        // Get address and final size from address generator. Size can be less due to transfer
        // uint32_t addr;
        // channel->get_addrgen()->get_next_transfer(&addr, &size);

        // Prepare L2 request
        vp::io_req *l2_req = _this->l2_free_reqs->pop();

        l2_req->prepare();
        l2_req->set_addr(addr);
        l2_req->set_size(size);

        *(Udma_tx_channel **)l2_req->arg_get(0) = channel;

        _this->top->trace.msg(vp::trace::LEVEL_DEBUG, "Reading from memory (addr: 0x%x, size: %d)\n", l2_req->get_addr(), size);

        int err = _this->top->l2_itf.req(l2_req);

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

    _this->check_state();
}


void Udma_tx_channels::handle_waiting(void *__this, vp::clock_event *event)
{
    Udma_tx_channels *_this = (Udma_tx_channels *)__this;

    vp::io_req *req = _this->l2_waiting_reqs->get_first();
    int64_t cycles = _this->top->get_cycles();

    while (req != NULL && req->get_latency() <= cycles)
    {
        _this->top->trace.msg(vp::trace::LEVEL_DEBUG, "Read request is ready, pushing to channel (req: %p)\n", req);

        _this->l2_waiting_reqs->pop();
        _this->l2_free_reqs->push(req);
        Udma_tx_channel *channel = *(Udma_tx_channel **)req->arg_get(0);
        channel->push_data(req->get_data(), req->get_size());

        req = _this->l2_waiting_reqs->get_first();
    }

    _this->check_state();
}


void Udma_tx_channels::check_state()
{
    if (!this->send_reqs_event->is_enqueued())
    {
        if (!this->pending_channels.empty() && !this->l2_free_reqs->is_empty() && this->pending_channels.front()->is_active())
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
}
