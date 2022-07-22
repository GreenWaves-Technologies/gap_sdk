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


/*! Fetch enable. */
static inline void hal_cl_ctrl_fetch_enable(uint32_t device_id, uint32_t nb_core)
{
    uint32_t base = (uint32_t) cl_glob_ctrl(device_id);
    uint32_t fetch = (((1 << nb_core) - 1));
    cluster_ctrl_unit_fetch_en_set(base, fetch);
}

static inline void hal_cl_ctrl_fetch_mask_enable(uint32_t device_id, uint32_t core_mask)
{
    uint32_t base = (uint32_t) cl_glob_ctrl(device_id);
    cluster_ctrl_unit_fetch_en_set(base, core_mask);
}


/*! ClockGate cluster. */
static inline void hal_cl_ctrl_clock_gate_enable(uint32_t device_id)
{
    uint32_t base = (uint32_t) cl_glob_ctrl(device_id);
    uint32_t cg = (CLUSTER_CTRL_UNIT_CLOCK_GATE_EN(1));
    cluster_ctrl_unit_clock_gate_set(base, cg);
}

static inline void hal_cl_ctrl_clock_gate_disable(uint32_t device_id)
{
    uint32_t base = (uint32_t) cl_glob_ctrl(device_id);
    uint32_t cg = (CLUSTER_CTRL_UNIT_CLOCK_GATE_EN(0));
    cluster_ctrl_unit_clock_gate_set(base, cg);
}


/*! Boot address. */
static inline void hal_cl_ctrl_boot_addr_set(uint32_t device_id, uint8_t core_id,
                                             uint32_t boot_addr)
{
    uint32_t base = (uint32_t) cl_glob_ctrl(device_id);
    uint32_t offset = (CLUSTER_CTRL_UNIT_BOOT_ADDR0_OFFSET + (core_id << 2));
    GAP_WRITE(base, offset, boot_addr);
}

