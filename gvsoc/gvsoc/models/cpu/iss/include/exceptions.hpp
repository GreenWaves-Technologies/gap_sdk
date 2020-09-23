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