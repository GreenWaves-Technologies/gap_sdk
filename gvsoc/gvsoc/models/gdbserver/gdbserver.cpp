/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna
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
 * Authors: Andreas Traber
 */

#include "gdbserver.hpp"
#include <stdarg.h>
#include "rsp.hpp"


Gdb_server::Gdb_server(js::config *config)
: vp::component(config)
{
    this->rsp = NULL;

    if (config->get_child_bool("enabled"))
    {
        this->rsp = new Rsp(this);
    }

    this->active_core = 9;
}


int Gdb_server::register_core(vp::Gdbserver_core *core)
{
    int id = core->gdbserver_get_id();

    this->trace.msg(vp::trace::LEVEL_INFO, "Registering core (id: %d)\n", id);

    this->cores[id] = core;
    this->cores_list.push_back(core);

    return 0;
}


void Gdb_server::signal(vp::Gdbserver_core *core)
{
    this->rsp->signal();
}


void Gdb_server::pre_pre_build()
{
    if (this->get_js_config()->get_child_bool("enabled"))
    {
        new_service("gdbserver", static_cast<Gdbserver_engine *>(this));
    }
}


int Gdb_server::build()
{
    traces.new_trace("trace", &trace, vp::DEBUG);
    this->new_master_port("out", &this->out);
    this->event = this->event_new(&Gdb_server::handle);
    this->pending_access = false;
    return 0;
}


void Gdb_server::start()
{
    if (this->rsp)
    {
        this->rsp->start(this->get_js_config()->get_child_int("port"));
    }
}


int Gdb_server::set_active_core(int id)
{
    this->trace.msg(vp::trace::LEVEL_DEBUG, "Setting active core (id: %d)\n", id);

    if (this->cores[id] != NULL)
    {
        this->active_core = id;
        return 0;
    }

    return -1;
}


vp::Gdbserver_core *Gdb_server::get_core(int id)
{
    if (id == -1)
    {
        id = this->active_core;
    }

    return this->cores[id];
}


void Gdb_server::handle(void *__this, vp::clock_event *event)
{
    Gdb_server *_this = (Gdb_server *)__this;
    if (_this->pending_access)
    {
        _this->pending_access = false;
        _this->rsp->io_access_done(0);
    }
}


int Gdb_server::io_access(uint32_t addr, int size, uint8_t *data, bool is_write)
{
    if (this->pending_access)
      return -1;

    this->pending_access = true;
    this->io_req.init();
    this->io_req.set_addr(addr);
    this->io_req.set_size(size);
    this->io_req.set_data(data);
    this->io_req.set_is_write(is_write);

    this->get_time_engine()->lock();
    int err = this->out.req(&this->io_req);
    if (err == vp::IO_REQ_OK)
    {
        int64_t latency = this->io_req.get_latency();
        this->event_enqueue(this->event, latency + 1);
    }
    else if (err == vp::IO_REQ_INVALID)
    {
        this->rsp->io_access_done(-1);
    }
    else
    {
        this->trace.fatal("Unsupported asynchronous reply\n");
    }
    this->get_time_engine()->unlock();

    return 0;
}


extern "C" vp::component *vp_constructor(js::config *config)
{
  return new Gdb_server(config);
}

#if 0

void Gdb_server::start() {
  target = std::make_shared<Target>(this);

  bkp = std::make_shared<Breakpoints>(this);

  
}
 
void Gdb_server::target_update_power() {
  target->update_power();
}

void Gdb_server::signal_exit(int status) {
  rsp->signal_exit(status);
}

void Gdb_server::refresh_target()
{
  target->reinitialize();
  target->update_power();
  bkp->enable_all();
}

int Gdb_server::stop()
{
  if (rsp != NULL)
  {
    rsp->stop();
    rsp = NULL;
  }
  return 1;
}

void Gdb_server::abort()
{
  if (rsp != NULL)
  {
    rsp->abort();
    rsp = NULL;
  }
}

#endif