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

#ifndef __POS_IMPLEM_SOC_EVENT_H__
#define __POS_IMPLEM_SOC_EVENT_H__

#include "hal/pulp.h"

extern volatile PI_FC_TINY unsigned int pos_soc_event_status[ARCHI_SOC_EVENT_NB_TOTAL/32];

void pos_soc_event_init();

static inline void pos_soc_event_register_callback_func(unsigned int event, void (*callback)(uint32_t, void *))
{
    pos_soc_event_callback[event] = callback;
}

static inline void pos_soc_event_register_callback_arg(unsigned int event, void *arg)
{
    pos_soc_event_callback_arg[event] = arg;
}

static inline void pos_soc_event_register_callback(unsigned int event, void (*callback)(uint32_t, void *), void *arg)
{
    pos_soc_event_register_callback_func(event, callback);
    pos_soc_event_register_callback_arg(event, arg);
}

static inline void pi_fc_event_handler_set(unsigned int event, void (*callback)(uint32_t, void *), void *arg)
{
    pos_soc_event_register_callback(event, callback, arg);
}

static inline void pi_soc_eu_pr_mask_set(int evt)
{
    soc_eu_prEventMask_setEvent(evt);
}

static inline void pos_soc_event_wait(int event)
{
    int reg = event >> 5;
    int bit = event & 0x1f;
    volatile uint32_t *status = (volatile uint32_t *)&pos_soc_event_status[reg];

    int irq = hal_irq_disable();
    while (__BITEXTRACTU_R(*status, 1, bit) == 0)
    {
        pos_irq_wait_for_interrupt();
        hal_irq_enable();
        hal_irq_disable();
    }
    *status = __BITCLR_R(*status, 1, bit);
    hal_irq_restore(irq);
}

static inline void pi_soc_eu_fc_mask_set(uint32_t event_num)
{
    soc_eu_fc_mask_clr_set(SOC_EU_ADDR, event_num);
}

static inline void pi_soc_eu_fc_mask_clear(uint32_t event_num)
{
    soc_eu_fc_mask_set_set(SOC_EU_ADDR, event_num);
}

#endif
