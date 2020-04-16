/*
 * Copyright (c) 2019, GreenWaves Technologies, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of GreenWaves Technologies, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "pmsis/implem/drivers/fc_event/fc_event.h"
#include "pmsis/implem/hal/hal.h"
#include "pmsis/implem/drivers/pmsis_it.h"

/*******************************************************************************
 * Variables, macros, structures,... definition
 ******************************************************************************/

/*******************************************************************************
 * Function definition
 ******************************************************************************/

static void fc_event_null_event(void *arg);

static volatile pi_fc_event_handler_t fc_event_handlers[NB_SOC_EVENTS];

static void fc_event_null_event(void *arg)
{
   return;
} 

void pi_fc_event_handler_init(uint32_t fc_event_irq)
{
    // open the mask for fc_soc_event irq
    for(int i=0; i<NB_SOC_EVENTS; i++)
    {
      pi_fc_event_handler_clear(i);
    }
    //NVIC_SetVector(fc_event_irq, (uint32_t)__handler_wrapper_light_fc_event_handler);
    NVIC_EnableIRQ(fc_event_irq);
}

void pi_fc_event_handler_set(uint32_t event_id, pi_fc_event_handler_t event_handler)
{
    fc_event_handlers[event_id] = event_handler;
}

void pi_fc_event_handler_clear(uint32_t event_id)
{
    fc_event_handlers[event_id] = (pi_fc_event_handler_t)fc_event_null_event;
}

// TODO: Use Eric's FIRQ ABI
__attribute__((section(".text")))
void fc_soc_event_handler(void)
{
    /* Pop one event element from the FIFO */
    uint32_t event = EU_SOC_EVENTS->CURRENT_EVENT;

    hal_eu_fc_evt_demux_trig_set(FC_SW_NOTIFY_EVENT, 0);
    /* Trigger an event in case someone is waiting for it
       it will check the termination using the pending variable */
    /* Now that we popped the element, we can clear the soc event FIFO event as the FIFO is
       generating an event as soon as the FIFO is not empty */
    EU_CORE_DEMUX->BUFFER_CLEAR = (1 << FC_SOC_EVENT_IRQN);

    // TODO: USE builtins
    event &= 0xFF;
    if (fc_event_handlers[event] != NULL)
    {
        fc_event_handlers[event]((void*)event);
    }
}
