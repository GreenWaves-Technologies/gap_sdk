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

#ifndef __CPU_ISS_PRIV_HPP
#define __CPU_ISS_PRIV_HPP



static inline iss_insn_t *csrrw_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_reg_t value;
  iss_reg_t reg_value = REG_GET(0);

  if (iss_csr_read(iss, UIM_GET(0), &value) == 0)
  { 
    if (insn->out_regs[0] != 0) 
      {
        REG_SET(0, value);
      }
  }

  iss_csr_write(iss, UIM_GET(0), reg_value);

  return insn->next;
}



static inline iss_insn_t *csrrc_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_reg_t value;
  iss_reg_t reg_value = REG_GET(0);

  if (iss_csr_read(iss, UIM_GET(0), &value) == 0)
  { 
    if (insn->out_regs[0] != 0) REG_SET(0, value);
  }
  iss_csr_write(iss, UIM_GET(0), value & ~reg_value);
  return insn->next;
}



static inline iss_insn_t *csrrs_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_reg_t value;
  iss_reg_t reg_value = REG_GET(0);

  if (iss_csr_read(iss, UIM_GET(0), &value) == 0)
  { 
    if (insn->out_regs[0] != 0) REG_SET(0, value);
  }
  iss_csr_write(iss, UIM_GET(0), value | reg_value);
  return insn->next;
}



static inline iss_insn_t *csrrwi_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_reg_t value;
  if (iss_csr_read(iss, UIM_GET(0), &value) == 0)
  { 
    if (insn->out_regs[0] != 0) REG_SET(0, value);
  }
  iss_csr_write(iss, UIM_GET(0),UIM_GET(1));
  return insn->next;
}



static inline iss_insn_t *csrrci_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_reg_t value;
  if (iss_csr_read(iss, UIM_GET(0), &value) == 0)
  { 
    if (insn->out_regs[0] != 0) REG_SET(0, value);
  }
  iss_csr_write(iss, UIM_GET(0), value & ~UIM_GET(1));
  return insn->next;
}



static inline iss_insn_t *csrrsi_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_reg_t value;
  if (iss_csr_read(iss, UIM_GET(0), &value) == 0)
  { 
    if (insn->out_regs[0] != 0) REG_SET(0, value);
  }
  iss_csr_write(iss, UIM_GET(0), value | UIM_GET(1));
  return insn->next;
}



static inline iss_insn_t *wfi_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_wait_for_interrupt(iss);
  return insn->next;
}



static inline iss_insn_t *mret_exec(iss_t *iss, iss_insn_t *insn)
{
  return iss_irq_handle_mret(iss);
}



static inline iss_insn_t *dret_exec(iss_t *iss, iss_insn_t *insn)
{
  return iss_irq_handle_dret(iss);
}



#endif
