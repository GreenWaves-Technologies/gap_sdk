#ifndef __PI_HAL_UDMA_CORE_H__
#define __PI_HAL_UDMA_CORE_H__

/**
 * Small low level driver for udma core main functionnalities
 */

#include "targets/pmsis_targets.h"
#include "pmsis_hal/GAP8/udma/udma_ctrl.h"

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

static inline void udma_deinit_device(uint32_t device_id)
{
    // enable clock gating for device with device_id
    udma_ctrl_cg_enable(device_id);
}

#endif  /* __PI_HAL_UDMA_CORE_H__ */
