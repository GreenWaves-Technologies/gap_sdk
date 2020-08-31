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

#ifndef __CPU_ISS_PULP_V2_HPP
#define __CPU_ISS_PULP_V2_HPP

#define PULPV2_HWLOOP_LPSTART0 0
#define PULPV2_HWLOOP_LPEND0   1
#define PULPV2_HWLOOP_LPCOUNT0 2

#define PULPV2_HWLOOP_LPSTART1 3
#define PULPV2_HWLOOP_LPEND1   4
#define PULPV2_HWLOOP_LPCOUNT1 5

#define PULPV2_HWLOOP_LPSTART(x) (PULPV2_HWLOOP_LPSTART0 + (x)*3)
#define PULPV2_HWLOOP_LPEND(x) (PULPV2_HWLOOP_LPEND0 + (x)*3)
#define PULPV2_HWLOOP_LPCOUNT(x) (PULPV2_HWLOOP_LPCOUNT0 + (x)*3)

static inline iss_insn_t *LB_RR_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  iss_lsu_load_signed(iss, insn, REG_GET(0) + REG_GET(1), 1, REG_OUT(0));
  return insn->next;
}

static inline iss_insn_t *LB_RR_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_lsu_check_stack_access(iss, REG_IN(0), REG_GET(0) + REG_GET(1));
  iss_lsu_check_stack_access(iss, REG_IN(1), REG_GET(0) + REG_GET(1));
  iss_lsu_load_signed_perf(iss, insn, REG_GET(0) + REG_GET(1), 1, REG_OUT(0));
  return insn->next;
}



static inline iss_insn_t *LH_RR_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  iss_lsu_load_signed(iss, insn, REG_GET(0) + REG_GET(1), 2, REG_OUT(0));
  return insn->next;
}

static inline iss_insn_t *LH_RR_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_lsu_check_stack_access(iss, REG_IN(0), REG_GET(0) + REG_GET(1));
  iss_lsu_check_stack_access(iss, REG_IN(1), REG_GET(0) + REG_GET(1));
  iss_lsu_load_signed_perf(iss, insn, REG_GET(0) + REG_GET(1), 2, REG_OUT(0));
  return insn->next;
}



static inline iss_insn_t *LW_RR_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  iss_lsu_load(iss, insn, REG_GET(0) + REG_GET(1), 4, REG_OUT(0));
  return insn->next;
}

static inline iss_insn_t *LW_RR_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_lsu_check_stack_access(iss, REG_IN(0), REG_GET(0) + REG_GET(1));
  iss_lsu_check_stack_access(iss, REG_IN(1), REG_GET(0) + REG_GET(1));
  iss_lsu_load_perf(iss, insn, REG_GET(0) + REG_GET(1), 4, REG_OUT(0));
  return insn->next;
}



static inline iss_insn_t *LBU_RR_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  iss_lsu_load(iss, insn, REG_GET(0) + REG_GET(1), 1, REG_OUT(0));
  return insn->next;
}

static inline iss_insn_t *LBU_RR_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_lsu_check_stack_access(iss, REG_IN(0), REG_GET(0) + REG_GET(1));
  iss_lsu_check_stack_access(iss, REG_IN(1), REG_GET(0) + REG_GET(1));
  iss_lsu_load_perf(iss, insn, REG_GET(0) + REG_GET(1), 1, REG_OUT(0));
  return insn->next;
}



static inline iss_insn_t *LHU_RR_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  iss_lsu_load(iss, insn, REG_GET(0) + REG_GET(1), 2, REG_OUT(0));
  return insn->next;
}

static inline iss_insn_t *LHU_RR_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_lsu_check_stack_access(iss, REG_IN(0), REG_GET(0) + REG_GET(1));
  iss_lsu_check_stack_access(iss, REG_IN(1), REG_GET(0) + REG_GET(1));
  iss_lsu_load_perf(iss, insn, REG_GET(0) + REG_GET(1), 2, REG_OUT(0));
  return insn->next;
}



static inline iss_insn_t *LB_POSTINC_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  iss_lsu_load_signed(iss, insn, REG_GET(0), 1, REG_OUT(0));
  IN_REG_SET(0, REG_GET(0) + SIM_GET(0));
  return insn->next;
}

static inline iss_insn_t *LB_POSTINC_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_lsu_load_signed_perf(iss, insn, REG_GET(0), 1, REG_OUT(0));
  IN_REG_SET(0, REG_GET(0) + SIM_GET(0));
  return insn->next;
}



static inline iss_insn_t *LH_POSTINC_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  iss_lsu_load_signed(iss, insn, REG_GET(0), 2, REG_OUT(0));
  IN_REG_SET(0, REG_GET(0) + SIM_GET(0));
  return insn->next;
}

static inline iss_insn_t *LH_POSTINC_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_lsu_load_signed_perf(iss, insn, REG_GET(0), 2, REG_OUT(0));
  IN_REG_SET(0, REG_GET(0) + SIM_GET(0));
  return insn->next;
}



static inline iss_insn_t *LW_POSTINC_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  iss_lsu_load_signed(iss, insn, REG_GET(0), 4, REG_OUT(0));
  IN_REG_SET(0, REG_GET(0) + SIM_GET(0));
  return insn->next;
}

static inline iss_insn_t *LW_POSTINC_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_lsu_load_signed_perf(iss, insn, REG_GET(0), 4, REG_OUT(0));
  IN_REG_SET(0, REG_GET(0) + SIM_GET(0));
  return insn->next;
}



static inline iss_insn_t *LBU_POSTINC_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  iss_lsu_load(iss, insn, REG_GET(0), 1, REG_OUT(0));
  IN_REG_SET(0, REG_GET(0) + SIM_GET(0));
  return insn->next;
}

static inline iss_insn_t *LBU_POSTINC_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_lsu_check_stack_access(iss, REG_IN(0), REG_GET(0));
  iss_lsu_load_perf(iss, insn, REG_GET(0), 1, REG_OUT(0));
  IN_REG_SET(0, REG_GET(0) + SIM_GET(0));
  return insn->next;
}




static inline iss_insn_t *LHU_POSTINC_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  iss_lsu_load(iss, insn, REG_GET(0), 2, REG_OUT(0));
  IN_REG_SET(0, REG_GET(0) + SIM_GET(0));
  return insn->next;
}

static inline iss_insn_t *LHU_POSTINC_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_lsu_check_stack_access(iss, REG_IN(0), REG_GET(0));
  iss_lsu_load_perf(iss, insn, REG_GET(0), 2, REG_OUT(0));
  IN_REG_SET(0, REG_GET(0) + SIM_GET(0));
  return insn->next;
}



static inline iss_insn_t *SB_POSTINC_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  iss_lsu_store(iss, insn, REG_GET(0), 1, REG_IN(1));
  IN_REG_SET(0, REG_GET(0) + SIM_GET(0));
  return insn->next;
}

static inline iss_insn_t *SB_POSTINC_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_lsu_check_stack_access(iss, REG_IN(0), REG_GET(0));
  iss_lsu_store_perf(iss, insn, REG_GET(0), 1, REG_IN(1));
  IN_REG_SET(0, REG_GET(0) + SIM_GET(0));
  return insn->next;
}



static inline iss_insn_t *SH_POSTINC_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  iss_lsu_store(iss, insn, REG_GET(0), 2, REG_IN(1));
  IN_REG_SET(0, REG_GET(0) + SIM_GET(0));
  return insn->next;
}

static inline iss_insn_t *SH_POSTINC_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_lsu_check_stack_access(iss, REG_IN(0), REG_GET(0));
  iss_lsu_store_perf(iss, insn, REG_GET(0), 2, REG_IN(1));
  IN_REG_SET(0, REG_GET(0) + SIM_GET(0));
  return insn->next;
}



static inline iss_insn_t *SW_POSTINC_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  iss_lsu_store(iss, insn, REG_GET(0), 4, REG_IN(1));
  IN_REG_SET(0, REG_GET(0) + SIM_GET(0));
  return insn->next;
}

static inline iss_insn_t *SW_POSTINC_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_lsu_check_stack_access(iss, REG_IN(0), REG_GET(0));
  iss_lsu_store_perf(iss, insn, REG_GET(0), 4, REG_IN(1));
  IN_REG_SET(0, REG_GET(0) + SIM_GET(0));
  return insn->next;
}



static inline iss_insn_t *LB_RR_POSTINC_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  iss_reg_t new_val = REG_GET(0) + REG_GET(1);
  iss_lsu_load_signed(iss, insn, REG_GET(0), 1, REG_OUT(0));
  IN_REG_SET(0, new_val);
  return insn->next;
}

static inline iss_insn_t *LB_RR_POSTINC_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_reg_t new_val = REG_GET(0) + REG_GET(1);
  iss_lsu_load_signed_perf(iss, insn, REG_GET(0), 1, REG_OUT(0));
  IN_REG_SET(0, new_val);
  return insn->next;
}



static inline iss_insn_t *LH_RR_POSTINC_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  iss_reg_t new_val = REG_GET(0) + REG_GET(1);
  iss_lsu_load_signed(iss, insn, REG_GET(0), 2, REG_OUT(0));
  IN_REG_SET(0, new_val);
  return insn->next;
}

static inline iss_insn_t *LH_RR_POSTINC_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_reg_t new_val = REG_GET(0) + REG_GET(1);
  iss_lsu_load_signed_perf(iss, insn, REG_GET(0), 2, REG_OUT(0));
  IN_REG_SET(0, new_val);
  return insn->next;
}



static inline iss_insn_t *LW_RR_POSTINC_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  iss_reg_t new_val = REG_GET(0) + REG_GET(1);
  iss_lsu_load_signed(iss, insn, REG_GET(0), 4, REG_OUT(0));
  IN_REG_SET(0, new_val);
  return insn->next;
}

static inline iss_insn_t *LW_RR_POSTINC_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_reg_t new_val = REG_GET(0) + REG_GET(1);
  iss_lsu_load_signed_perf(iss, insn, REG_GET(0), 4, REG_OUT(0));
  IN_REG_SET(0, new_val);
  return insn->next;
}



static inline iss_insn_t *LBU_RR_POSTINC_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  iss_reg_t new_val = REG_GET(0) + REG_GET(1);
  iss_lsu_load(iss, insn, REG_GET(0), 1, REG_OUT(0));
  IN_REG_SET(0, new_val);
  return insn->next;
}

static inline iss_insn_t *LBU_RR_POSTINC_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_reg_t new_val = REG_GET(0) + REG_GET(1);
  iss_lsu_check_stack_access(iss, REG_IN(0), REG_GET(0));
  iss_lsu_load_perf(iss, insn, REG_GET(0), 1, REG_OUT(0));
  IN_REG_SET(0, new_val);
  return insn->next;
}



static inline iss_insn_t *LHU_RR_POSTINC_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  iss_reg_t new_val = REG_GET(0) + REG_GET(1);
  iss_lsu_load(iss, insn, REG_GET(0), 2, REG_OUT(0));
  IN_REG_SET(0, new_val);
  return insn->next;
}

static inline iss_insn_t *LHU_RR_POSTINC_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_reg_t new_val = REG_GET(0) + REG_GET(1);
  iss_lsu_check_stack_access(iss, REG_IN(0), REG_GET(0));
  iss_lsu_load_perf(iss, insn, REG_GET(0), 2, REG_OUT(0));
  IN_REG_SET(0, new_val);
  return insn->next;
}



static inline iss_insn_t *SB_RR_POSTINC_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  iss_reg_t new_val = REG_GET(0) + REG_GET(2);
  iss_lsu_store(iss, insn, REG_GET(0), 1, REG_IN(1));
  IN_REG_SET(0, new_val);
  return insn->next;
}

static inline iss_insn_t *SB_RR_POSTINC_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_reg_t new_val = REG_GET(0) + REG_GET(2);
  iss_lsu_check_stack_access(iss, REG_OUT(0), REG_GET(0));
  iss_lsu_store_perf(iss, insn, REG_GET(0), 1, REG_IN(1));
  IN_REG_SET(0, new_val);
  return insn->next;
}



static inline iss_insn_t *SH_RR_POSTINC_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  iss_reg_t new_val = REG_GET(0) + REG_GET(2);
  iss_lsu_store(iss, insn, REG_GET(0), 2, REG_IN(1));
  IN_REG_SET(0, new_val);
  return insn->next;
}

static inline iss_insn_t *SH_RR_POSTINC_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_reg_t new_val = REG_GET(0) + REG_GET(2);
  iss_lsu_check_stack_access(iss, REG_OUT(0), REG_GET(0));
  iss_lsu_store_perf(iss, insn, REG_GET(0), 2, REG_IN(1));
  IN_REG_SET(0, new_val);
  return insn->next;
}



static inline iss_insn_t *SW_RR_POSTINC_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  iss_reg_t new_val = REG_GET(0) + REG_GET(2);
  iss_lsu_store(iss, insn, REG_GET(0), 4, REG_IN(1));
  IN_REG_SET(0, new_val);
  return insn->next;
}

static inline iss_insn_t *SW_RR_POSTINC_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_reg_t new_val = REG_GET(0) + REG_GET(2);
  iss_lsu_check_stack_access(iss, REG_OUT(0), REG_GET(0));
  iss_lsu_store_perf(iss, insn, REG_GET(0), 4, REG_IN(1));
  IN_REG_SET(0, new_val);
  return insn->next;
}



static inline iss_insn_t *p_avgu_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_AVGU, REG_GET(0), REG_GET(1)));
  //setRegDelayed(cpu, pc->outReg[0], value, 2);
  return insn->next;
}



static inline iss_insn_t *p_slet_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, (int32_t)REG_GET(0) <= (int32_t)REG_GET(1));
  return insn->next;
}



static inline iss_insn_t *p_sletu_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, REG_GET(0) <= REG_GET(1));
  return insn->next;
}



static inline iss_insn_t *p_min_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_MINS, REG_GET(0), REG_GET(1)));
  //setRegDelayed(cpu, pc->outReg[0], value, 2);
  return insn->next;
}



static inline iss_insn_t *p_minu_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_MINU, REG_GET(0), REG_GET(1)));
  //setRegDelayed(cpu, pc->outReg[0], value, 2);
  return insn->next;
}



static inline iss_insn_t *p_max_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_MAXS, REG_GET(0), REG_GET(1)));
  //setRegDelayed(cpu, pc->outReg[0], value, 2);
  return insn->next;
}



static inline iss_insn_t *p_maxu_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_MAXU, REG_GET(0), REG_GET(1)));
  //setRegDelayed(cpu, pc->outReg[0], value, 2);
  return insn->next;
}



static inline iss_insn_t *p_ror_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_ROR, REG_GET(0), REG_GET(1)));
  //setRegDelayed(cpu, pc->outReg[0], value, 2);
  return insn->next;
}



static inline iss_insn_t *p_ff1_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL1(lib_FF1, REG_GET(0)));
  //setRegDelayed(cpu, pc->outReg[0], value, 2);
  return insn->next;
}



static inline iss_insn_t *p_fl1_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL1(lib_FL1, REG_GET(0)));
  //setRegDelayed(cpu, pc->outReg[0], value, 2);
  return insn->next;
}



static inline iss_insn_t *p_clb_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL1(lib_CLB, REG_GET(0)));
  //setRegDelayed(cpu, pc->outReg[0], value, 2);
  return insn->next;
}



static inline iss_insn_t *p_cnt_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL1(lib_CNT, REG_GET(0)));
  //setRegDelayed(cpu, pc->outReg[0], value, 2);
  return insn->next;
}



static inline iss_insn_t *p_exths_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, iss_get_signed_value(REG_GET(0), 16));
  return insn->next;
}



static inline iss_insn_t *p_exthz_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, iss_get_field(REG_GET(0), 0, 16));
  return insn->next;
}



static inline iss_insn_t *p_extbs_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, iss_get_signed_value(REG_GET(0), 8));
  return insn->next;
}



static inline iss_insn_t *p_extbz_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, iss_get_field(REG_GET(0), 0, 8));
  return insn->next;
}



static inline iss_insn_t *hwloop_check_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_reg_t pc = insn->addr;

  // First execute the instructions as it is the last one of the loop body.
  // The real handler has been saved when the loop was started.
  iss_insn_t *insn_next = iss_exec_insn_handler(iss, insn, insn->hwloop_handler);

  // First check HW loop 0 as it has higher priority compared to HW loop 1
  if (iss->cpu.pulpv2.hwloop_regs[PULPV2_HWLOOP_LPCOUNT0] && iss->cpu.pulpv2.hwloop_regs[PULPV2_HWLOOP_LPEND0] == pc)
  {
    iss->cpu.pulpv2.hwloop_regs[PULPV2_HWLOOP_LPCOUNT0]--;
    iss_decoder_msg(iss, "Reached end of HW loop (index: 0, loop count: %d)\n", iss->cpu.pulpv2.hwloop_regs[PULPV2_HWLOOP_LPCOUNT0]);

    // If counter is not zero, we must jump back to beginning of the loop.
    if (iss->cpu.pulpv2.hwloop_regs[PULPV2_HWLOOP_LPCOUNT0]) return iss->cpu.state.hwloop_start_insn[0];
  }

  // We get here either if HW loop 0 was not active or if the counter reached 0.
  // In both cases, HW loop 1 can jump back to the beginning of the loop.
  if (iss->cpu.pulpv2.hwloop_regs[PULPV2_HWLOOP_LPCOUNT1] && iss->cpu.pulpv2.hwloop_regs[PULPV2_HWLOOP_LPEND1] == pc)
  {
    iss->cpu.pulpv2.hwloop_regs[PULPV2_HWLOOP_LPCOUNT1]--;
    // If counter is not zero, we must jump back to beginning of the loop.
    iss_decoder_msg(iss, "Reached end of HW loop (index: 1, loop count: %d)\n", iss->cpu.pulpv2.hwloop_regs[PULPV2_HWLOOP_LPCOUNT1]);
    if (iss->cpu.pulpv2.hwloop_regs[PULPV2_HWLOOP_LPCOUNT1]) return iss->cpu.state.hwloop_start_insn[1];
  }

  // In case no HW loop jumped back, just continue with the next instruction.
  return insn_next;
}

static inline void hwloop_set_start(iss_t *iss, iss_insn_t *insn, int index, iss_reg_t start)
{
  iss->cpu.pulpv2.hwloop_regs[PULPV2_HWLOOP_LPSTART(index)] = start;
  iss->cpu.state.hwloop_start_insn[index] = insn_cache_get(iss, start);  
}

static inline void hwloop_set_end(iss_t *iss, iss_insn_t *insn, int index, iss_reg_t end)
{
  iss_insn_t *end_insn = insn_cache_get_decoded(iss, end);

  if (end_insn->hwloop_handler == NULL)
  {
    end_insn->hwloop_handler = end_insn->handler;
    end_insn->handler = hwloop_check_exec;
    end_insn->fast_handler = hwloop_check_exec;
  }

  iss->cpu.pulpv2.hwloop_regs[PULPV2_HWLOOP_LPEND(index)] = end;
}

static inline void hwloop_set_count(iss_t *iss, iss_insn_t *insn, int index, iss_reg_t count)
{
  iss->cpu.pulpv2.hwloop_regs[PULPV2_HWLOOP_LPCOUNT(index)] = count;
}

static inline void hwloop_set_all(iss_t *iss, iss_insn_t *insn, int index, iss_reg_t start, iss_reg_t end, iss_reg_t count)
{
  hwloop_set_end(iss, insn, index, end);
  hwloop_set_start(iss, insn, index, start);
  hwloop_set_count(iss, insn, index, count);
}

static inline iss_insn_t *lp_starti_exec(iss_t *iss, iss_insn_t *insn)
{
  hwloop_set_start(iss, insn, UIM_GET(0), insn->addr + (UIM_GET(1) << 1));
  return insn->next;
}



static inline iss_insn_t *lp_endi_exec(iss_t *iss, iss_insn_t *insn)
{
  hwloop_set_end(iss, insn, UIM_GET(0), insn->addr + (UIM_GET(1) << 1));
  return insn->next;
}



static inline iss_insn_t *lp_count_exec(iss_t *iss, iss_insn_t *insn)
{
  hwloop_set_count(iss, insn, UIM_GET(0), REG_GET(0));
  return insn->next;
}



static inline iss_insn_t *lp_counti_exec(iss_t *iss, iss_insn_t *insn)
{
  hwloop_set_count(iss, insn, UIM_GET(0), UIM_GET(1));
  return insn->next;
}



static inline iss_insn_t *lp_setup_exec(iss_t *iss, iss_insn_t *insn)
{
  int index = UIM_GET(0);
  iss_reg_t count = REG_GET(0);
  iss_reg_t start = insn->addr + insn->size;
  iss_reg_t end = insn->addr + (UIM_GET(1) << 1);

  hwloop_set_all(iss, insn, index, start, end, count);

  return insn->next;
}


static inline iss_insn_t *lp_setupi_exec(iss_t *iss, iss_insn_t *insn)
{
  int index = UIM_GET(0);
  iss_reg_t count = UIM_GET(1);
  iss_reg_t start = insn->addr + insn->size;
  iss_reg_t end = insn->addr + (UIM_GET(2) << 1);

  hwloop_set_all(iss, insn, index, start, end, count);

  return insn->next;
}











static inline iss_insn_t *p_abs_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL1(lib_ABS, REG_GET(0)));
  return insn->next;
}



static inline iss_insn_t *SB_RR_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  iss_lsu_store(iss, insn, REG_GET(0) + REG_GET(2), 1, REG_IN(1));
  return insn->next;
}

static inline iss_insn_t *SB_RR_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_lsu_check_stack_access(iss, REG_IN(0), REG_GET(0) + REG_GET(2));
  iss_lsu_check_stack_access(iss, REG_IN(2), REG_GET(0) + REG_GET(2));
  iss_lsu_store_perf(iss, insn, REG_GET(0) + REG_GET(2), 1, REG_IN(1));
  return insn->next;
}



static inline iss_insn_t *SH_RR_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  iss_lsu_store(iss, insn, REG_GET(0) + REG_GET(2), 2, REG_IN(1));
  return insn->next;
}

static inline iss_insn_t *SH_RR_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_lsu_check_stack_access(iss, REG_IN(0), REG_GET(0) + REG_GET(2));
  iss_lsu_check_stack_access(iss, REG_IN(2), REG_GET(0) + REG_GET(2));
  iss_lsu_store_perf(iss, insn, REG_GET(0) + REG_GET(2), 2, REG_IN(1));
  return insn->next;
}



static inline iss_insn_t *SW_RR_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  iss_lsu_store(iss, insn, REG_GET(0) + REG_GET(2), 4, REG_IN(1));
  return insn->next;
}

static inline iss_insn_t *SW_RR_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_lsu_check_stack_access(iss, REG_IN(0), REG_GET(0) + REG_GET(2));
  iss_lsu_check_stack_access(iss, REG_IN(2), REG_GET(0) + REG_GET(2));
  iss_lsu_store_perf(iss, insn, REG_GET(0) + REG_GET(2), 4, REG_IN(1));
  return insn->next;
}



static inline iss_insn_t *p_elw_exec(iss_t *iss, iss_insn_t *insn)
{
  uint32_t value = 0;
  iss->cpu.state.elw_insn = insn;
  iss_lsu_elw_perf(iss, insn, REG_GET(0) + SIM_GET(0), 4, REG_OUT(0));
  if (iss->cpu.state.insn_cycles != -1)
    iss->cpu.state.elw_insn = NULL;
  return insn->next;
}


#define PV_OP_RS_EXEC(insn_name,lib_name)                                           \
static inline iss_insn_t *pv_##insn_name##_h_exec(iss_t *iss, iss_insn_t *insn)                \
{                                                                                            \
  REG_SET(0, LIB_CALL2(lib_VEC_##lib_name##_int16_t_to_int32_t, REG_GET(0), REG_GET(1)));    \
  return insn->next;                                                                         \
}                                                                                            \
                                                                                             \
static inline iss_insn_t *pv_##insn_name##_sc_h_exec(iss_t *iss, iss_insn_t *insn)             \
{                                                                                            \
  REG_SET(0, LIB_CALL2(lib_VEC_##lib_name##_SC_int16_t_to_int32_t, REG_GET(0), REG_GET(1))); \
  return insn->next;                                                                         \
}                                                                                            \
                                                                                             \
static inline iss_insn_t *pv_##insn_name##_sci_h_exec(iss_t *iss, iss_insn_t *insn)            \
{                                                                                            \
  REG_SET(0, LIB_CALL2(lib_VEC_##lib_name##_SC_int16_t_to_int32_t, REG_GET(0), SIM_GET(0))); \
  return insn->next;                                                                         \
}                                                                                            \
                                                                                             \
static inline iss_insn_t *pv_##insn_name##_b_exec(iss_t *iss, iss_insn_t *insn)                \
{                                                                                            \
  REG_SET(0, LIB_CALL2(lib_VEC_##lib_name##_int8_t_to_int32_t, REG_GET(0), REG_GET(1)));    \
  return insn->next;                                                                         \
}                                                                                            \
                                                                                             \
static inline iss_insn_t *pv_##insn_name##_sc_b_exec(iss_t *iss, iss_insn_t *insn)             \
{                                                                                            \
  REG_SET(0, LIB_CALL2(lib_VEC_##lib_name##_SC_int8_t_to_int32_t, REG_GET(0), REG_GET(1))); \
  return insn->next;                                                                         \
}                                                                                            \
                                                                                             \
static inline iss_insn_t *pv_##insn_name##_sci_b_exec(iss_t *iss, iss_insn_t *insn)            \
{                                                                                            \
  REG_SET(0, LIB_CALL2(lib_VEC_##lib_name##_SC_int8_t_to_int32_t, REG_GET(0), SIM_GET(0))); \
  return insn->next;                                                                         \
}



#define PV_OP_RU_EXEC(insn_name,lib_name)                                           \
static inline iss_insn_t *pv_##insn_name##_h_exec(iss_t *iss, iss_insn_t *insn)                \
{                                                                                            \
  REG_SET(0, LIB_CALL2(lib_VEC_##lib_name##_uint16_t_to_uint32_t, REG_GET(0), REG_GET(1)));    \
  return insn->next;                                                                         \
}                                                                                            \
                                                                                             \
static inline iss_insn_t *pv_##insn_name##_sc_h_exec(iss_t *iss, iss_insn_t *insn)             \
{                                                                                            \
  REG_SET(0, LIB_CALL2(lib_VEC_##lib_name##_SC_uint16_t_to_uint32_t, REG_GET(0), REG_GET(1))); \
  return insn->next;                                                                         \
}                                                                                            \
                                                                                             \
static inline iss_insn_t *pv_##insn_name##_sci_h_exec(iss_t *iss, iss_insn_t *insn)            \
{                                                                                            \
  REG_SET(0, LIB_CALL2(lib_VEC_##lib_name##_SC_uint16_t_to_uint32_t, REG_GET(0), UIM_GET(0))); \
  return insn->next;                                                                         \
}                                                                                            \
                                                                                             \
static inline iss_insn_t *pv_##insn_name##_b_exec(iss_t *iss, iss_insn_t *insn)                \
{                                                                                            \
  REG_SET(0, LIB_CALL2(lib_VEC_##lib_name##_uint8_t_to_uint32_t, REG_GET(0), REG_GET(1)));    \
  return insn->next;                                                                         \
}                                                                                            \
                                                                                             \
static inline iss_insn_t *pv_##insn_name##_sc_b_exec(iss_t *iss, iss_insn_t *insn)             \
{                                                                                            \
  REG_SET(0, LIB_CALL2(lib_VEC_##lib_name##_SC_uint8_t_to_uint32_t, REG_GET(0), REG_GET(1))); \
  return insn->next;                                                                         \
}                                                                                            \
                                                                                             \
static inline iss_insn_t *pv_##insn_name##_sci_b_exec(iss_t *iss, iss_insn_t *insn)            \
{                                                                                            \
  REG_SET(0, LIB_CALL2(lib_VEC_##lib_name##_SC_uint8_t_to_uint32_t, REG_GET(0), UIM_GET(0))); \
  return insn->next;                                                                         \
}


#define PV_OP_RS_EXEC2(insn_name,lib_name)                                           \
static inline iss_insn_t *pv_##insn_name##_h_exec(iss_t *iss, iss_insn_t *insn)                \
{                                                                                            \
  REG_SET(0, LIB_CALL2(lib_VEC_##lib_name##_16, REG_GET(0), REG_GET(1)));    \
  return insn->next;                                                                         \
}                                                                                            \
                                                                                             \
static inline iss_insn_t *pv_##insn_name##_h_sc_exec(iss_t *iss, iss_insn_t *insn)             \
{                                                                                            \
  REG_SET(0, LIB_CALL2(lib_VEC_##lib_name##_SC_16, REG_GET(0), REG_GET(1))); \
  return insn->next;                                                                         \
}                                                                                            \
                                                                                             \
static inline iss_insn_t *pv_##insn_name##_h_sci_exec(iss_t *iss, iss_insn_t *insn)            \
{                                                                                            \
  REG_SET(0, LIB_CALL2(lib_VEC_##lib_name##_SC_16, REG_GET(0), SIM_GET(0))); \
  return insn->next;                                                                         \
}                                                                                            \
                                                                                             \
static inline iss_insn_t *pv_##insn_name##_b_exec(iss_t *iss, iss_insn_t *insn)                \
{                                                                                            \
  REG_SET(0, LIB_CALL2(lib_VEC_##lib_name##_8, REG_GET(0), REG_GET(1)));    \
  return insn->next;                                                                         \
}                                                                                            \
                                                                                             \
static inline iss_insn_t *pv_##insn_name##_b_sc_exec(iss_t *iss, iss_insn_t *insn)             \
{                                                                                            \
  REG_SET(0, LIB_CALL2(lib_VEC_##lib_name##_SC_8, REG_GET(0), REG_GET(1))); \
  return insn->next;                                                                         \
}                                                                                            \
                                                                                             \
static inline iss_insn_t *pv_##insn_name##_b_sci_exec(iss_t *iss, iss_insn_t *insn)            \
{                                                                                            \
  REG_SET(0, LIB_CALL2(lib_VEC_##lib_name##_SC_8, REG_GET(0), SIM_GET(0))); \
  return insn->next;                                                                         \
}



#define PV_OP_RU_EXEC2(insn_name,lib_name)                                           \
static inline iss_insn_t *pv_##insn_name##_h_exec(iss_t *iss, iss_insn_t *insn)                \
{                                                                                            \
  REG_SET(0, LIB_CALL2(lib_VEC_##lib_name##_16, REG_GET(0), REG_GET(1)));    \
  return insn->next;                                                                         \
}                                                                                            \
                                                                                             \
static inline iss_insn_t *pv_##insn_name##_h_sc_exec(iss_t *iss, iss_insn_t *insn)             \
{                                                                                            \
  REG_SET(0, LIB_CALL2(lib_VEC_##lib_name##_SC_16, REG_GET(0), REG_GET(1))); \
  return insn->next;                                                                         \
}                                                                                            \
                                                                                             \
static inline iss_insn_t *pv_##insn_name##_h_sci_exec(iss_t *iss, iss_insn_t *insn)            \
{                                                                                            \
  REG_SET(0, LIB_CALL2(lib_VEC_##lib_name##_SC_16, REG_GET(0), UIM_GET(0))); \
  return insn->next;                                                                         \
}                                                                                            \
                                                                                             \
static inline iss_insn_t *pv_##insn_name##_b_exec(iss_t *iss, iss_insn_t *insn)                \
{                                                                                            \
  REG_SET(0, LIB_CALL2(lib_VEC_##lib_name##_8, REG_GET(0), REG_GET(1)));    \
  return insn->next;                                                                         \
}                                                                                            \
                                                                                             \
static inline iss_insn_t *pv_##insn_name##_b_sc_exec(iss_t *iss, iss_insn_t *insn)             \
{                                                                                            \
  REG_SET(0, LIB_CALL2(lib_VEC_##lib_name##_SC_8, REG_GET(0), REG_GET(1))); \
  return insn->next;                                                                         \
}                                                                                            \
                                                                                             \
static inline iss_insn_t *pv_##insn_name##_b_sci_exec(iss_t *iss, iss_insn_t *insn)            \
{                                                                                            \
  REG_SET(0, LIB_CALL2(lib_VEC_##lib_name##_SC_8, REG_GET(0), UIM_GET(0))); \
  return insn->next;                                                                         \
}


#define PV_OP_RRS_EXEC2(insn_name,lib_name)                                           \
static inline iss_insn_t *pv_##insn_name##_h_exec(iss_t *iss, iss_insn_t *insn)                \
{                                                                                            \
  REG_SET(0, LIB_CALL3(lib_VEC_##lib_name##_16, REG_GET(2), REG_GET(0), REG_GET(1)));    \
  return insn->next;                                                                         \
}                                                                                            \
                                                                                             \
static inline iss_insn_t *pv_##insn_name##_h_sc_exec(iss_t *iss, iss_insn_t *insn)             \
{                                                                                            \
  REG_SET(0, LIB_CALL3(lib_VEC_##lib_name##_SC_16, REG_GET(2), REG_GET(0), REG_GET(1))); \
  return insn->next;                                                                         \
}                                                                                            \
                                                                                             \
static inline iss_insn_t *pv_##insn_name##_h_sci_exec(iss_t *iss, iss_insn_t *insn)            \
{                                                                                            \
  REG_SET(0, LIB_CALL3(lib_VEC_##lib_name##_SC_16, REG_GET(0), REG_GET(1), SIM_GET(0))); \
  return insn->next;                                                                         \
}                                                                                            \
                                                                                             \
static inline iss_insn_t *pv_##insn_name##_b_exec(iss_t *iss, iss_insn_t *insn)                \
{                                                                                            \
  REG_SET(0, LIB_CALL3(lib_VEC_##lib_name##_8, REG_GET(2), REG_GET(0), REG_GET(1)));    \
  return insn->next;                                                                         \
}                                                                                            \
                                                                                             \
static inline iss_insn_t *pv_##insn_name##_b_sc_exec(iss_t *iss, iss_insn_t *insn)             \
{                                                                                            \
  REG_SET(0, LIB_CALL3(lib_VEC_##lib_name##_SC_8, REG_GET(2), REG_GET(0), REG_GET(1))); \
  return insn->next;                                                                         \
}                                                                                            \
                                                                                             \
static inline iss_insn_t *pv_##insn_name##_b_sci_exec(iss_t *iss, iss_insn_t *insn)            \
{                                                                                            \
  REG_SET(0, LIB_CALL3(lib_VEC_##lib_name##_SC_8, REG_GET(0), REG_GET(1), SIM_GET(0))); \
  return insn->next;                                                                         \
}



#define PV_OP_RRU_EXEC2(insn_name,lib_name)                                           \
static inline iss_insn_t *pv_##insn_name##_h_exec(iss_t *iss, iss_insn_t *insn)                \
{                                                                                            \
  REG_SET(0, LIB_CALL3(lib_VEC_##lib_name##_16, REG_GET(2), REG_GET(0), REG_GET(1)));    \
  return insn->next;                                                                         \
}                                                                                            \
                                                                                             \
static inline iss_insn_t *pv_##insn_name##_h_sc_exec(iss_t *iss, iss_insn_t *insn)             \
{                                                                                            \
  REG_SET(0, LIB_CALL3(lib_VEC_##lib_name##_SC_16, REG_GET(2), REG_GET(0), REG_GET(1))); \
  return insn->next;                                                                         \
}                                                                                            \
                                                                                             \
static inline iss_insn_t *pv_##insn_name##_h_sci_exec(iss_t *iss, iss_insn_t *insn)            \
{                                                                                            \
  REG_SET(0, LIB_CALL3(lib_VEC_##lib_name##_SC_16, REG_GET(0), REG_GET(1), UIM_GET(0))); \
  return insn->next;                                                                         \
}                                                                                            \
                                                                                             \
static inline iss_insn_t *pv_##insn_name##_b_exec(iss_t *iss, iss_insn_t *insn)                \
{                                                                                            \
  REG_SET(0, LIB_CALL3(lib_VEC_##lib_name##_8, REG_GET(2), REG_GET(0), REG_GET(1)));    \
  return insn->next;                                                                         \
}                                                                                            \
                                                                                             \
static inline iss_insn_t *pv_##insn_name##_b_sc_exec(iss_t *iss, iss_insn_t *insn)             \
{                                                                                            \
  REG_SET(0, LIB_CALL3(lib_VEC_##lib_name##_SC_8, REG_GET(2), REG_GET(0), REG_GET(1))); \
  return insn->next;                                                                         \
}                                                                                            \
                                                                                             \
static inline iss_insn_t *pv_##insn_name##_b_sci_exec(iss_t *iss, iss_insn_t *insn)            \
{                                                                                            \
  REG_SET(0, LIB_CALL3(lib_VEC_##lib_name##_SC_8, REG_GET(0), REG_GET(1), UIM_GET(0))); \
  return insn->next;                                                                         \
}




#define PV_OP1_RS_EXEC(insn_name,lib_name)                          \
static inline iss_insn_t *pv_##insn_name##_h_exec(iss_t *iss, iss_insn_t *insn)      \
{                                                                                  \
  REG_SET(0, LIB_CALL1(lib_VEC_##lib_name##_int16_t_to_int32_t, REG_GET(0)));    \
  return insn->next;                                                               \
}                                                                                  \
                                                                                   \
static inline iss_insn_t *pv_##insn_name##_b_exec(iss_t *iss, iss_insn_t *insn)      \
{                                                                                  \
  REG_SET(0, LIB_CALL1(lib_VEC_##lib_name##_int8_t_to_int32_t, REG_GET(0)));    \
  return insn->next;                                                               \
}



PV_OP_RS_EXEC(add,ADD)

PV_OP_RS_EXEC(sub,SUB)

PV_OP_RS_EXEC(avg,AVG)

PV_OP_RU_EXEC(avgu,AVGU)

PV_OP_RS_EXEC(min,MIN)

PV_OP_RU_EXEC(minu,MINU)

PV_OP_RS_EXEC(max,MAX)

PV_OP_RU_EXEC(maxu,MAXU)

PV_OP_RU_EXEC(srl,SRL)

PV_OP_RS_EXEC(sra,SRA)

PV_OP_RU_EXEC(sll,SLL)

PV_OP_RS_EXEC(or,OR)

PV_OP_RS_EXEC(xor,XOR)

PV_OP_RS_EXEC(and,AND)

PV_OP1_RS_EXEC(abs,ABS)



static inline iss_insn_t *pv_extract_h_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_VEC_EXT_16, REG_GET(0), UIM_GET(0)));
  return insn->next;
}



static inline iss_insn_t *pv_extract_b_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_VEC_EXT_8, REG_GET(0), UIM_GET(0)));
  return insn->next;
}



static inline iss_insn_t *pv_extractu_h_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_VEC_EXTU_16, REG_GET(0), UIM_GET(0)));
  return insn->next;
}



static inline iss_insn_t *pv_extractu_b_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_VEC_EXTU_8, REG_GET(0), UIM_GET(0)));
  return insn->next;
}



static inline iss_insn_t *pv_insert_h_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL3(lib_VEC_INS_16, REG_GET(0), REG_GET(1), UIM_GET(0)));
  return insn->next;
}



static inline iss_insn_t *pv_insert_b_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL3(lib_VEC_INS_8, REG_GET(0), REG_GET(1), UIM_GET(0)));
  return insn->next;
}



PV_OP_RS_EXEC2(dotsp,DOTSP)

PV_OP_RU_EXEC2(dotup,DOTUP)

PV_OP_RS_EXEC2(dotusp,DOTUSP)

PV_OP_RRS_EXEC2(sdotsp,SDOTSP)

PV_OP_RRU_EXEC2(sdotup,SDOTUP)

PV_OP_RRS_EXEC2(sdotusp,SDOTUSP)



static inline iss_insn_t *pv_shuffle_h_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_VEC_SHUFFLE_16, REG_GET(0), REG_GET(1)));
  return insn->next;
}

static inline iss_insn_t *pv_shuffle_h_sci_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_VEC_SHUFFLE_SCI_16, REG_GET(0), UIM_GET(0)));
  return insn->next;
}



static inline iss_insn_t *pv_shuffle_b_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_VEC_SHUFFLE_8, REG_GET(0), REG_GET(1)));
  return insn->next;
}

static inline iss_insn_t *pv_shufflei0_b_sci_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_VEC_SHUFFLEI0_SCI_8, REG_GET(0), UIM_GET(0)));
  return insn->next;
}

static inline iss_insn_t *pv_shufflei1_b_sci_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_VEC_SHUFFLEI1_SCI_8, REG_GET(0), UIM_GET(0)));
  return insn->next;
}

static inline iss_insn_t *pv_shufflei2_b_sci_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_VEC_SHUFFLEI2_SCI_8, REG_GET(0), UIM_GET(0)));
  return insn->next;
}

static inline iss_insn_t *pv_shufflei3_b_sci_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_VEC_SHUFFLEI3_SCI_8, REG_GET(0), UIM_GET(0)));
  return insn->next;
}



static inline iss_insn_t *pv_shuffle2_h_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL3(lib_VEC_SHUFFLE2_16, REG_GET(0), REG_GET(1), REG_GET(2)));
  return insn->next;
}

static inline iss_insn_t *pv_shuffle2_b_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL3(lib_VEC_SHUFFLE2_8, REG_GET(0), REG_GET(1), REG_GET(2)));
  return insn->next;
}



static inline iss_insn_t *pv_pack_h_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_VEC_PACK_SC_16, REG_GET(0), REG_GET(1)));
  return insn->next;
}

static inline iss_insn_t *pv_packhi_b_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL3(lib_VEC_PACKHI_SC_8, REG_GET(0), REG_GET(1), REG_GET(2)));
  return insn->next;
}

static inline iss_insn_t *pv_packlo_b_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL3(lib_VEC_PACKLO_SC_8, REG_GET(0), REG_GET(1), REG_GET(2)));
  return insn->next;
}



PV_OP_RS_EXEC(cmpeq,CMPEQ)

PV_OP_RS_EXEC(cmpne,CMPNE)

PV_OP_RS_EXEC(cmpgt,CMPGT)

PV_OP_RS_EXEC(cmpge,CMPGE)

PV_OP_RS_EXEC(cmplt,CMPLT)

PV_OP_RS_EXEC(cmple,CMPLE)

PV_OP_RU_EXEC(cmpgtu,CMPGTU)

PV_OP_RU_EXEC(cmpgeu,CMPGEU)

PV_OP_RU_EXEC(cmpltu,CMPLTU)

PV_OP_RU_EXEC(cmpleu,CMPLEU)



static inline iss_insn_t *p_bneimm_exec_common(iss_t *iss, iss_insn_t *insn, int perf)
{
  if ((int32_t)REG_GET(0) != SIM_GET(1))
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

static inline iss_insn_t *p_bneimm_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  return p_bneimm_exec_common(iss, insn, 0);
}

static inline iss_insn_t *p_bneimm_exec(iss_t *iss, iss_insn_t *insn)
{
  return p_bneimm_exec_common(iss, insn, 1);
}



static inline iss_insn_t *p_beqimm_exec_common(iss_t *iss, iss_insn_t *insn, int perf)
{
  if ((int32_t)REG_GET(0) == SIM_GET(1))
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

static inline iss_insn_t *p_beqimm_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  return p_beqimm_exec_common(iss, insn, 0);
}

static inline iss_insn_t *p_beqimm_exec(iss_t *iss, iss_insn_t *insn)
{
  return p_beqimm_exec_common(iss, insn, 1);
}




static inline iss_insn_t *p_mac_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL3(lib_MAC, REG_GET(2), REG_GET(0), REG_GET(1)));
  return insn->next;
}



static inline iss_insn_t *p_msu_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL3(lib_MSU, REG_GET(2), REG_GET(0), REG_GET(1)));
  return insn->next;
}



static inline iss_insn_t *p_mul_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_MULS, REG_GET(0), REG_GET(1)));
  return insn->next;
}



static inline iss_insn_t *p_muls_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_MUL_SL_SL, REG_GET(0), REG_GET(1)));
  return insn->next;
}

static inline iss_insn_t *p_mulhhs_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_MUL_SH_SH, REG_GET(0), REG_GET(1)));
  return insn->next;
}

static inline iss_insn_t *p_mulsN_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL3(lib_MUL_SL_SL_NR, REG_GET(0), REG_GET(1), UIM_GET(0)));
  return insn->next;
}

static inline iss_insn_t *p_mulhhsN_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL3(lib_MUL_SH_SH_NR, REG_GET(0), REG_GET(1), UIM_GET(0)));
  return insn->next;
}

static inline iss_insn_t *p_mulsNR_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL3(lib_MUL_SL_SL_NR_R, REG_GET(0), REG_GET(1), UIM_GET(0)));
  return insn->next;
}

static inline iss_insn_t *p_mulhhsNR_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL3(lib_MUL_SH_SH_NR_R, REG_GET(0), REG_GET(1), UIM_GET(0)));
  return insn->next;
}



static inline iss_insn_t *p_mulu_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_MUL_ZL_ZL, REG_GET(0), REG_GET(1)));
  return insn->next;
}

static inline iss_insn_t *p_mulhhu_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_MUL_ZH_ZH, REG_GET(0), REG_GET(1)));
  return insn->next;
}

static inline iss_insn_t *p_muluN_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL3(lib_MUL_ZL_ZL_NR, REG_GET(0), REG_GET(1), UIM_GET(0)));
  return insn->next;
}

static inline iss_insn_t *p_mulhhuN_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL3(lib_MUL_ZH_ZH_NR, REG_GET(0), REG_GET(1), UIM_GET(0)));
  return insn->next;
}

static inline iss_insn_t *p_muluNR_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL3(lib_MUL_ZL_ZL_NR_R, REG_GET(0), REG_GET(1), UIM_GET(0)));
  return insn->next;
}

static inline iss_insn_t *p_mulhhuNR_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL3(lib_MUL_ZH_ZH_NR_R, REG_GET(0), REG_GET(1), UIM_GET(0)));
  return insn->next;
}



static inline iss_insn_t *p_macs_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL3(lib_MAC_SL_SL, REG_GET(2), REG_GET(0), REG_GET(1)));
  return insn->next;
}

static inline iss_insn_t *p_machhs_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL3(lib_MAC_SH_SH, REG_GET(2), REG_GET(0), REG_GET(1)));
  return insn->next;
}

static inline iss_insn_t *p_macsN_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL4(lib_MAC_SL_SL_NR, REG_GET(2), REG_GET(0), REG_GET(1), UIM_GET(0)));
  return insn->next;
}

static inline iss_insn_t *p_machhsN_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL4(lib_MAC_SH_SH_NR, REG_GET(2), REG_GET(0), REG_GET(1), UIM_GET(0)));
  return insn->next;
}

static inline iss_insn_t *p_macsNR_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL4(lib_MAC_SL_SL_NR_R, REG_GET(2), REG_GET(0), REG_GET(1), UIM_GET(0)));
  return insn->next;
}

static inline iss_insn_t *p_machhsNR_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL4(lib_MAC_SH_SH_NR_R, REG_GET(2), REG_GET(0), REG_GET(1), UIM_GET(0)));
  return insn->next;
}



static inline iss_insn_t *p_macu_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL3(lib_MAC_ZL_ZL, REG_GET(2), REG_GET(0), REG_GET(1)));
  return insn->next;
}

static inline iss_insn_t *p_machhu_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL3(lib_MAC_ZH_ZH, REG_GET(2), REG_GET(0), REG_GET(1)));
  return insn->next;
}

static inline iss_insn_t *p_macuN_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL4(lib_MAC_ZL_ZL_NR, REG_GET(2), REG_GET(0), REG_GET(1), UIM_GET(0)));
  return insn->next;
}

static inline iss_insn_t *p_machhuN_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL4(lib_MAC_ZH_ZH_NR, REG_GET(2), REG_GET(0), REG_GET(1), UIM_GET(0)));
  return insn->next;
}

static inline iss_insn_t *p_macuNR_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL4(lib_MAC_ZL_ZL_NR_R, REG_GET(2), REG_GET(0), REG_GET(1), UIM_GET(0)));
  return insn->next;
}

static inline iss_insn_t *p_machhuNR_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL4(lib_MAC_ZH_ZH_NR_R, REG_GET(2), REG_GET(0), REG_GET(1), UIM_GET(0)));
  return insn->next;
}



static inline iss_insn_t *p_addNi_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL3(lib_ADD_NR, REG_GET(0), REG_GET(1), UIM_GET(0)));
  return insn->next;
}

static inline iss_insn_t *p_adduNi_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL3(lib_ADD_NRU, REG_GET(0), REG_GET(1), UIM_GET(0)));
  return insn->next;
}

static inline iss_insn_t *p_addRNi_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL3(lib_ADD_NR_R, REG_GET(0), REG_GET(1), UIM_GET(0)));
  return insn->next;
}

static inline iss_insn_t *p_adduRNi_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL3(lib_ADD_NR_RU, REG_GET(0), REG_GET(1), UIM_GET(0)));
  return insn->next;
}



static inline iss_insn_t *p_subNi_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL3(lib_SUB_NR, REG_GET(0), REG_GET(1), UIM_GET(0)));
  return insn->next;
}

static inline iss_insn_t *p_subuNi_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL3(lib_SUB_NRU, REG_GET(0), REG_GET(1), UIM_GET(0)));
  return insn->next;
}

static inline iss_insn_t *p_subRNi_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL3(lib_SUB_NR_R, REG_GET(0), REG_GET(1), UIM_GET(0)));
  return insn->next;
}

static inline iss_insn_t *p_subuRNi_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL3(lib_SUB_NR_RU, REG_GET(0), REG_GET(1), UIM_GET(0)));
  return insn->next;
}



static inline iss_insn_t *p_addN_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL3(lib_ADD_NR, REG_GET(0), REG_GET(1), REG_GET(2)));
  return insn->next;
}

static inline iss_insn_t *p_adduN_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL3(lib_ADD_NRU, REG_GET(0), REG_GET(1), REG_GET(2)));
  return insn->next;
}

static inline iss_insn_t *p_addRN_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL3(lib_ADD_NR_R, REG_GET(0), REG_GET(1), REG_GET(2)));
  return insn->next;
}

static inline iss_insn_t *p_adduRN_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL3(lib_ADD_NR_RU, REG_GET(0), REG_GET(1), REG_GET(2)));
  return insn->next;
}



static inline iss_insn_t *p_subN_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL3(lib_SUB_NR, REG_GET(0), REG_GET(1), REG_GET(2)));
  return insn->next;
}

static inline iss_insn_t *p_subuN_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL3(lib_SUB_NRU, REG_GET(0), REG_GET(1), REG_GET(2)));
  return insn->next;
}

static inline iss_insn_t *p_subRN_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL3(lib_SUB_NR_R, REG_GET(0), REG_GET(1), REG_GET(2)));
  return insn->next;
}

static inline iss_insn_t *p_subuRN_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL3(lib_SUB_NR_RU, REG_GET(0), REG_GET(1), REG_GET(2)));
  return insn->next;
}



static inline iss_insn_t *p_clipi_exec(iss_t *iss, iss_insn_t *insn)
{
  int low = (int) -(1 << MAX((int)UIM_GET(0)-1, 0));
  int high = (1 << MAX((int)UIM_GET(0)-1, 0)) - 1 ;
  REG_SET(0, LIB_CALL3(lib_CLIP, REG_GET(0), low, high));
  return insn->next;
}

static inline iss_insn_t *p_clipui_exec(iss_t *iss, iss_insn_t *insn)
{
  int low = 0;
  int high = (1 << MAX((int)UIM_GET(0)-1, 0)) - 1;
  REG_SET(0, LIB_CALL3(lib_CLIP, REG_GET(0), low, high));
  return insn->next;
}

static inline iss_insn_t *p_clip_exec(iss_t *iss, iss_insn_t *insn)
{
  int low = -REG_GET(1) - 1;
  int high = REG_GET(1);
  REG_SET(0, LIB_CALL3(lib_CLIP, REG_GET(0), low, high));
  return insn->next;
}

static inline iss_insn_t *p_clipu_exec(iss_t *iss, iss_insn_t *insn)
{
  int low = 0;
  int high = REG_GET(1);
  REG_SET(0, LIB_CALL3(lib_CLIP, REG_GET(0), low, high));
  return insn->next;
}



static inline iss_insn_t *p_bclri_exec(iss_t *iss, iss_insn_t *insn)
{
  int width = UIM_GET(0) + 1;
  int shift = UIM_GET(1);
  REG_SET(0, LIB_CALL2(lib_BCLR, REG_GET(0), ((1ULL<<width)-1) << shift));

  return insn->next;
}



static inline iss_insn_t *p_bclr_exec(iss_t *iss, iss_insn_t *insn)
{
  int width = ((REG_GET(1) >> 5) & 0x1f) + 1;
  int shift = REG_GET(1) & 0x1f;
  REG_SET(0, LIB_CALL2(lib_BCLR, REG_GET(0), ((1ULL<<width)-1) << shift));
  return insn->next;
}



static inline iss_insn_t *p_extracti_exec(iss_t *iss, iss_insn_t *insn)
{
  int width = UIM_GET(0) + 1;
  int shift = UIM_GET(1);
  REG_SET(0, LIB_CALL3(lib_BEXTRACT, REG_GET(0), width, shift));
  return insn->next;
}



static inline iss_insn_t *p_extractui_exec(iss_t *iss, iss_insn_t *insn)
{
  int width = UIM_GET(0) + 1;
  int shift = UIM_GET(1);
  REG_SET(0, LIB_CALL3(lib_BEXTRACTU, REG_GET(0), ((1ULL << width) - 1) << shift, shift));
  return insn->next;
}



static inline iss_insn_t *p_extract_exec(iss_t *iss, iss_insn_t *insn)
{
  int width = ((REG_GET(1) >> 5) & 0x1f) + 1;
  int shift = REG_GET(1) & 0x1f;
  REG_SET(0, LIB_CALL3(lib_BEXTRACT, REG_GET(0), width, shift));
  return insn->next;
}



static inline iss_insn_t *p_extractu_exec(iss_t *iss, iss_insn_t *insn)
{
  int width = ((REG_GET(1) >> 5) & 0x1f) + 1;
  int shift = REG_GET(1) & 0x1f;
  REG_SET(0, LIB_CALL3(lib_BEXTRACTU, REG_GET(0), ((1ULL << width) - 1) << shift, shift));
  return insn->next;
}



static inline iss_insn_t *p_inserti_exec(iss_t *iss, iss_insn_t *insn)
{
  int width = UIM_GET(0) + 1;
  int shift = UIM_GET(1);
  REG_SET(0, LIB_CALL4(lib_BINSERT, REG_GET(0), REG_GET(1), ((1ULL << width) - 1) << shift, shift));
  return insn->next;
}



static inline iss_insn_t *p_insert_exec(iss_t *iss, iss_insn_t *insn)
{
  int width = ((REG_GET(2) >> 5) & 0x1F) + 1;
  int shift = REG_GET(2) & 0x1F;
  REG_SET(0, LIB_CALL4(lib_BINSERT, REG_GET(0), REG_GET(1), ((1ULL << width) - 1) << shift, shift));
  return insn->next;
}



static inline iss_insn_t *p_bseti_exec(iss_t *iss, iss_insn_t *insn)
{
  int width = UIM_GET(0) + 1;
  int shift = UIM_GET(1);
  REG_SET(0, LIB_CALL2(lib_BSET, REG_GET(0), ((1ULL << (width)) - 1) << shift));
  return insn->next;
}



static inline iss_insn_t *p_bset_exec(iss_t *iss, iss_insn_t *insn)
{
  int width = ((REG_GET(1) >> 5) & 0x1f) + 1;
  int shift = REG_GET(1) & 0x1f;
  REG_SET(0, LIB_CALL2(lib_BSET, REG_GET(0), ((1ULL << (width)) - 1) << shift));
  return insn->next;
}

static inline void iss_isa_pulpv2_init(iss_t *iss)
{
  iss->cpu.pulpv2.hwloop = false;
}

static inline void iss_isa_pulpv2_activate(iss_t *iss)
{
  iss->cpu.pulpv2.hwloop = true;
  iss->cpu.pulpv2.hwloop_regs[PULPV2_HWLOOP_LPCOUNT(0)] = 0;
  iss->cpu.pulpv2.hwloop_regs[PULPV2_HWLOOP_LPCOUNT(1)] = 0;

}

#endif
