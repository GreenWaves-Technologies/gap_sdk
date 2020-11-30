/*
 * Copyright (C) 2019 GreenWaves Technologies
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

#include "pmsis.h"
#include <hal/pulp.h>
#include <stdio.h>
#include <stdlib.h>


void pos_irq_c_handler_stub();


uint32_t pos_irq_c_handlers[32];


static unsigned int pos_irq_get_itvec(unsigned int ItBaseAddr, unsigned int ItIndex, unsigned int ItHandler)
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



void pos_irq_set_handler(int irq, void (*handler)())
{
#if defined(__RISCV_GENERIC__)
  if (irq < 16)
    return;

  irq -= 16;
#endif

  unsigned int base = pos_irq_get_fc_vector_base();

  unsigned int jmpAddr = base + 0x4 * irq;

  *(volatile unsigned int *)jmpAddr = pos_irq_get_itvec(base, irq, (unsigned int)handler);

  //selective_flush_icache_addr(jmpAddr & ~(ICACHE_LINE_SIZE-1));

  //if (!rt_is_fc() || plp_pmu_cluster_isOn(0)) flush_all_icache_banks_common(plp_icache_cluster_remote_base(0));

#if defined(PLP_FC_HAS_ICACHE)
  flush_all_icache_banks_common(plp_icache_fc_base());
#endif

}



void pos_irq_illegal_instr()
{
  //unsigned int mepc = hal_mepc_read();
  //rt_warning("Reached illegal instruction (PC: 0x%x, opcode: 0x%x\n", mepc, *(int *)mepc);
}


void pos_irq_set_c_handler(int irq, void (*handler)())
{
    pos_irq_c_handlers[irq] = (uint32_t)handler;
    pos_irq_set_handler(irq, pos_irq_c_handler_stub);
}


void pos_irq_init()
{
    // We may enter the runtime with some interrupts active for example
    // if we force the boot to jump to the runtime through jtag.
    pos_irq_mask_clr(-1);

    // As the FC code may not be at the beginning of the L2, set the
    // vector base to get proper interrupt handlers
    pos_irq_set_fc_vector_base(pos_irq_vector_base());
}
