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

#ifndef __CPU_ISS_RV32I_HPP
#define __CPU_ISS_RV32I_HPP

#include "iss_core.hpp"
#include "isa_lib/int.h"
#include "isa_lib/macros.h"



static inline iss_insn_t *lui_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, UIM_GET(0));
  return insn->next;
}



static inline iss_insn_t *auipc_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, UIM_GET(0));
  return insn->next;
}

static inline void auipc_decode(iss_t *iss, iss_insn_t *insn)
{
  insn->uim[0] += insn->addr;
}



static inline iss_insn_t *jal_exec_common(iss_t *iss, iss_insn_t *insn, int perf)
{
  unsigned int D = insn->out_regs[0];
  if (D != 0) REG_SET(0, insn->addr + insn->size);
  if (perf)
  {
    iss_pccr_account_event(iss, CSR_PCER_JUMP, 1);
  }
  iss_perf_account_jump(iss);
  return insn->next;
}

static inline iss_insn_t *jal_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  return jal_exec_common(iss, insn, 0);
}

static inline iss_insn_t *jal_exec(iss_t *iss, iss_insn_t *insn)
{
  return jal_exec_common(iss, insn, 1);
}



static inline void jal_decode(iss_t *iss, iss_insn_t *insn)
{


  insn->next = insn_cache_get(iss, insn->addr + insn->sim[0]);
}



static inline iss_insn_t *jalr_exec_common(iss_t *iss, iss_insn_t *insn, int perf)
{
  iss_insn_t *next_insn = insn_cache_get(iss, insn->sim[0] + iss_get_reg_for_jump(iss, insn->in_regs[0]));
  unsigned int D = insn->out_regs[0];
  if (D != 0) REG_SET(0, insn->addr + insn->size);
  if (perf)
  {
    iss_pccr_account_event(iss, CSR_PCER_JUMP, 1);
  }
  iss_perf_account_jump(iss);
  return next_insn;
}

static inline iss_insn_t *jalr_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  return jalr_exec_common(iss, insn, 0);
}

static inline iss_insn_t *jalr_exec(iss_t *iss, iss_insn_t *insn)
{
  return jalr_exec_common(iss, insn, 1);
}



static inline void bxx_decode(iss_t *iss, iss_insn_t *insn)
{
  unsigned int next_pc = insn->addr + SIM_GET(0);
  insn->branch = insn_cache_get(iss, next_pc);
}



static inline iss_insn_t *beq_exec_common(iss_t *iss, iss_insn_t *insn, int perf)
{
  if (REG_GET(0) == REG_GET(1))
  {
    if (perf)
    {
      iss_pccr_account_event(iss, CSR_PCER_BRANCH, 1);
      iss_pccr_account_event(iss, CSR_PCER_TAKEN_BRANCH, 1);
    }
    iss_perf_account_taken_branch(iss);
    return insn->branch;
  }
  else
  {
    if (perf)
    {
      iss_pccr_account_event(iss, CSR_PCER_BRANCH, 1);
    }
    return insn->next;
  }
}

static inline iss_insn_t *beq_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  return beq_exec_common(iss, insn, 0);
}

static inline iss_insn_t *beq_exec(iss_t *iss, iss_insn_t *insn)
{
  return beq_exec_common(iss, insn, 1);
}



static inline iss_insn_t *bne_exec_common(iss_t *iss, iss_insn_t *insn, int perf)
{
  if (REG_GET(0) != REG_GET(1)) {
    if (perf)
    {
      iss_pccr_account_event(iss, CSR_PCER_BRANCH, 1);
      iss_pccr_account_event(iss, CSR_PCER_TAKEN_BRANCH, 1);
    }
    iss_perf_account_taken_branch(iss);
    return insn->branch;
  }
  else
  {
    if (perf)
    {
      iss_pccr_account_event(iss, CSR_PCER_BRANCH, 1);
    }
    return insn->next;
  }
}

static inline iss_insn_t *bne_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  return bne_exec_common(iss, insn, 0);
}

static inline iss_insn_t *bne_exec(iss_t *iss, iss_insn_t *insn)
{
  return bne_exec_common(iss, insn, 1);
}



static inline iss_insn_t *blt_exec_common(iss_t *iss, iss_insn_t *insn, int perf)
{
  if ((int32_t)REG_GET(0) < (int32_t)REG_GET(1))
  {
    if (perf)
    {
      iss_pccr_account_event(iss, CSR_PCER_BRANCH, 1);
      iss_pccr_account_event(iss, CSR_PCER_TAKEN_BRANCH, 1);
    }
    iss_perf_account_taken_branch(iss);
    return insn->branch;
  }
  else
  {
    if (perf)
    {
      iss_pccr_account_event(iss, CSR_PCER_BRANCH, 1);
    }
    return insn->next;
  }
}

static inline iss_insn_t *blt_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  return blt_exec_common(iss, insn, 0);
}

static inline iss_insn_t *blt_exec(iss_t *iss, iss_insn_t *insn)
{
  return blt_exec_common(iss, insn, 1);
}



static inline iss_insn_t *bge_exec_common(iss_t *iss, iss_insn_t *insn, int perf)
{
  if ((int32_t)REG_GET(0) >= (int32_t)REG_GET(1))
  {
    if (perf)
    {
      iss_pccr_account_event(iss, CSR_PCER_BRANCH, 1);
      iss_pccr_account_event(iss, CSR_PCER_TAKEN_BRANCH, 1);
    }
    iss_perf_account_taken_branch(iss);
    return insn->branch;
  }
  else
  {
    if (perf)
    {
      iss_pccr_account_event(iss, CSR_PCER_BRANCH, 1);
    }
    return insn->next;
  }
}

static inline iss_insn_t *bge_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  return bge_exec_common(iss, insn, 0);
}

static inline iss_insn_t *bge_exec(iss_t *iss, iss_insn_t *insn)
{
  return bge_exec_common(iss, insn, 1);
}



static inline iss_insn_t *bltu_exec_common(iss_t *iss, iss_insn_t *insn, int perf)
{
  if (REG_GET(0) < REG_GET(1))
  {
    if (perf)
    {
      iss_pccr_account_event(iss, CSR_PCER_BRANCH, 1);
      iss_pccr_account_event(iss, CSR_PCER_TAKEN_BRANCH, 1);
    }
    iss_perf_account_taken_branch(iss);
    return insn->branch;
  }
  else
  {
    if (perf)
    {
      iss_pccr_account_event(iss, CSR_PCER_BRANCH, 1);
    }
    return insn->next;
  }
}

static inline iss_insn_t *bltu_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  return bltu_exec_common(iss, insn, 0);
}

static inline iss_insn_t *bltu_exec(iss_t *iss, iss_insn_t *insn)
{
  return bltu_exec_common(iss, insn, 1);
}



static inline iss_insn_t *bgeu_exec_common(iss_t *iss, iss_insn_t *insn, int perf)
{
  if (REG_GET(0) >= REG_GET(1))
  {
    if (perf)
    {
      iss_pccr_account_event(iss, CSR_PCER_BRANCH, 1);
      iss_pccr_account_event(iss, CSR_PCER_TAKEN_BRANCH, 1);
    }
    iss_perf_account_taken_branch(iss);
    return insn->branch;
  }
  else
  {
    if (perf)
    {
      iss_pccr_account_event(iss, CSR_PCER_BRANCH, 1);
    }
    return insn->next;
  }
}

static inline iss_insn_t *bgeu_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  return bgeu_exec_common(iss, insn, 0);
}

static inline iss_insn_t *bgeu_exec(iss_t *iss, iss_insn_t *insn)
{
  return bgeu_exec_common(iss, insn, 1);
}



static inline iss_insn_t *lb_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  iss_lsu_load_signed(iss, insn, REG_GET(0) + SIM_GET(0), 1, REG_OUT(0));
  return insn->next;
}

static inline iss_insn_t *lb_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_lsu_check_stack_access(iss, REG_IN(0), REG_GET(0) + SIM_GET(0));
  iss_lsu_load_signed_perf(iss, insn, REG_GET(0) + SIM_GET(0), 1, REG_OUT(0));
  return insn->next;
}



static inline iss_insn_t *lh_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  iss_lsu_load_signed(iss, insn, REG_GET(0) + SIM_GET(0), 2, REG_OUT(0));
  return insn->next;
}

static inline iss_insn_t *lh_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_lsu_check_stack_access(iss, REG_IN(0), REG_GET(0) + SIM_GET(0));
  iss_lsu_load_signed_perf(iss, insn, REG_GET(0) + SIM_GET(0), 2, REG_OUT(0));
  return insn->next;
}



static inline iss_insn_t *lw_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  iss_lsu_load(iss, insn, REG_GET(0) + SIM_GET(0), 4, REG_OUT(0));
  return insn->next;
}

static inline iss_insn_t *lw_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_lsu_check_stack_access(iss, REG_IN(0), REG_GET(0) + SIM_GET(0));
  iss_lsu_load_perf(iss, insn, REG_GET(0) + SIM_GET(0), 4, REG_OUT(0));
  return insn->next;
}



static inline iss_insn_t *lbu_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  iss_lsu_load(iss, insn, REG_GET(0) + SIM_GET(0), 1, REG_OUT(0));
  return insn->next;
}

static inline iss_insn_t *lbu_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_lsu_check_stack_access(iss, REG_IN(0), REG_GET(0) + SIM_GET(0));
  iss_lsu_load_perf(iss, insn, REG_GET(0) + SIM_GET(0), 1, REG_OUT(0));
  return insn->next;
}



static inline iss_insn_t *lhu_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  iss_lsu_load(iss, insn, REG_GET(0) + SIM_GET(0), 2, REG_OUT(0));
  return insn->next;
}

static inline iss_insn_t *lhu_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_lsu_check_stack_access(iss, REG_IN(0), REG_GET(0) + SIM_GET(0));
  iss_lsu_load_perf(iss, insn, REG_GET(0) + SIM_GET(0), 2, REG_OUT(0));
  return insn->next;
}



static inline iss_insn_t *sb_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  iss_lsu_store(iss, insn, REG_GET(0) + SIM_GET(0), 1, REG_IN(1));
  return insn->next;
}

static inline iss_insn_t *sb_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_lsu_check_stack_access(iss, REG_OUT(0), REG_GET(0) + SIM_GET(0));
  iss_lsu_store_perf(iss, insn, REG_GET(0) + SIM_GET(0), 1, REG_IN(1));
  return insn->next;
}



static inline iss_insn_t *sh_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  iss_lsu_store(iss, insn, REG_GET(0) + SIM_GET(0), 2, REG_IN(1));
  return insn->next;
}

static inline iss_insn_t *sh_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_lsu_check_stack_access(iss, REG_OUT(0), REG_GET(0) + SIM_GET(0));
  iss_lsu_store_perf(iss, insn, REG_GET(0) + SIM_GET(0), 2, REG_IN(1));
  return insn->next;
}



static inline iss_insn_t *sw_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  iss_lsu_store(iss, insn, REG_GET(0) + SIM_GET(0), 4, REG_IN(1));
  return insn->next;
}


static inline iss_insn_t *sw_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_lsu_check_stack_access(iss, REG_OUT(0), REG_GET(0) + SIM_GET(0));
  iss_lsu_store_perf(iss, insn, REG_GET(0) + SIM_GET(0), 4, REG_IN(1));
  return insn->next;
}


static inline iss_insn_t *addi_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_ADD, REG_GET(0), SIM_GET(0)));
  return insn->next;
}



static inline iss_insn_t *nop_exec(iss_t *iss, iss_insn_t *insn)
{
  return insn->next;
}



static inline iss_insn_t *slti_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, (int32_t)REG_GET(0) < insn->sim[0]);
  return insn->next;
}



static inline iss_insn_t *sltiu_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, REG_GET(0) < (uint32_t)SIM_GET(0));
  return insn->next;
}



static inline iss_insn_t *xori_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_XOR, REG_GET(0), SIM_GET(0)));
  return insn->next;
}



static inline iss_insn_t *ori_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_OR, REG_GET(0), SIM_GET(0)));
  return insn->next;
}



static inline iss_insn_t *andi_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_AND, REG_GET(0), SIM_GET(0)));
  return insn->next;
}



static inline iss_insn_t *slli_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_SLL, REG_GET(0), UIM_GET(0)));
  return insn->next;
}



static inline iss_insn_t *srli_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_SRL, REG_GET(0), UIM_GET(0)));
  return insn->next;
}



static inline iss_insn_t *srai_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_SRA, REG_GET(0), UIM_GET(0)));
  return insn->next;
}



static inline iss_insn_t *add_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_ADD, REG_GET(0), REG_GET(1)));
  return insn->next;
}



static inline iss_insn_t *sub_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_SUB, REG_GET(0), REG_GET(1)));
  return insn->next;
}



static inline iss_insn_t *sll_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_SLL, REG_GET(0), REG_GET(1)));
  return insn->next;
}



static inline iss_insn_t *slt_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, (int32_t)REG_GET(0) < (int32_t)REG_GET(1));
  return insn->next;
}



static inline iss_insn_t *sltu_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, REG_GET(0) <REG_GET(1));
  return insn->next;
}



static inline iss_insn_t *xor_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_XOR, REG_GET(0), REG_GET(1)));
  return insn->next;
}



static inline iss_insn_t *srl_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_SRL, REG_GET(0), REG_GET(1)));
  return insn->next;
}



static inline iss_insn_t *sra_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_SRA, REG_GET(0), REG_GET(1)));
  return insn->next;
}



static inline iss_insn_t *or_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_OR, REG_GET(0), REG_GET(1)));
  return insn->next;
}



static inline iss_insn_t *and_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_AND, REG_GET(0), REG_GET(1)));
  return insn->next;
}



static inline iss_insn_t *fence_exec(iss_t *iss, iss_insn_t *insn)
{
  return insn->next;
}



static inline iss_insn_t *fence_i_exec(iss_t *iss, iss_insn_t *insn)
{
  return insn->next;
}



static inline iss_insn_t *ebreak_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_insn_t *next = insn->next;
  if (next)
  {
    next = iss_decode_pc_noexec(iss, next);
    if (insn->next->opcode == 0x40705013)
    {
      iss_handle_riscv_ebreak(iss, insn);
      return insn->next;
    }
  }

  if (iss->cpu.state.debug_mode)
  {
    return iss->cpu.irq.debug_handler;
  }
  else
  {
    iss_handle_ebreak(iss, insn);
  }

  //cpu->state.error = GVSIM_STATE_DEBUG_STALL;
  return insn->next;
}



static inline iss_insn_t *ecall_exec(iss_t *iss, iss_insn_t *insn)
{
/*
  printf("SYSCall %d, Fun(n:%d, A0:%d, A1:%d, A2:%d, A3:%d)\n",
   pc->uim[0], getReg(cpu, 17), getReg(cpu, 10), getReg(cpu, 11), getReg(cpu, 12), getReg(cpu, 13));
*/

  iss_handle_ecall(iss, insn);

  return iss_except_raise(iss, ISS_EXCEPT_ECALL);
#if 0
  if (!cpu->conf->useSyscalls) {
    triggerException(cpu, pc, EXCEPTION_ECALL);
    return pc->next;
  } else {
    rv32_SYS_calls(cpu, pc);
    return pc->next;
  }
#endif

  return insn->next;
}


#endif
