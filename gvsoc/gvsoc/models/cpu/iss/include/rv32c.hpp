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

#ifndef __CPU_ISS_RV32C_HPP
#define __CPU_ISS_RV32C_HPP

#include "iss_core.hpp"
#include "isa_lib/int.h"
#include "isa_lib/macros.h"



static inline iss_insn_t *c_addi4spn_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  return addi_exec(iss, insn);
}

static inline iss_insn_t *c_addi4spn_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_pccr_account_event(iss, CSR_PCER_RVC, 1);
  return addi_exec(iss, insn);
}



static inline iss_insn_t *c_lw_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  return lw_exec_fast(iss, insn);
}

static inline iss_insn_t *c_lw_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_pccr_account_event(iss, CSR_PCER_RVC, 1);
  return lw_exec(iss, insn);
}



static inline iss_insn_t *c_sw_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  return sw_exec_fast(iss, insn);
}

static inline iss_insn_t *c_sw_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_pccr_account_event(iss, CSR_PCER_RVC, 1);
  return sw_exec(iss, insn);
}



static inline iss_insn_t *c_swsp_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  return sw_exec_fast(iss, insn);
}

static inline iss_insn_t *c_swsp_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_pccr_account_event(iss, CSR_PCER_RVC, 1);
  return sw_exec(iss, insn);
}



static inline iss_insn_t *c_nop_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  return nop_exec(iss, insn);
}

static inline iss_insn_t *c_nop_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_pccr_account_event(iss, CSR_PCER_RVC, 1);
  return nop_exec(iss, insn);
}



static inline iss_insn_t *c_addi_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  return addi_exec(iss, insn);
}

static inline iss_insn_t *c_addi_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_pccr_account_event(iss, CSR_PCER_RVC, 1);
  return addi_exec(iss, insn);
}



static inline iss_insn_t *c_jal_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  return jal_exec_fast(iss, insn);
}

static inline iss_insn_t *c_jal_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_pccr_account_event(iss, CSR_PCER_RVC, 1);
  return jal_exec(iss, insn);
}



static inline iss_insn_t *c_li_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  return addi_exec(iss, insn);
}

static inline iss_insn_t *c_li_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_pccr_account_event(iss, CSR_PCER_RVC, 1);
  return addi_exec(iss, insn);
}



static inline iss_insn_t *c_addi16sp_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  return addi_exec(iss, insn);
}

static inline iss_insn_t *c_addi16sp_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_pccr_account_event(iss, CSR_PCER_RVC, 1);
  return addi_exec(iss, insn);
}



static inline iss_insn_t *c_jalr_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  return jalr_exec_fast(iss, insn);
}

static inline iss_insn_t *c_jalr_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_pccr_account_event(iss, CSR_PCER_RVC, 1);
  return jalr_exec(iss, insn);
}



static inline iss_insn_t *c_lui_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  return lui_exec(iss, insn);
}

static inline iss_insn_t *c_lui_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_pccr_account_event(iss, CSR_PCER_RVC, 1);
  return lui_exec(iss, insn);
}



static inline iss_insn_t *c_srli_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  return srli_exec(iss, insn);
}

static inline iss_insn_t *c_srli_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_pccr_account_event(iss, CSR_PCER_RVC, 1);
  return srli_exec(iss, insn);
}



static inline iss_insn_t *c_srai_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  return srai_exec(iss, insn);
}

static inline iss_insn_t *c_srai_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_pccr_account_event(iss, CSR_PCER_RVC, 1);
  return srai_exec(iss, insn);
}



static inline iss_insn_t *c_andi_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  return andi_exec(iss, insn);
}

static inline iss_insn_t *c_andi_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_pccr_account_event(iss, CSR_PCER_RVC, 1);
  return andi_exec(iss, insn);
}



static inline iss_insn_t *c_sub_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  return sub_exec(iss, insn);
}

static inline iss_insn_t *c_sub_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_pccr_account_event(iss, CSR_PCER_RVC, 1);
  return sub_exec(iss, insn);
}



static inline iss_insn_t *c_xor_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  return xor_exec(iss, insn);
}

static inline iss_insn_t *c_xor_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_pccr_account_event(iss, CSR_PCER_RVC, 1);
  return xor_exec(iss, insn);
}



static inline iss_insn_t *c_or_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  return or_exec(iss, insn);
}

static inline iss_insn_t *c_or_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_pccr_account_event(iss, CSR_PCER_RVC, 1);
  return or_exec(iss, insn);
}



static inline iss_insn_t *c_and_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  return and_exec(iss, insn);
}

static inline iss_insn_t *c_and_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_pccr_account_event(iss, CSR_PCER_RVC, 1);
  return and_exec(iss, insn);
}



static inline iss_insn_t *c_j_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  return jal_exec_fast(iss, insn);
}

static inline iss_insn_t *c_j_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_pccr_account_event(iss, CSR_PCER_RVC, 1);
  return jal_exec(iss, insn);
}



static inline iss_insn_t *c_beqz_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  return beq_exec_fast(iss, insn);
}

static inline iss_insn_t *c_beqz_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_pccr_account_event(iss, CSR_PCER_RVC, 1);
  return beq_exec(iss, insn);
}



static inline iss_insn_t *c_bnez_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  return bne_exec_fast(iss, insn);
}

static inline iss_insn_t *c_bnez_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_pccr_account_event(iss, CSR_PCER_RVC, 1);
  return bne_exec(iss, insn);
}



static inline iss_insn_t *c_slli_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  return slli_exec(iss, insn);
}

static inline iss_insn_t *c_slli_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_pccr_account_event(iss, CSR_PCER_RVC, 1);
  return slli_exec(iss, insn);
}



static inline iss_insn_t *c_lwsp_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  return lw_exec_fast(iss, insn);
}

static inline iss_insn_t *c_lwsp_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_pccr_account_event(iss, CSR_PCER_RVC, 1);
  return lw_exec(iss, insn);
}



static inline iss_insn_t *c_jr_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  return jalr_exec_fast(iss, insn);
}

static inline iss_insn_t *c_jr_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_pccr_account_event(iss, CSR_PCER_RVC, 1);
  return jalr_exec(iss, insn);
}



static inline iss_insn_t *c_mv_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  return add_exec(iss, insn);
}

static inline iss_insn_t *c_mv_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_pccr_account_event(iss, CSR_PCER_RVC, 1);
  return add_exec(iss, insn);
}



static inline iss_insn_t *c_add_exec_fast(iss_t *iss, iss_insn_t *insn)
{
  return add_exec(iss, insn);
}

static inline iss_insn_t *c_add_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_pccr_account_event(iss, CSR_PCER_RVC, 1);
  return add_exec(iss, insn);
}



static inline iss_insn_t *c_ebreak_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_pccr_account_event(iss, CSR_PCER_RVC, 1);
  iss_handle_ebreak(iss, insn);
  //iss_set_halt_mode(iss, true, 3);
  return insn->next;
}



static inline iss_insn_t *c_sbreak_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_pccr_account_event(iss, CSR_PCER_RVC, 1);
  iss_set_halt_mode(iss, true, 3);
  return insn->next;
}



#endif
