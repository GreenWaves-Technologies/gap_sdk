/* vim: set ts=2 sw=2 expandtab: */
/*
 * Copyright (C) 2019 TU Kaiserslautern
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
 * Author: Éder F. Zulian, TUK (zulian@eit.uni-kl.de)
 */

#ifndef __EMS_GVSOC_TLM_BR_H__
#define __EMS_GVSOC_TLM_BR_H__

#include <tlm.h>
#include <tlm_utils/peq_with_cb_and_phase.h>
#include <tlm_utils/simple_initiator_socket.h>

#include "ems_common.h"

class gvsoc_tlm_br: public sc_module
{
  friend class ddr;

public:
  tlm_utils::simple_initiator_socket<gvsoc_tlm_br> isocket;

  SC_HAS_PROCESS(gvsoc_tlm_br);
  gvsoc_tlm_br(sc_core::sc_module_name name, ddr *vp_component,
               double accept_delay_ps, uint32_t bpa) :
    sc_core::sc_module(name),
    isocket("isocket"),
    peq(this, &gvsoc_tlm_br::peq_cb),
    vp_component(vp_component),
    curr_req(nullptr),
    req_in_progress(nullptr),
    bytes_per_access(bpa)
  {
    resp_accept_delay = sc_core::sc_time(accept_delay_ps, SC_PS);
    isocket.register_nb_transport_bw(this, &gvsoc_tlm_br::nb_transport_bw);
    SC_THREAD(run);
  }

  // Module interface - backward path only
  tlm::tlm_sync_enum nb_transport_bw(tlm::tlm_generic_payload &p, tlm::tlm_phase &phase, sc_core::sc_time &d);

private:
  // Thread process member function
  void run();
  // Conversion vp::io_req to tlm::tlm_generic_payload
  void req_to_gp(vp::io_req *r, tlm::tlm_generic_payload *p, uint32_t tid, bool last);
  // Called at the end of the lifetime of a transaction to inspect it
  void inspect(tlm::tlm_generic_payload &p);
  // Payload event queue (PEQ)
  void peq_cb(tlm::tlm_generic_payload &p, const tlm::tlm_phase &phase);

  ddr *vp_component;
  vp::io_req *curr_req;
  sc_core::sc_event event;
  sc_core::sc_event end_req_event;
  sc_core::sc_event all_trans_completed;
  tlm::tlm_generic_payload *req_in_progress;
  tlm_utils::peq_with_cb_and_phase<gvsoc_tlm_br> peq;
  ems::mm mm;
  sc_core::sc_time resp_accept_delay;
  uint32_t bytes_per_access;
};

void gvsoc_tlm_br::req_to_gp(vp::io_req *r, tlm::tlm_generic_payload *p, uint32_t tid, bool last)
{
  assert(r);
  assert(p);
  assert(p->has_mm());

  tlm::tlm_command cmd = r->get_is_write() ? tlm::TLM_WRITE_COMMAND : tlm::TLM_READ_COMMAND;
  uint32_t offset = tid * bytes_per_access;
  sc_dt::uint64 addr = r->get_addr() + offset;
  unsigned char *data = r->get_data();
  unsigned char *dptr = &data[offset];
  unsigned int dlen = bytes_per_access;

  p->set_command(cmd);
  p->set_address(addr);
  p->set_data_ptr(dptr);
  p->set_data_length(dlen);
  p->set_streaming_width(dlen);
  p->set_byte_enable_ptr(nullptr);
  p->set_dmi_allowed(false);
  p->set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);

  ems::req_extension *ext = new ems::req_extension(r, tid, last);
  p->set_auto_extension(ext);
  //debug(name() << " Request p: " << p << " addr: 0x" << std::setfill('0') << std::setw(16) << std::hex << addr << std::dec << " cmd:" << (cmd ? " write" : " read") << " dlen: " << dlen << " tid: " << tid << " last: " << (last ? "true" : "false"));
}

void gvsoc_tlm_br::inspect(tlm::tlm_generic_payload &p)
{
  if (p.is_response_error()) {
    debug(name() << " Request p: " << &p << " response error" << p.get_response_string());
    SC_REPORT_ERROR(name(), p.get_response_string().c_str());
  }

  ems::req_extension *re;
  p.get_extension(re);
  assert(re->req == curr_req);

  if (re->last)
    all_trans_completed.notify();
}

void gvsoc_tlm_br::peq_cb(tlm::tlm_generic_payload &p, const tlm::tlm_phase &phase)
{
  tlm::tlm_phase fw_phase = tlm::UNINITIALIZED_PHASE;
  tlm::tlm_sync_enum s;

  switch (phase) {
    case tlm::END_REQ:
     // There must be a request in progress
      assert(req_in_progress);
      // Request exclusion rule:
      // END_REQ indicates that the downstream component is ready to accept
      // another BEGIN_REQ
      req_in_progress = nullptr;
      // Request channel is free
      end_req_event.notify();
      break;

    case tlm::BEGIN_RESP:
      if (&p == req_in_progress) {
        // Request exclusion rule:
        // BEGIN_RESP for the immediately preceding transaction (the exact
        // request that is in progress) indicates that the downstream component
        // is ready to accept another BEGIN_REQ
        // BEGIN_RESP for the immediately preceding request carries an implicit
        // END_REQ
        req_in_progress = nullptr;
        end_req_event.notify();
      }
      // Inspect transaction
      inspect(p);
      // Send final phase transition to downstream component
      fw_phase = tlm::END_RESP;
      s = isocket->nb_transport_fw(p, fw_phase, resp_accept_delay);
      // Transaction is complete in either case
      assert(s == tlm::TLM_COMPLETED || s == tlm::TLM_ACCEPTED);
      // Allow the memory manager to free the transaction object
      p.release();
      break;

    case tlm::BEGIN_REQ:
    case tlm::END_RESP:
      debug(name() << " Illegal phase" << phase);
      SC_REPORT_FATAL(name(), " Illegal phase");
      break;

    default:
      debug(name() << " Illegal phase" << phase);
      SC_REPORT_FATAL(name(), " Illegal phase");
      break;
  }

}

tlm::tlm_sync_enum gvsoc_tlm_br::nb_transport_bw(tlm::tlm_generic_payload &p, tlm::tlm_phase &phase, sc_core::sc_time &d)
{
  peq.notify(p, phase, d);
  return tlm::TLM_ACCEPTED;
}

void gvsoc_tlm_br::run()
{
  tlm::tlm_generic_payload *p;
  tlm::tlm_phase phase;
  sc_core::sc_time delay;
  tlm::tlm_sync_enum status;

  while (1) {
    while (vp_component->current_reqs == 0) {
      wait(event);
    }

    vp::io_req *req = vp_component->first_pending_reqs;
    vp_component->first_pending_reqs = vp_component->first_pending_reqs->get_next();

    curr_req = req;
    // A request corresponds to one or more transactions
    uint32_t nt = req->get_size() / bytes_per_access;
    uint32_t n_trans = nt > 0 ? nt : 1;

    for (auto t = 0; t < n_trans; t++) {
      // Get a generic payload from memory manager
      p = mm.palloc();
      p->acquire();
      // Convert packet to tlm generic payload
      bool last = (t == (n_trans - 1));
      req_to_gp(req, p, t, last);
      // Honor BEGIN_REQ/END_REQ exclusion rule
      if (req_in_progress) {
        wait(end_req_event);
      }
      // Non-blocking transport call on the forward path (send it downstream)
      req_in_progress = p;
      phase = tlm::BEGIN_REQ;
      delay = sc_core::SC_ZERO_TIME;
      status = isocket->nb_transport_fw(*p, phase, delay);

      switch (status) {
        case tlm::TLM_ACCEPTED:
          // Backward path is being used
          // Nothing to do, but expect an incoming call to nb_transport_bw()
          break;
        case tlm::TLM_UPDATED:
          // Return path is being used
          // The callee may have updated phase and delay
          // Honor the timing annotation
          peq.notify(*p, phase, delay);
          break;
        case tlm::TLM_COMPLETED:
          // Return path is being used
          // Early completion
          // The target has terminated the transaction
          req_in_progress = nullptr;
          inspect(*p);
          p->release();
          break;
        default:
          SC_REPORT_FATAL(name(), "Invalid status");
          break;
      }
    }

    wait(all_trans_completed);

    vp_component->current_reqs--;

    req->get_resp_port()->resp(req);

    if (vp_component->current_reqs >= vp_component->max_reqs) {
      vp::io_req *stalled_req = vp_component->first_stalled_req;
      vp_component->first_stalled_req = vp_component->first_stalled_req->get_next();
      stalled_req->get_resp_port()->grant(stalled_req);
    }
  }
}

#endif /* __EMS_GVSOC_TLM_BR_H__ */

