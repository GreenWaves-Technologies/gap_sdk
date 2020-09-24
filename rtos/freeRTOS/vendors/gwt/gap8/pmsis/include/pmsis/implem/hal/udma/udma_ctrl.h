#ifndef __PI_HAL_UDMA_CTRL_H__
#define __PI_HAL_UDMA_CTRL_H__

/**
 * Small low level driver for ctrl side of udma (cg and event forward)
 */

#include "pmsis/targets/target.h"
#include "pmsis/implem/hal/soc_eu/soc_eu.h"


static inline uint32_t udma_ctrl_get_clock_gating_register(void)
{
    return hal_read32(&UDMA_GC->CG);
}

static inline uint32_t udma_ctrl_get_event_register(void)
{
    return hal_read32(&UDMA_GC->EVTIN);
}

static inline void udma_ctrl_cg_disable(uint32_t udma_device_id)
{
    hal_or32(&UDMA_GC->CG,1 << udma_device_id);
}

static inline void udma_ctrl_cg_enable(uint32_t udma_device_id)
{
    hal_and32(&UDMA_GC->CG,~(1 << udma_device_id));
}

static inline void udma_ctrl_enable_event_forward(uint8_t udma_evt_nb,
                                                  uint32_t event_id)
{
    // clear current event
    hal_and32(&UDMA_GC->EVTIN,
              ~(UDMA_GC_EVTIN_MASK(0xFF) << UDMA_GC_EVTIN_SHIFT_ID(udma_evt_nb)));
    // set new event
    hal_or32(&UDMA_GC->EVTIN,
             (UDMA_GC_EVTIN_MASK(event_id) << UDMA_GC_EVTIN_SHIFT_ID(udma_evt_nb)));

    // tell soc eu to forward us the event
    hal_soc_eu_set_pr_mask(event_id);
}

static inline void udma_ctrl_disable_event_forward(uint8_t udma_evt_nb,
                                                   uint32_t event_id)
{
    // clear event from udma forward
    hal_and32(&UDMA_GC->EVTIN,
              ~(UDMA_GC_EVTIN_MASK(0xFF) << UDMA_GC_EVTIN_SHIFT_ID(udma_evt_nb)));

    // tell soc eu to stop forwarding us the event
    hal_soc_eu_clear_pr_mask(event_id);
}

#endif  /* __PI_HAL_UDMA_CTRL_H__ */
