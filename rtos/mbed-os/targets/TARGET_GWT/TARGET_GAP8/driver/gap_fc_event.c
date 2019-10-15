/*
 * Copyright (c) 2018, GreenWaves Technologies, Inc.
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

#include "gap_fc_event.h"

/*******************************************************************************
 * Variables, macros, structures,... definition
 ******************************************************************************/
uint32_t fc_handler_vector[UDMA_CHANNEL_NUM];

/*******************************************************************************
 * Function definition
 ******************************************************************************/

__attribute__((section(".text")))
void FC_EventHandler()
{
    /* Pop one event element from the FIFO */
    uint32_t event = EU_SOC_EVENTS->CURRENT_EVENT;

    /* Trigger an event in case someone is waiting for it
       it will check the termination using the pending variable */
    EU_EVT_DemuxTrigSet(FC_SW_NOTIFY_EVENT, 0);
    /* Now that we popped the element, we can clear the soc event FIFO event as the FIFO is
       generating an event as soon as the FIFO is not empty */
    EU_CORE_DEMUX->BUFFER_CLEAR = (1 << FC_SOC_EVENT_IRQn);

    event &= 0xFF;

    if (event < UDMA_EVENT_RESERVED1) {
        UDMA_EventHandler(event, 0);
    } else if (event == RTC_APB_EVENT) {
        RTC_APB_IRQHandler();
    } else if (event == RTC_EVENT) {
        RTC_IRQHandler();
    } else if (event == GPIO_EVENT) {
        GPIO_IRQHandler();
    } else if (PWM0_EVENT <= event && event <= PWM3_EVENT) {
        PWM_IRQHandler(event);
    } else if (event <= PMU_DLC_EVENT_BRIDGE_SCU_OK) {
        PMU_IRQHandler(event);
    } else {

    }
}
