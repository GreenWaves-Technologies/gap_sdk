#ifndef __PI_HAL_UDMA_CORE_H__
#define __PI_HAL_UDMA_CORE_H__

/**
 * Small low level driver for udma core main functionnalities
 */

#include "pmsis/targets/target.h"
#include "pmsis/implem/hal/udma/udma_ctrl.h"

#define UDMA_MAX_SIZE_LOG2    (17)
#define UDMA_MAX_SIZE         (1 << UDMA_MAX_SIZE_LOG2)

typedef enum
{
    RX_CHANNEL      = 0,
    TX_CHANNEL      = 1,
    COMMAND_CHANNEL = 2
} udma_channel_e;

static inline void udma_init_device(uint32_t device_id)
{
    // disable clock gating for device with device_id
    udma_ctrl_cg_disable(device_id);
}

/*
 * Common uDMA channel enqueue
 */
static inline void udma_enqueue_channel(udma_core_t *udma_struct, uint32_t addr,
        uint32_t size, uint32_t config, udma_channel_e channel)
{
    switch(channel)
    {
        // push to rx channel
        case(RX_CHANNEL):
            {
                hal_write32(&udma_struct->rx_saddr,addr);
                hal_write32(&udma_struct->rx_size,size);
                hal_write32(&udma_struct->rx_cfg,config);
            }
            break;
        // push to tx channel
        case(TX_CHANNEL):
            {
                hal_write32(&udma_struct->tx_saddr,addr);
                hal_write32(&udma_struct->tx_size,size);
                hal_write32(&udma_struct->tx_cfg,config);
            }
            break;
        default:
            break;
    }
}

static inline void udma_channel_clear(udma_core_t *udma, udma_channel_e channel)
{
    switch (channel)
    {
    case RX_CHANNEL :
        hal_write32(&(udma->rx_cfg), UDMA_CORE_RX_CFG_CLR(1));
        break;
    case TX_CHANNEL :
        hal_write32(&(udma->tx_cfg), UDMA_CORE_TX_CFG_CLR(1));
        break;
    default :
        break;
    }
}

static inline uint32_t udma_channel_busy_get(udma_core_t *udma, udma_channel_e channel)
{
    uint32_t cfg = 0;
    switch (channel)
    {
    case RX_CHANNEL :
        cfg = hal_read32(&(udma->rx_cfg));
        return ((cfg & UDMA_CORE_RX_CFG_PENDING_MASK) >> UDMA_CORE_RX_CFG_PENDING_SHIFT);

    case TX_CHANNEL :
        cfg = hal_read32(&(udma->tx_cfg));
        return ((cfg & UDMA_CORE_TX_CFG_PENDING_MASK) >> UDMA_CORE_TX_CFG_PENDING_SHIFT);

    default :
        return 0xFFFFFFFF;
    }
}

static inline void udma_deinit_device(uint32_t device_id)
{
    // enable clock gating for device with device_id
    udma_ctrl_cg_enable(device_id);
}

#endif  /* __PI_HAL_UDMA_CORE_H__ */
