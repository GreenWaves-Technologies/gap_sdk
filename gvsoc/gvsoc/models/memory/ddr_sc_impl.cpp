/* vim: set ts=2 sw=2 expandtab:*/
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
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */

#include <vp/vp.hpp>
#include <vp/itf/io.hpp>
#include <stdio.h>
#include <string.h>

#include "ems_mm.h"
#include "ems_common.h"
#include "ems_at_bus.h"
#include "tlm2_base_protocol_checker.h"
#ifdef __VP_USE_SYSTEMC_GEM5
#include "ems_gem5_tlm_br.h"
#endif /* __VP_USE_SYSTEMC_GEM5 */
#ifdef __VP_USE_SYSTEMC_DRAMSYS
#include "DRAMSys.h"
#else
#include "ems_at_target.h"
#endif /* __VP_USE_SYSTEMC_DRAMSYS */

#define BYTES_PER_ACCESS    64
#define ACCEPT_DELAY_PS     1000
#define TARGET_LATENCY_PS   2000

class gvsoc_tlm_br;

class ddr : public vp::component
{
  friend class gvsoc_tlm_br;

public:
  ddr(js::config *config);

  int build();
  void start();
  void stop();
  void elab();

  static vp::io_req_status_e req(void *__this, vp::io_req *req);

protected:
  vp::io_req *first_pending_reqs = nullptr;
  vp::io_req *first_stalled_req = nullptr;

private:
  vp::trace trace;
  vp::io_slave in;

  uint64_t size = 0;
  int max_reqs = 4;
  int current_reqs = 0;
  int count = 0;
  vp::io_req *last_pending_reqs = nullptr;
  // SystemC TLM-2.0 bridge (transactor component)
  gvsoc_tlm_br *sc_bridge;
  // Interconnect component
  ems::at_bus *at_bus;
  // A protocol checker for each hop
  // pcib: protocol checker between initiator and bus
  tlm_utils::tlm2_base_protocol_checker<> *pcib;
  // pcbt: protocol checker between bus and target
  tlm_utils::tlm2_base_protocol_checker<> *pcbt;
#ifdef __VP_USE_SYSTEMC_GEM5
  // gem5-SystemC TLM-2.0 bridge
  ems::gem5_tlm_br *g5tbr;
#endif /* __VP_USE_SYSTEMC_GEM5 */
#ifdef __VP_USE_SYSTEMC_DRAMSYS
  DRAMSys *dramsys;
#else
  ems::at_target *at_target;
#endif /* __VP_USE_SYSTEMC_DRAMSYS */
};

#include "ems_gvsoc_tlm_br.h"

ddr::ddr(js::config *config) : vp::component(config)
{
}

vp::io_req_status_e ddr::req(void *__this, vp::io_req *req)
{
  ddr *_this = (ddr *)__this;

  uint64_t offset = req->get_addr();
  uint8_t *data = req->get_data();
  uint64_t size = req->get_size();

  _this->trace.msg("ddr access (offset: 0x%x, size: 0x%x, is_write: %d)\n", offset, size, req->get_is_write());

  if (offset + size > _this->size) {
    _this->warning.warning("Received out-of-bound request (reqAddr: 0x%llx, reqSize: 0x%llx, memSize: 0x%llx)\n", offset, size, _this->size);
    return vp::IO_REQ_INVALID;
  }

  if (_this->first_pending_reqs)
    _this->last_pending_reqs->set_next(req);
  else
    _this->first_pending_reqs = req;
  _this->last_pending_reqs = req;
  req->set_next(nullptr);

  _this->current_reqs++;

  if (_this->current_reqs > _this->max_reqs)
  {
    if (_this->first_stalled_req == nullptr)
      _this->first_stalled_req = _this->last_pending_reqs;
    _this->trace.msg("ddr access (offset: 0x%x, size: 0x%x DENIED)\n", offset, size);
    return vp::IO_REQ_DENIED;
  }
  else
  {
    _this->sc_bridge->event.notify();
    _this->trace.msg("ddr access (offset: 0x%x, size: 0x%x sc_bridge notified)\n", offset, size);
    return vp::IO_REQ_PENDING;
  }
}

int ddr::build()
{
  traces.new_trace("trace", &trace, vp::DEBUG);
  in.set_req_meth(&ddr::req);
  new_slave_port("input", &in);
  return 0;
}

void ddr::elab()
{
  sc_bridge = new gvsoc_tlm_br("sc_br", this, ACCEPT_DELAY_PS, BYTES_PER_ACCESS);
  at_bus = new ems::at_bus("at_bus");
  pcib = new tlm_utils::tlm2_base_protocol_checker<>("pcib");
  pcbt = new tlm_utils::tlm2_base_protocol_checker<>("pcbt");

  sc_bridge->isocket.bind(pcib->target_socket);
  pcib->initiator_socket.bind(at_bus->tsocket);
  at_bus->isocket.bind(pcbt->target_socket);

#ifdef __VP_USE_SYSTEMC_GEM5
  pcbt->set_num_checks(0);
  std::string cfg = std::string(__GEM5_PATH) + std::string("/") + get_config_str("tlm/gem5-config");
  debug("gem5-config: " << cfg);
  g5tbr = new ems::gem5_tlm_br("g5tbr", cfg);
  for (auto adapt : g5tbr->adapters) {
      adapt->isocket.bind(at_bus->tsocket);
  }
#endif /* __VP_USE_SYSTEMC_GEM5 */
#ifdef __VP_USE_SYSTEMC_DRAMSYS
  std::string res = std::string(__DRAMSYS_PATH) + std::string("/DRAMSys/library/resources/");
  std::string sim = res + std::string("simulations/") + get_config_str("tlm/dramsys-config");
  debug("dramsys-config: " << sim);
  dramsys = new DRAMSys("DRAMSys", sim, res);
  pcbt->initiator_socket.bind(dramsys->tSocket);
#else
  at_target = new ems::at_target("at_target", ACCEPT_DELAY_PS, TARGET_LATENCY_PS, BYTES_PER_ACCESS);
  pcbt->initiator_socket.bind(at_target->tsocket);
#endif /* __VP_USE_SYSTEMC_DRAMSYS */
}

void ddr::start()
{
  size = get_config_int("size");
  trace.msg("Building ddr (size: 0x%lx)\n", size);
}

void ddr::stop()
{
#ifdef __VP_USE_SYSTEMC_DRAMSYS
  delete dramsys;
#else
  delete at_target;
#endif /* __VP_USE_SYSTEMC_DRAMSYS */

#ifdef __VP_USE_SYSTEMC_GEM5
  delete g5tbr;
#endif /* __VP_USE_SYSTEMC_GEM5 */
  delete pcib;
  delete pcbt;
  delete at_bus;
  delete sc_bridge;
}

extern "C" vp::component *vp_constructor(js::config *config)
{
  return new ddr(config);
}

