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
 * Authors: Stefan Mach, ETH (smach@iis.ee.ethz.ch)
 *          Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */

#ifndef __CPU_ISS_RVXF16ALT_HPP
#define __CPU_ISS_RVXF16ALT_HPP

#include "iss_core.hpp"
#include "isa_lib/int.h"
#include "isa_lib/macros.h"



static inline iss_insn_t *flah_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_lsu_check_stack_access(iss, REG_IN(0), REG_GET(0) + SIM_GET(0));
  iss_lsu_load(iss, insn, REG_GET(0) + SIM_GET(0), 2, REG_OUT(0));
  return insn->next;
}


static inline iss_insn_t *fsah_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_lsu_check_stack_access(iss, REG_OUT(0), REG_GET(0) + SIM_GET(0));
  iss_lsu_store(iss, insn, REG_GET(0) + SIM_GET(0), 2, REG_IN(1));
  return insn->next;
}


static inline iss_insn_t *fmadd_ah_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL4(lib_flexfloat_madd_round, REG_GET(0), REG_GET(1), REG_GET(2), 8, 7, UIM_GET(0)));
  return insn->next;
}



static inline iss_insn_t *fmsub_ah_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL4(lib_flexfloat_msub_round, REG_GET(0), REG_GET(1), REG_GET(2), 8, 7, UIM_GET(0)));
  return insn->next;
}



static inline iss_insn_t *fnmsub_ah_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL4(lib_flexfloat_nmsub_round, REG_GET(0), REG_GET(1), REG_GET(2), 8, 7, UIM_GET(0)));
  return insn->next;
}



static inline iss_insn_t *fnmadd_ah_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL4(lib_flexfloat_nmadd_round, REG_GET(0), REG_GET(1), REG_GET(2), 8, 7, UIM_GET(0)));
  return insn->next;
}



static inline iss_insn_t *fadd_ah_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL3(lib_flexfloat_add_round, REG_GET(0), REG_GET(1), 8, 7, UIM_GET(0)));
  return insn->next;
}



static inline iss_insn_t *fsub_ah_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL3(lib_flexfloat_sub_round, REG_GET(0), REG_GET(1), 8, 7, UIM_GET(0)));
  return insn->next;
}



static inline iss_insn_t *fmul_ah_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL3(lib_flexfloat_mul_round, REG_GET(0), REG_GET(1), 8, 7, UIM_GET(0)));
  return insn->next;
}



static inline iss_insn_t *fdiv_ah_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL3(lib_flexfloat_div_round, REG_GET(0), REG_GET(1), 8, 7, UIM_GET(0)));
  return insn->next;
}



static inline iss_insn_t *fsqrt_ah_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL2(lib_flexfloat_sqrt_round, REG_GET(0), 8, 7, UIM_GET(0)));
  return insn->next;
}



static inline iss_insn_t *fsgnj_ah_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL2(lib_flexfloat_sgnj, REG_GET(0), REG_GET(1), 8, 7));
  return insn->next;
}



static inline iss_insn_t *fsgnjn_ah_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL2(lib_flexfloat_sgnjn, REG_GET(0), REG_GET(1), 8, 7));
  return insn->next;
}



static inline iss_insn_t *fsgnjx_ah_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL2(lib_flexfloat_sgnjx, REG_GET(0), REG_GET(1), 8, 7));
  return insn->next;
}



static inline iss_insn_t *fmin_ah_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL2(lib_flexfloat_min, REG_GET(0), REG_GET(1), 8, 7));
  return insn->next;
}



static inline iss_insn_t *fmax_ah_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL2(lib_flexfloat_max, REG_GET(0), REG_GET(1), 8, 7));
  return insn->next;
}



static inline iss_insn_t *fcvt_w_ah_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL2(lib_flexfloat_cvt_w_ff_round, REG_GET(0), 8, 7, UIM_GET(0)));
  return insn->next;
}



static inline iss_insn_t *fcvt_wu_ah_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL2(lib_flexfloat_cvt_wu_ff_round, REG_GET(0), 8, 7, UIM_GET(0)));
  return insn->next;
}



static inline iss_insn_t *fmv_x_ah_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL1(lib_flexfloat_fmv_x_ff, REG_GET(0), 8, 7));
  return insn->next;
}



static inline iss_insn_t *fmv_ah_x_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL1(lib_flexfloat_fmv_ff_x, REG_GET(0), 8, 7));
  return insn->next;
}



static inline iss_insn_t *feq_ah_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL2(lib_flexfloat_eq, REG_GET(0), REG_GET(1), 8, 7));
  return insn->next;
}



static inline iss_insn_t *flt_ah_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL2(lib_flexfloat_lt, REG_GET(0), REG_GET(1), 8, 7));
  return insn->next;
}



static inline iss_insn_t *fle_ah_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL2(lib_flexfloat_le, REG_GET(0), REG_GET(1), 8, 7));
  return insn->next;
}



static inline iss_insn_t *fclass_ah_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL1(lib_flexfloat_class, REG_GET(0), 8, 7));
  return insn->next;
}



static inline iss_insn_t *fcvt_ah_w_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL2(lib_flexfloat_cvt_ff_w_round, REG_GET(0), 8, 7, UIM_GET(0)));
  return insn->next;
}



static inline iss_insn_t *fcvt_ah_wu_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL2(lib_flexfloat_cvt_ff_wu_round, REG_GET(0), 8, 7, UIM_GET(0)));
  return insn->next;
}


static inline iss_insn_t *fcvt_s_ah_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL4(lib_flexfloat_cvt_ff_ff_round, REG_GET(0), 8, 7, 8, 23, UIM_GET(0)));
  return insn->next;
}


static inline iss_insn_t *fcvt_ah_s_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL4(lib_flexfloat_cvt_ff_ff_round, REG_GET(0), 8, 23, 8, 7, UIM_GET(0)));
  return insn->next;
}


static inline iss_insn_t *fcvt_h_ah_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL4(lib_flexfloat_cvt_ff_ff_round, REG_GET(0), 8, 7, 5, 10, UIM_GET(0)));
  return insn->next;
}


static inline iss_insn_t *fcvt_ah_h_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL4(lib_flexfloat_cvt_ff_ff_round, REG_GET(0), 5, 10, 8, 7, UIM_GET(0)));
  return insn->next;
}


//
// RV64Xf16alt
//
static inline iss_insn_t *fcvt_l_ah_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL2(lib_flexfloat_cvt_l_ff_round, REG_GET(0), 8, 7, UIM_GET(0)));
  return insn->next;
}



static inline iss_insn_t *fcvt_lu_ah_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL2(lib_flexfloat_cvt_lu_ff_round, REG_GET(0), 8, 7, UIM_GET(0)));
  return insn->next;
}



static inline iss_insn_t *fcvt_ah_l_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL2(lib_flexfloat_cvt_ff_l_round, REG_GET(0), 8, 7, UIM_GET(0)));
  return insn->next;
}



static inline iss_insn_t *fcvt_ah_lu_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL2(lib_flexfloat_cvt_ff_lu_round, REG_GET(0), 8, 7, UIM_GET(0)));
  return insn->next;
}


#endif
