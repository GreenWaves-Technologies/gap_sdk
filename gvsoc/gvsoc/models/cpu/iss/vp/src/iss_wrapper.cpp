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
#include "iss.hpp"
#include <algorithm>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#ifndef O_BINARY
# define O_BINARY 0
#endif

#if 0
// Old pulp debug unit, no more supported
#define HALT_CAUSE_EBREAK    0
#define HALT_CAUSE_ECALL     1
#define HALT_CAUSE_ILLEGAL   2
#define HALT_CAUSE_INVALID   3
#define HALT_CAUSE_INTERRUPT 4 
#define HALT_CAUSE_HALT      15
#define HALT_CAUSE_STEP      15

#else

#define HALT_CAUSE_EBREAK      1
#define HALT_CAUSE_TRIGGER     2
#define HALT_CAUSE_HALT        3
#define HALT_CAUSE_STEP        4
#define HALT_CAUSE_RESET_HALT  5

#endif

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
  if (_this->active_pc_trace_event.get_event_active()) \
  { \
    _this->active_pc_trace_event.event((uint8_t *)&_this->cpu.current_insn->addr); \
  } \
  if (_this->func_trace_event.get_event_active() || _this->inline_trace_event.get_event_active() || _this->file_trace_event.get_event_active() || _this->line_trace_event.get_event_active()) \
  { \
    _this->dump_debug_traces(); \
  } \
  if (_this->ipc_stat_event.get_event_active()) \
  { \
    _this->ipc_stat_nb_insn++; \
  } \
 \
  iss_insn_t *insn = _this->cpu.current_insn; \
  int cycles = func(_this); \
  if (_this->power.get_power_trace()->get_active()) \
  { \
  _this->insn_groups_power[insn->decoder_item->u.insn.power_group].account_energy_quantum(); \
 } \
  trdb_record_instruction(_this, insn); \
  if (!_this->stalled.get()) \
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

  int debug_mode = _this->cpu.state.debug_mode;

  EXEC_INSTR_COMMON(_this, event, iss_exec_step_nofetch_perf);
  if (_this->step_mode.get() && !debug_mode)
  {
    _this->do_step.set(false);
    _this->hit_reg |= 1;
    if (_this->gdbserver)
    {
      _this->halted.set(true);
      _this->gdbserver->signal(_this);
    }
    else
    {
      _this->set_halt_mode(true, HALT_CAUSE_STEP);
    }
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
  _this->stalled.dec(1);
  _this->wakeup_latency = req->get_latency();
  if (_this->misaligned_access.get())
  {
    _this->misaligned_access.set(false);
  }
  else
  {
    // First call the ISS to finish the instruction
    _this->cpu.state.stall_callback(_this);
    iss_exec_insn_terminate(_this);
  }
  _this->check_state();
}

void iss_wrapper::fetch_grant(void *_this, vp::io_req *req)
{

}

void iss_wrapper::fetch_response(void *__this, vp::io_req *req)
{
  iss_t *_this = (iss_t *)__this;

  _this->stalled.dec(1);
  if (_this->cpu.state.fetch_stall_callback)
  {
    _this->cpu.state.fetch_stall_callback(_this);
  }
  _this->check_state();
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

  if (_this->busy_itf.is_bound())
  {
    _this->busy_itf.sync(active);
  }

  // TODO this could be better handler is the clock would be taken into
  // account in the core state machine
  uint8_t value = active && _this->is_active_reg.get();
  if (value)
  {
    _this->state_event.event((uint8_t *)&value);
    _this->busy.set(1);
  }
  else
  {
    _this->state_event.event(NULL);
    _this->busy.release();
    if (_this->active_pc_trace_event.get_event_active())
      _this->active_pc_trace_event.event(NULL);
  }

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

void iss_wrapper::flush_cache_ack_sync(void *__this, bool active)
{
    iss_t *_this = (iss_t *)__this;
    //iss_exec_insn_resume(_this);
    iss_exec_insn_terminate(_this);
    _this->check_state();
}


void iss_wrapper::set_halt_mode(bool halted, int cause)
{
  if (this->riscv_dbg_unit)
  {
    if (!this->cpu.state.debug_mode)
    {
      this->cpu.csr.dcsr = (this->cpu.csr.dcsr & ~(0x7 << 6)) | (cause << 6);
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

    this->trace.msg(vp::trace::LEVEL_TRACE, "Checking state (active: %d, halted: %d, fetch_enable: %d, stalled: %d, wfi: %d, irq_req: %d, debug_mode: %d)\n",
      is_active_reg.get(), halted.get(), fetch_enable_reg.get(), stalled.get(), wfi.get(), irq_req, this->cpu.state.debug_mode);

    if (!halted.get() && fetch_enable_reg.get() && !stalled.get() && (!wfi.get() || irq_req != -1 || this->cpu.state.debug_mode))
    {
      // Check if we can directly reenqueue next instruction because it has already
      // been fetched or we need to fetch it
      if (this->cpu.state.do_fetch)
      {
        // In case we need to fetch it, first trigger the fetch
        prefetcher_fetch(this, this->cpu.current_insn);

        // Then enqueue the instruction only if the fetch ws completed synchronously.
        // Otherwise, the next instruction will be enqueued when we receive the fetch
        // response
        if (this->stalled.get())
        {
          return;
        }
      }

      wfi.set(false);
      is_active_reg.set(true);
      uint8_t one = 1;
      this->state_event.event(&one);
      this->busy.set(1);
      if (this->ipc_stat_event.get_event_active())
        this->trigger_ipc_stat();

      if (step_mode.get() && !this->cpu.state.debug_mode)
      {
        do_step.set(true);
      }

      if (this->cpu.csr.pcmr & CSR_PCMR_ACTIVE && this->cpu.csr.pcer & (1<<CSR_PCER_CYCLES))
      {
        this->cpu.csr.pccr[CSR_PCER_CYCLES] += 1 + this->wakeup_latency;
      }

      enqueue_next_instr(1 + this->wakeup_latency);

      this->wakeup_latency = 0;
    }
  }
  else
  {
    if (halted.get() && !do_step.get())
    {
      if (this->active_pc_trace_event.get_event_active())
        this->active_pc_trace_event.event(NULL);
      is_active_reg.set(false);
      this->state_event.event(NULL);
      if (this->ipc_stat_event.get_event_active())
        this->stop_ipc_stat();
      this->halt_core();
    }
    else if (wfi.get())
    {
      if (irq_req == -1)
      {
        if (this->active_pc_trace_event.get_event_active())
          this->active_pc_trace_event.event(NULL);
        is_active_reg.set(false);
        this->state_event.event(NULL);
        this->busy.release();
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

  iss_pccr_account_event(this, CSR_PCER_MISALIGNED, 1);

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


void iss_wrapper::irq_req_sync_handler(void *__this, vp::clock_event *event)
{
  iss_t *_this = (iss_t *)__this;
  _this->irq_req = _this->irq_req_value;
  _this->irq_check();
  iss_irq_req(_this, _this->irq_req);
  _this->wfi.set(false);
  _this->check_state();
}


void iss_wrapper::irq_req_sync(void *__this, int irq)
{
  iss_t *_this = (iss_t *)__this;
  _this->irq_req_value = irq;
  // Handle the sync in an event with 0 delay to always take into account interrupts after the
  // instruction has been executed
  if (!_this->irq_sync_event->is_enqueued())
  {
    _this->event_enqueue(_this->irq_sync_event, 0);
  }
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
  std::string str = "";
  while(size != 0)
  {
    req->init();
    req->set_debug(true);
    req->set_addr(addr);
    req->set_size(1);
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

    addr++;
    size--;
    buffer++;
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

  switch (id)
  {
    case 0x4:
    {
      std::string path = this->read_user_string(this->cpu.regfile.regs[11]);
      printf("%s", path.c_str());
      break;
    }

    case 0x1:
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

      break;
    }

    case 0x2:
    {
      this->cpu.regfile.regs[10] = close(this->cpu.regfile.regs[11]);
      break;
    }

    case 0x5:
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

        fflush(NULL);

        size -= iter_size;
        addr += iter_size;
      }

      this->cpu.regfile.regs[10] = size;
      break;
    }

    case 0x6:
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

      break;
    }

    case 0xA:
    {
      iss_reg_t args[2];
      if (this->user_access(this->cpu.regfile.regs[11], (uint8_t *)args, sizeof(args), false))
      {
        this->cpu.regfile.regs[10] = -1;
        return;
      }

      int pos = lseek(args[0], args[1], SEEK_SET);
      this->cpu.regfile.regs[10] = pos != args[1];
      break;
    }

    case 0x18:
    {
      int status = this->cpu.regfile.regs[11] == 0x20026 ? 0 : 1;

      this->clock->stop_retain(-1);
      this->clock->stop_engine(status & 0x7fffffff);

      break;
    }

    case 0x0C:
    {
      struct stat buf;
      fstat(this->cpu.regfile.regs[11], &buf);
      this->cpu.regfile.regs[10] = buf.st_size;
      break;
    }

    case 0x100:
    {
      iss_reg_t args[2];
      if (this->user_access(this->cpu.regfile.regs[11], (uint8_t *)args, sizeof(args), false))
      {
        this->cpu.regfile.regs[10] = -1;
        return;
      }
      std::string path = this->read_user_string(args[0]);
      if (path == "")
      {
        this->warning.force_warning("Invalid user string while opening trace (addr: 0x%x)\n", args[0]);
      }
      else
      {
        if (args[1])
        {
          this->traces.get_trace_manager()->add_trace_path(0, path);
        }
        else
        {
          this->traces.get_trace_manager()->add_exclude_trace_path(0, path);
        }
        this->traces.get_trace_manager()->check_traces();
      }
      break;
    }

    case 0x101:
    {
      iss_reg_t args[1];
      if (this->user_access(this->cpu.regfile.regs[11], (uint8_t *)args, sizeof(args), false))
      {
        this->cpu.regfile.regs[10] = -1;
        return;
      }

      iss_csr_write(this, CSR_PCER, args[0]);

      break;
    }

    case 0x102:
    {
      iss_csr_write(this, CSR_PCCR(31), 0);
      this->cycle_count = 0;
      iss_reg_t value;
      iss_csr_read(this, CSR_PCMR, &value);
      if ((value & 1) == 1)
      {
        this->cycle_count_start = this->get_cycles();
      }
      break;
    }

    case 0x103:
    {
      iss_reg_t value;
      iss_csr_read(this, CSR_PCMR, &value);
      if ((value & 1) == 0)
      {
        this->cycle_count_start = this->get_cycles();
      }

      iss_csr_write(this, CSR_PCMR, 1);
      break;
    }

    case 0x104:
    {

      iss_reg_t value;
      iss_csr_read(this, CSR_PCMR, &value);
      if ((value & 1) == 1)
      {
        this->cycle_count += this->get_cycles() - this->cycle_count_start;
      }

      iss_csr_write(this, CSR_PCMR, 0);
      break;
    }

    case 0x105:
    {
      iss_reg_t args[1];
      if (this->user_access(this->cpu.regfile.regs[11], (uint8_t *)args, sizeof(args), false))
      {
        return;
      }
      iss_csr_read(this, CSR_PCCR(args[0]), &this->cpu.regfile.regs[10]);
      break;
    }

    case 0x106:
    {
      iss_reg_t args[2];
      if (this->user_access(this->cpu.regfile.regs[11], (uint8_t *)args, sizeof(args), false))
      {
        this->cpu.regfile.regs[10] = -1;
        return;
      }
      std::string path = this->read_user_string(args[0]);
      std::string mode = this->read_user_string(args[1]);
      if (path == "")
      {
        this->warning.force_warning("Invalid user string while opening trace (addr: 0x%x)\n", args[0]);
      }
      else
      {
        if (mode == "")
        {
          this->warning.force_warning("Invalid user string while opening trace (addr: 0x%x)\n", args[1]);
        }
        else
        {
          this->cpu.regfile.regs[10] = 0;
          FILE *file = fopen(path.c_str(), mode.c_str());
          if (file == NULL)
          {
            this->cpu.regfile.regs[10] = -1;
            return;
          }

          iss_reg_t value;
          iss_csr_read(this, CSR_PCMR, &value);
          if ((value & 1) == 1)
          {
            this->cycle_count += this->get_cycles() - this->cycle_count_start;
            this->cycle_count_start = this->get_cycles();
          }

          fprintf(file, "PCER values at timestamp %ld ps, duration %ld cycles\n", this->get_time(), this->cycle_count);
          fprintf(file, "Index; Name; Description; Value\n");
          for (int i=0; i<31; i++)
          {
            if (this->pcer_info[i].name != "")
            {
              iss_reg_t value;
              iss_csr_read(this, CSR_PCCR(i), &value);
              fprintf(file, "%d; %s; %s; %d\n", i, this->pcer_info[i].name.c_str(), this->pcer_info[i].help.c_str(), value);
            }
          }


          fclose(file);
        }
      }
        break;
    }

    case 0x107: {
      iss_reg_t args[1];
      if (this->user_access(this->cpu.regfile.regs[11], (uint8_t *)args, sizeof(args), false))
      {
        this->cpu.regfile.regs[10] = -1;
        return;
      }
      this->traces.get_trace_manager()->set_global_enable(args[0]);
      break;
    }

    case 0x108: {
      iss_reg_t args[1];
      if (this->user_access(this->cpu.regfile.regs[11], (uint8_t *)args, sizeof(args), false))
      {
        this->cpu.regfile.regs[10] = -1;
        return;
      }

      int result = -1;
      std::string path = this->read_user_string(args[0]);
      if (path == "")
      {
        this->warning.force_warning("Invalid user string while opening VCD trace (addr: 0x%x)\n", args[0]);
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
    
    case 0x109:
    break;
    
    case 0x10A: {
      iss_reg_t args[2];
      if (this->user_access(this->cpu.regfile.regs[11], (uint8_t *)args, sizeof(args), false))
      {
        this->cpu.regfile.regs[10] = -1;
        return;
      }

      int id = args[0];
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
          if (0)
            trace->event(NULL);
          else
            trace->event((uint8_t *)&args[1]);
        }
      }

      break; 
    }
    
    case 0x10C: {
      iss_reg_t args[1];
      if (this->user_access(this->cpu.regfile.regs[11], (uint8_t *)args, sizeof(args), false))
      {
        this->cpu.regfile.regs[10] = -1;
        return;
      }

      int id = args[0];
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
          trace->event(NULL);
        }
      }

      break; 
    }
    
    case 0x10B: {
      iss_reg_t args[2];
      if (this->user_access(this->cpu.regfile.regs[11], (uint8_t *)args, sizeof(args), false))
      {
        this->cpu.regfile.regs[10] = -1;
        return;
      }

      int id = args[0];
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
          std::string path = this->read_user_string(args[1]);

          trace->event_string(path);
        }
      }
      break;
    }

    case 0x10D: {
      this->get_engine()->stop_exec();

      break; 
    }
    
    default:
      this->warning.force_warning("Unknown ebreak call (id: %d)\n", id);
      break;
  }
}



void iss_wrapper::handle_ebreak()
{
  int id = this->cpu.regfile.regs[10];

  switch (id)
  {
  // TODO deprecated, should be removed
#if 0
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
  #endif

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
  traces.new_trace("gdbserver_trace", &gdbserver_trace, vp::DEBUG);
  traces.new_trace("decode_trace", &decode_trace, vp::DEBUG);
  traces.new_trace("insn", &insn_trace, vp::DEBUG);
  this->insn_trace.register_callback(std::bind(&iss_wrapper::insn_trace_callback, this));

  traces.new_trace("csr", &csr_trace, vp::TRACE);
  traces.new_trace("perf", &perf_counter_trace, vp::TRACE);

  traces.new_trace_event("state", &state_event, 8);
  this->new_reg("busy", &this->busy, 1);
  traces.new_trace_event("pc", &pc_trace_event, 32);
  traces.new_trace_event("active_pc", &active_pc_trace_event, 32);
  this->pc_trace_event.register_callback(std::bind(&iss_wrapper::insn_trace_callback, this));
  traces.new_trace_event_string("asm", &insn_trace_event);
  traces.new_trace_event_string("func", &func_trace_event);
  traces.new_trace_event_string("inline_func", &inline_trace_event);
  traces.new_trace_event_string("file", &file_trace_event);
  traces.new_trace_event_string("binaries", &binaries_trace_event);
  traces.new_trace_event("line", &line_trace_event, 32);

  traces.new_trace_event_real("ipc_stat", &ipc_stat_event);

  this->new_reg("bootaddr", &this->bootaddr_reg, get_config_int("boot_addr"));
  
  this->new_reg("fetch_enable", &this->fetch_enable_reg, get_js_config()->get("fetch_enable")->get_bool());
  this->new_reg("is_active", &this->is_active_reg, false);
  this->new_reg("stalled", &this->stalled, false);
  this->new_reg("wfi", &this->wfi, false);
  this->new_reg("misaligned_access", &this->misaligned_access, false);
  this->new_reg("halted", &this->halted, false);
  this->new_reg("step_mode", &this->step_mode, false);
  this->new_reg("do_step", &this->do_step, false);

  if (this->get_js_config()->get("**/insn_groups"))
  {
    js::config *config = this->get_js_config()->get("**/insn_groups");
    this->insn_groups_power.resize(config->get_size());
    for (int i=0; i<config->get_size(); i++)
    {
      power.new_power_source("power_insn_" + std::to_string(i), &this->insn_groups_power[i], config->get_elem(i));
    }
  }
  else
  {
    this->insn_groups_power.resize(1);
    power.new_power_source("power_insn", &this->insn_groups_power[0], this->get_js_config()->get("**/insn"));
  }
  power.new_power_source("background", &background_power, this->get_js_config()->get("**/power_models/background"));

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

  new_master_port("busy", &busy_itf);

  fetchen_itf.set_sync_meth(&iss_wrapper::fetchen_sync);
  new_slave_port("fetchen", &fetchen_itf);

  flush_cache_itf.set_sync_meth(&iss_wrapper::flush_cache_sync);
  new_slave_port("flush_cache", &flush_cache_itf);

  flush_cache_ack_itf.set_sync_meth(&iss_wrapper::flush_cache_ack_sync);
  new_slave_port("flush_cache_ack", &flush_cache_ack_itf);
  new_master_port("flush_cache_req", &flush_cache_req_itf);

  halt_itf.set_sync_meth(&iss_wrapper::halt_sync);
  new_slave_port("halt", &halt_itf);

  new_master_port("halt_status", &halt_status_itf);

  for (int i=0; i<32; i++)
  {
    new_master_port("ext_counter[" + std::to_string(i) + "]", &ext_counter[i]);
    this->pcer_info[i].name  = "";
  }

  current_event = event_new(iss_wrapper::exec_first_instr);
  instr_event = event_new(iss_wrapper::exec_instr);
  check_all_event = event_new(iss_wrapper::exec_instr_check_all);
  misaligned_event = event_new(iss_wrapper::exec_misaligned);
  irq_sync_event = event_new(iss_wrapper::irq_req_sync_handler);

  this->riscv_dbg_unit = this->get_js_config()->get_child_bool("riscv_dbg_unit");
  this->bootaddr_offset = get_config_int("bootaddr_offset");

  this->cpu.config.mhartid = (get_config_int("cluster_id") << 5) | get_config_int("core_id");
  this->cpu.config.misa = this->get_js_config()->get_int("misa");

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

  this->target_open();

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

  this->background_power.leakage_power_start();
  this->background_power.dynamic_power_start();

  this->gdbserver = (vp::Gdbserver_engine *)this->get_service("gdbserver");

  if (this->gdbserver)
  {
    this->gdbserver->register_core(this);
    this->halted.set(true);
    this->check_state();
  }
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

    for (int i=0; i<32; i++)
    {
      this->pcer_trace_event[i].event(NULL);
    }

    this->ipc_stat_nb_insn = 0;
    this->ipc_stat_delay = 10;
    this->clock_active = false;

    this->active_pc_trace_event.event(NULL);

    if (this->get_js_config()->get("**/binaries") != NULL)
    {
      std::string binaries = "static enable";
      for (auto x:this->get_js_config()->get("**/binaries")->get_elems())
      {
        binaries += " " +  x->get_str();
      }

      this->binaries_trace_event.event_string(binaries);
    }

    iss_reset(this, 1);
  }
  else
  {
    iss_reset(this, 0);

    uint64_t zero = 0;
    for (int i=0; i<32; i++)
    {
      this->pcer_trace_event[i].event((uint8_t *)&zero);
    }

    iss_pc_set(this, this->bootaddr_reg.get() + this->bootaddr_offset);
    iss_irq_set_vector_table(this, this->bootaddr_reg.get() & ~((1<<8) - 1));

    if (this->gdbserver)
    {
      this->halted.set(true);
    }

    check_state();
  }
}


iss_wrapper::iss_wrapper(js::config *config)
: vp::component(config)
{
}


int iss_wrapper::gdbserver_get_id()
{
  return this->cpu.config.mhartid;
}


std::string iss_wrapper::gdbserver_get_name()
{
  return this->get_name();
}


int iss_wrapper::gdbserver_reg_set(int reg, uint8_t *value)
{
    this->gdbserver_trace.msg(vp::trace::LEVEL_DEBUG, "Setting register from gdbserver (reg: %d, value: 0x%x)\n", reg, *(uint32_t *)value);

    if (reg == 32)
    {
        iss_pc_set(this, *(uint32_t *)value);
    }
    else
    {
        this->gdbserver_trace.msg(vp::trace::LEVEL_ERROR, "Setting invalid register (reg: %d, value: 0x%x)\n", reg, *(uint32_t *)value);
    }

    return 0;
}


int iss_wrapper::gdbserver_reg_get(int reg, uint8_t *value)
{
  fprintf(stderr, "UNIMPLEMENTED AT %s %d\n", __FILE__, __LINE__);
  return 0;
}


int iss_wrapper::gdbserver_regs_get(int *nb_regs, int *reg_size, uint8_t *value)
{
    if (nb_regs)
    {
        *nb_regs = 33;
    }

    if (reg_size)
    {
        *reg_size = 4;
    }

    if (value)
    {
        uint32_t *regs = (uint32_t *)value;
        for (int i=0; i<32; i++)
        {
            regs[i] = iss_get_reg(this, i);
        }

        if (this->cpu.current_insn)
        {
            regs[32] = this->cpu.current_insn->addr;
        }
        else
        {
            regs[32] = 0;
        }
    }

    return 0;
}


int iss_wrapper::gdbserver_stop()
{
    this->halted.set(true);
    this->gdbserver->signal(this);
    this->check_state();
    return 0;
}


int iss_wrapper::gdbserver_cont()
{
    this->halted.set(false);
    this->check_state();

    return 0;
}


int iss_wrapper::gdbserver_stepi()
{
    fprintf(stderr, "STEP\n");
    this->step_mode.set(true);
    this->halted.set(false);
    this->check_state();
    return 0;
}


int iss_wrapper::gdbserver_state()
{
    return vp::Gdbserver_core::state::running;
}


void iss_wrapper::declare_pcer(int index, std::string name, std::string help)
{
    this->pcer_info[index].name = name;
    this->pcer_info[index].help = help;
}


void iss_wrapper::target_open()
{
    this->declare_pcer(CSR_PCER_CYCLES, "Cycles", "Count the number of cycles the core was running");
    this->declare_pcer(CSR_PCER_INSTR, "instr", "Count the number of instructions executed");
    this->declare_pcer(CSR_PCER_LD_STALL, "ld_stall", "Number of load use hazards");
    this->declare_pcer(CSR_PCER_JMP_STALL, "jmp_stall", "Number of jump register hazards");
    this->declare_pcer(CSR_PCER_IMISS, "imiss", "Cycles waiting for instruction fetches. i.e. the number of instructions wasted due to non-ideal caches");
    this->declare_pcer(CSR_PCER_LD, "ld", "Number of memory loads executed. Misaligned accesses are counted twice");
    this->declare_pcer(CSR_PCER_ST, "st", "Number of memory stores executed. Misaligned accesses are counted twice");
    this->declare_pcer(CSR_PCER_JUMP, "jump", "Number of jump instructions seen, i.e. j, jr, jal, jalr");
    this->declare_pcer(CSR_PCER_BRANCH, "branch", "Number of branch instructions seen, i.e. bf, bnf");
    this->declare_pcer(CSR_PCER_TAKEN_BRANCH, "taken_branch", "Number of taken branch instructions seen, i.e. bf, bnf");
    this->declare_pcer(CSR_PCER_RVC, "rvc", "Number of compressed instructions");
    this->declare_pcer(CSR_PCER_LD_EXT, "ld_ext", "Number of memory loads to EXT executed. Misaligned accesses are counted twice. Every non-TCDM access is considered external");
    this->declare_pcer(CSR_PCER_ST_EXT, "st_ext", "Number of memory stores to EXT executed. Misaligned accesses are counted twice. Every non-TCDM access is considered external");
    this->declare_pcer(CSR_PCER_LD_EXT_CYC, "ld_ext_cycles", "Cycles used for memory loads to EXT. Every non-TCDM access is considered external");
    this->declare_pcer(CSR_PCER_ST_EXT_CYC, "st_ext_cycles", "Cycles used for memory stores to EXT. Every non-TCDM access is considered external");
    this->declare_pcer(CSR_PCER_TCDM_CONT, "tcdm_cont", "Cycles wasted due to TCDM/log-interconnect contention");
    
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
}


#ifndef EXTERNAL_ISS_WRAPPER

extern "C" vp::component *vp_constructor(js::config *config)
{
  return new iss_wrapper(config);
}

#endif