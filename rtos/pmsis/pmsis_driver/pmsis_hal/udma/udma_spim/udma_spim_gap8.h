#ifndef __UDMA_SPIM_GAP8_H__
#define __UDMA_SPIM_GAP8_H__

#include "targets/pmsis_targets.h"

static inline void spim_enqueue_channel(spi_t *spim, uint32_t addr,
        uint32_t size, uint32_t config, udma_channel_e channel)
{
    udma_enqueue_channel(&spim->udma, addr, size, config, channel);
}

#endif
