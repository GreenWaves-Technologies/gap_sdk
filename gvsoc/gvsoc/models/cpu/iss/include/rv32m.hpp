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

#ifndef __CPU_ISS_RV32M_HPP
#define __CPU_ISS_RV32M_HPP



static inline iss_insn_t *mul_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_MULU, REG_GET(0), REG_GET(1)));
  return insn->next;
}



static inline iss_insn_t *mulh_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, ((int64_t)(int32_t)REG_GET(0) * (int64_t)(int32_t)REG_GET(1)) >> 32);
  //setRegTimed(cpu, pc, pc->outReg[0], ((int64_t)(int32_t)getReg(cpu, pc->inReg[0]) * (int64_t)(int32_t)getReg(cpu, pc->inReg[1])) >> 32);
  return insn->next;
}



static inline iss_insn_t *mulhsu_exec(iss_t *iss, iss_insn_t *insn)
{
 REG_SET(0, ((int64_t)(int32_t)REG_GET(0) * (uint64_t)REG_GET(1)) >> 32);
  //setRegTimed(cpu, pc, pc->outReg[0], ((int64_t)(int32_t)getReg(cpu, pc->inReg[0]) * (uint64_t)getReg(cpu, pc->inReg[1])) >> 32);
  return insn->next;
}

static inline iss_insn_t *mulhu_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, ((uint64_t)REG_GET(0) * (uint64_t)REG_GET(1)) >> 32);
  //setRegTimed(cpu, pc, pc->outReg[0], ((uint64_t)getReg(cpu, pc->inReg[0]) * (uint64_t)getReg(cpu, pc->inReg[1])) >> 32);
  return insn->next;
}

static inline iss_insn_t *div_exec(iss_t *iss, iss_insn_t *insn)
{
  int32_t divider = REG_GET(1);
  int32_t dividend = REG_GET(0);
  int32_t result;
  if (divider == 0) result = -1;
  else if (divider == (1<<31) && dividend == -1) result = (1<<31);
  else if (dividend == (1<<31) && divider == -1) result = (1<<31);
  else result = dividend / divider;
  REG_SET(0, result);
  //setRegTimed(cpu, pc, pc->outReg[0], result);
  return insn->next;
}

static inline iss_insn_t *divu_exec(iss_t *iss, iss_insn_t *insn)
{
  uint32_t divider = REG_GET(1);
  uint32_t dividend = REG_GET(0);
  uint32_t result;
  if (divider == 0) result = -1;
  else result = dividend / divider;
  REG_SET(0, result);
  //setRegTimed(cpu, pc, pc->outReg[0], result);
  return insn->next;
}

static inline iss_insn_t *rem_exec(iss_t *iss, iss_insn_t *insn)
{
  int32_t divider = REG_GET(1);
  int32_t dividend = REG_GET(0);
  int32_t result;
  if (divider == 0) result = dividend;
  else if (divider == (1<<31) && dividend == -1) result = 0;
  else if (dividend == (1<<31) && divider == -1) result = 0;
  else result = dividend % divider;
  REG_SET(0, result);
  //setRegTimed(cpu, pc, pc->outReg[0], result);
  return insn->next;
}


static inline iss_insn_t *remu_exec(iss_t *iss, iss_insn_t *insn)
{
  uint32_t divider = REG_GET(1);
  uint32_t dividend = REG_GET(0);
  uint32_t result;
  if (divider == 0) result = dividend;
  else result = dividend % divider;
  //setRegTimed(cpu, pc, pc->outReg[0], result);
  REG_SET(0, result);
  return insn->next;
}



#endif
