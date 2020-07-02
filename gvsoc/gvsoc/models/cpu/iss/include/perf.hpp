/*
 * Copyright (C) 2020  GreenWaves Technologies, SAS
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
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