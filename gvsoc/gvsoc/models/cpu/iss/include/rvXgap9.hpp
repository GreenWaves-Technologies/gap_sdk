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