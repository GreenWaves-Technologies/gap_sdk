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

#ifndef __PI_HAL_ITC_H__
#define __PI_HAL_ITC_H__

#include "pmsis/targets/target.h"

/**
 * @brief Get mask of active IRQ.
 *
 * @return Mask of active IRQ.
 */
static inline uint32_t hal_itc_mask_get(void)
{
    return hal_read32(&(fc_itc->mask));
}

/**
 * @brief Enable an IRQ.
 * @param IRQn           IRQ to enable.
 */
static inline void hal_itc_mask_set(uint32_t IRQn)
{
    hal_write32(&(fc_itc->mask_set), (uint32_t) (1UL << IRQn));
}

/**
 * @brief Disable an IRQ.
 * @param IRQn           IRQ to disable.
 */
static inline void hal_itc_mask_clear(uint32_t IRQn)
{
    hal_write32(&(fc_itc->mask_clear), (uint32_t) (1UL << IRQn));
}

/**
 * @brief Disable all IRQ.
 */
static inline void hal_itc_mask_all_clear(void)
{
    hal_write32(&(fc_itc->mask_clear), 0xFFFFFFFF);
}

/**
 * @brief Get received/triggered IRQ.
 *
 * @return Mask of triggered IRQ.
 */
static inline uint32_t hal_itc_irq_get(void)
{
    return hal_read32(&(fc_itc->irq));
}

/**
 * @brief Set IRQ Status.
 */
static inline void hal_itc_irq_status_set(uint32_t val)
{
    hal_write32(&(fc_itc->irq), val);
}

/**
 * @brief Set/Trigger an IRQ.
 * Setting an IRQ in this register will trigger the core and will know
 * an IRQ has been triggered.
 * @param IRQn           IRQ to set.
 */
static inline void hal_itc_irq_set(uint32_t IRQn)
{
    hal_write32(&(fc_itc->irq_set), (uint32_t) (1UL << IRQn));
}

/**
 * @brief Clear an IRQ.
 * @param IRQn           IRQ to clear.
 */
static inline void hal_itc_irq_clear(uint32_t IRQn)
{
    hal_write32(&(fc_itc->irq_clear), (uint32_t) (1UL << IRQn));
}

/**
 * @brief Clear all IRQ.
 */
static inline void hal_itc_irq_all_clear(void)
{
    hal_write32(&(fc_itc->irq_clear), 0xFFFFFFFF);
}

/* */
static inline uint32_t hal_itc_ack_get(void)
{
    return hal_read32(&(fc_itc->ack));
}

/* */
static inline void hal_itc_ack_set(uint32_t IRQn)
{
    hal_write32(&(fc_itc->ack_set), (uint32_t) (1UL << IRQn));
}

/* */
static inline void hal_itc_ack_clear(uint32_t IRQn)
{
    hal_write32(&(fc_itc->ack_clear), (uint32_t) (1UL << IRQn));
}

/**
 * @brief Reset IRQ and mask.
 */
static inline void hal_itc_reset(void)
{
    hal_write32(&(fc_itc->mask_clear), 0xFFFFFFFF);
    hal_write32(&(fc_itc->irq_clear), 0xFFFFFFFF);
}

/**
 * @brief Pop an event form the ITC FIFO.
 *
 * @return First event in fifo.
 */
static inline uint32_t hal_itc_event_fifo_pop(void)
{
    return hal_read32(&(fc_itc->fifo));
}

/**
 * @brief Wait for an event/IRQ.
 */
static inline void hal_itc_wait_for_interrupt(void)
{
    asm volatile ("wfi");
}

/**
 * @brief Wait for an event for a given IRQ.
 * @param IRQn           IRQ to wait for.
 */
static inline void hal_itc_wait_for_event_no_irq(uint32_t IRQn)
{
    uint32_t mask = (1UL << IRQn);
    uint32_t irq = __disable_irq();
    hal_itc_mask_set(IRQn);

    while ((hal_itc_irq_get() & mask) == 0)
    {
        hal_itc_wait_for_interrupt();
    }

    hal_itc_irq_clear(IRQn);
    hal_itc_mask_clear(IRQn);
    __restore_irq(irq);
}

#endif /* __PI_HAL_ITC_H__ */
