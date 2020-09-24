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

#ifndef __CPU_ISS_RVXCPLX_HPP
#define __CPU_ISS_RVXCPLX_HPP

#include "iss_core.hpp"
#include "isa_lib/int.h"
#include "isa_lib/macros.h"



static inline iss_insn_t *gap9_CPLXMUL_H_I_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL4(lib_CPLXMUL_H_I, REG_GET(0), REG_GET(1), REG_GET(2), 0));
  return insn->next;
}



static inline iss_insn_t *gap9_CPLXMUL_H_I_DIV2_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL4(lib_CPLXMUL_H_I, REG_GET(0), REG_GET(1), REG_GET(2), 1));
  return insn->next;
}



static inline iss_insn_t *gap9_CPLXMUL_H_I_DIV4_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL4(lib_CPLXMUL_H_I, REG_GET(0), REG_GET(1), REG_GET(2), 2));
  return insn->next;
}



static inline iss_insn_t *gap9_CPLXMUL_H_I_DIV8_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL4(lib_CPLXMUL_H_I, REG_GET(0), REG_GET(1), REG_GET(2), 3));
  return insn->next;
}



static inline iss_insn_t *gap9_CPLXMUL_H_R_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL4(lib_CPLXMUL_H_R, REG_GET(0), REG_GET(1), REG_GET(2), 0));
  return insn->next;
}



static inline iss_insn_t *gap9_CPLXMUL_H_R_DIV2_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL4(lib_CPLXMUL_H_R, REG_GET(0), REG_GET(1), REG_GET(2), 1));
  return insn->next;
}



static inline iss_insn_t *gap9_CPLXMUL_H_R_DIV4_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL4(lib_CPLXMUL_H_R, REG_GET(0), REG_GET(1), REG_GET(2), 2));
  return insn->next;
}



static inline iss_insn_t *gap9_CPLXMUL_H_R_DIV8_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL4(lib_CPLXMUL_H_R, REG_GET(0), REG_GET(1), REG_GET(2), 3));
  return insn->next;
}



static inline iss_insn_t *gap9_CPLX_CONJ_16_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL1(lib_CPLX_CONJ_16, REG_GET(0)));
  return insn->next;
}



static inline iss_insn_t *gap9_VEC_ADD_16_ROTMJ_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_VEC_ADD_16_ROTMJ, REG_GET(0), REG_GET(1)));
  return insn->next;
}



static inline iss_insn_t *gap9_VEC_ADD_16_ROTMJ_DIV2_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_VEC_ADD_16_ROTMJ_DIV2, REG_GET(0), REG_GET(1)));
  return insn->next;
}



static inline iss_insn_t *gap9_VEC_ADD_16_ROTMJ_DIV4_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_VEC_ADD_16_ROTMJ_DIV4, REG_GET(0), REG_GET(1)));
  return insn->next;
}



static inline iss_insn_t *gap9_VEC_ADD_16_ROTMJ_DIV8_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_VEC_ADD_16_ROTMJ_DIV8, REG_GET(0), REG_GET(1)));
  return insn->next;
}



static inline iss_insn_t *gap9_VEC_ADD_16_DIV2_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_VEC_ADD_int16_t_to_int32_t_div2, REG_GET(0), REG_GET(1)));
  return insn->next;
}



static inline iss_insn_t *gap9_VEC_ADD_16_DIV4_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_VEC_ADD_int16_t_to_int32_t_div4, REG_GET(0), REG_GET(1)));
  return insn->next;
}



static inline iss_insn_t *gap9_VEC_ADD_16_DIV8_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_VEC_ADD_int16_t_to_int32_t_div8, REG_GET(0), REG_GET(1)));
  return insn->next;
}



static inline iss_insn_t *gap9_VEC_SUB_16_DIV2_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_VEC_SUB_int16_t_to_int32_t_div2, REG_GET(0), REG_GET(1)));
  return insn->next;
}



static inline iss_insn_t *gap9_VEC_SUB_16_DIV4_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_VEC_SUB_int16_t_to_int32_t_div4, REG_GET(0), REG_GET(1)));
  return insn->next;
}



static inline iss_insn_t *gap9_VEC_SUB_16_DIV8_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_VEC_SUB_int16_t_to_int32_t_div8, REG_GET(0), REG_GET(1)));
  return insn->next;
}



static inline iss_insn_t *gap9_VEC_PACK_SC_H_16_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_VEC_PACK_SC_H_16, REG_GET(0), REG_GET(1)));
  return insn->next;
}



static inline iss_insn_t *gap9_BITREV_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL3(lib_BITREV, REG_GET(0), UIM_GET(0), UIM_GET(1)+1));
  return insn->next;
}


#endif