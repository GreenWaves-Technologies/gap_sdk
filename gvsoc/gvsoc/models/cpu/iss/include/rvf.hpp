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

#ifndef __CPU_ISS_RVF_HPP
#define __CPU_ISS_RVF_HPP

#include "iss_core.hpp"
#include "isa_lib/int.h"
#include "isa_lib/macros.h"



static inline iss_insn_t *flw_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_lsu_check_stack_access(iss, REG_IN(0), REG_GET(0) + SIM_GET(0));
  iss_lsu_load(iss, insn, REG_GET(0) + SIM_GET(0), 4, REG_OUT(0));
  return insn->next;
}



static inline iss_insn_t *fsw_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_lsu_check_stack_access(iss, REG_OUT(0), REG_GET(0) + SIM_GET(0));
  iss_lsu_store(iss, insn, REG_GET(0) + SIM_GET(0), 4, REG_IN(1));
  return insn->next;
}



static inline iss_insn_t *fmadd_s_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL4(lib_flexfloat_madd_round, REG_GET(0), REG_GET(1), REG_GET(2), 8, 23, UIM_GET(0)));
  return insn->next;
}



static inline iss_insn_t *fmsub_s_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL4(lib_flexfloat_msub_round, REG_GET(0), REG_GET(1), REG_GET(2), 8, 23, UIM_GET(0)));
  return insn->next;
}



static inline iss_insn_t *fnmsub_s_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL4(lib_flexfloat_nmsub_round, REG_GET(0), REG_GET(1), REG_GET(2), 8, 23, UIM_GET(0)));
  return insn->next;
}



static inline iss_insn_t *fnmadd_s_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL4(lib_flexfloat_nmadd_round, REG_GET(0), REG_GET(1), REG_GET(2), 8, 23, UIM_GET(0)));
  return insn->next;
}



static inline iss_insn_t *fadd_s_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL3(lib_flexfloat_add_round, REG_GET(0), REG_GET(1), 8, 23, UIM_GET(0)));
  return insn->next;
}



static inline iss_insn_t *fsub_s_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL3(lib_flexfloat_sub_round, REG_GET(0), REG_GET(1), 8, 23, UIM_GET(0)));
  return insn->next;
}



static inline iss_insn_t *fmul_s_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL3(lib_flexfloat_mul_round, REG_GET(0), REG_GET(1), 8, 23, UIM_GET(0)));
  return insn->next;
}



static inline iss_insn_t *fdiv_s_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL3(lib_flexfloat_div_round, REG_GET(0), REG_GET(1), 8, 23, UIM_GET(0)));
  return insn->next;
}



static inline iss_insn_t *fsqrt_s_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL2(lib_flexfloat_sqrt_round, REG_GET(0), 8, 23, UIM_GET(0)));
  return insn->next;
}



static inline iss_insn_t *fsgnj_s_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL2(lib_flexfloat_sgnj, REG_GET(0), REG_GET(1), 8, 23));
  return insn->next;
}



static inline iss_insn_t *fsgnjn_s_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL2(lib_flexfloat_sgnjn, REG_GET(0), REG_GET(1), 8, 23));
  return insn->next;
}



static inline iss_insn_t *fsgnjx_s_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL2(lib_flexfloat_sgnjx, REG_GET(0), REG_GET(1), 8, 23));
  return insn->next;
}



static inline iss_insn_t *fmin_s_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL2(lib_flexfloat_min, REG_GET(0), REG_GET(1), 8, 23));
  return insn->next;
}



static inline iss_insn_t *fmax_s_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL2(lib_flexfloat_max, REG_GET(0), REG_GET(1), 8, 23));
  return insn->next;
}



static inline iss_insn_t *fcvt_w_s_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL2(lib_flexfloat_cvt_w_ff_round, REG_GET(0), 8, 23, UIM_GET(0)));
  return insn->next;
}



static inline iss_insn_t *fcvt_wu_s_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL2(lib_flexfloat_cvt_wu_ff_round, REG_GET(0), 8, 23, UIM_GET(0)));
  return insn->next;
}



static inline iss_insn_t *fmv_x_s_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL1(lib_flexfloat_fmv_x_ff, REG_GET(0), 8, 23));
  return insn->next;
}



static inline iss_insn_t *fmv_s_x_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL1(lib_flexfloat_fmv_ff_x, REG_GET(0), 8, 23));
  return insn->next;
}



static inline iss_insn_t *feq_s_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL2(lib_flexfloat_eq, REG_GET(0), REG_GET(1), 8, 23));
  return insn->next;
}



static inline iss_insn_t *flt_s_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL2(lib_flexfloat_lt, REG_GET(0), REG_GET(1), 8, 23));
  return insn->next;
}



static inline iss_insn_t *fle_s_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL2(lib_flexfloat_le, REG_GET(0), REG_GET(1), 8, 23));
  return insn->next;
}



static inline iss_insn_t *fclass_s_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL1(lib_flexfloat_class, REG_GET(0), 8, 23));
  return insn->next;
}



static inline iss_insn_t *fcvt_s_w_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL2(lib_flexfloat_cvt_ff_w_round, REG_GET(0), 8, 23, UIM_GET(0)));
  return insn->next;
}



static inline iss_insn_t *fcvt_s_wu_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL2(lib_flexfloat_cvt_ff_wu_round, REG_GET(0), 8, 23, UIM_GET(0)));
  return insn->next;
}


//
// RV64F
//
static inline iss_insn_t *fcvt_l_s_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL2(lib_flexfloat_cvt_l_ff_round, REG_GET(0), 8, 23, UIM_GET(0)));
  return insn->next;
}



static inline iss_insn_t *fcvt_lu_s_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL2(lib_flexfloat_cvt_lu_ff_round, REG_GET(0), 8, 23, UIM_GET(0)));
  return insn->next;
}



static inline iss_insn_t *fcvt_s_l_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL2(lib_flexfloat_cvt_ff_l_round, REG_GET(0), 8, 23, UIM_GET(0)));
  return insn->next;
}



static inline iss_insn_t *fcvt_s_lu_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL2(lib_flexfloat_cvt_ff_lu_round, REG_GET(0), 8, 23, UIM_GET(0)));
  return insn->next;
}


//
// COMPRESSED INSTRUCTIONS
//
static inline iss_insn_t *c_fsw_exec(iss_t *iss, iss_insn_t *insn)
{
  //perfAccountRvc(cpu);
  return fsw_exec(iss, insn);
}



static inline iss_insn_t *c_fswsp_exec(iss_t *iss, iss_insn_t *insn)
{
  //perfAccountRvc(cpu);
  return fsw_exec(iss, insn);
}



static inline iss_insn_t *c_flw_exec(iss_t *iss, iss_insn_t *insn)
{
  //perfAccountRvc(cpu);
  return flw_exec(iss, insn);
}



static inline iss_insn_t *c_flwsp_exec(iss_t *iss, iss_insn_t *insn)
{
  //perfAccountRvc(cpu);
  return flw_exec(iss, insn);
}

#endif
