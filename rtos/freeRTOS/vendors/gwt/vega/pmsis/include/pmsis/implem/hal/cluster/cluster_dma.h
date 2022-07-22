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

#include <stdint.h>
#include "pmsis/targets/target.h"

#define CL_DMA_FIX        ( 0 )
#define CL_DMA_INC        ( 1 )
#define CL_DMA_IS_1D_EXT  ( 0 )
#define CL_DMA_IS_2D_EXT  ( 1 )
#define CL_DMA_IS_1D_TCDM ( 0 )
#define CL_DMA_IS_2D_TCDM ( 1 )
#define CL_DMA_ELE_DIS    ( 0 )
#define CL_DMA_ELE_ENA    ( 1 )
#define CL_DMA_ILE_DIS    ( 0 )
#define CL_DMA_ILE_ENA    ( 1 )
#define CL_DMA_BLE_DIS    ( 0 )
#define CL_DMA_BLE_ENA    ( 1 )

/* CMD Register bitfield. */
/* Used to format command. */
#define CL_DMA_LEN_SHIFT      ( 0 )
#define CL_DMA_LEN_WIDTH      ( 17 )
#define CL_DMA_LEN_MASK       ( 0x1FFFF << CL_DMA_LEN_SHIFT)

#define CL_DMA_TYPE_SHIFT     ( 17 )
#define CL_DMA_TYPE_WIDTH     ( 1 )
#define CL_DMA_TYPE_MASK      ( 0x1 << CL_DMA_TYPE_SHIFT)

#define CL_DMA_INC_SHIFT      ( 18 )
#define CL_DMA_INC_WIDTH      ( 1 )
#define CL_DMA_INC_MASK       ( 0x1 << CL_DMA_INC_SHIFT)

#define CL_DMA_2D_EXT_SHIFT   ( 19 )
#define CL_DMA_2D_EXT_WIDTH   ( 1 )
#define CL_DMA_2D_EXT_MASK    ( 0x1 << CL_DMA_2D_EXT_SHIFT)

#define CL_DMA_ELE_SHIFT      ( 20 )
#define CL_DMA_ELE_WIDTH      ( 1 )
#define CL_DMA_ELE_MASK       ( 0x1 << CL_DMA_ELE_SHIFT)

#define CL_DMA_ILE_SHIFT      ( 21 )
#define CL_DMA_ILE_WIDTH      ( 1 )
#define CL_DMA_ILE_MASK       ( 0x1 << CL_DMA_ILE_SHIFT)

#define CL_DMA_BLE_SHIFT      ( 22 )
#define CL_DMA_BLE_WIDTH      ( 1 )
#define CL_DMA_BLE_MASK       ( 0x1 << CL_DMA_BLE_SHIFT)

#define CL_DMA_2D_TCDM_SHIFT  ( 23 )
#define CL_DMA_2D_TCDM_WIDTH  ( 1 )
#define CL_DMA_2D_TCDM_MASK   ( 0x1 << CL_DMA_2D_TCDM_SHIFT)


/* CMD specific. */
/* Get TID. */
#define CL_DMA_CMD_TID_GET_SHIFT       ( 0 )
#define CL_DMA_CMD_TID_GET_WIDTH       ( 4 )
#define CL_DMA_CMD_TID_GET_MASK        ( 0xF << CL_DMA_CMD_TID_GET_SHIFT )
/* TCDM ADDR. */
#define CL_DMA_CMD_TCDM_ADDR_SHIFT     ( 0 )
#define CL_DMA_CMD_TCDM_ADDR_WIDTH     ( 32 )
#define CL_DMA_CMD_TCDM_ADDR_MASK      ( 0xFFFFFFFF << CL_DMA_CMD_TCDM_ADDR_SHIFT )
/* EXT ADDR. */
#define CL_DMA_CMD_EXT_ADDR_SHIFT      ( 0 )
#define CL_DMA_CMD_EXT_ADDR_WIDTH      ( 32 )
#define CL_DMA_CMD_EXT_ADDR_MASK       ( 0xFFFFFFFF << CL_DMA_CMD_EXT_ADDR_SHIFT )
/* 2D STRIDE. */
#define CL_DMA_CMD_2D_STRIDE_SHIFT     ( 0 )
#define CL_DMA_CMD_2D_STRIDE_WIDTH     ( 32 )
#define CL_DMA_CMD_2D_STRIDE_MASK      ( 0xFFFFFFFF << CL_DMA_CMD_2D_STRIDE_SHIFT )
/* 2D COUNT. */
#define CL_DMA_CMD_2D_COUNT_SHIFT      ( 0 )
#define CL_DMA_CMD_2D_COUNT_WIDTH      ( 32 )
#define CL_DMA_CMD_2D_COUNT_MASK       ( 0xFFFFFFFF << CL_DMA_CMD_2D_COUNT_SHIFT )

/* STATUS specific. */
/* Free TID[i] or status of TID[i]. */
#define CL_DMA_STATUS_TID_STATUS_SHIFT ( 0 )
#define CL_DMA_STATUS_TID_STATUS_WIDTH ( 16 )
#define CL_DMA_STATUS_TID_STATUS_MASK  ( 0xFFFF << CL_DMA_STATUS_TID_STATUS_SHIFT )
/* Status of transfer allocator for TID[i]. */
#define CL_DMA_STATUS_TID_ALLOC_SHIFT  ( 16 )
#define CL_DMA_STATUS_TID_ALLOC_WIDTH  ( 16 )
#define CL_DMA_STATUS_TID_ALLOC_MASK   ( 0xFFFF0000 << CL_DMA_STATUS_TID_ALLOC_SHIFT )



/* Form a command from a transfer's size, 2d,... */
static inline uint32_t hal_cl_dma_cmd_make(uint32_t len, uint8_t type,
                                           uint8_t inc, uint8_t ext_2d,
                                           uint8_t ele, uint8_t ile, uint8_t ble,
                                           uint8_t tcdm_2d)
{
    uint32_t cmd = 0;
    cmd = ((len     << CL_DMA_LEN_SHIFT     ) |
           (type    << CL_DMA_TYPE_SHIFT    ) |
           (inc     << CL_DMA_INC_SHIFT     ) |
           (ext_2d  << CL_DMA_2D_EXT_SHIFT  ) |
           (ele     << CL_DMA_ELE_SHIFT     ) |
           (ile     << CL_DMA_ILE_SHIFT     ) |
           (ble     << CL_DMA_BLE_SHIFT     ) |
           (tcdm_2d << CL_DMA_2D_TCDM_SHIFT ));
    return cmd;
}

/* CMD register. */
static inline void hal_cl_dma_cmd_set(uint32_t device_id, uint32_t cmd)
{
    cl_dma_cmd_set((uint32_t) cl_dma(device_id), cmd);
}

static inline uint32_t hal_cl_dma_cmd_get(uint32_t device_id)
{
    return cl_dma_cmd_get((uint32_t) cl_dma(device_id));
}

/* STATUS register. */
static inline void hal_cl_dma_status_set(uint32_t device_id, uint32_t status)
{
    cl_dma_status_set((uint32_t) cl_dma(device_id), status);
}

static inline uint32_t hal_cl_dma_status_get(uint32_t device_id)
{
    return cl_dma_status_get((uint32_t) cl_dma(device_id));
}


/*! CMD. */
/* Get transfer ID(TID). */
static inline uint16_t hal_cl_dma_tid_get(uint32_t device_id)
{
    return (hal_cl_dma_cmd_get(device_id) & CL_DMA_STATUS_TID_STATUS_MASK);
}

/* Enqueue a 1D transfer. */
static inline void hal_cl_dma_1d_transfer_push(uint32_t device_id, uint32_t cmd,
                                               uint32_t loc, uint32_t ext)
{
    hal_cl_dma_cmd_set(device_id, cmd);
    hal_cl_dma_cmd_set(device_id, loc);
    hal_cl_dma_cmd_set(device_id, ext);
}

/* Enqueue a 2D transfer. */
static inline void hal_cl_dma_2d_transfer_push(uint32_t device_id, uint32_t cmd,
                                               uint32_t loc, uint32_t ext,
                                               uint32_t stride, uint32_t length,
                                               uint32_t tcdm_2d)
{
    hal_cl_dma_cmd_set(device_id, cmd);
    hal_cl_dma_cmd_set(device_id, loc);
    hal_cl_dma_cmd_set(device_id, ext);
    hal_cl_dma_cmd_set(device_id, length);
    hal_cl_dma_cmd_set(device_id, stride);
    if (tcdm_2d)
    {
        hal_cl_dma_cmd_set(device_id, length);
        hal_cl_dma_cmd_set(device_id, stride);
    }
}

/*! STATUS. */
/* Free a TID. */
static inline void hal_cl_dma_tid_free(uint32_t device_id, uint8_t tid)
{
    hal_cl_dma_status_set(device_id, 1 << tid);
}

/* Flush TID counters : wait end of all transfers & free all counters. */
static inline void hal_cl_dma_flush(uint32_t device_id)
{
    while (hal_cl_dma_status_get(device_id) & CL_DMA_STATUS_TID_STATUS_MASK)
    {
        hal_cl_eu_event_mask_wait(1 << CL_IRQ_DMA0);
    }
    hal_cl_dma_status_set(device_id, -1); /* -1 = 0xFFFFFFFFu */
}

/* Wait end of one transfer, then free the slot. */
static inline void hal_cl_dma_wait(uint32_t device_id, uint8_t tid)
{
    while (hal_cl_dma_status_get(device_id) & (1 << tid))
    {
        hal_cl_eu_event_mask_wait(1 << CL_IRQ_DMA0);
    }
    hal_cl_dma_tid_free(device_id, tid);
}
