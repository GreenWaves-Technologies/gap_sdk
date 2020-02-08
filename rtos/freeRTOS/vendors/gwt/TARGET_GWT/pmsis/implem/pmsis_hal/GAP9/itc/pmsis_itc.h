#ifndef __PMSIS_ITC_H__
#define __PMSIS_ITC_H__

#include "targets/pmsis_targets.h"

#ifdef __GAP9__

/*! @brief Get masked IRQ. */
static inline uint32_t hal_itc_mask_get()
{
    return fc_itc->mask;
}

/*!
 * @brief Enable an IRQ.
 * @param IRQn      IRQ to enable.
 */
static inline void hal_itc_mask_set(IRQn_Type IRQn)
{
    fc_itc->mask_set = (uint32_t)(1UL << IRQn);
}

/*!
 * @brief Disable an IRQ.
 * @param IRQn      IRQ to disable.
 */
static inline void hal_itc_mask_clear(IRQn_Type IRQn)
{
    fc_itc->mask_clear = (uint32_t)(1UL << IRQn);
}

/*! @brief Get received IRQ. */
static inline uint32_t hal_itc_irq_get()
{
    return fc_itc->irq;
}

/*! @brief Set IRQ Status. */
static inline void hal_itc_irq_status_set(uint32_t val)
{
    fc_itc->irq = val;
}

/*!
 * @brief Set an IRQ.
 * Setting an IRQ in this register will trigger the core and will know
 * an IRQ has been triggered.
 * @param IRQn      IRQ to set.
 */
static inline void hal_itc_irq_set(IRQn_Type IRQn)
{
    fc_itc->irq_set = (uint32_t)(1UL << IRQn);
}

/*!
 * @brief Clear an IRQ.
 * @param IRQn      IRQ to clear.
 */
static inline void hal_itc_irq_clear(IRQn_Type IRQn)
{
    fc_itc->irq_clear = (uint32_t)(1UL << IRQn);
}

/* */
static inline uint32_t hal_itc_ack_get()
{
    return fc_itc->ack;
}

/* */
static inline void hal_itc_ack_set(IRQn_Type IRQn)
{
    fc_itc->ack_set = (uint32_t)(1UL << IRQn);
}

/* */
static inline void hal_itc_ack_clear(IRQn_Type IRQn)
{
    fc_itc->ack_clear = (uint32_t)(1UL << IRQn);
}

/*!
 * @brief Reset IRQ and mask.
 */
static inline void hal_itc_reset()
{
    fc_itc->mask_clear = 0xFFFFFFFF;
    fc_itc->irq_clear = 0xFFFFFFFF;
    fc_itc->ack_clear = 0xFFFFFFFF;
}

/*! @brief Pop an event form the ITC FIFO. */
static inline uint32_t hal_itc_event_fifo_pop()
{
    return fc_itc->fifo;
}

/*! @brief Wait for an event/IRQ. */
static inline void hal_itc_wait_for_interrupt()
{
    asm volatile ("wfi");
}

/*!
 * @brief Wait for an event for a given IRQ.
 * @param IRQn      IRQ to wait for.
 */
static inline void hal_itc_wait_for_event_no_irq(IRQn_Type IRQn)
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
#endif /*__GAP9__ */
#endif /* __PMSIS_ITC_H__ */
