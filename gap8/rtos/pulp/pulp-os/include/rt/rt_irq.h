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

#ifndef __RT_RT_IRQ_H__
#define __RT_RT_IRQ_H__

/// @cond IMPLEM

#include "hal/pulp.h"

void __rt_irq_init();
void rt_irq_set_handler(int irq, void (*handler)());


extern void __rt_fc_socevents_handler();

#ifdef __riscv__

static inline int rt_irq_disable()
{
  return hal_irq_disable();
}

static inline void rt_irq_restore(int irq)
{
  hal_irq_restore(irq);
}

static inline int disable_irq(void)
{
  return rt_irq_disable();
}

static inline void restore_irq(int irq_enable)
{
  rt_irq_restore(irq_enable);
}


static inline void rt_irq_enable()
{
  __asm__ __volatile__ ("" : : : "memory");
  hal_irq_enable();
}

static inline void rt_irq_mask_set(unsigned int mask)
{
#if defined(ARCHI_CORE_RISCV_ITC)
  hal_spr_read_then_set_from_reg(0x304, mask);
#endif
#if defined(ITC_VERSION) && defined(EU_VERSION)
  if (hal_is_fc()) hal_itc_enable_set(mask);
  else eu_irq_maskSet(mask);
#elif defined(ITC_VERSION)
  hal_itc_enable_set(mask);
#elif defined(EU_VERSION)
  eu_irq_maskSet(mask);
  // This is needed on architectures where the FC is using an event unit as we
  // use an elw instead of a wfi with interrupts disabled. The fact that the event
  // is active will make the core goes out of elw and the interrupt handler
  // will be called as soon as interrupts are enabled.
  eu_evt_maskSet(mask);
#endif
}

static inline void rt_irq_mask_clr(unsigned int mask)
{
#if defined(ARCHI_CORE_RISCV_ITC)
  hal_spr_read_then_clr_from_reg(0x304, mask);
#endif
#if defined(ITC_VERSION) && defined(EU_VERSION)
  if (hal_is_fc()) hal_itc_enable_clr(mask);
  else eu_irq_maskClr(mask);
#elif defined(ITC_VERSION)
  hal_itc_enable_clr(mask);
#elif defined(EU_VERSION)
  eu_irq_maskClr(mask);
  if (hal_is_fc()) eu_evt_maskClr(mask);
#endif
}

static inline void rt_irq_clr(unsigned int mask)
{
#if defined(ITC_VERSION)
  hal_itc_status_clr(mask);
#elif defined(EU_VERSION) && EU_VERSION >= 3
  eu_evt_clr(mask);
#endif
}

#else

int rt_irq_disable();

void rt_irq_restore(int irq);

void rt_irq_enable();

void rt_irq_mask_set(unsigned int mask);

void rt_irq_mask_clr(unsigned int mask);

#endif

#if 0
static inline int rt_irq_disable()
{
  int core_id = hal_core_id();
  int state = pulp_irq_mask_low_read(core_id);
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

static inline void rt_irq_restore(int state)
{
  pulp_irq_mask_low_set(hal_core_id(), state);
}

static inline void rt_irq_enable()
{
}
#endif

/// @endcond

#endif
