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

#ifndef __CPU_ISS_ISS_INSN_EXEC_HPP
#define __CPU_ISS_ISS_INSN_EXEC_HPP

#if defined(ISS_HAS_PERF_COUNTERS)
void update_external_pccr(iss_t *iss, int id, unsigned int pcer, unsigned int pcmr);
#endif

static inline int iss_exec_account_cycles(iss_t *iss, int cycles);

iss_insn_t *iss_exec_insn_with_trace(iss_t *iss, iss_insn_t *insn);
void iss_trace_dump(iss_t *iss, iss_insn_t *insn);
void iss_trace_init(iss_t *iss);


static inline void iss_exec_insn_resume(iss_t *iss)
{
  iss->cpu.state.insn_cycles = iss->cpu.state.saved_insn_cycles;
}

static inline void iss_exec_insn_terminate(iss_t *iss)
{
  iss_exec_account_cycles(iss, iss->cpu.state.insn_cycles);

  if (iss_insn_trace_active(iss))
  {
    iss_trace_dump(iss, iss->cpu.stall_insn);
  }
}

static inline void iss_exec_insn_stall(iss_t *iss)
{
  iss->cpu.stall_insn = iss->cpu.current_insn;
  iss->cpu.state.saved_insn_cycles = iss->cpu.state.insn_cycles;
  iss->cpu.state.insn_cycles = -1;
}

static inline iss_insn_t *iss_exec_insn_handler(iss_t *instance, iss_insn_t *insn, iss_insn_t *(*handler)(iss_t *, iss_insn_t *))
{
  return handler(instance, insn);
}


static inline iss_insn_t *iss_exec_insn_fast(iss_t *iss, iss_insn_t *insn)
{
  return iss_exec_insn_handler(iss, insn, insn->fast_handler);
}

static inline iss_insn_t *iss_exec_insn(iss_t *iss, iss_insn_t *insn)
{
  return iss_exec_insn_handler(iss, insn, insn->handler);
}

static inline iss_insn_t *iss_exec_stalled_insn_fast(iss_t *iss, iss_insn_t *insn)
{
  iss_perf_account_dependency_stall(iss, insn->latency);
  return iss_exec_insn_handler(iss, insn, insn->stall_fast_handler);
}

static inline iss_insn_t *iss_exec_stalled_insn(iss_t *iss, iss_insn_t *insn)
{
  iss_perf_account_dependency_stall(iss, insn->latency);
  iss_pccr_account_event(iss, CSR_PCER_LD_STALL, 1);
  return iss_exec_insn_handler(iss, insn, insn->stall_handler);
}



#define ISS_EXEC_NO_FETCH_COMMON(iss,func) \
do { \
  iss->cpu.state.insn_cycles = 1; \
  iss_insn_t *insn = iss->cpu.current_insn; \
  prefetcher_fetch(iss, insn->addr); \
  iss->cpu.prev_insn = insn; \
  iss->cpu.current_insn = func(iss, insn); \
} while(0)


static inline int iss_exec_step_nofetch(iss_t *iss)
{
  ISS_EXEC_NO_FETCH_COMMON(iss,iss_exec_insn_fast);

  return iss->cpu.state.insn_cycles;
}

static inline int iss_exec_step(iss_t *iss)
{
  return iss_exec_step_nofetch(iss);
}


static inline int iss_exec_switch_to_fast(iss_t *iss)
{
#ifdef VP_TRACE_ACTIVE
  return false;
#else
  return !(iss->cpu.csr.pcmr & CSR_PCMR_ACTIVE);
#endif
}

static inline int iss_exec_account_cycles(iss_t *iss, int cycles)
{
  if (iss->cpu.csr.pcmr & CSR_PCMR_ACTIVE)
  {
    if (cycles >= 0 && (iss->cpu.csr.pcer & (1<<CSR_PCER_CYCLES)))
    {
      iss->cpu.csr.pccr[CSR_PCER_CYCLES] += cycles;
    }
  }

  iss_pccr_incr(iss, CSR_PCER_CYCLES, cycles);

#if defined(ISS_HAS_PERF_COUNTERS)
  for (int i=CSR_PCER_NB_INTERNAL_EVENTS; i<CSR_PCER_NB_EVENTS; i++)
  {
    if (iss_pccr_trace_active(iss, i))
    {
      update_external_pccr(iss, i, iss->cpu.csr.pcer, iss->cpu.csr.pcmr);
    }
  }
#endif
  return 0;
}

static inline int iss_exec_step_nofetch_perf(iss_t *iss)
{
  iss_irq_check(iss);
  ISS_EXEC_NO_FETCH_COMMON(iss,iss_exec_insn);

  int cycles = iss->cpu.state.insn_cycles;

  iss_exec_account_cycles(iss, iss->cpu.state.insn_cycles);

  if (iss->cpu.csr.pcmr & CSR_PCMR_ACTIVE)
  {
    if (iss->cpu.csr.pcer & (1<<CSR_PCER_INSTR))
      iss->cpu.csr.pccr[CSR_PCER_INSTR] += 1;
  }
  iss_pccr_incr(iss, CSR_PCER_INSTR, 1);

  return cycles;
}



static inline int iss_exec_step_check_all(iss_t *iss)
{
  return iss_exec_step_nofetch_perf(iss);
}



static inline int iss_exec_is_stalled(iss_t *iss)
{
  return iss->cpu.state.insn_cycles == -1;
}

#if 0
static inline int iss_exec_step(iss_t *iss)
{
  uint64_t opcode = prefetcher_get_word(iss, 0x1a000000);
  return 1;
}
#endif

#endif
