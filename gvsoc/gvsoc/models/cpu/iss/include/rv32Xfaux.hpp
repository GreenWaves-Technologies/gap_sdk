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

#ifndef __CPU_ISS_RV32XFAUX_HPP
#define __CPU_ISS_RV32XFAUX_HPP

#include "iss_core.hpp"
#include "isa_lib/int.h"
#include "isa_lib/macros.h"


//
// The definitions herein only work for FLEN=32 -> i.e. no D extension!!
//

//
// with Xf16
//
static inline iss_insn_t *fmulex_s_h_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL3(lib_flexfloat_mul_round, \
                          LIB_FF_CALL4(lib_flexfloat_cvt_ff_ff_round, REG_GET(0), 5, 10, 8, 23, UIM_GET(0)), \
                          LIB_FF_CALL4(lib_flexfloat_cvt_ff_ff_round, REG_GET(1), 5, 10, 8, 23, UIM_GET(0)), \
                          8, 23, UIM_GET(0)));
  return insn->next;
}



static inline iss_insn_t *fmacex_s_h_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL4(lib_flexfloat_madd_round, \
                          LIB_FF_CALL4(lib_flexfloat_cvt_ff_ff_round, REG_GET(0), 5, 10, 8, 23, UIM_GET(0)), \
                          LIB_FF_CALL4(lib_flexfloat_cvt_ff_ff_round, REG_GET(1), 5, 10, 8, 23, UIM_GET(0)), \
                          REG_GET(2), \
                          8, 23, UIM_GET(0)));
  return insn->next;
}


// TODO: PROPER DOTP
static inline iss_insn_t *vfdotp_h_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL4(lib_flexfloat_madd_round, \
                          REG_GET(0) & 0xffff, \
                          REG_GET(1) & 0xffff, \
                          LIB_FF_CALL3(lib_flexfloat_mul_round, REG_GET(0)>>16, REG_GET(1)>>16, 5, 10, 0) & 0xffff, \
                          5, 10, 0));
  return insn->next;
}



// TODO: PROPER DOTP
static inline iss_insn_t *vfdotp_r_h_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL4(lib_flexfloat_madd_round, \
                          REG_GET(0) & 0xffff, \
                          REG_GET(1) & 0xffff, \
                          LIB_FF_CALL3(lib_flexfloat_mul_round, REG_GET(0)>>16, REG_GET(1) & 0xffff, 5, 10, 0) & 0xffff, \
                          5, 10, 0));
  return insn->next;
}



// TODO: PROPER DOTP
static inline iss_insn_t *vfdotpex_s_h_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL4(lib_flexfloat_madd_round, \
                          LIB_FF_CALL4(lib_flexfloat_cvt_ff_ff_round, REG_GET(0) & 0xffff, 5, 10, 8, 23, 0), \
                          LIB_FF_CALL4(lib_flexfloat_cvt_ff_ff_round, REG_GET(1) & 0xffff, 5, 10, 8, 23, 0), \
                          LIB_FF_CALL3(lib_flexfloat_mul_round, \
                                       LIB_FF_CALL4(lib_flexfloat_cvt_ff_ff_round, REG_GET(0) >> 16, 5, 10, 8, 23, 0), \
                                       LIB_FF_CALL4(lib_flexfloat_cvt_ff_ff_round, REG_GET(1) >> 16, 5, 10, 8, 23, 0), \
                                       8, 23, 0), \
                          8, 23, 0));
  return insn->next;
}



// TODO: PROPER DOTP
static inline iss_insn_t *vfdotpex_s_r_h_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL4(lib_flexfloat_madd_round, \
                          LIB_FF_CALL4(lib_flexfloat_cvt_ff_ff_round, REG_GET(0) & 0xffff, 5, 10, 8, 23, 0), \
                          LIB_FF_CALL4(lib_flexfloat_cvt_ff_ff_round, REG_GET(1) & 0xffff, 5, 10, 8, 23, 0), \
                          LIB_FF_CALL3(lib_flexfloat_mul_round, \
                                       LIB_FF_CALL4(lib_flexfloat_cvt_ff_ff_round, REG_GET(0) >> 16, 5, 10, 8, 23, 0), \
                                       LIB_FF_CALL4(lib_flexfloat_cvt_ff_ff_round, REG_GET(1) & 0xffff, 5, 10, 8, 23, 0), \
                                       8, 23, 0), \
                          8, 23, 0));
  return insn->next;
}



static inline iss_insn_t *vfavg_h_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, (LIB_FF_CALL3(lib_flexfloat_avg_round, REG_GET(0) & 0xffff, REG_GET(1) & 0xffff, 5, 10, 0) & 0xffff) |\
            ((LIB_FF_CALL3(lib_flexfloat_avg_round, REG_GET(0) >> 16, REG_GET(1) >> 16, 5, 10, 0) & 0xffff) << 16));
  return insn->next;
}



static inline iss_insn_t *vfavg_r_h_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, (LIB_FF_CALL3(lib_flexfloat_avg_round, REG_GET(0) & 0xffff, REG_GET(1) & 0xffff, 5, 10, 0) & 0xffff) |\
            ((LIB_FF_CALL3(lib_flexfloat_avg_round, REG_GET(0) >> 16, REG_GET(1) & 0xffff, 5, 10, 0) & 0xffff) << 16));
  return insn->next;
}



//
// with Xf16alt
//
static inline iss_insn_t *fmulex_s_ah_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL3(lib_flexfloat_mul_round, \
                          LIB_FF_CALL4(lib_flexfloat_cvt_ff_ff_round, REG_GET(0), 8, 7, 8, 23, UIM_GET(0)), \
                          LIB_FF_CALL4(lib_flexfloat_cvt_ff_ff_round, REG_GET(1), 8, 7, 8, 23, UIM_GET(0)), \
                          8, 23, UIM_GET(0)));
  return insn->next;
}



static inline iss_insn_t *fmacex_s_ah_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL4(lib_flexfloat_madd_round, \
                          LIB_FF_CALL4(lib_flexfloat_cvt_ff_ff_round, REG_GET(0), 8, 7, 8, 23, UIM_GET(0)), \
                          LIB_FF_CALL4(lib_flexfloat_cvt_ff_ff_round, REG_GET(1), 8, 7, 8, 23, UIM_GET(0)), \
                          REG_GET(2), \
                          8, 23, UIM_GET(0)));
  return insn->next;
}


// TODO: PROPER DOTP
static inline iss_insn_t *vfdotp_ah_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL4(lib_flexfloat_madd_round, \
                          REG_GET(0) & 0xffff, \
                          REG_GET(1) & 0xffff, \
                          LIB_FF_CALL3(lib_flexfloat_mul_round, REG_GET(0)>>16, REG_GET(1)>>16, 8, 7, 0) & 0xffff, \
                          8, 7, 0));
  return insn->next;
}



// TODO: PROPER DOTP
static inline iss_insn_t *vfdotp_r_ah_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL4(lib_flexfloat_madd_round, \
                          REG_GET(0) & 0xffff, \
                          REG_GET(1) & 0xffff, \
                          LIB_FF_CALL3(lib_flexfloat_mul_round, REG_GET(0)>>16, REG_GET(1) & 0xffff, 8, 7, 0) & 0xffff, \
                          8, 7, 0));
  return insn->next;
}



// TODO: PROPER DOTP
static inline iss_insn_t *vfdotpex_s_ah_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL4(lib_flexfloat_madd_round, \
                          LIB_FF_CALL4(lib_flexfloat_cvt_ff_ff_round, REG_GET(0) & 0xffff, 8, 7, 8, 23, 0), \
                          LIB_FF_CALL4(lib_flexfloat_cvt_ff_ff_round, REG_GET(1) & 0xffff, 8, 7, 8, 23, 0), \
                          LIB_FF_CALL3(lib_flexfloat_mul_round, \
                                       LIB_FF_CALL4(lib_flexfloat_cvt_ff_ff_round, REG_GET(0) >> 16, 8, 7, 8, 23, 0), \
                                       LIB_FF_CALL4(lib_flexfloat_cvt_ff_ff_round, REG_GET(1) >> 16, 8, 7, 8, 23, 0), \
                                       8, 23, 0), \
                          8, 23, 0));
  return insn->next;
}



// TODO: PROPER DOTP
static inline iss_insn_t *vfdotpex_s_r_ah_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL4(lib_flexfloat_madd_round, \
                          LIB_FF_CALL4(lib_flexfloat_cvt_ff_ff_round, REG_GET(0) & 0xffff, 8, 7, 8, 23, 0), \
                          LIB_FF_CALL4(lib_flexfloat_cvt_ff_ff_round, REG_GET(1) & 0xffff, 8, 7, 8, 23, 0), \
                          LIB_FF_CALL3(lib_flexfloat_mul_round, \
                                       LIB_FF_CALL4(lib_flexfloat_cvt_ff_ff_round, REG_GET(0) >> 16, 8, 7, 8, 23, 0), \
                                       LIB_FF_CALL4(lib_flexfloat_cvt_ff_ff_round, REG_GET(1) & 0xffff, 8, 7, 8, 23, 0), \
                                       8, 23, 0), \
                          8, 23, 0));
  return insn->next;
}



static inline iss_insn_t *vfavg_ah_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, (LIB_FF_CALL3(lib_flexfloat_avg_round, REG_GET(0) & 0xffff, REG_GET(1) & 0xffff, 8, 7, 0) & 0xffff) |\
            ((LIB_FF_CALL3(lib_flexfloat_avg_round, REG_GET(0) >> 16, REG_GET(1) >> 16, 8, 7, 0) & 0xffff) << 16));
  return insn->next;
}



static inline iss_insn_t *vfavg_r_ah_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, (LIB_FF_CALL3(lib_flexfloat_avg_round, REG_GET(0) & 0xffff, REG_GET(1) & 0xffff, 8, 7, 0) & 0xffff) |\
            ((LIB_FF_CALL3(lib_flexfloat_avg_round, REG_GET(0) >> 16, REG_GET(1) & 0xffff, 8, 7, 0) & 0xffff) << 16));
  return insn->next;
}




//
// with Xf8
//
static inline iss_insn_t *fmulex_s_b_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL3(lib_flexfloat_mul_round, \
                          LIB_FF_CALL4(lib_flexfloat_cvt_ff_ff_round, REG_GET(0), 5, 2, 8, 23, UIM_GET(0)), \
                          LIB_FF_CALL4(lib_flexfloat_cvt_ff_ff_round, REG_GET(1), 5, 2, 8, 23, UIM_GET(0)), \
                          8, 23, UIM_GET(0)));
  return insn->next;
}



static inline iss_insn_t *fmacex_s_b_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL4(lib_flexfloat_madd_round, \
                          LIB_FF_CALL4(lib_flexfloat_cvt_ff_ff_round, REG_GET(0), 5, 2, 8, 23, UIM_GET(0)), \
                          LIB_FF_CALL4(lib_flexfloat_cvt_ff_ff_round, REG_GET(1), 5, 2, 8, 23, UIM_GET(0)), \
                          REG_GET(2), \
                          8, 23, UIM_GET(0)));
  return insn->next;
}


// TODO: PROPER DOTP
static inline iss_insn_t *vfdotp_b_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL4(lib_flexfloat_madd_round, \
                          REG_GET(0) & 0xff, \
                          REG_GET(1) & 0xff, \
                          LIB_FF_CALL4(lib_flexfloat_madd_round, \
                                       REG_GET(0)>>8, \
                                       REG_GET(1)>>8, \
                                       LIB_FF_CALL4(lib_flexfloat_madd_round, \
                                                    REG_GET(0)>>16, \
                                                    REG_GET(1)>>16, \
                                                    LIB_FF_CALL3(lib_flexfloat_mul_round, \
                                                                 REG_GET(0)>>24, \
                                                                 REG_GET(1)>>24, \
                                                                 5, 2, 0) & 0xff, \
                                                    5, 2, 0) & 0xff, \
                                       5, 2, 0) & 0xff, \
                          5, 2, 0));
  return insn->next;
}



// TODO: PROPER DOTP
static inline iss_insn_t *vfdotp_r_b_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL4(lib_flexfloat_madd_round, \
                          REG_GET(0) & 0xff, \
                          REG_GET(1) & 0xff, \
                          LIB_FF_CALL4(lib_flexfloat_madd_round, \
                                       REG_GET(0)>>8, \
                                       REG_GET(1) & 0xff, \
                                       LIB_FF_CALL4(lib_flexfloat_madd_round, \
                                                    REG_GET(0)>>16, \
                                                    REG_GET(1) & 0xff, \
                                                    LIB_FF_CALL3(lib_flexfloat_mul_round, \
                                                                 REG_GET(0)>>24, \
                                                                 REG_GET(1) & 0xff, \
                                                                 5, 2, 0) & 0xff, \
                                                    5, 2, 0) & 0xff, \
                                       5, 2, 0) & 0xff, \
                          5, 2, 0));
  return insn->next;
}



// TODO: PROPER DOTP
static inline iss_insn_t *vfdotpex_s_b_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_FF_CALL4(lib_flexfloat_madd_round, \
                          LIB_FF_CALL4(lib_flexfloat_cvt_ff_ff_round, REG_GET(0) & 0xff, 5, 2, 8, 23, 0), \
                          LIB_FF_CALL4(lib_flexfloat_cvt_ff_ff_round, REG_GET(1) & 0xff, 5, 2, 8, 23, 0), \
                          LIB_FF_CALL4(lib_flexfloat_madd_round, \
                                       LIB_FF_CALL4(lib_flexfloat_cvt_ff_ff_round, REG_GET(0) >> 8, 5, 2, 8, 23, 0), \
                                       LIB_FF_CALL4(lib_flexfloat_cvt_ff_ff_round, REG_GET(1) >> 8, 5, 2, 8, 23, 0), \
                                       LIB_FF_CALL4(lib_flexfloat_madd_round, \
                                                    LIB_FF_CALL4(lib_flexfloat_cvt_ff_ff_round, REG_GET(0) >> 16, 5, 2, 8, 23, 0), \
                                                    LIB_FF_CALL4(lib_flexfloat_cvt_ff_ff_round, REG_GET(1) >> 16, 5, 2, 8, 23, 0), \
                                                    LIB_FF_CALL3(lib_flexfloat_mul_round, \
                                                                 LIB_FF_CALL4(lib_flexfloat_cvt_ff_ff_round, REG_GET(0) >> 24, 5, 2, 8, 23, 0), \
                                                                 LIB_FF_CALL4(lib_flexfloat_cvt_ff_ff_round, REG_GET(1) >> 24, 5, 2, 8, 23, 0), \
                                                                 8, 23, 0), \
                                                    8, 23, 0), \
                                       8, 23, 0), \
                          8, 23, 0));
  return insn->next;
}



// TODO: PROPER DOTP
static inline iss_insn_t *vfdotpex_s_r_b_exec(iss_t *iss, iss_insn_t *insn)
{
 REG_SET(0, LIB_FF_CALL4(lib_flexfloat_madd_round, \
                          LIB_FF_CALL4(lib_flexfloat_cvt_ff_ff_round, REG_GET(0) & 0xff, 5, 2, 8, 23, 0), \
                          LIB_FF_CALL4(lib_flexfloat_cvt_ff_ff_round, REG_GET(1) & 0xff, 5, 2, 8, 23, 0), \
                          LIB_FF_CALL4(lib_flexfloat_madd_round, \
                                       LIB_FF_CALL4(lib_flexfloat_cvt_ff_ff_round, REG_GET(0) >> 8, 5, 2, 8, 23, 0), \
                                       LIB_FF_CALL4(lib_flexfloat_cvt_ff_ff_round, REG_GET(1) & 0xff, 5, 2, 8, 23, 0), \
                                       LIB_FF_CALL4(lib_flexfloat_madd_round, \
                                                    LIB_FF_CALL4(lib_flexfloat_cvt_ff_ff_round, REG_GET(0) >> 16, 5, 2, 8, 23, 0), \
                                                    LIB_FF_CALL4(lib_flexfloat_cvt_ff_ff_round, REG_GET(1) & 0xff, 5, 2, 8, 23, 0), \
                                                    LIB_FF_CALL3(lib_flexfloat_mul_round, \
                                                                 LIB_FF_CALL4(lib_flexfloat_cvt_ff_ff_round, REG_GET(0) >> 24, 5, 2, 8, 23, 0), \
                                                                 LIB_FF_CALL4(lib_flexfloat_cvt_ff_ff_round, REG_GET(1) & 0xff, 5, 2, 8, 23, 0), \
                                                                 8, 23, 0), \
                                                    8, 23, 0), \
                                       8, 23, 0), \
                          8, 23, 0));
  return insn->next;
}



static inline iss_insn_t *vfavg_b_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, (LIB_FF_CALL3(lib_flexfloat_avg_round, REG_GET(0) & 0xff, REG_GET(1) & 0xff, 5, 2, 0) & 0xff) |\
            ((LIB_FF_CALL3(lib_flexfloat_avg_round, REG_GET(0) >> 8, REG_GET(1) >> 8, 5, 2, 0) & 0xff) << 8) |\
            ((LIB_FF_CALL3(lib_flexfloat_avg_round, REG_GET(0) >> 16, REG_GET(1) >> 16, 5, 2, 0) & 0xff) << 16) |\
            ((LIB_FF_CALL3(lib_flexfloat_avg_round, REG_GET(0) >> 24, REG_GET(1) >> 24, 5, 2, 0) & 0xff) << 24));
  return insn->next;
}



static inline iss_insn_t *vfavg_r_b_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, (LIB_FF_CALL3(lib_flexfloat_avg_round, REG_GET(0) & 0xff, REG_GET(1) & 0xff, 5, 2, 0) & 0xff) |\
            ((LIB_FF_CALL3(lib_flexfloat_avg_round, REG_GET(0) >> 8, REG_GET(1) & 0xff, 5, 2, 0) & 0xff) << 8) |\
            ((LIB_FF_CALL3(lib_flexfloat_avg_round, REG_GET(0) >> 16, REG_GET(1) & 0xff, 5, 2, 0) & 0xff) << 16) |\
            ((LIB_FF_CALL3(lib_flexfloat_avg_round, REG_GET(0) >> 24, REG_GET(1) & 0xff, 5, 2, 0) & 0xff) << 24));
  return insn->next;
}






#endif
