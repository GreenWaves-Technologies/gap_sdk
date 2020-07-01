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

#ifndef __CPU_ISS_EXCEPTIONS_HPP
#define __CPU_ISS_EXCEPTIONS_HPP

static inline iss_insn_t *iss_except_raise(iss_t *iss, int id)
{
#if defined(PRIV_1_10)
  if (id == ISS_EXCEPT_DEBUG)
  {
    iss->cpu.csr.depc = iss->cpu.current_insn->addr;
    iss->cpu.irq.debug_saved_irq_enable = iss->cpu.irq.irq_enable;
    iss->cpu.irq.irq_enable = 0;
    return iss->cpu.irq.debug_handler;
  }
  else
  {
    iss->cpu.csr.epc = iss->cpu.current_insn->addr;
    iss->cpu.irq.saved_irq_enable = iss->cpu.irq.irq_enable;
    iss->cpu.irq.irq_enable = 0;
    iss->cpu.csr.mcause = 0xb;
    iss_insn_t *insn = iss->cpu.irq.vectors[0];
    if (insn == NULL)
      insn = insn_cache_get(iss, 0);
    return insn; 
  }
#else
  iss->cpu.csr.epc = iss->cpu.current_insn->addr;
  iss->cpu.irq.saved_irq_enable = iss->cpu.irq.irq_enable;
  iss->cpu.irq.irq_enable = 0;
  iss->cpu.csr.mcause = 0xb;
  iss_insn_t *insn = iss->cpu.irq.vectors[32 + id];
  if (insn == NULL)
    insn = insn_cache_get(iss, 0);
  return insn;
 #endif
}

#endif