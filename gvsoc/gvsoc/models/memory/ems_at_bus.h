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

#ifndef __EMS_AT_BUS_H__
#define __EMS_AT_BUS_H__

#include <queue>
#include <vector>

#include <tlm.h>
#include <tlm_utils/multi_passthrough_target_socket.h>
#include <tlm_utils/multi_passthrough_initiator_socket.h>
#include <tlm_utils/peq_with_cb_and_phase.h>
#include <tlm_utils/instance_specific_extensions.h>

namespace ems {

class at_bus : public sc_core::sc_module
{
public:
  tlm_utils::multi_passthrough_initiator_socket<at_bus> isocket;
  tlm_utils::multi_passthrough_target_socket<at_bus> tsocket;

  SC_HAS_PROCESS(at_bus);
  at_bus(sc_core::sc_module_name name) :
    sc_core::sc_module(name),
    isocket("isocket"),
    tsocket("tsocket"),
    fw_peq(this, &at_bus::fw_peq_cb),
    bw_peq(this, &at_bus::bw_peq_cb),
    tcnt(0)
  {
    tsocket.register_nb_transport_fw(this, &at_bus::nb_transport_fw);
    tsocket.register_transport_dbg(this, &at_bus::transport_dbg);
    isocket.register_nb_transport_bw(this, &at_bus::nb_transport_bw);
  }

  void end_of_elaboration()
  {
    unsigned int num_targets = isocket.size();
    unsigned int num_initiators = tsocket.size();

    for (auto i = 0; i < num_targets; ++i) {
      req_in_progress.push_back(nullptr);
      pending_req.push_back(queue<tlm::tlm_generic_payload *>());
    }

    for (auto i = 0; i < num_initiators; ++i) {
      resp_in_progress.push_back(nullptr);
      pending_resp.push_back(queue<tlm::tlm_generic_payload *>());
    }
  }

  // Module interface - forward path
  tlm::tlm_sync_enum nb_transport_fw(int id, tlm::tlm_generic_payload &p, tlm::tlm_phase &phase, sc_core::sc_time &d);
  unsigned int transport_dbg(int id, tlm::tlm_generic_payload &p);
  // Module interface - backward path
  tlm::tlm_sync_enum nb_transport_bw(int id, tlm::tlm_generic_payload &p, tlm::tlm_phase &phase, sc_core::sc_time &d);

private:
  // Total transactions counter
  sc_dt::uint64 tcnt;

  // Address decoder. The target device information can be extracted from the
  // address in order to select the downstream communication socket.
  int decode_address(const sc_dt::uint64 address, sc_dt::uint64 &masked_address);

  // Called on receiving TLM_COMPLETED to inspect transaction
  void inspect(tlm::tlm_generic_payload &p);

  // Forward path payload event queue (PEQ)
  tlm_utils::peq_with_cb_and_phase<at_bus> fw_peq;
  void fw_peq_cb(tlm::tlm_generic_payload &p, const tlm::tlm_phase &phase);
  // Backward path payload event queue (PEQ)
  tlm_utils::peq_with_cb_and_phase<at_bus> bw_peq;
  void bw_peq_cb(tlm::tlm_generic_payload &p, const tlm::tlm_phase &phase);

  // Route extension
  struct route_ext : tlm_utils::instance_specific_extension<route_ext> {
    int usock;          // upstream socket index
    int dsock;          // downstream socket index
    bool ds_completed;  // transaction completed by downstream component
    sc_dt::uint64 num;  // transaction id number
  };
  tlm_utils::instance_specific_extension_accessor accessor;

  // Request in progress (one per target device)
  std::vector<tlm::tlm_generic_payload *> req_in_progress;
  // Pending requests (one queue per target device)
  std::vector<std::queue<tlm::tlm_generic_payload *>> pending_req;

  // Response in progress (one per target device)
  std::vector<tlm::tlm_generic_payload *> resp_in_progress;
  // Pending responses (one queue per initiator device)
  std::vector<std::queue<tlm::tlm_generic_payload *>> pending_resp;
};

int at_bus::decode_address(sc_dt::uint64 address, sc_dt::uint64 &masked_address)
{
  // XXX:
  // Right now it just sends to the same target and masked_address is just a
  // copy of address
  sc_dt::uint64 mask = 0xffffffffffffffffULL;
  sc_dt::uint64 offset = 0x0000000000000000ULL;
  masked_address = ((address - offset) & mask);
  return 0;
}

void at_bus::inspect(tlm::tlm_generic_payload &p)
{
  if (p.is_response_error()) {
    SC_REPORT_ERROR(name(), p.get_response_string().c_str());
  }
}

void at_bus::fw_peq_cb(tlm::tlm_generic_payload &p, const tlm::tlm_phase &phase)
{
  route_ext *re;
  accessor(p).get_extension(re);
  int usock = re->usock;
  int dsock = re->dsock;
  tlm::tlm_phase bw_phase = tlm::UNINITIALIZED_PHASE;
  tlm::tlm_phase fw_phase = tlm::UNINITIALIZED_PHASE;
  sc_core::sc_time d = sc_core::SC_ZERO_TIME;
  tlm::tlm_sync_enum s;
  tlm::tlm_generic_payload *pl = nullptr;

  switch (phase) {
    case tlm::BEGIN_REQ:
      if (!req_in_progress[dsock]) {
        // Downstream socket is idle
        // Mark it as busy
        req_in_progress[dsock] = &p;
        // Forward BEGIN_REQ downstream
        fw_phase = tlm::BEGIN_REQ;
        d = sc_core::SC_ZERO_TIME;
        s = isocket[dsock]->nb_transport_fw(p, fw_phase, d);
        switch (s) {
          case tlm::TLM_ACCEPTED:
            // Backward path is being used
            // Nothing to do, but expect an incoming call to nb_transport_bw
            break;
          case tlm::TLM_UPDATED:
            // Return path is being used
            // The downstream component may have updated phase and delay
            // Honor the timing annotation
            bw_peq.notify(p, phase, d);
            break;
          case tlm::TLM_COMPLETED:
            // Return path is being used
            // Early completion
            // The downstream component has terminated the transaction
            inspect(p);
            // For this transaction, no further communication in the
            // downstream direction shall be performed
            re->ds_completed = true;
            // Request exclusion rule:
            // TLM_COMPLETED from downstream carries an implicit BEGIN_RESP
            // BEGIN_RESP for the immediately preceding request carries an
            // implicit END_REQ
            req_in_progress[dsock] = nullptr;
            // TLM_ACCEPTED was returned in nb_transport_fw()
            // Upstream component is waiting for backward path communication
            bw_phase = tlm::BEGIN_RESP;
            // Honor timing annotation
            bw_peq.notify(p, bw_phase, d);
            break;
          default:
            debug(name() << " Invalid status");
            SC_REPORT_FATAL(name(), " Invalid status");
            break;
        }
      } else {
        // Downstream socket is busy
        // Queue request to be treated when the next END_REQ (explicit or
        // implicit) happens
        pending_req[dsock].push(&p);
      }
      break;

    case tlm::END_RESP:
      // If this point was reached there must be a response in progress
      assert(resp_in_progress[usock]);
      // Check if further communications downstream should be performed
      if (!re->ds_completed) {
        // Forward END_RESP downstream
        fw_phase = tlm::END_RESP;
        d = sc_core::SC_ZERO_TIME;
        s = isocket[dsock]->nb_transport_fw(p, fw_phase, d);
        // Transaction is complete in either case
        assert(s == tlm::TLM_COMPLETED || s == tlm::TLM_ACCEPTED);
      }
      // Release transaction object
      p.release();
      // Response channel is free
      // Are there pending responses for this upstream socket?
      if (pending_resp[usock].empty()) {
        resp_in_progress[usock] = nullptr;
      } else {
        // Get the next pending response from the queue
        pl = pending_resp[usock].front();
        pending_resp[usock].pop();
        resp_in_progress[usock] = pl;
        bw_phase = tlm::BEGIN_RESP;
        d = sc_core::SC_ZERO_TIME;
        // Schedule transmission
        bw_peq.notify(*pl, bw_phase, d);
      }
      break;

    case tlm::END_REQ:
    case tlm::BEGIN_RESP:
      debug(name() << " Illegal phase" << phase);
      SC_REPORT_FATAL(name(), " Illegal phase");
      break;

    default:
      debug(name() << " Illegal phase" << phase);
      SC_REPORT_FATAL(name(), " Illegal phase");
      break;
  }
}

tlm::tlm_sync_enum at_bus::nb_transport_fw(int id, tlm::tlm_generic_payload &p, tlm::tlm_phase &phase, sc_core::sc_time &d)
{
  route_ext *re;
  accessor(p).get_extension(re);
  int dsock;
  sc_dt::uint64 masked_address;

  switch (phase) {
    case tlm::BEGIN_REQ:
      // Acquire transaction object
      p.acquire();
      // Decode address
      dsock = decode_address(p.get_address(), masked_address);
      assert(dsock < isocket.size());
      p.set_address(masked_address);
      // Add routing extension
      if (!re) {
        re = new route_ext;
        accessor(p).set_extension(re);
      }
      re->usock = id;
      re->dsock = dsock;
      re->ds_completed = false;
      re->num = tcnt++;
      // Notify forward path PEQ
      fw_peq.notify(p, phase, d);
      break;

    case tlm::END_RESP:
      assert(re);
      assert(re->usock == id);
      assert(resp_in_progress[re->usock]);
      // Put the transaction into forward path PEQ with annotated delay
      // Schedule it for treatment
      fw_peq.notify(p, phase, d);
      break;

    case tlm::END_REQ:
    case tlm::BEGIN_RESP:
      debug(name() << " Illegal phase" << phase);
      SC_REPORT_FATAL(name(), " Illegal phase");
      break;

    default:
      debug(name() << " Illegal phase" << phase);
      SC_REPORT_FATAL(name(), " Illegal phase");
      break;
  }

  return tlm::TLM_ACCEPTED;
}

void at_bus::bw_peq_cb(tlm::tlm_generic_payload &p, const tlm::tlm_phase &phase)
{
  route_ext *re;
  accessor(p).get_extension(re);
  int usock = re->usock;
  int dsock = re->dsock;
  tlm::tlm_phase bw_phase = tlm::UNINITIALIZED_PHASE;
  tlm::tlm_phase fw_phase = tlm::UNINITIALIZED_PHASE;
  sc_core::sc_time d = sc_core::SC_ZERO_TIME;
  tlm::tlm_sync_enum s;
  tlm::tlm_generic_payload *pl = nullptr;

  switch (phase) {
    case tlm::END_REQ:
      // There must be a request in progress for the downstream socket
      assert(req_in_progress[dsock]);
      // Request exclusion rule:
      // END_REQ indicates that the downstream component is ready to accept
      // another BEGIN_REQ

      // Request channel is free
      // Are there pending requests for the downstream socket?
      if (pending_req[dsock].empty()) {
        req_in_progress[dsock] = nullptr;
      } else {
        // Get the next pending request from the queue
        pl = pending_req[dsock].front();
        pending_req[dsock].pop();
        req_in_progress[dsock] = pl;
        fw_phase = tlm::BEGIN_REQ;
        d = sc_core::SC_ZERO_TIME;
        // Schedule transmission
        fw_peq.notify(*pl, fw_phase, d);
      }

      // Send END_REQ upstream
      bw_phase = tlm::END_REQ;
      d = sc_core::SC_ZERO_TIME;
      s = tsocket[usock]->nb_transport_bw(p, bw_phase, d);
      assert(s == tlm::TLM_ACCEPTED);
      break;

    case tlm::BEGIN_RESP:
      if (&p == req_in_progress[dsock]) {
        // Request exclusion rule:
        // BEGIN_RESP for the immediately preceding transaction (the exact
        // request that is pending) indicates that the downstream component is
        // ready to accept another BEGIN_REQ
        // END_REQ was omitted by the downstream component
        req_in_progress[dsock] = nullptr;
      }

      if (!resp_in_progress[usock]) {
        // Upstream socket is idle
        // Mark it as busy
        resp_in_progress[usock] = &p;
        // Send BEGIN_RESP upstream
        // After that, for this transaction, no further communication in the
        // upstream direction will happen
        bw_phase = tlm::BEGIN_RESP;
        d = sc_core::SC_ZERO_TIME;
        s = tsocket[usock]->nb_transport_bw(p, bw_phase, d);
        switch (s) {
          case tlm::TLM_ACCEPTED:
            // Forward path is being used
            // Nothing to do, but expect an incoming call to nb_transport_fw
            // with END_RESP
            break;
          case tlm::TLM_UPDATED:
            // Return path is being used
            // The upstream component may have updated phase and delay
            // Honor the timing annotation
            fw_peq.notify(p, phase, d);
            break;
          case tlm::TLM_COMPLETED:
            // Return path is being used
            // Early completion
            // The upstream component has terminated the transaction
            inspect(p);
            // Response exclusion rule:
            // TLM_COMPLETED from upstream carries an implicit END_RESP
            resp_in_progress[usock] = nullptr;
            // TLM_ACCEPTED was returned in nb_transport_bw()
            // Downstream component may be waiting for forward path
            // communication (END_RESP) in case it hasn't completed the
            // transaction
            fw_phase = tlm::END_RESP;
            // Honor timing annotation
            fw_peq.notify(p, fw_phase, d);
            break;
          default:
            debug(name() << " Invalid status");
            SC_REPORT_FATAL(name(), " Invalid status");
            break;
        }
      } else {
        // Response exclusion rule:
        // Upstream socket is busy
        // Queue response to be treated when the next END_RESP (explicit or
        // implicit) happens
        pending_resp[usock].push(&p);
      }
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

tlm::tlm_sync_enum at_bus::nb_transport_bw(int id, tlm::tlm_generic_payload &p, tlm::tlm_phase &phase, sc_core::sc_time &d)
{
  // Check integrity of the routing extension
  route_ext *re;
  accessor(p).get_extension(re);
  assert(re);
  assert(re->dsock == id);

  switch (phase) {
    case tlm::END_REQ:
    case tlm::BEGIN_RESP:
      // Put the transaction into backward path PEQ with annotated delay
      // Schedule it for treatment
      bw_peq.notify(p, phase, d);
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

  return tlm::TLM_ACCEPTED;
}

unsigned int at_bus::transport_dbg(int id, tlm::tlm_generic_payload &p)
{
  sc_dt::uint64 masked_address;
  int dsock = decode_address(p.get_address(), masked_address);
  assert(dsock < isocket.size());
  p.set_address(masked_address);
  return isocket[dsock]->transport_dbg(p);
}

} // namespace ems

#endif /* __EMS_AT_BUS_H__ */

