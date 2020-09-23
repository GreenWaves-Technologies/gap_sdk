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

#include <vp/vp.hpp>
#include <vp/itf/io.hpp>
#include "archi/gvsoc/gvsoc.h"
#include "iss.hpp"
#include <algorithm>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#ifndef O_BINARY
# define O_BINARY 0
#endif

#define HALT_CAUSE_EBREAK    0
#define HALT_CAUSE_ECALL     1
#define HALT_CAUSE_ILLEGAL   2
#define HALT_CAUSE_INVALID   3
#define HALT_CAUSE_INTERRUPT 4 
#define HALT_CAUSE_HALT      15
#define HALT_CAUSE_STEP      15

#ifdef USE_TRDB

#define trdb_get_packet(ptr,member) \
  ((struct tr_packet *)(((char *)(ptr)) - ((size_t) &(((struct tr_packet *)0)->member))))


static inline void trdb_record_instruction(iss_wrapper *_this, iss_insn_t *insn)
{
  struct tr_instr instr;
  instr.valid = true;
  instr.exception = false;
  instr.iaddr = insn->addr;
  instr.instr = insn->opcode;
  instr.compressed = insn->size == 2;
  
  if (trdb_compress_trace_step(_this->trdb, &_this->trdb_packet_list, &instr))
  {
    struct tr_packet *packet = trdb_get_packet(_this->trdb_packet_list.next, list);
    size_t nb_bits = 0;
    int alignment = 0;
    trdb_serialize_packet(_this->trdb, packet, &nb_bits, alignment, _this->trdb_pending_word);
    ////printf("Got nb bits %ld %lx\n", nb_bits, (*(uint64_t *)_this->trdb_pending_word) & ((1<<nb_bits)-1));
    trdb_free_packet_list(&_this->trdb_packet_list);
    INIT_LIST_HEAD(&_this->trdb_packet_list);
  }
}

#else

#define trdb_record_instruction

#endif


#define EXEC_INSTR_COMMON(_this, event, func) \
do { \
  \
  _this->trace.msg("Executing instruction\n"); \
  if (_this->pc_trace_event.get_event_active()) \
  { \
    _this->pc_trace_event.event((uint8_t *)&_this->cpu.current_insn->addr); \
  } \
  if (_this->func_trace_event.get_event_active() || _this->inline_trace_event.get_event_active() || _this->file_trace_event.get_event_active() || _this->line_trace_event.get_event_active()) \
  { \
    _this->dump_debug_traces(); \
  } \
  if (_this->ipc_stat_event.get_event_active()) \
  { \
    _this->ipc_stat_nb_insn++; \
  } \
  if (_this->power_trace.get_active()) \
  { \
  _this->insn_power.account_event(); \
 } \
 \
  iss_insn_t *insn = _this->cpu.current_insn; \
  int cycles = func(_this); \
  trdb_record_instruction(_this, insn); \
  if (cycles >= 0) \
  { \
    _this->enqueue_next_instr(cycles); \
  } \
  else \
  { \
    if (_this->misaligned_access.get()) \
    { \
      _this->event_enqueue(_this->misaligned_event, _this->misaligned_latency); \
    } \
    else \
    { \
      _this->is_active_reg.set(false); \
      _this->stalled.set(true);     \
    } \
  } \
} while(0)

void iss_wrapper::dump_debug_traces()
{
  const char *func, *inline_func, *file;
  int line;

  if (!iss_trace_pc_info(this->cpu.current_insn->addr, &func, &inline_func, &file, &line))
  {
    this->func_trace_event.event_string(func);
    this->inline_trace_event.event_string(inline_func);
    this->file_trace_event.event_string(file);
    this->line_trace_event.event((uint8_t *)&line);
  }
}

void iss_wrapper::exec_instr(void *__this, vp::clock_event *event)
{
  iss_t *_this = (iss_t *)__this;

  EXEC_INSTR_COMMON(_this, event, iss_exec_step_nofetch);
}

void iss_wrapper::exec_instr_check_all(void *__this, vp::clock_event *event)
{
  iss_t *_this = (iss_t *)__this;

  // Switch back to optimize instruction handler only
  // if HW counters are disabled as they are checked with the slow handler
  if (iss_exec_switch_to_fast(_this))
  {
    _this->current_event = _this->instr_event;
  }

  EXEC_INSTR_COMMON(_this, event, iss_exec_step_nofetch_perf);
  if (_this->step_mode.get())
  {
    _this->do_step.set(false);
    _this->hit_reg |= 1;
    _this->set_halt_mode(true, HALT_CAUSE_HALT);
    _this->check_state();
  }
}

void iss_wrapper::exec_first_instr(vp::clock_event *event)
{
  current_event = event_new(iss_wrapper::exec_instr);
  iss_start(this);
  exec_instr((void *)this, event);
}

void iss_wrapper::exec_first_instr(void *__this, vp::clock_event *event)
{
  iss_t *_this = (iss_t *)__this;
  _this->exec_first_instr(event);
}

void iss_wrapper::data_grant(void *__this, vp::io_req *req)
{
}

void iss_wrapper::data_response(void *__this, vp::io_req *req)
{
  iss_t *_this = (iss_t *)__this;
  _this->stalled.set(false);
  _this->wakeup_latency = req->get_latency();
  if (_this->misaligned_access.get())
  {
    _this->misaligned_access.set(false);
  }
  else
  {
    // First call the ISS to finish the instruction
    _this->cpu.state.stall_callback(_this);
    iss_exec_insn_resume(_this);
    iss_exec_insn_terminate(_this);
  }
  _this->check_state();
}

void iss_wrapper::fetch_grant(void *_this, vp::io_req *req)
{

}

void iss_wrapper::fetch_response(void *_this, vp::io_req *req)
{

}

void iss_wrapper::bootaddr_sync(void *__this, uint32_t value)
{
  iss_t *_this = (iss_t *)__this;
  _this->trace.msg("Setting boot address (value: 0x%x)\n", value);
  _this->bootaddr_reg.set(value);
  iss_irq_set_vector_table(_this, _this->bootaddr_reg.get() & ~((1<<8) - 1));
}

void iss_wrapper::gen_ipc_stat(bool pulse)
{
  if (!pulse)
    this->ipc_stat_event.event_real((float)this->ipc_stat_nb_insn / 100);
  else
    this->ipc_stat_event.event_real_pulse(this->get_period(), (float)this->ipc_stat_nb_insn / 100, 0);

  this->ipc_stat_nb_insn = 0;
  if (this->ipc_stat_delay == 10)
    this->ipc_stat_delay = 30;
  else
    this->ipc_stat_delay = 100;
}

void iss_wrapper::trigger_ipc_stat()
{
  // In case the core is resuming execution, set IPC to 1 as we are executing
  // first instruction to not have the signal to zero.
  if (this->ipc_stat_delay == 10)
  {
    this->ipc_stat_event.event_real(1.0);
  }

  if (this->ipc_stat_event.get_event_active() && !this->ipc_clock_event->is_enqueued() && this->is_active_reg.get() && this->clock_active)
  {
    this->event_enqueue(this->ipc_clock_event, this->ipc_stat_delay);
  }
}

void iss_wrapper::stop_ipc_stat()
{
  this->gen_ipc_stat(true);
  if (this->ipc_clock_event->is_enqueued())
  {
    this->event_cancel(this->ipc_clock_event);
  }
  this->ipc_stat_delay = 10;
}

void iss_wrapper::ipc_stat_handler(void *__this, vp::clock_event *event)
{
  iss_t *_this = (iss_t *)__this;
  _this->gen_ipc_stat();
  _this->trigger_ipc_stat();
}

void iss_wrapper::clock_sync(void *__this, bool active)
{
  iss_t *_this = (iss_t *)__this;
  _this->trace.msg("Setting clock (active: %d)\n", active);

  _this->clock_active = active;

  // TODO this could be better handler is the clock would be taken into
  // account in the core state machine
  uint8_t value = active && _this->is_active_reg.get();
  if (value)
    _this->state_event.event((uint8_t *)&value);
  else
    _this->state_event.event(NULL);

  if (_this->ipc_stat_event.get_event_active())
  {
    if (value)
    {
      _this->trigger_ipc_stat();
    }
    else
    {
      _this->stop_ipc_stat();
    }
  }
}

void iss_wrapper::fetchen_sync(void *__this, bool active)
{
  iss_t *_this = (iss_t *)__this;
  _this->trace.msg("Setting fetch enable (active: %d)\n", active);
  int old_val = _this->fetch_enable_reg.get();
  _this->fetch_enable_reg.set(active);
  if (!old_val && active)
  {
    iss_pc_set(_this, _this->bootaddr_reg.get() + _this->bootaddr_offset);
  }
  _this->check_state();
}

void iss_wrapper::flush_cache_sync(void *__this, bool active)
{
  iss_t *_this = (iss_t *)__this;
  if (_this->iss_opened)
  {
    iss_cache_flush(_this);
  }
}


void iss_wrapper::set_halt_mode(bool halted, int cause)
{
  if (this->riscv_dbg_unit)
  {
    if (!this->cpu.state.debug_mode)
    {
      this->debug_req();
    }
  }
  else
  {
    this->halt_cause = cause;

    if (this->halted.get() && !halted)
    {
      this->get_clock()->release();
    }
    else if (!this->halted.get() && halted)
    {
      this->get_clock()->retain();
    }

    this->halted.set(halted);

    if (this->halt_status_itf.is_bound()) 
      this->halt_status_itf.sync(this->halted.get());
  }
}



void iss_wrapper::halt_core()
{
  this->trace.msg("Halting core\n");

  if (this->cpu.prev_insn == NULL)
    this->ppc = 0;
  else
    this->ppc = this->cpu.prev_insn->addr;
  this->npc = this->cpu.current_insn->addr;
}



void iss_wrapper::halt_sync(void *__this, bool halted)
{
  iss_t *_this = (iss_t *)__this;
  _this->trace.msg("Received halt signal sync (halted: 0x%d)\n", halted);
  _this->set_halt_mode(halted, HALT_CAUSE_HALT);

  _this->check_state();
}


void iss_wrapper::check_state()
{
  vp::clock_event *event = current_event;

  current_event = check_all_event;

  if (!is_active_reg.get())
  {
    if (!halted.get() && fetch_enable_reg.get() && !stalled.get() && (!wfi.get() || irq_req != -1 || this->cpu.state.debug_mode))
    {
      wfi.set(false);
      is_active_reg.set(true);
      uint8_t one = 1;
      this->state_event.event(&one);
      if (this->ipc_stat_event.get_event_active())
        this->trigger_ipc_stat();

      if (step_mode.get())
        do_step.set(true);
      enqueue_next_instr(1 + this->wakeup_latency);

      if (this->cpu.csr.pcmr & CSR_PCMR_ACTIVE && this->cpu.csr.pcer & (1<<CSR_PCER_CYCLES))
      {
        this->cpu.csr.pccr[CSR_PCER_CYCLES] += 1 + this->wakeup_latency;
      }

      this->wakeup_latency = 0;
    }
  }
  else
  {
    uint8_t zero = 0;
    if (halted.get() && !do_step.get())
    {
      is_active_reg.set(false);
      this->state_event.event(&zero);
      if (this->ipc_stat_event.get_event_active())
        this->stop_ipc_stat();
      this->halt_core();
    }
    else if (wfi.get())
    {
      if (irq_req == -1)
      {
        is_active_reg.set(false);
        this->state_event.event(&zero);
        if (this->ipc_stat_event.get_event_active())
          this->stop_ipc_stat();
      }
      else
        wfi.set(false);
    }

    if (!is_active_reg.get())
    {
      if (event->is_enqueued())
      {
        event_cancel(event);
      }
    }
  }
}

int iss_wrapper::data_misaligned_req(iss_addr_t addr, uint8_t *data_ptr, int size, bool is_write)
{

  iss_addr_t addr0 = addr & ADDR_MASK;
  iss_addr_t addr1 = (addr + size - 1) & ADDR_MASK;

  decode_trace.msg("Misaligned data request (addr: 0x%lx, size: 0x%x, is_write: %d)\n", addr, size, is_write);

  static uint8_t one = 1, zero = 0;
  this->misaligned_req_event.event_pulse(this->get_period(), &one, &zero);

  // The access is a misaligned access
  // Change the event so that we can do the first access now and the next access
  // during the next cycle
  int size0 = addr1 - addr;
  int size1 = size - size0;
  
  misaligned_access.set(true);

  // Remember the access properties for the second access
  misaligned_size = size1;
  misaligned_data = data_ptr + size0;
  misaligned_addr = addr1;
  misaligned_is_write = is_write;

  // And do the first one now
  int err = data_req_aligned(addr, data_ptr, size0, is_write);
  if (err == vp::IO_REQ_OK)
  {
    // As the transaction is split into 2 parts, we must tell the ISS
    // that the access is pending as the instruction must be executed
    // only when the second access is finished.
    misaligned_latency = io_req.get_latency() + 1;
    return vp::IO_REQ_PENDING;
  }
  else
  {
    trace.force_warning("UNIMPLEMENTED AT %s %d\n", __FILE__, __LINE__);
    return 0;
  }
}

void iss_wrapper::irq_check()
{
  current_event = check_all_event;
}


void iss_wrapper::wait_for_interrupt()
{
  wfi.set(true);
  check_state();
}


void iss_wrapper::irq_req_sync(void *__this, int irq)
{
  iss_t *_this = (iss_t *)__this;
  _this->irq_req = irq;
  _this->irq_check();
  iss_irq_req(_this, irq);
  _this->wfi.set(false);
  _this->check_state();
}

void iss_wrapper::debug_req()
{
  this->cpu.irq.req_debug = true;
  this->irq_check();
  this->wfi.set(false);
  this->check_state();
}

bool iss_wrapper::user_access(iss_addr_t addr, uint8_t *buffer, iss_addr_t size, bool is_write)
{
  vp::io_req *req = &io_req;
  req->init();
  req->set_debug(true);
  req->set_addr(addr);
  req->set_size(size);
  req->set_is_write(is_write);
  req->set_data(buffer);
  int err = data.req(req);
  if (err != vp::IO_REQ_OK) 
  {
    if (err == vp::IO_REQ_INVALID)
      this->warning.fatal("Invalid IO response during debug request\n");
    else
      this->warning.fatal("Pending IO response during debug request\n");

    return true;
  }
  return false;
}

std::string iss_wrapper::read_user_string(iss_addr_t addr, int size)
{
  vp::io_req *req = &io_req;
  std::string str = "";
  while(size != 0)
  {
    uint8_t buffer;
    req->init();
    req->set_debug(true);
    req->set_addr(addr);
    req->set_size(1);
    req->set_is_write(false);
    req->set_data(&buffer);
    int err = data.req(req);
    if (err != vp::IO_REQ_OK) 
    {
      if (err == vp::IO_REQ_INVALID)
        return "";
      else
        this->warning.fatal("Pending IO response during debug request\n");
    }

    if (buffer == 0)
      return str;

    str += buffer;
    addr++;

    if (size > 0)
      size--;
  }

  return str;
}

static const int open_modeflags[12] = {
        O_RDONLY,
        O_RDONLY | O_BINARY,
        O_RDWR,
        O_RDWR | O_BINARY,
        O_WRONLY | O_CREAT | O_TRUNC,
        O_WRONLY | O_CREAT | O_TRUNC | O_BINARY,
        O_RDWR | O_CREAT | O_TRUNC,
        O_RDWR | O_CREAT | O_TRUNC | O_BINARY,
        O_WRONLY | O_CREAT | O_APPEND,
        O_WRONLY | O_CREAT | O_APPEND | O_BINARY,
        O_RDWR | O_CREAT | O_APPEND,
        O_RDWR | O_CREAT | O_APPEND | O_BINARY
};

void iss_wrapper::handle_riscv_ebreak()
{
  int id = this->cpu.regfile.regs[10];

  if (id == 0x4)
  {
    std::string path = this->read_user_string(this->cpu.regfile.regs[11]);
    printf("%s", path.c_str());
  }
  else if (id == 0x1)
  {
    iss_reg_t args[3];
    if (this->user_access(this->cpu.regfile.regs[11], (uint8_t *)args, sizeof(args), false))
    {
      this->cpu.regfile.regs[10] = -1;
      return;
    }
    std::string path = this->read_user_string(args[0], args[2]);


    unsigned int mode = args[1];

    this->cpu.regfile.regs[10] = open(path.c_str(), open_modeflags[mode], 0644);

    if (this->cpu.regfile.regs[10] == -1)
      this->warning.force_warning("Caught error during semi-hosted call (name: open, path: %s, mode: 0x%x, error: %s)\n", path.c_str(), mode, strerror(errno));


  }
  else if (id == 0x2)
  {
    this->cpu.regfile.regs[10] = close(this->cpu.regfile.regs[11]);
  }
  else if (id == 0x5)
  {
    iss_reg_t args[3];
    if (this->user_access(this->cpu.regfile.regs[11], (uint8_t *)args, sizeof(args), false))
    {
      this->cpu.regfile.regs[10] = -1;
      return;
    }

    uint8_t buffer[1024];
    int size = args[2];
    iss_reg_t addr = args[1];
    while(size)
    {
      int iter_size = 1024;
      if (size < 1024)
        iter_size = size;

      if (this->user_access(addr, buffer, iter_size, false))
      {
        this->cpu.regfile.regs[10] = -1;
        return;
      }

      if (write(args[0], (void *)(long)buffer, iter_size) != iter_size)
        break;

      size -= iter_size;
      addr += iter_size;
    }

    this->cpu.regfile.regs[10] = size;
  }
  else if (id == 0x6)
  {
    iss_reg_t args[3];
    if (this->user_access(this->cpu.regfile.regs[11], (uint8_t *)args, sizeof(args), false))
    {
      this->cpu.regfile.regs[10] = -1;
      return;
    }

    uint8_t buffer[1024];
    int size = args[2];
    iss_reg_t addr = args[1];
    while(size)
    {
      int iter_size = 1024;
      if (size < 1024)
        iter_size = size;

      int read_size = read(args[0], (void *)(long)buffer, iter_size);

      if (read_size <= 0)
      {
        if (read_size < 0)
        {
          this->cpu.regfile.regs[10] = -1;
          return;
        }
        else
        {
          break;
        }
      }

      if (this->user_access(addr, buffer, read_size, true))
      {
        this->cpu.regfile.regs[10] = -1;
        return;
      }

      size -= read_size;
      addr += read_size;
    }

    this->cpu.regfile.regs[10] = size;
  }
  else if (id == 0xA)
  {
    iss_reg_t args[2];
    if (this->user_access(this->cpu.regfile.regs[11], (uint8_t *)args, sizeof(args), false))
    {
      this->cpu.regfile.regs[10] = -1;
      return;
    }

    int pos = lseek(args[0], args[1], SEEK_SET);
    this->cpu.regfile.regs[10] = pos != args[1];
  }
  else if (id == 0x18)
  {
    if (this->cpu.regfile.regs[11] == 0x20026)
      exit(0);
    else
      exit(1);
  }
  else if (id == 0x0C)
  {
    struct stat buf;
    fstat(this->cpu.regfile.regs[11], &buf);
    this->cpu.regfile.regs[10] = buf.st_size;
  }
  else
  {
    this->warning.force_warning("Unknown ebreak call (id: %d)\n", id);
  }
}



void iss_wrapper::handle_ebreak()
{
  int id = this->cpu.regfile.regs[10];

  switch (id)
  {
    case GV_SEMIHOSTING_TRACE_OPEN: {
      int result = -1;
      std::string path = this->read_user_string(this->cpu.regfile.regs[11]);
      if (path == "")
      {
        this->warning.force_warning("Invalid user string while opening trace (addr: 0x%x)\n", this->cpu.regfile.regs[11]);
      }
      else
      {
        vp::trace *trace = this->traces.get_trace_manager()->get_trace_from_path(path);
        if (trace == NULL)
        {
          this->warning.force_warning("Invalid trace (path: %s)\n", path.c_str());
        }
        else
        {
          this->trace.msg("Opened trace (path: %s, id: %d)\n", path.c_str(), trace->id);
          result = trace->id;
        }
      }

      this->cpu.regfile.regs[10] = result;

      break;
    }
    
    case GV_SEMIHOSTING_TRACE_ENABLE: {
      int id = this->cpu.regfile.regs[11];
      vp::trace *trace = this->traces.get_trace_manager()->get_trace_from_id(id);
      if (trace == NULL)
      {
        this->warning.force_warning("Unknown trace ID while dumping trace (id: %d)\n", id);
      }
      else
      {
        trace->set_active(this->cpu.regfile.regs[12]);
      }

      break; 
    }
    
    case GV_SEMIHOSTING_VCD_CONFIGURE: {
      int enabled = this->cpu.regfile.regs[11];
      this->traces.get_trace_manager()->set_global_enable(enabled);
    }
    break;

    case GV_SEMIHOSTING_VCD_OPEN_TRACE: {
      int result = -1;
      std::string path = this->read_user_string(this->cpu.regfile.regs[11]);
      if (path == "")
      {
        this->warning.force_warning("Invalid user string while opening VCD trace (addr: 0x%x)\n", this->cpu.regfile.regs[11]);
      }
      else
      {
        vp::trace *trace = this->traces.get_trace_manager()->get_trace_from_path(path);
        if (trace == NULL)
        {
          this->warning.force_warning("Invalid VCD trace (path: %s)\n", path.c_str());
        }
        else
        {
          this->trace.msg("Opened VCD trace (path: %s, id: %d)\n", path.c_str(), trace->id);
          result = trace->id;
        }
      }

      this->cpu.regfile.regs[10] = result;

      break;
    }
    
    case GV_SEMIHOSTING_VCD_CONF_TRACE:
    break;
    
    case GV_SEMIHOSTING_VCD_DUMP_TRACE: {
      int id = this->cpu.regfile.regs[11];
      vp::trace *trace = this->traces.get_trace_manager()->get_trace_from_id(id);
      if (trace == NULL)
      {
        this->warning.force_warning("Unknown trace ID while dumping VCD trace (id: %d)\n", id);
      }
      else
      {
        if (trace->width > 32)
        {
          this->warning.force_warning("Trying to write to VCD trace whose width is bigger than 32 (id: %d)\n", id);
        }
        else
        {
          if (this->cpu.regfile.regs[12] == 1)
            trace->event(NULL);
          else
            trace->event((uint8_t *)&this->cpu.regfile.regs[14]);
        }
      }

      break; 
    }
    
    case GV_SEMIHOSTING_VCD_DUMP_TRACE_STRING: {
      int id = this->cpu.regfile.regs[11];
      vp::trace *trace = this->traces.get_trace_manager()->get_trace_from_id(id);
      if (trace == NULL)
      {
        this->warning.force_warning("Unknown trace ID while dumping VCD trace (id: %d)\n", id);
      }
      else
      {
        if (!trace->is_string)
        {
          this->warning.force_warning("Trying to write string to VCD trace which is not a string (id: %d)\n", id);
        }
        else
        {
          std::string path = this->read_user_string(this->cpu.regfile.regs[12]);

          trace->event_string(path);
        }
      }
      break;
    }
    
    default:
      this->warning.force_warning("Unknown ebreak call (id: %d)\n", id);
      break;
  }
}

vp::io_req_status_e iss_wrapper::dbg_unit_req(void *__this, vp::io_req *req)
{
  iss_t *_this = (iss_t *)__this;

  uint64_t offset = req->get_addr();
  uint8_t *data = req->get_data();
  uint64_t size = req->get_size();

  _this->trace.msg("IO access (offset: 0x%x, size: 0x%x, is_write: %d)\n", offset, size, req->get_is_write());

  if (size != ISS_REG_WIDTH/8)
    return vp::IO_REQ_INVALID;

  if (offset >= 0x4000)
  {
    offset -= 0x4000;

    if (size != 4) return vp::IO_REQ_INVALID;


    bool err;
    if (req->get_is_write())
      err = iss_csr_write(_this, offset / 4, *(iss_reg_t *)data);
    else
      err = iss_csr_read(_this, offset / 4, (iss_reg_t *)data);

    if (err) return vp::IO_REQ_INVALID;
  }
  else if (offset >= 0x2000)
  {
    if (!_this->halted.get())
    {
      _this->trace.force_warning("Trying to access debug registers while core is not halted\n");
      return vp::IO_REQ_INVALID;
    }

    if (offset == 0x2000)
    {
      if (req->get_is_write())
      {
        // Writing NPC will force the core to jump to the written PC
        // even if the core is sleeping
        iss_cache_flush(_this);
        _this->npc = *(iss_reg_t *)data;
        iss_pc_set(_this, _this->npc);
        _this->wfi.set(false);
        _this->check_state();
      }
      else
        *(iss_reg_t *)data = _this->npc;
    }
    else if (offset == 0x2004)
    {
      if (req->get_is_write())
        _this->trace.force_warning("UNIMPLEMENTED AT %s %d\n", __FILE__, __LINE__);
      else
        *(iss_reg_t *)data = _this->ppc;
    }
    else
    {
      return vp::IO_REQ_INVALID;
    }
  }
  else if (offset >= 0x400)
  {
    offset -= 0x400;
    int reg_id = offset / 4;

    if (!_this->halted.get())
    {
      _this->trace.force_warning("Trying to access GPR while core is not halted\n");
      return vp::IO_REQ_INVALID;
    }

    if (reg_id >= ISS_NB_REGS)
      return vp::IO_REQ_INVALID;

    if (req->get_is_write())
      iss_set_reg(_this, reg_id, *(iss_reg_t *)data);
    else
      *(iss_reg_t *)data = iss_get_reg(_this, reg_id);
  }
  else if (offset < 0x80)
  {
    if (offset == 0x00)
    {
      if (req->get_is_write())
      {
        bool step_mode = (*(iss_reg_t *)data) & 1;
        bool halt_mode = ((*(iss_reg_t *)data) >> 16) & 1;
        _this->trace.msg("Writing DBG_CTRL (value: 0x%x, halt: %d, step: %d)\n", *(iss_reg_t *)data, halt_mode, step_mode);

        _this->set_halt_mode(halt_mode, HALT_CAUSE_HALT);
        _this->step_mode.set(step_mode);

        _this->check_state();
      }
      else
      {
        *(iss_reg_t *)data = (_this->halted.get() << 16) | _this->step_mode.get();
      }
    }
    else if (offset == 0x04)
    {
      if (req->get_is_write())
      {
        _this->hit_reg = *(iss_reg_t *)data;
      }
      else
      {
        *(iss_reg_t *)data = _this->hit_reg;
      }
    }
    else if (offset == 0x0C)
    {
      if (req->get_is_write())
        return vp::IO_REQ_INVALID;

      *(iss_reg_t *)data = _this->halt_cause;
    }
  }
  else
  {
    _this->trace.force_warning("UNIMPLEMENTED AT %s %d\n", __FILE__, __LINE__);
    return vp::IO_REQ_INVALID;
  }

  return vp::IO_REQ_OK;
}



void iss_wrapper::insn_trace_callback()
{
  // This is called when the state of the instruction trace has changed, we need
  // to flush the ISS instruction cache, as it keeps the state of the trace
  if (this->iss_opened)
  {
    iss_cache_flush(this);
  }
}



int iss_wrapper::build()
{
  traces.new_trace("trace", &trace, vp::DEBUG);
  traces.new_trace("decode_trace", &decode_trace, vp::DEBUG);
  traces.new_trace("insn", &insn_trace, vp::DEBUG);
  this->insn_trace.register_callback(std::bind(&iss_wrapper::insn_trace_callback, this));

  traces.new_trace("csr", &csr_trace, vp::TRACE);
  traces.new_trace("perf", &perf_counter_trace, vp::TRACE);

  traces.new_trace_event("state", &state_event, 8);
  traces.new_trace_event("pc", &pc_trace_event, 32);
  this->pc_trace_event.register_callback(std::bind(&iss_wrapper::insn_trace_callback, this));
  traces.new_trace_event_string("asm", &insn_trace_event);
  traces.new_trace_event_string("func", &func_trace_event);
  traces.new_trace_event_string("inline_func", &inline_trace_event);
  traces.new_trace_event_string("file", &file_trace_event);
  traces.new_trace_event("line", &line_trace_event, 32);
  traces.new_trace_event("misaligned", &misaligned_req_event, 1);

  // TODO this should come from the config file as different chips may not have
  // same counters
  traces.new_trace_event("pcer_cycles", &pcer_trace_event[0], 1);
  traces.new_trace_event("pcer_instr", &pcer_trace_event[1], 1);
  traces.new_trace_event("pcer_ld_stall", &pcer_trace_event[2], 1);
  traces.new_trace_event("pcer_jmp_stall", &pcer_trace_event[3], 1);
  traces.new_trace_event("pcer_imiss", &pcer_trace_event[4], 1);
  traces.new_trace_event("pcer_ld", &pcer_trace_event[5], 1);
  traces.new_trace_event("pcer_st", &pcer_trace_event[6], 1);
  traces.new_trace_event("pcer_jump", &pcer_trace_event[7], 1);
  traces.new_trace_event("pcer_branch", &pcer_trace_event[8], 1);
  traces.new_trace_event("pcer_taken_branch", &pcer_trace_event[9], 1);
  traces.new_trace_event("pcer_rvc", &pcer_trace_event[10], 1);
  traces.new_trace_event("pcer_ld_ext", &pcer_trace_event[11], 1);
  traces.new_trace_event("pcer_st_ext", &pcer_trace_event[12], 1);
  traces.new_trace_event("pcer_ld_ext_cycles", &pcer_trace_event[13], 1);
  traces.new_trace_event("pcer_st_ext_cycles", &pcer_trace_event[14], 1);
  traces.new_trace_event("pcer_tcdm_cont", &pcer_trace_event[15], 1);

  traces.new_trace_event_real("ipc_stat", &ipc_stat_event);

  power.new_trace("power_trace", &power_trace);

  this->new_reg("bootaddr", &this->bootaddr_reg, get_config_int("boot_addr"));
  
  this->new_reg("fetch_enable", &this->fetch_enable_reg, get_js_config()->get("fetch_enable")->get_bool());
  this->new_reg("is_active", &this->is_active_reg, false);
  this->new_reg("stalled", &this->stalled, false);
  this->new_reg("wfi", &this->wfi, false);
  this->new_reg("misaligned_access", &this->misaligned_access, false);
  this->new_reg("halted", &this->halted, false);
  this->new_reg("step_mode", &this->step_mode, false);
  this->new_reg("do_step", &this->do_step, false);

  power.new_event("power_insn", &insn_power, this->get_js_config()->get("**/insn"), &power_trace);
  power.new_event("power_clock_gated", &clock_gated_power, this->get_js_config()->get("**/clock_gated"), &power_trace);
  power.new_leakage_event("leakage", &leakage_power, this->get_js_config()->get("**/leakage"), &power_trace);

  data.set_resp_meth(&iss_wrapper::data_response);
  data.set_grant_meth(&iss_wrapper::data_grant);
  new_master_port("data", &data);

  fetch.set_resp_meth(&iss_wrapper::fetch_response);
  fetch.set_grant_meth(&iss_wrapper::fetch_grant);
  new_master_port("fetch", &fetch);

  dbg_unit.set_req_meth(&iss_wrapper::dbg_unit_req);
  new_slave_port("dbg_unit", &dbg_unit);

  bootaddr_itf.set_sync_meth(&iss_wrapper::bootaddr_sync);
  new_slave_port("bootaddr", &bootaddr_itf);

  clock_itf.set_sync_meth(&iss_wrapper::clock_sync);
  new_slave_port("clock", &clock_itf);

  irq_req_itf.set_sync_meth(&iss_wrapper::irq_req_sync);
  new_slave_port("irq_req", &irq_req_itf);
  new_master_port("irq_ack", &irq_ack_itf);

  fetchen_itf.set_sync_meth(&iss_wrapper::fetchen_sync);
  new_slave_port("fetchen", &fetchen_itf);

  flush_cache_itf.set_sync_meth(&iss_wrapper::flush_cache_sync);
  new_slave_port("flush_cache", &flush_cache_itf);

  halt_itf.set_sync_meth(&iss_wrapper::halt_sync);
  new_slave_port("halt", &halt_itf);

  new_master_port("halt_status", &halt_status_itf);

  for (int i=0; i<32; i++)
  {
    new_master_port("ext_counter[" + std::to_string(i) + "]", &ext_counter[i]);
  }

  current_event = event_new(iss_wrapper::exec_first_instr);
  instr_event = event_new(iss_wrapper::exec_instr);
  check_all_event = event_new(iss_wrapper::exec_instr_check_all);
  misaligned_event = event_new(iss_wrapper::exec_misaligned);

  this->riscv_dbg_unit = this->get_js_config()->get_child_bool("riscv_dbg_unit");
  this->bootaddr_offset = get_config_int("bootaddr_offset");
  this->cpu.config.mhartid = (get_config_int("cluster_id") << 5) | get_config_int("core_id");
  string isa = get_config_str("isa");
  //transform(isa.begin(), isa.end(), isa.begin(),(int (*)(int))tolower);
  this->cpu.config.isa = strdup(isa.c_str());
  this->cpu.config.debug_handler = this->get_js_config()->get_int("debug_handler");

  this->is_active_reg.set(false);

  ipc_clock_event = this->event_new(iss_wrapper::ipc_stat_handler);

  this->ipc_stat_delay = 0;
  this->iss_opened = false;

  return 0;
}



void iss_wrapper::start()
{

  vp_assert_always(this->data.is_bound(), &this->trace, "Data master port is not connected\n");
  vp_assert_always(this->fetch.is_bound(), &this->trace, "Fetch master port is not connected\n");
  vp_assert_always(this->irq_ack_itf.is_bound(), &this->trace, "IRQ ack master port is not connected\n");



  if (iss_open(this)) throw logic_error("Error while instantiating the ISS");

  this->iss_opened = true;

  for (auto x:this->get_js_config()->get("**/debug_binaries")->get_elems())
  {
    iss_register_debug_info(this, x->get_str().c_str());
  }


  trace.msg("ISS start (fetch: %d, is_active: %d, boot_addr: 0x%lx)\n", fetch_enable_reg.get(), is_active_reg.get(), get_config_int("boot_addr"));

#ifdef USE_TRDB
  this->trdb = trdb_new();
  INIT_LIST_HEAD(&this->trdb_packet_list);
#endif

  this->leakage_power.power_on();
}

void iss_wrapper::pre_reset()
{
  if (this->is_active_reg.get())
  {
    this->event_cancel(this->current_event);
  }
}

void iss_wrapper::reset(bool active)
{
  if (active)
  {
    this->irq_req = -1;
    this->wakeup_latency = 0;

    for (int i=0; i<CSR_PCER_NB_EVENTS; i++)
    {
      this->pcer_trace_event[i].event(NULL);
    }
    this->misaligned_req_event.event(NULL);

    this->ipc_stat_nb_insn = 0;
    this->ipc_stat_delay = 10;

    iss_reset(this, 1);
  }
  else
  {
    iss_reset(this, 0);

    uint64_t zero = 0;
    for (int i=0; i<CSR_PCER_NB_EVENTS; i++)
    {
      this->pcer_trace_event[i].event((uint8_t *)&zero);
    }
    this->misaligned_req_event.event((uint8_t *)&zero);

    iss_pc_set(this, this->bootaddr_reg.get() + this->bootaddr_offset);
    iss_irq_set_vector_table(this, this->bootaddr_reg.get() & ~((1<<8) - 1));

    check_state();
  }
}


iss_wrapper::iss_wrapper(js::config *config)
: vp::component(config)
{
}


extern "C" vp::component *vp_constructor(js::config *config)
{
  return new iss_wrapper(config);
}
