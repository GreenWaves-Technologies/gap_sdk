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

#ifndef __CPU_ISS_IRQ_HPP
#define __CPU_ISS_IRQ_HPP

static inline void iss_irq_check(iss_t *iss)
{
  if (iss->cpu.irq.req_debug && !iss->cpu.state.debug_mode)
  {
    iss->cpu.state.debug_mode = true;
    iss->cpu.csr.depc = iss->cpu.current_insn->addr;
    iss->cpu.irq.debug_saved_irq_enable = iss->cpu.irq.irq_enable;
    iss->cpu.irq.irq_enable = 0;
    iss->cpu.irq.req_debug = -1;
    iss->cpu.current_insn = iss->cpu.irq.debug_handler;
  }
  else
  {
    int req_irq = iss->cpu.irq.req_irq;
    if (req_irq != -1 && iss->cpu.irq.irq_enable)
    {
      // In case we interrupt a pending elw, we need to replay after the irq
      // handling
      if (iss->cpu.state.elw_insn != NULL)
      {
        iss_msg(iss, "Interrupting pending elw\n");
        iss->cpu.current_insn = iss->cpu.state.elw_insn;
        iss->cpu.state.elw_insn = NULL;
      }

      iss->cpu.csr.epc = iss->cpu.current_insn->addr;
      iss->cpu.irq.saved_irq_enable = iss->cpu.irq.irq_enable;
      iss->cpu.irq.irq_enable = 0;
      iss->cpu.irq.req_irq = -1;
      iss->cpu.current_insn = iss->cpu.irq.vectors[req_irq];
      iss->cpu.csr.mcause = (1<<31) | (unsigned int)req_irq;

      iss_irq_ack(iss, req_irq);
    }

    iss->cpu.state.elw_insn = NULL;
  }
}



static inline iss_insn_t *iss_irq_handle_mret(iss_t *iss)
{
  iss_trigger_check_all(iss);
  iss->cpu.irq.irq_enable = iss->cpu.irq.saved_irq_enable;
  iss->cpu.csr.mcause = 0;

  return insn_cache_get(iss, iss->cpu.csr.epc);

}



static inline iss_insn_t *iss_irq_handle_dret(iss_t *iss)
{
  iss_trigger_check_all(iss);
  iss->cpu.irq.irq_enable = iss->cpu.irq.debug_saved_irq_enable;

  return insn_cache_get(iss, iss->cpu.csr.depc);

}



static inline void iss_irq_enable(iss_t *iss, int enable)
{
  iss->cpu.irq.irq_enable = enable;
  iss_trigger_irq_check(iss);
}

static inline void iss_irq_req(iss_t *iss, int irq)
{
  iss->cpu.irq.req_irq = irq;

  if (iss->cpu.state.elw_insn != NULL)
  {
    iss_msg(iss, "Unstalling core due to IRQ\n");
    iss_unstall(iss);
  }
}

static inline void iss_irq_set_vector_table(iss_t *iss, iss_addr_t base)
{
  iss_msg(iss, "Setting vector table (addr: 0x%x)\n", base);
  for (int i=0; i<32; i++)
  {
    iss->cpu.irq.vectors[i] = insn_cache_get(iss, base + i * 4);
  }

  for (int i=32; i<35; i++)
  {
    iss->cpu.irq.vectors[i] = insn_cache_get(iss, base + i * 4);
  }
  iss->cpu.irq.vector_base = base;
}

static inline void iss_irq_build(iss_t *iss)
{
  for (int i=0; i<32; i++)
  {
    iss->cpu.irq.vectors[i] = NULL;
  }

}

static inline void iss_irq_init(iss_t *iss)
{
  iss->cpu.irq.irq_enable = 0;
  iss->cpu.irq.req_irq = -1;
  iss->cpu.irq.req_debug = false;
#if defined(PRIV_1_10)
  iss->cpu.irq.debug_handler = insn_cache_get(iss, iss->cpu.config.debug_handler);
#endif
}

static inline void iss_irq_flush(iss_t *iss)
{
  iss_irq_set_vector_table(iss, iss->cpu.irq.vector_base);
#if defined(PRIV_1_10)
  iss->cpu.irq.debug_handler = insn_cache_get(iss, iss->cpu.config.debug_handler);
#endif
}

#endif
