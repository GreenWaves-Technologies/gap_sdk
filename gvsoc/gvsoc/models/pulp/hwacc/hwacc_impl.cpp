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

#ifdef __VP_USE_SYSTEMC
#include <systemc.h>
#endif

#define NB_MASTER_PORTS 4

class hwacc;

#ifdef __VP_USE_SYSTEMC
SC_MODULE(hwacc_module)
{

  friend class hwacc;

  SC_HAS_PROCESS(hwacc_module);
  hwacc_module(sc_module_name nm, hwacc *vp_module) : sc_module(nm), vp_module(vp_module)
  {
    SC_THREAD(run);
  }

  void run();

private:
  sc_event  event;
  hwacc *vp_module;

};
#endif

class hwacc : public vp::component
{

  friend class hwacc_module;

public:

  hwacc(js::config *config);

  int build();
  void start();
  void elab();

  static vp::io_req_status_e req(void *__this, vp::io_req *req);

protected:
  vp::io_req *first_pending_reqs = NULL;
  vp::io_req *first_stalled_req = NULL;

private:

  vp::trace     trace;
  vp::io_slave in;
  vp::io_master         out[NB_MASTER_PORTS];
  vp::wire_master<bool> irq;

  int max_reqs = 4;
  int current_reqs = 0;
  int count = 0;
  vp::io_req *last_pending_reqs = NULL;
#ifdef __VP_USE_SYSTEMC
  hwacc_module *sc_module;
#endif
};

hwacc::hwacc(js::config *config)
: vp::component(config)
{

}

vp::io_req_status_e hwacc::req(void *__this, vp::io_req *req)
{
  hwacc *_this = (hwacc *)__this;

  uint64_t offset = req->get_addr();
  uint8_t *data = req->get_data();
  uint64_t size = req->get_size();

  // Requests from riscv cores will arrive here
  // Following values can be returned depending on the desired behavior:
  //   - vp::IO_REQ_INVALID : invalid access
  //   - vp::IO_REQ_DENIED  : request is not granted, usually to simulate some bus contention. This usually prevent pipelining on initiator side.
  //   - vp::IO_REQ_PENDING : request is granted but the response is not ready. This can be used to simulate some latency in case the latency is not know in advance. This will usually block the initiator after a while, once its pipeline is empty.
  //   - vp::IO_REQ_OK      : request has been fully handled. A latency can still be returned to simluate some timing behavior. This can be used for simple requests where the latency is known in advance.

  // In case vp::IO_REQ_DENIED or vp::IO_REQ_PENDING is returned, the request is owned by the hwpe, which has to call a callback later on when the request is considered granted or replied, to continue the interaction.


  _this->trace.msg("Hwacc access (offset: 0x%x, size: 0x%x, is_write: %d)\n", offset, size, req->get_is_write());

  if (offset + size > 0x80) {
    _this->warning.warning("Received out-of-bound request (reqAhwacc: 0x%llx, reqSize: 0x%llx)\n", offset, size);
    return vp::IO_REQ_INVALID;
  }

  // In this case, we just put the request in a fifo, the systemc part will handle it.
  if (_this->first_pending_reqs)
    _this->last_pending_reqs->set_next(req);
  else
    _this->first_pending_reqs = req;
  _this->last_pending_reqs = req;
  req->set_next(NULL);

  _this->current_reqs++;

  if (_this->current_reqs > _this->max_reqs)
  {
    if (_this->first_stalled_req == NULL)
      _this->first_stalled_req = _this->last_pending_reqs;
    return vp::IO_REQ_DENIED;
  }
  else
  {
#ifdef __VP_USE_SYSTEMC
    _this->sc_module->event.notify();
#endif
    return vp::IO_REQ_PENDING;
  }
}

int hwacc::build()
{
  traces.new_trace("trace", &trace, vp::DEBUG);

  // This section declares the in/out ports with the rest of the system

  // Input requests from cores will come here
  in.set_req_meth(&hwacc::req);
  new_slave_port("input", &in);

  // hwpe can send irq requests here
  this->new_master_port("irq", &this->irq);

  // hwpe can access the L1 through 4 32bits ports here
  for (int i=0; i<NB_MASTER_PORTS; i++)
  {
    this->new_master_port("out_" + std::to_string(i), &this->out[i]);
  }

  return 0;
}


void hwacc::elab()
{
#ifdef __VP_USE_SYSTEMC
  sc_module = new hwacc_module("wrapper2", this);
#endif
}


void hwacc::start()
{
}

extern "C" vp::component *vp_constructor(js::config *config)
{
  return new hwacc(config);
}


#ifdef __VP_USE_SYSTEMC
// This is a System C thread which can interact with gvsoc and gem5 to do the bridge
void hwacc_module::run()
{
  while(1)
  {

    // Wait for seomthing from gvsoc
    while(vp_module->current_reqs == 0){
      wait(event);
    }

    vp::io_req *req = vp_module->first_pending_reqs;
    vp_module->first_pending_reqs = vp_module->first_pending_reqs->get_next();

    printf("Handling request\n");

    // Do some dummy operations

    // We first do a read to l1 to get the value written by the test
    vp::io_req l1_req;
    uint32_t word;

    l1_req.set_addr(0x0008000);
    l1_req.set_data((uint8_t *)&word);
    l1_req.set_size(4);
    l1_req.set_is_write(false);

    vp_module->out[0].req(&l1_req);

    // The latency cn be taken into account to simulate some timing behaviors
    printf("Received read %x latency %ld\n", word, l1_req.get_latency());

    // Such a wait can be used to model timing
    wait(100, SC_NS);

    vp_module->current_reqs--;

    // A request is asynchronous and should be replied to unblock the core
    req->get_resp_port()->resp(req);

    if (vp_module->current_reqs >= vp_module->max_reqs)
    {
      vp::io_req *stalled_req = vp_module->first_stalled_req;
      vp_module->first_stalled_req = vp_module->first_stalled_req->get_next();
      stalled_req->get_resp_port()->grant(stalled_req);
    }

  }
}
#endif