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
