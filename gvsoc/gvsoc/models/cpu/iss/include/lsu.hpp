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

#ifndef __CPU_ISS_LSU_HPP
#define __CPU_ISS_LSU_HPP

#include "iss_core.hpp"

static inline void iss_lsu_load_perf(iss_t *iss, iss_insn_t *insn, iss_addr_t addr, int size, int reg)
{
  iss_pccr_account_event(iss, CSR_PCER_LD, 1);
  iss_lsu_load(iss, insn, addr, size, reg);
}

static inline void iss_lsu_elw_perf(iss_t *iss, iss_insn_t *insn, iss_addr_t addr, int size, int reg)
{
  iss_pccr_account_event(iss, CSR_PCER_LD, 1);
  iss_lsu_elw(iss, insn, addr, size, reg);
}

static inline void iss_lsu_load_signed_perf(iss_t *iss, iss_insn_t *insn, iss_addr_t addr, int size, int reg)
{
  iss_pccr_account_event(iss, CSR_PCER_LD, 1);
  iss_lsu_load_signed(iss, insn, addr, size, reg);
}

static inline void iss_lsu_store_perf(iss_t *iss, iss_insn_t *insn, iss_addr_t addr, int size, int reg)
{
  iss_pccr_account_event(iss, CSR_PCER_ST, 1);
  iss_lsu_store(iss, insn, addr, size, reg);
}

static inline void iss_lsu_check_stack_access(iss_t *iss, int reg, iss_addr_t addr)
{
  // Could be optimized when decoding instruction by pointing to different instruction
  // handlers when register 2 is seen but the gain is small compared to the cost of the full
  // load.
  if (iss->cpu.csr.stack_conf && reg == 2)
  {
    if (addr < iss->cpu.csr.stack_start || addr >= iss->cpu.csr.stack_end)
    {
      iss_fatal(iss, "SP-based access outside stack (addr: 0x%x, stack_start: 0x%x, stack_end: 0x%x)\n", addr, iss->cpu.csr.stack_start, iss->cpu.csr.stack_end);
    }
  }
}

#endif
