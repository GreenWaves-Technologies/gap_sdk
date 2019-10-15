/* vim: set ts=2 sw=2 expandtab: */
/*
 * Copyright (C) 2018 TU Kaiserslautern
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

#ifndef __EMS_TARGET_H__
#define __EMS_TARGET_H__

#include <sys/mman.h>

#include <tlm.h>
#include <tlm_utils/simple_target_socket.h>

#define GIGA_BYTE   (1 * 1024 * 1024 * 1024)
#define MEM_SIZE    (1 * (GIGA_BYTE))
// When defined calloc() is used, mmap() otherwise.
#define EMS_TARGET_USE_CALLOC

namespace ems {
// Module able to buffer a second request before sending a response to the
// first
class at_target : sc_core::sc_module
{
public:
  tlm_utils::simple_target_socket<at_target> tsocket;

  SC_HAS_PROCESS(at_target);
  at_target(sc_core::sc_module_name name, double accept_delay_ps,
            double internal_latency_ps, uint32_t bpa) :
    sc_core::sc_module(name),
    tsocket("tsocket"),
    req_in_progress(nullptr),
    resp_in_progress(false),
    next_response_pending(nullptr),
    end_req_pending(nullptr),
    bytes_per_access(bpa),
    peq(this, &at_target::peq_cb)
  {
    accept_delay = sc_core::sc_time(accept_delay_ps, SC_PS);
    internal_latency = sc_core::sc_time(internal_latency_ps, SC_PS);
    tsocket.register_nb_transport_fw(this, &at_target::nb_transport_fw);
    tsocket.register_transport_dbg(this, &at_target::transport_dbg);

#ifdef EMS_TARGET_USE_CALLOC
    mem = reinterpret_cast<unsigned char*>(calloc(MEM_SIZE, sizeof(char)));
    if (!mem) {
      debug(this->name() << " calloc() failed");
      sc_core::sc_stop();
    }
#else
    mem = reinterpret_cast<unsigned char*>(mmap(0, MEM_SIZE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0));
    if (mem == MAP_FAILED) {
      debug(this->name() << " mmap() failed");
      sc_core::sc_stop();
    }
#endif /* EMS_TARGET_USE_CALLOC */

    SC_METHOD(execute_process);
    sensitive << target_done_event;
    dont_initialize();
  }

  ~at_target()
  {
    if (mem) {
#ifdef EMS_TARGET_USE_CALLOC
      free(mem);
#else
      munmap(mem, MEM_SIZE);
#endif /* EMS_TARGET_USE_CALLOC */
    }
  }

  // Module interface
  tlm::tlm_sync_enum nb_transport_fw(tlm::tlm_generic_payload &p, tlm::tlm_phase &phase, sc_core::sc_time &delay);
  unsigned int transport_dbg(tlm::tlm_generic_payload &p);

private:
  unsigned int execute(tlm::tlm_generic_payload *p);
  void execute_process();
  void send_begin_resp(tlm::tlm_generic_payload &p);
  void send_end_req(tlm::tlm_generic_payload &p);

  // Payload event queue (PEQ)
  tlm_utils::peq_with_cb_and_phase<at_target> peq;
  void peq_cb(tlm::tlm_generic_payload &p, const tlm::tlm_phase &phase);

  sc_core::sc_time accept_delay;
  sc_core::sc_time internal_latency;
  tlm::tlm_generic_payload *req_in_progress;
  bool resp_in_progress;
  sc_event target_done_event;
  tlm::tlm_generic_payload *next_response_pending;
  tlm::tlm_generic_payload *end_req_pending;
  uint32_t bytes_per_access;
  unsigned char *mem;
};

void at_target::send_end_req(tlm::tlm_generic_payload &p)
{
  tlm::tlm_phase bw_phase = tlm::END_REQ;
  // Request accept delay (END_REQ delay)
  sc_core::sc_time d = accept_delay;
  // Upstream component cannot terminate transaction at this point
  tlm::tlm_sync_enum s = tsocket->nb_transport_bw(p, bw_phase, d);
  assert(s == tlm::TLM_ACCEPTED);
  // Schedule internal event to send BEGIN_RESP after internal_latency
  target_done_event.notify(internal_latency);
  assert(req_in_progress == nullptr);
  req_in_progress = &p;
}

unsigned int at_target::execute(tlm::tlm_generic_payload *p)
{
  tlm::tlm_command cmd = p->get_command();
  sc_dt::uint64 addr = p->get_address();
  unsigned char *dptr = p->get_data_ptr();
  unsigned int dlen = p->get_data_length();
  unsigned char *ben = p->get_byte_enable_ptr();
  unsigned int swidth = p->get_streaming_width();

#if !defined(__VP_USE_SYSTEMC_GEM5)
  if (addr >= sc_dt::uint64(MEM_SIZE) || (addr % bytes_per_access)) {
#else
  if (addr >= sc_dt::uint64(MEM_SIZE)) {
#endif /* !defined(__VP_USE_SYSTEMC_GEM5) */
    debug(name() << " Address error p: " << p << " addr: 0x" << std::setfill('0') << std::setw(16) << std::hex << addr << std::dec << " cmd:" << (cmd ? " write" : " read") << " bytes_per_access: " << bytes_per_access);
    p->set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
    return 0;
  }

#if !defined(__VP_USE_SYSTEMC_GEM5)
  if (dlen != bytes_per_access || swidth < dlen) {
    debug(name() << " Burst error p: " << p << " addr: 0x" << std::setfill('0') << std::setw(16) << std::hex << addr << std::dec << " cmd:" << (cmd ? " write" : " read") << " dlen: " << dlen << " bytes_per_access: " << bytes_per_access << " swidth: " << swidth);
    p->set_response_status(tlm::TLM_BURST_ERROR_RESPONSE);
    return 0;
  }
#endif /* !defined(__VP_USE_SYSTEMC_GEM5) */

  if (ben != nullptr) {
    debug(name() << " Byte enable error p: " << p << " addr: 0x" << std::setfill('0') << std::setw(16) << std::hex << addr << std::dec << " cmd:" << (cmd ? " write" : " read"));
    p->set_response_status(tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE);
    return 0;
  }

  switch (cmd) {
    case tlm::TLM_READ_COMMAND:
      memcpy(dptr, mem, dlen);
      //debug(name() << " READ addr: 0x" << std::setfill('0') << std::setw(16) << std::hex << addr << std::dec << " dlen: " << dlen);
      break;
    case tlm::TLM_WRITE_COMMAND:
      memcpy(mem, dptr, dlen);
      //debug(name() << " WRITE addr: 0x" << std::setfill('0') << std::setw(16) << std::hex << addr << std::dec << " dlen: " << dlen);
      break;
    default:
      debug(name() << " Invalid command")
      SC_REPORT_FATAL(name(), "Invalid command");
      break;
  }

  p->set_response_status(tlm::TLM_OK_RESPONSE);
  return dlen;
}

// Method process sensitive to target_done_event
void at_target::execute_process()
{
  // Execute the read or write commands
  execute(req_in_progress);

  // at_target must honor BEGIN_RESP/END_RESP exclusion rule i.e.
  // must not send BEGIN_RESP until receiving previous END_RESP
  // or BEGIN_REQ
  if (resp_in_progress) {
    // at_target allows only two transactions in-flight
    if (next_response_pending) {
      SC_REPORT_FATAL(name(), "Too many pending responses");
    }
    next_response_pending = req_in_progress;
  } else {
    send_begin_resp(*req_in_progress);
  }
}

void at_target::send_begin_resp(tlm::tlm_generic_payload &p)
{
  tlm::tlm_sync_enum status;
  tlm::tlm_phase phase;
  sc_core::sc_time delay;

  resp_in_progress = true;

  // It is the responsibility of the upstream component to calculate the
  // response accept delay and to send END_RESP back downstream when it is
  // ready to receive the next transfer.
  // It would be natural for the upstream component to delay the END_RESP
  // until the end of the final beat of the data transfer, but it is not
  // obliged to do so.
  phase = tlm::BEGIN_RESP;
  delay = sc_core::SC_ZERO_TIME;
  status = tsocket->nb_transport_bw(p, phase, delay);

  switch (status) {
    case tlm::TLM_UPDATED:
      // Return path is being used phase and delay updated by the callee
      // Honor timing annotation
      peq.notify(p, phase, delay);
      break;
    case tlm::TLM_COMPLETED:
      // Return path is being used
      // Transaction terminated by initiator
      req_in_progress = nullptr;
      resp_in_progress = false;
      break;
    default:
      break;
  }
  p.release();
}

// TLM-2 non-blocking transport method
tlm::tlm_sync_enum at_target::nb_transport_fw(tlm::tlm_generic_payload &p, tlm::tlm_phase &phase, sc_core::sc_time &delay)
{
  if (phase == tlm::BEGIN_REQ) {
    p.acquire();
  }
  // Enqueue the transaction until the annotated time has elapsed
  peq.notify(p, phase, delay);
  return tlm::TLM_ACCEPTED;
}

void at_target::peq_cb(tlm::tlm_generic_payload &p, const tlm::tlm_phase &phase)
{
  switch (phase) {
    case tlm::BEGIN_REQ:
      if (!req_in_progress) {
        send_end_req(p);
      } else {
        // Put back-pressure on initiator by deferring END_REQ until pipeline
        // is clear
        end_req_pending = &p;
      }
      break;
    case tlm::END_RESP:
      // On receiving END_RESP, the target can release the transaction and
      // allow other pending transactions to proceed
      if (!resp_in_progress) {
        debug(name() << " Illegal phase END_RESP received by target")
        SC_REPORT_FATAL(name(), " Illegal phase END_RESP received by target");
      }

      req_in_progress = nullptr;
      // at_target itself is now clear to issue the next BEGIN_RESP
      resp_in_progress = false;

      if (next_response_pending) {
        send_begin_resp(*next_response_pending);
        next_response_pending = nullptr;
      }

      // and to unblock the initiator by issuing END_REQ
      if (end_req_pending) {
        send_end_req(*end_req_pending);
        end_req_pending = nullptr;
      }
      break;
    case tlm::END_REQ:
    case tlm::BEGIN_RESP:
      debug(name() << " Illegal phase received by target")
      SC_REPORT_FATAL(name(), " Illegal phase received by target");
      break;
    default:
      debug(name() << " Illegal phase received by target")
      SC_REPORT_FATAL(name(), " Illegal phase received by target");
      break;
  }
}

unsigned int at_target::transport_dbg(tlm::tlm_generic_payload &p)
{
  return execute(&p);
}

} // namespace ems

#endif /* __EMS_TARGET_H__ */

