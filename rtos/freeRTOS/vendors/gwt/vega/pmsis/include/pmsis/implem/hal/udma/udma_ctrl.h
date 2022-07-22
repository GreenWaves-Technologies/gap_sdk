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
#include "pmsis/implem/hal/soc_eu/soc_eu.h"


/* CG register. */
static inline uint32_t udma_ctrl_cg_get(void)
{
    return GAP_READ(&(udma_ctrl->cg), 0);
}

static inline void udma_ctrl_cg_set(uint32_t cg)
{
    GAP_WRITE(&(udma_ctrl->cg), 0, cg);
}

/* EVENT register. */
static inline uint32_t udma_ctrl_event_get(void)
{
    return GAP_READ(&(udma_ctrl->event), 0);
}

static inline void udma_ctrl_event_set(uint32_t event)
{
    GAP_WRITE(&(udma_ctrl->event), 0, event);
}


static inline void hal_udma_ctrl_cg_disable(uint32_t udma_device_id)
{
    uint32_t cg = udma_ctrl_cg_get();
    cg = GAP_BINSERT_R(cg, 1, 1, udma_device_id);
    udma_ctrl_cg_set(cg);
}

static inline void hal_udma_ctrl_cg_enable(uint32_t udma_device_id)
{
    uint32_t cg = udma_ctrl_cg_get();
    cg = GAP_BINSERT_R(cg, 0, 1, udma_device_id);
    udma_ctrl_cg_set(cg);
}


static inline void hal_udma_ctrl_event_forward_enable(uint8_t udma_evt_nb,
                                                      uint32_t event_id)
{
    #if 0
    // clear current event
    hal_and32(&UDMA_GC->EVTIN,
              ~(UDMA_GC_EVTIN_MASK(0xFF) << UDMA_GC_EVTIN_SHIFT_ID(udma_evt_nb)));
    // set new event
    hal_or32(&UDMA_GC->EVTIN,
             (UDMA_GC_EVTIN_MASK(event_id) << UDMA_GC_EVTIN_SHIFT_ID(udma_evt_nb)));
    #else
    uint32_t udma_evt_off = (udma_evt_nb << 3);
    uint32_t event = udma_ctrl_event_get();
    event = GAP_BINSERT_R(event, event_id, 8, udma_evt_off);
    udma_ctrl_event_set(event);
    #endif

    // tell soc eu to forward us the event
    hal_soc_eu_set_pr_mask(event_id);
}

static inline void hal_udma_ctrl_event_forward_disable(uint8_t udma_evt_nb,
                                                       uint32_t event_id)
{
    #if 0
    // clear event from udma forward
    hal_and32(&UDMA_GC->EVTIN,
              ~(UDMA_GC_EVTIN_MASK(0xFF) << UDMA_GC_EVTIN_SHIFT_ID(udma_evt_nb)));
    #else
    uint32_t udma_evt_off = (udma_evt_nb << 3);
    uint32_t event = udma_ctrl_event_get();
    event = GAP_BINSERT_R(event, 0x00, 8, udma_evt_off);
    udma_ctrl_event_set(event);
    #endif

    // tell soc eu to stop forwarding us the event
    hal_soc_eu_clear_pr_mask(event_id);
}


/* Compat. */
#define udma_ctrl_cg_disable hal_udma_ctrl_cg_disable
#define udma_init_device     hal_udma_ctrl_cg_disable

#define udma_ctrl_cg_enable  hal_udma_ctrl_cg_enable
#define udma_deinit_device   hal_udma_ctrl_cg_enable
