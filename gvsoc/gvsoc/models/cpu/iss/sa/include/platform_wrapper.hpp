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

#ifndef __PLATFORM_WRAPPER_HPP
#define __PLATFORM_WRAPPER_HPP

#include "iss.hpp"
#include <string>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

typedef struct iss_s
{
  iss_cpu_t cpu;

  int fast_mode;

  bfd *abfd;
  asymbol **asymbols;
  asection *textSection;
  unsigned long int textSectionStart;
  unsigned long int textSectionEnd;
  unsigned int a_argc;
  unsigned int a_argv;
  unsigned int a_argbuf;
  unsigned int a_stack;
  int hit_exit;
  int exit_status;

  unsigned char *mem_array;
  size_t mem_size;

} iss_t;

void handle_syscall(iss_t *iss, iss_insn_t *insn);


//#define USE_INSN_TRACES 1

static inline bool iss_insn_trace_active(iss_t *iss)
{
#ifdef USE_INSN_TRACES
  return true;
#else
  return false;
#endif
}

#define iss_fatal(iss, fmt, x...)

#define iss_warning(iss, fmt, x...)

#define iss_force_warning(iss, fmt, x...)

#define iss_msg(iss, fmt, x...)

#define iss_csr_msg(iss, fmt, x...)

#define iss_decoder_msg(iss, fmt, x...)

#define iss_perf_counter_msg(iss, fmt, x...)

#if USE_INSN_TRACES

inline void iss_insn_msg(iss_t *iss, const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  if (vfprintf(stdout, fmt, ap) < 0) {}
}

#else

#define iss_insn_msg(iss, fmt, x...)

#endif

static inline void iss_pccr_incr(iss_t *iss, unsigned int event, int incr)
{
}

static inline void iss_lsu_elw(iss_t *iss, iss_insn_t *insn, iss_addr_t addr, int size, int reg)
{
}

static inline void iss_unstall(iss_t *iss)
{
}

static inline void iss_insn_event_dump(iss_t *iss, const char *msg)
{
}

static inline int iss_insn_event_active(iss_t *iss)
{
  return 0;
}

static inline void iss_handle_ebreak(iss_t *iss, iss_insn_t *insn)
{
}

static inline int iss_pccr_trace_active(iss_t *iss, unsigned int event)
{
  return 0;
}

static inline int iss_fetch_req(iss_t *iss, uint64_t addr, uint8_t *data, uint64_t size, bool is_write)
{
  memcpy(data, iss->mem_array + addr, size);
  return 0;
}

static inline int iss_irq_ack(iss_t *iss, int irq)
{
}

static inline void iss_lsu_load(iss_t *iss, iss_insn_t *insn, iss_addr_t addr, int size, int reg)
{
  if (addr + size > iss->mem_size)
    return;

  iss_set_reg(iss, reg, iss_get_zext_value(*(iss_reg_t *)(iss->mem_array + addr), size*8));
}

static inline void iss_lsu_load_signed(iss_t *iss, iss_insn_t *insn, iss_addr_t addr, int size, int reg)
{
  if (addr + size > iss->mem_size)
    return;
  
  iss_set_reg(iss, reg, iss_get_zext_value(*(iss_reg_t *)(iss->mem_array + addr), size*8));
}

static inline void iss_lsu_store(iss_t *iss, iss_insn_t *insn, iss_addr_t addr, int size, int reg)
{
  if (addr + size > iss->mem_size)
    return;
  
  memcpy(iss->mem_array + addr, &iss->cpu.regfile.regs[reg], size);
}

static inline void iss_handle_ecall(iss_t *iss, iss_insn_t *insn)
{
  handle_syscall(iss, insn);
}

static inline void iss_set_halt_mode(iss_t *iss, bool halted, int cause)
{
}

static inline void iss_wait_for_interrupt(iss_t *iss)
{
}

static inline void iss_trigger_check_all(iss_t *iss)
{
}

static inline void iss_trigger_irq_check(iss_t *iss)
{
}

static void iss_csr_ext_counter_set(iss_t *iss, int id, unsigned int value)
{
}

static bool iss_csr_ext_counter_is_bound(iss_t *iss, int id)
{
  return false;
}

static void iss_csr_ext_counter_get(iss_t *iss, int id, unsigned int *value)
{
}

static inline void iss_init(iss_t *iss)
{
}



#endif