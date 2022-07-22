/*
 * Copyright (C) 2020  GreenWaves Technologies, SAS
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

#include "../mem_plug.hpp"


/*
 * MEM_PLUG
 */

Mem_plug::Mem_plug(vp::block *parent, vp::component *comp, std::string path, vp::io_master *out_itf,
    int nb_ports, int output_latency)
: Mem_plug_implem(parent, comp, path, out_itf, nb_ports, output_latency)
{
}

Mem_plug_implem::Mem_plug_implem(vp::block *parent, vp::component *comp, std::string path,
    vp::io_master *out_itf, int nb_ports, int output_latency)
: block(parent), waiting_reqs(this), nb_pending_reqs(this, 0), output_latency(output_latency)
{
    comp->traces.new_trace(path, &this->trace, vp::DEBUG);
    this->nb_ports = nb_ports;

    for (int i=0; i<nb_ports; i++)
    {
        this->ports.push_back(new Mem_plug_port(this, comp, path + "/port_" + std::to_string(i), out_itf));
    }
}


void Mem_plug_implem::check_state()
{
    // Check if we can push a pending request to an available port
    if (this->nb_pending_reqs.get() < this->nb_ports && !this->waiting_reqs.empty())
    {
        // If so just take the first available one and forward it the request
        Mem_plug_req *req = (Mem_plug_req *)this->waiting_reqs.pop();
        this->nb_pending_reqs.set(this->nb_pending_reqs.get() + 1);

        for (Mem_plug_port *port: this->ports)
        {
            if (port->enqueue(req))
                break;
        }
    }
}


void Mem_plug_implem::handle_request_done(Mem_plug_req *req)
{
    // Decrease number of requests being done
    this->nb_pending_reqs.set(this->nb_pending_reqs.get() - 1);
    // Notify the caller about the termination
    req->handle_termination();
    // And let the FSM check if another request can be processed since one port is now available
    this->check_state();
}


void Mem_plug::enqueue(Mem_plug_req *req)
{
    this->trace.msg(vp::trace::LEVEL_TRACE, "Enqueueing request (addr: 0x%x, data: %p, size: 0x%x, is_write: %d)\n",
        req->addr, req->data, req->size, req->is_write);

    // Just push the request to the list of waiting requests and let the FSM check if it can
    // be processed
    this->waiting_reqs.push_back(req);

    this->check_state();
}



/*
 * MEM_PLUG_PORT
 */

Mem_plug_port::Mem_plug_port(Mem_plug_implem *top, vp::component *comp, std::string path, vp::io_master *out_itf)
    : block(top), top(top), pending_req(this), comp(comp)
{
    comp->traces.new_trace(path, &this->trace, vp::DEBUG);
    this->event = comp->event_new((void *)this, Mem_plug_port::event_handler);
    this->out_itf = out_itf;
}

bool Mem_plug_port::enqueue(Mem_plug_req *req)
{
    if (!this->pending_req.empty())
        return false;

    this->trace.msg(vp::trace::LEVEL_TRACE, "Enqueueing request (req: %p, addr: 0x%x, data: %p, size: 0x%x, is_write: %d)\n",
        req, req->addr, req->data, req->size, req->is_write);

    // Enqueue the request
    this->pending_req.push_back(req);

    // And the FSM do regular accesses to L2 
    this->check_state();

    return true;
}

void Mem_plug_port::check_state()
{
    if (!this->event->is_enqueued())
    {
        if (!this->pending_req.empty())
        {
            // Take into account the latency of the last access to respect the bandwidth
            int64_t cycles = 1;
            int64_t time = this->comp->get_time();
            if (this->ready_time != -1 && this->ready_time > time)
            {
                cycles = this->ready_time - time;
            }
            this->comp->event_enqueue(this->event, cycles);
        }
    }
}

void Mem_plug_port::event_handler(void *__this, vp::clock_event *event)
{
    Mem_plug_port *_this = (Mem_plug_port *)__this;

    vp::io_req *out_req = &_this->out_req;
    Mem_plug_req *req = (Mem_plug_req *)_this->pending_req.head();

    // If a reset occured on the one who created the request, the request has been removed
    // so we have to check if it is still there
    if (req == NULL)
    {
        return;
    }

    // Prepare the output request
    out_req->prepare();
    out_req->set_addr(req->addr);
    out_req->set_data(req->data);
    out_req->set_size(req->size < 4 ? req->size : 4);
    out_req->set_is_write(req->is_write);

    _this->trace.msg(vp::trace::LEVEL_TRACE, "Sending L2 request (req: %p, addr: 0x%x, data: %p, size: 0x%x, is_write: %d)\n",
        req, out_req->get_addr(), out_req->get_data(), out_req->get_size(), out_req->get_is_write());

    // Update the input request
    req->addr += 4;
    req->data += 4;
    req->size -= 4;

    // Send the output request
    vp::io_req_status_e err = _this->out_itf->req(out_req);

    // For now we just support synchronous reply
    if (err != vp::IO_REQ_OK)
    {
        vp_warning_always(&_this->trace, "UNIMPLEMENTED AT %s %d\n", __FILE__, __LINE__);
    }

    _this->trace.msg(vp::trace::LEVEL_TRACE, "Received data (value: 0x%x)\n",
        *(uint32_t *)out_req->get_data());

    // Store the latency so that the FSM schedule the next access after the latency
    int64_t latency = out_req->get_full_latency() + _this->top->output_latency;
    _this->ready_time = _this->comp->get_time() + latency;

    // If the input request is done, remove it and notify the caller
    if (req->size <= 0)
    {
        _this->pending_req.pop();
        _this->top->handle_request_done(req);
    }

    _this->check_state();
}


/*
 * MEM_PLUG_REQ
 */

Mem_plug_req::Mem_plug_req(uint32_t addr, uint8_t *data, uint32_t size, bool is_write)
    : addr(addr), data(data), size(size), is_write(is_write)
{
}

