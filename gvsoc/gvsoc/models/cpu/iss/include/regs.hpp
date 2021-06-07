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

#ifndef __CPU_ISS_ISS_REGS_HPP
#define __CPU_ISS_ISS_REGS_HPP

#include "types.hpp"

static iss_reg_t null_reg = 0;

static inline iss_reg_t *iss_reg_ref(iss_t *iss, int reg)
{
  if (reg == 0)
    return &null_reg;
  else
    return &iss->cpu.regfile.regs[reg];
}

static inline iss_reg_t *iss_reg_store_ref(iss_t *iss, int reg)
{
  return &iss->cpu.regfile.regs[reg];
}

static inline void iss_set_reg(iss_t *iss, int reg, iss_reg_t value)
{
  if (reg != 0)
    iss->cpu.regfile.regs[reg] = value;
}

static inline iss_reg_t iss_get_reg_untimed(iss_t *iss, int reg)
{
  return iss->cpu.regfile.regs[reg];
}

static inline iss_reg_t iss_get_reg(iss_t *iss, int reg)
{
  return iss_get_reg_untimed(iss, reg);
}

static inline iss_reg64_t iss_get_reg64_untimed(iss_t *iss, int reg)
{
  if (reg == 0)
      return 0;
  else
      return (((uint64_t)iss->cpu.regfile.regs[reg+1]) << 32) + iss->cpu.regfile.regs[reg];
}

static inline void iss_set_reg64(iss_t *iss, int reg, iss_reg64_t value)
{
  if (reg != 0)
  {
    iss->cpu.regfile.regs[reg] = value & 0xFFFFFFFF;
    iss->cpu.regfile.regs[reg+1] = value >> 32;
  }
}

static inline iss_reg64_t iss_get_reg64(iss_t *iss, int reg)
{
  return iss_get_reg64_untimed(iss, reg);
}

static inline iss_reg_t iss_get_reg_for_jump(iss_t *iss, int reg) {
  //unsigned long regCycle = cpu->regsCycle[reg];
  //if (cpu->cycles < regCycle + 1) {
  //  accountJumpStalls(cpu, regCycle + 1 - cpu->cycles);
  //  cpu->cycles = regCycle + 1;
  //}
  return iss_get_reg_untimed(iss, reg);
}

static inline void iss_set_spec_purp_reg(iss_t *iss, int spreg, iss_reg_t value)
{
  if ((spreg >= 0) && (spreg < 6))
    iss->cpu.pulp_nn.spr_ml[spreg] = value;
}

static inline iss_reg_t iss_get_spec_purp_reg(iss_t *iss, int spreg)
{
  if ((spreg >= 0) && (spreg < 6))
    return iss->cpu.pulp_nn.spr_ml[spreg];
  else
    return 0;
}

#endif
