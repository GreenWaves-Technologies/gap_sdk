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
