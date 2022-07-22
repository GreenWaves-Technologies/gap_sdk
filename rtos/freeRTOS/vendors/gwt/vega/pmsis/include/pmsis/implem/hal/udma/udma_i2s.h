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
#include "pmsis/implem/hal/udma/udma_core.h"
#include "pmsis/implem/hal/udma/udma_ctrl.h"

/* Compat PulpOs. */
#define __RT_FLAGS_MUX_BIT   16
#define __RT_FLAGS_MUX_WIDTH 2

/** Specify that the I2S channel should be multiplexed with only 1 stream. */
#define RT_I2S_FLAGS_MUX_1CH       (0<<__RT_FLAGS_MUX_BIT)

/** Specify that the I2S channel should be multiplexed with 2 streams on the same pad. */
#define RT_I2S_FLAGS_MUX_1CH_DUAL  (1<<__RT_FLAGS_MUX_BIT)

/** Specify that the I2S channel should be multiplexed with 2 streams, each one on a distinct pad. */
#define RT_I2S_FLAGS_MUX_2CH       (2<<__RT_FLAGS_MUX_BIT)

/** Specify that the I2S channel should be multiplexed with 4 streams, with the 2 first on the first pad, and the 2 lasts on the second pad. */
#define RT_I2S_FLAGS_MUX_2CH_DUAL  (3<<__RT_FLAGS_MUX_BIT)


/* I2S udma configuration. */
static inline void i2s_udma_channel_set(uint32_t device_id, uint32_t l2buf,
                                        uint32_t size, uint32_t cfg,
                                        udma_channel_e channel)
{
    hal_udma_enqueue_channel((udma_core_cmd_t *) &(i2s(device_id)->udma),
                             l2buf, size, cfg, channel);
    //printf("I2S: %x %x %x %x %d\n", &(i2s(device_id)->udma), l2buf, size, cfg, channel);
}



/*! UDMA. */
static inline void hal_udma_i2s_enqueue(uint32_t device_id, uint32_t l2buf,
                                        uint32_t size, uint32_t cfg,
                                        udma_channel_e channel)
{
    i2s_udma_channel_set(device_id, l2buf, size, cfg, channel);
}

static inline void hal_udma_i2s_clear(uint32_t device_id, udma_channel_e channel)
{
    hal_udma_channel_clear((udma_core_cmd_t *) &(i2s(device_id)->udma), channel);
}
