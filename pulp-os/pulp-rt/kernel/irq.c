/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna
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
 * Copyright (C) 2018 GreenWaves Technologies
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
 * Authors: Eric Flamand, GreenWaves Technologies (eric.flamand@greenwaves-technologies.com)
 *          Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */

#include "rt/rt_api.h"

#if defined(__riscv__)

static unsigned int __rt_get_itvec(unsigned int ItBaseAddr, unsigned int ItIndex, unsigned int ItHandler)

{
  /* Prepare 32bit container to be stored at
   *(ItBaseAddr+ItIndex) containing a relative jump from
    (ItBaseAddr+ItIndex) to Handler */

  unsigned int S = ((unsigned int) ItHandler - (ItBaseAddr+ItIndex*4));
  unsigned int R = 0x6F; /* Jal opcode with x0 as target, eg no return */

  /* Forge JAL x0, Address: with Address = S => Bin[31:0] = [S20
  | S10:1 | S11 | S19:12 | 00000 01101111] */

  R = __BITINSERT(R, __BITEXTRACT(S,  1, 20),  1, 31);
  R = __BITINSERT(R, __BITEXTRACT(S, 10,  1), 10, 21);
  R = __BITINSERT(R, __BITEXTRACT(S,  1, 11),  1, 20);
  R = __BITINSERT(R, __BITEXTRACT(S,  8, 12),  8, 12);

  return R;
}

void rt_irq_set_handler(int irq, void (*handler)())
{
  unsigned int base = __rt_get_fc_vector_base();

  unsigned int jmpAddr = base + 0x4 * irq;

  *(volatile unsigned int *)jmpAddr = __rt_get_itvec(base, irq, (unsigned int)handler);

  //selective_flush_icache_addr(jmpAddr & ~(ICACHE_LINE_SIZE-1));

  //if (!rt_is_fc() || plp_pmu_cluster_isOn(0)) flush_all_icache_banks_common(plp_icache_cluster_remote_base(0));

#if defined(PLP_FC_HAS_ICACHE)
  flush_all_icache_banks_common(plp_icache_fc_base());
#endif

}

#else

static RT_FC_DATA void (*__rt_irq_handlers[32])();
static RT_FC_DATA uint32_t __rt_irq_mask = 0;
static RT_FC_DATA uint32_t __rt_irq_enabled = 0;

void rt_irq_set_handler(int irq, void (*handler)())
{
  __rt_irq_handlers[irq] = handler;
}

void __rt_irq_handle()
{
  unsigned int core_id = rt_core_id();
  unsigned int irq_id;

  for(irq_id = pulp_irq_id_read(core_id); irq_id != 0xFF; irq_id = pulp_irq_id_read(core_id))
  {
    pulp_irq_buff_low_clear(1 << irq_id);

    __rt_irq_handlers[irq_id]();
  }

}

int rt_irq_disable()
{
  int core_id = hal_core_id();
  int state = __rt_irq_enabled;

  if (__rt_irq_enabled) __rt_irq_mask = pulp_irq_mask_low_read(core_id);
  __rt_irq_enabled = 0;
  pulp_irq_mask_low_set(core_id, 0);
  // As we are deactivating the interrupts in a peripheral,
  // we have to put some nops to make sure interrupts are really inactive
  // when we execute the next code.
  __asm__ volatile ("l.nop" :  :  : "memory");
  __asm__ volatile ("l.nop" :  :  : "memory");
  __asm__ volatile ("l.nop" :  :  : "memory");
  __asm__ volatile ("l.nop" :  :  : "memory");
  __asm__ volatile ("l.nop" :  :  : "memory");
  return state;
}

void rt_irq_restore(int irq)
{
  // the parameter irq indicates if interrupts were enabled when rt_irq_disable
  // was called while __rt_irq_mask is the mask to apply if interrupts must be
  // reenabled. We have to differentiate in case someone activate an interrupt
  // handler while interrupts are disabled, in which case we have to be careful
  // to keep the HW mask to zero.
  __rt_irq_enabled = irq;
  if (__rt_irq_enabled) 
    {
      pulp_irq_mask_low_set(rt_core_id(), __rt_irq_mask);
    }
}

void rt_irq_enable()
{
  __rt_irq_enabled = 1;
  hal_irq_enable();
  pulp_irq_mask_low_set(rt_core_id(), __rt_irq_mask);
}

void rt_irq_mask_set(unsigned int mask)
{
  __rt_irq_mask |= mask;
  if (__rt_irq_enabled) pulp_irq_mask_low_set(rt_core_id(), __rt_irq_mask);
  eu_evt_maskSet(mask);
}

void rt_irq_mask_clr(unsigned int mask)
{
  __rt_irq_mask &= ~mask;
  if (__rt_irq_enabled) pulp_irq_mask_low_set(rt_core_id(), __rt_irq_mask);
  eu_evt_maskClr(mask);
}

void rt_wait_for_interrupt()
{
  if (__rt_irq_enabled) pulp_irq_mask_low_set(rt_core_id(), 0);
  hal_irq_disable();
  pulp_irq_mask_low_set(rt_core_id(), __rt_irq_mask);
  eu_evt_wait();
  if (!__rt_irq_enabled)
    pulp_irq_mask_low_set(rt_core_id(), 0);
  else
    hal_irq_enable();
}

#endif

#include <stdio.h>
#include <stdlib.h>

void __attribute__((weak)) illegal_insn_handler_c()
{

}

void __rt_handle_illegal_instr()
{
#ifdef __riscv__
  unsigned int mepc = hal_mepc_read();
  rt_warning("Reached illegal instruction (PC: 0x%x, opcode: 0x%x\n", mepc, *(int *)mepc);
  illegal_insn_handler_c();
#endif
}

void __rt_irq_init()
{
  // We may enter the runtime with some interrupts active for example
  // if we force the boot to jump to the runtime through jtag.
  rt_irq_mask_clr(-1);

#if defined(ARCHI_HAS_FC)
  // As the FC code may not be at the beginning of the L2, set the
  // vector base to get proper interrupt handlers
  __rt_set_fc_vector_base((int)rt_irq_vector_base());
#endif
}
