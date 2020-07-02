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

#endif
