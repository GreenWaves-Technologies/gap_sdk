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

#ifndef __ARCHI_CLUSTER_EVENT_UNIT_REGMAP_H__
#define __ARCHI_CLUSTER_EVENT_UNIT_REGMAP_H__

#include <stdint.h>

/*! Event_Unit Core Demux */
typedef struct
{
    volatile uint32_t event_mask;
    volatile uint32_t event_mask_and;
    volatile uint32_t event_mask_or;
    volatile uint32_t irq_mask;
    volatile uint32_t irq_mask_and;
    volatile uint32_t irq_mask_or;
    volatile uint32_t status;
    volatile uint32_t buffer;
    volatile uint32_t buffer_masked;
    volatile uint32_t buffer_irq_masked;
    volatile uint32_t buffer_clear;
    volatile uint32_t sw_evt_mask;
    volatile uint32_t sw_evt_mask_and;
    volatile uint32_t sw_evt_mask_or;
    volatile uint32_t event_wait;
    volatile uint32_t event_wait_clear;
    volatile uint32_t sec_irq_mask;
    volatile uint32_t sec_irq_mask_and;
    volatile uint32_t sec_irq_mask_or;
} __attribute__((packed)) cluster_eu_core_demux_t;

/*! Event_Unit Loop Demux */
typedef struct
{
    volatile uint32_t state;
    volatile uint32_t start;
    volatile uint32_t end;
    volatile uint32_t incr;
    volatile uint32_t chunk;
    volatile uint32_t epoch;
    volatile uint32_t single;
} __attribute__((packed)) cluster_eu_loop_demux_t;

/*! Event_Unit Dispatch Demux */
typedef struct
{
    volatile uint32_t fifo_access;
    volatile uint32_t team_config;
} __attribute__((packed)) cluster_eu_dispatch_demux_t;

/*! Event_Unit Mutex Demux */
typedef struct
{
    volatile uint32_t mutex;
} __attribute__((packed)) cluster_eu_mutex_demux_t;

/*! Event_Unit SW_Events Demux */
typedef struct
{
    volatile uint32_t trigger[8];
    volatile uint32_t reserved_0[8];
    volatile uint32_t trigger_wait[8];
    volatile uint32_t reserved_1[8];
    volatile uint32_t trigger_wait_clear[8];
} __attribute__((packed)) cluster_eu_sw_evt_demux_t;

/*! Event_Unit Barrier Demux */
typedef struct
{
    volatile uint32_t trigger_mask;
    volatile uint32_t status;
    volatile uint32_t status_summary;
    volatile uint32_t target_mask;
    volatile uint32_t trigger;
    volatile uint32_t trigger_self;
    volatile uint32_t trigger_wait;
    volatile uint32_t trigger_wait_clear;
} __attribute__((packed)) cluster_eu_barrier_demux_t;

#endif  /* __ARCHI_CLUSTER_EVENT_UNIT_REGMAP_H__ */
