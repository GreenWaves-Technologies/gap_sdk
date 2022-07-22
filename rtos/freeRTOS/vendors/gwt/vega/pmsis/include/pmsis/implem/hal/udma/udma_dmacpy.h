/*
 * Copyright (c) 2020, GreenWaves Technologies, Inc.
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

#ifndef __PMSIS_HAL_UDMA_UDMA_DMACPY_H__
#define __PMSIS_HAL_UDMA_UDMA_DMACPY_H__

#include "pmsis/targets/target.h"
#include "pmsis/implem/hal/udma/udma_core.h"
#include "pmsis/implem/hal/udma/udma_ctrl.h"

/* DMACPY udma configuration. */
static inline void dmacpy_udma_channel_set(uint32_t device_id, uint32_t l2_buf,
                                           uint32_t size, uint32_t cfg,
                                           udma_channel_e channel)
{
    udma_enqueue_channel(&(dmacpy(device_id)->udma), l2_buf, size, cfg, channel);
}


/* DST_ADDR Register. */
static inline void dmacpy_dst_addr_set(uint32_t device_id, uint32_t addr)
{
    hal_write32(&(dmacpy(device_id)->dst_addr), addr);
}

static inline uint32_t dmacpy_dst_addr_get(uint32_t device_id)
{
    return hal_read32(&(dmacpy(device_id)->dst_addr));
}


/* SRC_ADDR Register. */
static inline void dmacpy_src_addr_set(uint32_t device_id, uint32_t addr)
{
    hal_write32(&(dmacpy(device_id)->src_addr), addr);
}

static inline uint32_t dmacpy_src_addr_get(uint32_t device_id)
{
    return hal_read32(&(dmacpy(device_id)->src_addr));
}


/* MEM_SEL Register. */
static inline void dmacpy_mem_sel_set(uint32_t device_id, uint32_t mem_sel)
{
    hal_write32(&(dmacpy(device_id)->mem_sel), mem_sel);
}

static inline uint32_t dmacpy_mem_sel_get(uint32_t device_id)
{
    return hal_read32(&(dmacpy(device_id)->mem_sel));
}


/*! DST_ADDR. */
static inline void hal_dmacpy_dst_addr_set(uint32_t device_id, uint32_t addr)
{
    dmacpy_dst_addr_set(device_id, addr);
}

static inline uint32_t hal_dmacpy_dst_addr_get(uint32_t device_id)
{
    return dmacpy_dst_addr_get(device_id);
}


/*! SRC_ADDR. */
static inline void hal_dmacpy_src_addr_set(uint32_t device_id, uint32_t addr)
{
    dmacpy_src_addr_set(device_id, addr);
}

static inline uint32_t hal_dmacpy_src_addr_get(uint32_t device_id)
{
    return dmacpy_src_addr_get(device_id);
}


/*! MEM_SEL. */
static inline void hal_dmacpy_mem_sel_set(uint32_t device_id, uint32_t mem_sel)
{
    dmacpy_mem_sel_set(device_id, mem_sel);
}

static inline uint32_t hal_dmacpy_mem_sel_get(uint32_t device_id)
{
    return dmacpy_mem_sel_get(device_id);
}


/*! UDMA. */
static inline void hal_dmacpy_enqueue(uint32_t device_id, uint32_t l2_buf,
                                      uint32_t size, uint32_t cfg, uint32_t src,
                                      uint32_t dst, uint32_t mem_sel,
                                      udma_channel_e channel)
{
    cfg |= UDMA_CORE_RX_CFG_EN(1);
    dmacpy_dst_addr_set(device_id, dst);
    dmacpy_src_addr_set(device_id, src);
    dmacpy_mem_sel_set(device_id, mem_sel);
    dmacpy_udma_channel_set(device_id, l2_buf, size, cfg, channel);
}

#endif  /* __PMSIS_HAL_UDMA_UDMA_DMACPY_H__ */
