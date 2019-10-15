
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

#ifndef _GAP_ITC_H_
#define _GAP_ITC_H_

#include <GAP9.h>

/*!
 * @addtogroup event_unit
 * @{
 */
/*******************************************************************************
 * Definitions
 ******************************************************************************/

/** @name ITC management
 *  The following function can be used for selecting which events can wake-up the core, put the core the sleep, clear events and so on.
 */
/**@{*/

static inline void ITC_WaitIRQ()
{
    asm volatile ("wfi");
}

/*!
 * @brief Wait specified event to wake up CORE without generate an IRQ.
 *
 * The purpose of this API is to do UDMA blocking transfer for perapherals which use event to wake up CORE.
 * When CORE execute WFI (wait for interrupt) instruction, it goes into sleep mode, waiting for an trigger.
 * While for blocking transfer, it sends only an event to wake up the core without generating an IRQ.
 *
 * @param IRQn The waiting event number
 */
static inline int ITC_WaitEvent_NOIRQ(IRQn_Type IRQn)
{
    int mask = (1UL << IRQn);

    uint32_t irq = __disable_irq();

    FC_ITC->MASK_SET = mask;

    while((FC_ITC->INT & mask) == 0) {
        asm volatile ("wfi");

        if ((FC_ITC->INT & mask) || (FC_ITC->ACK & mask)) {
            break;
        } else {
            __restore_irq(irq);

            irq = __disable_irq();
        }
    }

    /* Clear mask and this event  */
    FC_ITC->MASK_CLR = mask;
    FC_ITC->INT_CLR  = mask;
    FC_ITC->ACK_CLR  = mask;

    int event = FC_ITC->CURRENT_EVENT;

    __restore_irq(irq);

    return event;
}

/*!
 * @brief Wait specified event to wake up CORE and generate an IRQ.
 *
 * The purpose of this API is to wait an IRQ. When CORE execute WFI (wait for interrupt) instruction,
 * it goes into sleep mode, waiting for an IRQ to wake it up.
 *
 * @param IRQn The waiting event number
 */
static inline void ITC_WaitEvent_IRQ(IRQn_Type IRQn)
{
    int mask = (1UL << IRQn);

    FC_ITC->MASK_SET = mask;

    do {
        if ((FC_ITC->ACK & mask) == 0) {
            asm volatile ("wfi");
        }
    } while (1);

    FC_ITC->ACK_CLR = mask;
}

static inline int ITC_GetIRQ()
{
    return FC_ITC->INT;
}

static inline void ITC_SetIRQ(IRQn_Type IRQn)
{
    int mask = (1UL << IRQn);

    FC_ITC->INT_SET  = mask;
}

static inline void ITC_ClearIRQ(IRQn_Type IRQn)
{
    int mask = (1UL << IRQn);

    FC_ITC->INT_CLR = mask;
}

#endif /*_GAP_ITC_H_*/
