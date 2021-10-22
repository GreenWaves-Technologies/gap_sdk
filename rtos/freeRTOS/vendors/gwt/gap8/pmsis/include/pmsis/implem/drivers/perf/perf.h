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

#include "pmsis/drivers/perf.h"
#include "pmsis/chips/gap8/perf.h"
#include "pmsis/implem/drivers/timer/timer.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define CSR_PCMR_DISABLE    ( 0x0 ) /* Disable Performance counters. */
#define CSR_PCMR_ENABLE     ( 0x1 ) /* Enable Performance counters. */
#define CSR_PCMR_SATURATE   ( 0x2 ) /* Activate counter saturation. */

#define PERF_TIMER_FC       ( FC_TIMER_1 ) /* FC Timer for perf counter. */
#define PERF_TIMER_CL       ( CL_TIMER_0 ) /* CL Timer for perf counter. */

/*******************************************************************************
 * Driver data
 *****************************************************************************/

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

static inline void __pi_perf_mask_events_set(uint32_t mask)
{
    __PCER_Set(mask);
}

static inline void __pi_perf_counters_reset()
{
    __PCCR31_Set(0);
}

static inline void __pi_perf_counter_enable(uint32_t mask)
{
    __PCMR_Set(mask);
}

static inline uint32_t __pi_perf_counter_get(uint32_t event)
{
    return __PCCRs_Get(event);
}

static inline void pi_perf_fc_reset()
{
    /* Reset Timer counter. */
    pi_timer_reset(PERF_TIMER_FC);

    /* Reset all Performace Counter Counter Register(PCCR) to 0. */
    __pi_perf_counters_reset();
}

static inline void pi_perf_fc_start()
{
    /* Start timer if used. */
    pi_timer_start(PERF_TIMER_FC);

    /* Enable performance counters. */
    __pi_perf_counter_enable(CSR_PCMR_ENABLE | CSR_PCMR_SATURATE);
}

static inline void pi_perf_fc_stop()
{
    /* Disable performance counters. */
    __pi_perf_counter_enable(CSR_PCMR_DISABLE);

    /* Disable timer if used. */
    pi_timer_stop(PERF_TIMER_FC);
}

static inline uint32_t pi_perf_fc_read(uint32_t event)
{
    uint32_t val = 0;
    if (event == PI_PERF_CYCLES)
    {
        val = pi_timer_value_read(PERF_TIMER_FC);
    }
    else
    {
        val = __pi_perf_counter_get(event);
    }
    return val;
}

#if defined(FEATURE_CLUSTER)
static inline void pi_perf_cl_reset()
{
    uint8_t cid = pi_core_id();

    /* Reset Timer counter. */
    if (cid == (uint8_t) ARCHI_CLUSTER_MASTER_CORE)
    {
        pi_timer_reset(PERF_TIMER_CL);
    }
    /* Reset all Performace COunter Counter Register(PCCR) to 0. */
    __pi_perf_counters_reset();
}

static inline void pi_perf_cl_start()
{
    uint8_t cid = pi_core_id();

    /* Start timer if used. */
    if (cid == (uint8_t) ARCHI_CLUSTER_MASTER_CORE)
    {
        pi_timer_start(PERF_TIMER_CL);
    }

    /* Enable performance counters. */
    __pi_perf_counter_enable(CSR_PCMR_ENABLE | CSR_PCMR_SATURATE);
}

static inline void pi_perf_cl_stop()
{
    uint8_t cid = pi_core_id();

    /* Disable performance counters. */
    __pi_perf_counter_enable(CSR_PCMR_DISABLE);

    /* Disable timer if used. */
    if (cid == (uint8_t) ARCHI_CLUSTER_MASTER_CORE)
    {
        pi_timer_stop(PERF_TIMER_CL);
    }
}

static inline uint32_t pi_perf_cl_read(uint32_t event)
{
    uint32_t val = 0;
    if (event == PI_PERF_CYCLES)
    {
        val = pi_timer_value_read(PERF_TIMER_CL);
    }
    else
    {
        val = __pi_perf_counter_get(event);
    }
    return val;
}
#endif  /* FEATURE_CLUSTER */

/*******************************************************************************
 * API implementation
 ******************************************************************************/

static inline void pi_perf_conf(unsigned events)
{
    /* Set Performance Counter Event Register(PCER) with given events mask. */
    __pi_perf_mask_events_set(events & ~(1 << PI_PERF_CYCLES));
}

static inline void pi_perf_reset()
{
    /* Reset all performance counters to 0. */
    #if defined(FEATURE_CLUSTER)
    if (!pi_is_fc())
    {
        pi_perf_cl_reset();
    }
    else
    #endif  /* FEATURE_CLUSTER */
    {
        pi_perf_fc_reset();
    }
}

static inline void pi_perf_start()
{
    /* Initialize timer if needed and start counters. */
    #if defined(FEATURE_CLUSTER)
    if (!pi_is_fc())
    {
        pi_perf_cl_start();
    }
    else
    #endif  /* FEATURE_CLUSTER */
    {
        pi_perf_fc_start();
    }
}

static inline void pi_perf_stop()
{
    /* Stop counters and timers, and save values. */
    #if defined(FEATURE_CLUSTER)
    if (!pi_is_fc())
    {
        pi_perf_cl_stop();
    }
    else
    #endif  /* FEATURE_CLUSTER */
    {
        pi_perf_fc_stop();
    }
}

static inline unsigned int pi_perf_read(int id)
{
    #if defined(FEATURE_CLUSTER)
    if (!pi_is_fc())
    {
        return pi_perf_cl_read(id);
    }
    else
    #endif  /* FEATURE_CLUSTER */
    {
        return pi_perf_fc_read(id);
    }
}
