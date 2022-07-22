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

#ifndef __POS_IMPLEM_IRQ_H__
#define __POS_IMPLEM_IRQ_H__

#include "hal/pulp.h"
#include "archi/chips/gap9_v2/itc/itc.h"



void pos_irq_init();

void pos_irq_set_handler(int irq, void (*handler)());

void pos_irq_set_c_handler(int irq, void (*handler)());

unsigned int pos_irq_get_itvec(unsigned int ItBaseAddr, unsigned int ItIndex, unsigned int ItHandler);


static inline void pos_irq_mask_set(unsigned int mask)
{
#if defined(__RISCV_GENERIC__)
    // Generic riscv case, e.g. Ibex
    hal_spr_read_then_set_from_reg(0x304, mask);

#elif defined(ITC_VERSION) && defined(EU_VERSION)
    // Case with ITC on FC and event unit on cluster, e.g. Wolfe
    hal_itc_enable_set(mask);

#elif defined(ITC_VERSION)
    // Case with only ITC, e.g. Pulpissimo
    hal_itc_enable_set(mask);

#elif defined(EU_VERSION)
    // Case with only event unit, e.g. Gap
    eu_irq_maskSet(mask);
    // This is needed on architectures where the FC is using an event unit as we
    // use an elw instead of a wfi with interrupts disabled. The fact that the event
    // is active will make the core goes out of elw and the interrupt handler
    // will be called as soon as interrupts are enabled.
    eu_evt_maskSet(mask);
#endif
}



static inline void pos_irq_mask_clr(unsigned int mask)
{
#if defined(__RISCV_GENERIC__)
    hal_spr_read_then_clr_from_reg(0x304, mask);

#elif defined(ITC_VERSION) && defined(EU_VERSION)
    hal_itc_enable_clr(mask);

#elif defined(ITC_VERSION)
    hal_itc_enable_clr(mask);

#elif defined(EU_VERSION)
    eu_irq_maskClr(mask);
    eu_evt_maskClr(mask);
#endif
}


static inline void pos_irq_cl_mask_set(unsigned int mask)
{
    eu_irq_maskSet(mask);
}


static inline void pos_irq_cl_mask_clr(unsigned int mask)
{
    eu_irq_maskClr(mask);
}

static inline void pos_irq_fc_clr(unsigned int mask)
{
    hal_itc_status_clr(mask);
}


static inline void pos_irq_clr(unsigned int mask)
{
#if defined(__RISCV_GENERIC__)
    // TODO 

#elif defined(ITC_VERSION) && defined(EU_VERSION)
    if (hal_is_fc())
        hal_itc_status_clr(mask);
    else
        eu_evt_clr(mask);

#elif defined(ITC_VERSION)
    hal_itc_status_clr(mask);

#elif defined(EU_VERSION) && EU_VERSION >= 3
    eu_evt_clr(mask);
#endif
}

static inline unsigned int pos_irq_get_fc_vector_base()
{
    if (hal_is_fc())
    {
#if defined(__RISCV_GENERIC__)
        return hal_spr_read(0x305) & ~1;
#elif defined(ARCHI_CORE_HAS_SECURITY) && !defined(ARCHI_CORE_HAS_1_10)
        return hal_spr_read(SR_MTVEC);
#elif defined(ARCHI_CORE_HAS_1_10)
        return hal_spr_read(SR_MTVEC) & ~1;
#elif defined(APB_SOC_VERSION) && APB_SOC_VERSION >= 2
        return apb_soc_ctrl_fc_boot_get(ARCHI_APB_SOC_CTRL_ADDR);
#endif
    }
    else
    {
#if defined(ARCHI_HAS_CLUSTER)
#if defined(ARCHI_CLUSTER_CTRL_ADDR)
        return plp_ctrl_bootaddr_get() - 0x80;
#endif
#endif
    }

    return 0;
}



static inline void pos_irq_set_fc_vector_base(unsigned int base)
{
    if (hal_is_fc())
    {
#if defined(__RISCV_GENERIC__)
        hal_spr_write(0x305, base);
#elif defined(ARCHI_CORE_HAS_SECURITY) && !defined(ARCHI_CORE_HAS_1_10)
        hal_spr_write(SR_MTVEC, base);
#elif defined(ARCHI_CORE_HAS_1_10)
        hal_spr_write(SR_MTVEC, base | 1);
#elif defined(APB_SOC_VERSION) && APB_SOC_VERSION >= 2
        apb_soc_ctrl_fc_boot_set(ARCHI_APB_SOC_CTRL_ADDR, base);
#endif
    }
    else
    {
#if defined(ARCHI_HAS_CLUSTER)
#if defined(ARCHI_CLUSTER_CTRL_ADDR)
        plp_ctrl_bootaddr_set(base);
#endif
#endif
    }
}


static inline void pos_irq_wait_for_interrupt()
{
#if !defined(ARCHI_HAS_FC) || defined(ARCHI_HAS_FC_EU)
    eu_evt_wait();
#else
    hal_itc_wait_for_interrupt();
#endif
}


POS_TEXT_L2 static inline void pos_wait_for_event(unsigned int mask) {
#if defined(ITC_VERSION)
  int end = 0;
  do {

    unsigned int state = hal_irq_disable();
    if ((hal_itc_status_value_get() & mask) == 0) {
      hal_itc_enable_set(mask);
      asm volatile ("wfi");
      hal_itc_enable_clr(mask);
    } else {
      hal_itc_status_clr(mask);
      end = 1;
    }
    hal_irq_restore(state);
  } while (!end);
#else
  eu_evt_maskWaitAndClr(mask);
#endif
}


POS_TEXT_L2 static inline void pos_wait_for_any_event(unsigned int mask)
{
    hal_itc_enable_set(mask);
    asm volatile ("wfi");
    hal_itc_enable_clr(mask);
    hal_itc_status_clr(mask);
    hal_irq_enable();
    hal_irq_disable();
}


static inline int disable_irq(void)
{
  return hal_irq_disable();
}

static inline void restore_irq(int irq_enable)
{
  hal_irq_restore(irq_enable);
}

static inline int pi_irq_disable(void)
{
    return hal_irq_disable();
}

static inline void pi_irq_enable()
{
    hal_irq_enable();
}

static inline void pi_irq_restore(int irq_enable)
{
    hal_irq_restore(irq_enable);
}

static inline void pi_irq_handler_set(int irq, void (*handler)())
{
    pos_irq_set_handler(irq, handler);
}

static inline void pi_irq_mask_enable(int irq)
{
    pos_irq_mask_set(1 << irq);
}

static inline void pi_irq_mask_disable(int irq)
{
    pos_irq_mask_clr(1 << irq);
}


#endif
