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

#ifndef __CPU_ISS_RVXGAP8_HPP
#define __CPU_ISS_RVXGAP8_HPP

#include "iss_core.hpp"
#include "isa_lib/int.h"
#include "isa_lib/macros.h"



static inline iss_insn_t *lib_CPLXMULS_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_CPLXMULS, REG_GET(0), REG_GET(1)));
  return insn->next;
}



static inline iss_insn_t *lib_CPLXMULS_DIV2_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_CPLXMULS_DIV2, REG_GET(0), REG_GET(1)));
  return insn->next;
}



static inline iss_insn_t *lib_CPLXMULS_DIV4_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_CPLXMULS_DIV4, REG_GET(0), REG_GET(1)));
  return insn->next;
}



static inline iss_insn_t *lib_CPLXMULS_SC_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_CPLXMULS_SC, REG_GET(0), REG_GET(1)));
  return insn->next;
}



static inline iss_insn_t *lib_CPLXMULS_SCI_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_CPLXMULS_SC, REG_GET(0), SIM_GET(0)));
  return insn->next;
}



static inline iss_insn_t *lib_CPLX_CONJ_16_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL1(lib_CPLX_CONJ_16, REG_GET(0)));
  return insn->next;
}



static inline iss_insn_t *lib_VEC_ADD_16_ROTMJ_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_VEC_ADD_16_ROTMJ, REG_GET(0), REG_GET(1)));
  return insn->next;
}



static inline iss_insn_t *lib_VEC_ADD_16_ROTMJ_DIV2_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_VEC_ADD_16_ROTMJ_DIV2, REG_GET(0), REG_GET(1)));
  return insn->next;
}



static inline iss_insn_t *lib_VEC_ADD_16_ROTMJ_DIV4_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_VEC_ADD_16_ROTMJ_DIV4, REG_GET(0), REG_GET(1)));
  return insn->next;
}



static inline iss_insn_t *lib_VITOP_MAX_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_VITOP_MAX, REG_GET(0), REG_GET(1)));
  return insn->next;
}



static inline iss_insn_t *lib_VITOP_SEL_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_VITOP_SEL, REG_GET(0), REG_GET(1)));
  return insn->next;
}



static inline iss_insn_t *lib_VEC_PACK_SC_H_16_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_VEC_PACK_SC_H_16, REG_GET(0), REG_GET(1)));
  return insn->next;
}



static inline iss_insn_t *lib_VEC_PACK_SC_HL_16_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_VEC_PACK_SC_HL_16, REG_GET(0), REG_GET(1)));
  return insn->next;
}



static inline iss_insn_t *lib_VEC_ADD_8_DIV2_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_VEC_ADD_int8_t_to_int32_t_div2, REG_GET(0), REG_GET(1)));
  return insn->next;
}



static inline iss_insn_t *lib_VEC_ADD_8_DIV4_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_VEC_ADD_int8_t_to_int32_t_div4, REG_GET(0), REG_GET(1)));
  return insn->next;
}



static inline iss_insn_t *lib_VEC_ADD_16_DIV2_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_VEC_ADD_int16_t_to_int32_t_div2, REG_GET(0), REG_GET(1)));
  return insn->next;
}



static inline iss_insn_t *lib_VEC_ADD_16_DIV4_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_VEC_ADD_int16_t_to_int32_t_div4, REG_GET(0), REG_GET(1)));
  return insn->next;
}



static inline iss_insn_t *lib_VEC_SUB_8_DIV2_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_VEC_SUB_int8_t_to_int32_t_div2, REG_GET(0), REG_GET(1)));
  return insn->next;
}



static inline iss_insn_t *lib_VEC_SUB_8_DIV4_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_VEC_SUB_int8_t_to_int32_t_div4, REG_GET(0), REG_GET(1)));
  return insn->next;
}



static inline iss_insn_t *lib_VEC_SUB_16_DIV2_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_VEC_SUB_int16_t_to_int32_t_div2, REG_GET(0), REG_GET(1)));
  return insn->next;
}



static inline iss_insn_t *lib_VEC_SUB_16_DIV4_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL2(lib_VEC_SUB_int16_t_to_int32_t_div4, REG_GET(0), REG_GET(1)));
  return insn->next;
}

#endif