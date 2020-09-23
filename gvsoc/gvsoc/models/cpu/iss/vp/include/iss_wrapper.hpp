/*
 * Copyright (C) 2020 GreenWaves Technologies, SAS, ETH Zurich and
 *                    University of Bologna
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

#ifndef __CPU_ISS_ISS_WRAPPER_HPP
#define __CPU_ISS_ISS_WRAPPER_HPP

#include <vp/vp.hpp>
#include <vp/itf/io.hpp>
#include <vp/itf/wire.hpp>

#ifdef USE_TRDB
#define HAVE_DECL_BASENAME 1
#include "trace_debugger.h"
#endif

class iss_wrapper : public vp::component
{

public:

  iss_wrapper(js::config *config);

  int build();
  void start();
  void pre_reset();
  void reset(bool active);

  static void data_grant(void *_this, vp::io_req *req);
  static void data_response(void *_this, vp::io_req *req);

  static void fetch_grant(void *_this, vp::io_req *req);
  static void fetch_response(void *_this, vp::io_req *req);

  static void exec_instr(void *__this, vp::clock_event *event);
  static void exec_first_instr(void *__this, vp::clock_event *event);
  void exec_first_instr(vp::clock_event *event);
  static void exec_instr_check_all(void *__this, vp::clock_event *event);
  static inline void exec_misaligned(void *__this, vp::clock_event *event);

  static void irq_req_sync(void *__this, int irq);
  void debug_req();

  inline int data_req(iss_addr_t addr, uint8_t *data, int size, bool is_write);
  inline int data_req_aligned(iss_addr_t addr, uint8_t *data_ptr, int size, bool is_write);
  int data_misaligned_req(iss_addr_t addr, uint8_t *data_ptr, int size, bool is_write);

  bool user_access(iss_addr_t addr, uint8_t *data, iss_addr_t size, bool is_write);
  std::string read_user_string(iss_addr_t addr, int len=-1);

  static vp::io_req_status_e dbg_unit_req(void *__this, vp::io_req *req);

  void irq_check();
  void wait_for_interrupt();
  
  void set_halt_mode(bool halted, int cause);
  void check_state();

  void handle_ebreak();
  void handle_riscv_ebreak();

  void dump_debug_traces();

  inline void trigger_check_all() { current_event = check_all_event; }

  void insn_trace_callback();

  vp::io_master data;
  vp::io_master fetch;
  vp::io_slave  dbg_unit;

  vp::wire_slave<int>      irq_req_itf;
  vp::wire_master<int>     irq_ack_itf;

  vp::wire_master<uint32_t> ext_counter[32];

  vp::io_req     io_req;
  vp::io_req     fetch_req;

  iss_cpu_t cpu;

  vp::trace     trace;
  vp::trace     decode_trace;
  vp::trace     insn_trace;
  vp::trace     csr_trace;
  vp::trace     perf_counter_trace;

  vp::reg_32    bootaddr_reg;
  vp::reg_1     fetch_enable_reg;
  vp::reg_1     is_active_reg;
  vp::reg_1     stalled;
  vp::reg_1     wfi;
  vp::reg_1     misaligned_access;
  vp::reg_1     halted;
  vp::reg_1     step_mode;
  vp::reg_1     do_step;

  vp::power_trace power_trace;

  vp::power_source insn_power;
  vp::power_source clock_gated_power;
  vp::power_source leakage_power;

  vp::trace     state_event;
  vp::trace     pc_trace_event;
  vp::trace     func_trace_event;
  vp::trace     inline_trace_event;
  vp::trace     line_trace_event;
  vp::trace     file_trace_event;
  vp::trace     pcer_trace_event[32];
  vp::trace     insn_trace_event;
  vp::trace     misaligned_req_event;

  static void ipc_stat_handler(void *__this, vp::clock_event *event);
  void gen_ipc_stat(bool pulse=false);
  void trigger_ipc_stat();
  void stop_ipc_stat();
  int ipc_stat_nb_insn;
  vp::trace     ipc_stat_event;
  vp::clock_event *ipc_clock_event;
  int ipc_stat_delay;
  
#ifdef USE_TRDB
  trdb_ctx *trdb;
  struct list_head trdb_packet_list;
  uint8_t trdb_pending_word[16];
#endif

private:

  vp::clock_event *current_event;
  vp::clock_event *instr_event;
  vp::clock_event *check_all_event;
  vp::clock_event *misaligned_event;

  int irq_req;

  bool iss_opened;
  int halt_cause;
  int64_t wakeup_latency;
  int bootaddr_offset;
  iss_reg_t hit_reg = 0;
  bool riscv_dbg_unit;

  iss_reg_t ppc;
  iss_reg_t npc;

  int        misaligned_size;
  uint8_t   *misaligned_data;
  iss_addr_t misaligned_addr;
  bool       misaligned_is_write;
  int64_t    misaligned_latency;

  vp::wire_slave<uint32_t> bootaddr_itf;
  vp::wire_slave<bool>     clock_itf;
  vp::wire_slave<bool>     fetchen_itf;
  vp::wire_slave<bool>     flush_cache_itf;
  vp::wire_slave<bool>     halt_itf;
  vp::wire_master<bool>     halt_status_itf;

  bool clock_active;

  static void clock_sync(void *_this, bool active);
  static void bootaddr_sync(void *_this, uint32_t value);
  static void fetchen_sync(void *_this, bool active);
  static void flush_cache_sync(void *_this, bool active);
  static void halt_sync(void *_this, bool active);
  inline void enqueue_next_instr(int64_t cycles);
  void halt_core();
};
\
inline void iss_wrapper::enqueue_next_instr(int64_t cycles)
{
  if (is_active_reg.get())
  {
    trace.msg("Enqueue next instruction (cycles: %ld)\n", cycles);
    event_enqueue(current_event, cycles);
  }
}

void iss_wrapper::exec_misaligned(void *__this, vp::clock_event *event)
{
  iss_wrapper *_this = (iss_wrapper *)__this;

  iss_exec_insn_resume(_this);

  // As the 2 load accesses for misaligned access are generated by the
  // wrapper, we need to account the extra access here.
  _this->cpu.state.insn_cycles++;
  iss_pccr_account_event(_this, CSR_PCER_LD, 1);

  if (_this->data_req_aligned(_this->misaligned_addr, _this->misaligned_data,
    _this->misaligned_size, _this->misaligned_is_write) == vp::IO_REQ_OK)
  {
    iss_exec_insn_terminate(_this);
    _this->misaligned_access.set(false);
    _this->enqueue_next_instr(_this->io_req.get_latency() + 1);
  }
  else
  {
    _this->trace.warning("UNIMPLEMENTED AT %s %d\n", __FILE__, __LINE__);
  }
}

inline int iss_wrapper::data_req_aligned(iss_addr_t addr, uint8_t *data_ptr, int size, bool is_write)
{
  decode_trace.msg("Data request (addr: 0x%lx, size: 0x%x, is_write: %d)\n", addr, size, is_write);
  vp::io_req *req = &io_req;
  req->init();
  req->set_addr(addr);
  req->set_size(size);
  req->set_is_write(is_write);
  req->set_data(data_ptr);
  int err = data.req(req);
  if (err == vp::IO_REQ_OK) 
  {
    this->cpu.state.insn_cycles += req->get_latency();
  }
  else if (err == vp::IO_REQ_INVALID) 
  {
    vp_warning_always(&this->warning, "Invalid access (pc: 0x%x, offset: 0x%x, size: 0x%x, is_write: %d)\n", this->cpu.current_insn->addr, addr, size, is_write);
  }
  return err;
}

#define ADDR_MASK (~(ISS_REG_WIDTH/8 - 1))

inline int iss_wrapper::data_req(iss_addr_t addr, uint8_t *data_ptr, int size, bool is_write)
{

  iss_addr_t addr0 = addr & ADDR_MASK;
  iss_addr_t addr1 = (addr + size - 1) & ADDR_MASK;

  if (likely(addr0 == addr1))
    return data_req_aligned(addr, data_ptr, size, is_write);
  else
    return data_misaligned_req(addr, data_ptr, size, is_write);
}

#endif
