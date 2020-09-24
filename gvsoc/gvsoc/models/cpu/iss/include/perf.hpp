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

#ifndef __CPU_ISS_ISS_INSN_PERF_HPP
#define __CPU_ISS_ISS_INSN_PERF_HPP


static inline void iss_pccr_account_event(iss_t *iss, unsigned int event, int incr)
{
  if (iss->cpu.csr.pcmr & CSR_PCMR_ACTIVE && (iss->cpu.csr.pcer & (1<<event)))
  {
    iss->cpu.csr.pccr[event] += incr;
  }

  iss_pccr_incr(iss, event, incr);

  //if (cpu->traceEvent) sim_trace_event_incr(cpu, event, incr);
}

static inline void iss_perf_account_taken_branch(iss_t *iss)
{
  iss->cpu.state.insn_cycles += 2;  
}

static inline void iss_perf_account_dependency_stall(iss_t *iss, int latency)
{
  iss->cpu.state.insn_cycles += latency - 1;
}

static inline void iss_perf_account_jump(iss_t *iss)
{
  iss->cpu.state.insn_cycles += 1;  
}

#endif