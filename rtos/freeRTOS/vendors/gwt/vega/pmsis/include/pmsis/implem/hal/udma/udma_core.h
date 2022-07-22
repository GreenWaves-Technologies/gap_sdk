/*
 * Copyright (c) 2021, GreenWaves Technologies, Inc.
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

#pragma once

#include "pmsis/targets/target.h"
#include "pmsis/implem/hal/udma/udma_ctrl.h"


#define UDMA_MAX_SIZE_LOG2    (17)
#define UDMA_MAX_SIZE         (1 << UDMA_MAX_SIZE_LOG2)

#define UDMA_CHAN_SADDR_OFF   ( 0 )
#define UDMA_CHAN_SIZE_OFF    ( 1 )
#define UDMA_CHAN_CFG_OFF     ( 2 )


typedef enum
{
    RX_CHANNEL      = 0,
    TX_CHANNEL      = 1,
    CMD_CHANNEL     = 2,
    COMMAND_CHANNEL = 2
} udma_channel_e;


/* uDMA channel enqueue. */
static inline void hal_udma_enqueue_channel(udma_core_cmd_t *udma,
                                            uint32_t addr, uint32_t size,
                                            uint32_t config, udma_channel_e channel)
{
    config |= UDMA_CORE_CMD_RX_CFG_EN_MASK;
    uint32_t chan_off = (channel << 4);
    GAP_WRITE(udma, chan_off + (UDMA_CHAN_SADDR_OFF << 2), addr);
    GAP_WRITE(udma, chan_off + (UDMA_CHAN_SIZE_OFF << 2), size);
    GAP_WRITE(udma, chan_off + (UDMA_CHAN_CFG_OFF << 2), config);
}

static inline void hal_udma_channel_clear(udma_core_cmd_t *udma,
                                          udma_channel_e channel)
{
    uint32_t chan_off = (channel << 4);
    uint32_t config = UDMA_CORE_CMD_RX_CFG_CLR(1);
    GAP_WRITE(udma, chan_off + (UDMA_CHAN_CFG_OFF << 2), config);
}

static inline uint32_t hal_udma_channel_busy_get(udma_core_cmd_t *udma,
                                                 udma_channel_e channel)
{
    uint32_t chan_off = (channel << 4);
    uint32_t cfg = GAP_READ(udma, chan_off + (UDMA_CHAN_CFG_OFF << 2));
    return ((cfg & UDMA_CORE_CMD_RX_CFG_PENDING_MASK) >> UDMA_CORE_CMD_RX_CFG_PENDING_SHIFT);
}

/* Compat. */
#define udma_channel_busy_get hal_udma_channel_busy_get
#define udma_channel_clear    hal_udma_channel_clear
#define udma_enqueue_channel  hal_udma_enqueue_channel
