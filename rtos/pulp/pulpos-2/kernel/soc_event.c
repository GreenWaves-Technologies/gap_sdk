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

PI_FC_TINY void *pos_soc_event_callback[ARCHI_SOC_EVENT_NB_TOTAL];
PI_FC_TINY void *pos_soc_event_callback_arg[ARCHI_SOC_EVENT_NB_TOTAL];

volatile PI_FC_TINY unsigned int pos_soc_event_status[ARCHI_SOC_EVENT_NB_TOTAL/32];

extern void POS_HANDLER pos_soc_event_handler_asm();

void pos_soc_event_init()
{

#if defined(ARCHI_HAS_FC)
    // Deactivate all soc events as they are active by default
    soc_eu_eventMask_reset(SOC_FC_FIRST_MASK);
#endif

#if defined(ARCHI_HAS_FC)
    // Activate soc events handler
    pos_irq_set_handler(ARCHI_FC_EVT_SOC_EVT, pos_soc_event_handler_asm);
    pos_irq_mask_set(1<<ARCHI_FC_EVT_SOC_EVT);
#endif
}