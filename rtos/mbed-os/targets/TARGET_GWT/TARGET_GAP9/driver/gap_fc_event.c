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
void __attribute__((weak)) CSI2_RX0_IRQHandler() {}
void __attribute__((weak)) CSI2_RX1_IRQHandler() {}

__attribute__((section(".text")))
void FC_EventHandler()
{
    /* Pop one event element from the FIFO */
    uint32_t event = FC_ITC->CURRENT_EVENT & 0xFF;

    if(event == UDMA_EVENT_FILTER_EOT) {
        FILTER_IRQHandler();
    } else if(event == UDMA_EVENT_FILTER_ACT) {
        FILTER_BINCNT_IRQHandler();
    } else if(event == UDMA_EVENT_CSI2_RX0) {
        CSI2_RX0_IRQHandler();
    } else if(event == UDMA_EVENT_CSI2_RX1) {
        CSI2_RX1_IRQHandler();
    } else if(event < UDMA_EVENT_RESERVED0) {
        UDMA_EventHandler(event, 0);
    } else if (event == S_EVENT_RTC_APB) {
        RTC_APB_IRQHandler();
    } else if (event == S_EVENT_RTC) {
        RTC_IRQHandler();
    } else if(event == S_EVENT_GPIO) {
        GPIO_IRQHandler();
    }
    #ifdef FEATURE_CLUSTER
    #endif
}
