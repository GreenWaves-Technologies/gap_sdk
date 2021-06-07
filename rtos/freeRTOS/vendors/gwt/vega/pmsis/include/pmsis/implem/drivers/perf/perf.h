/*
 * Copyright (c) 2019, GreenWaves Technologies, Inc.
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

#ifndef __PI_PERF_H__
#define __PI_PERF_H__

#include "pmsis/implem/drivers/perf/perf_internal.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Driver data
 *****************************************************************************/

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

static inline void pi_perf_conf(unsigned events)
{
    #if (FEATURE_CLUSTER == 1)
    if (!pi_is_fc())
    {
        __pi_perf_cl_conf(events);
    }
    else
    #endif  /* FEATURE_CLUSTER */
    {
        __pi_perf_fc_conf(events);
    }
}

static inline void pi_perf_reset()
{
    /* Reset all performance counters to 0. */
    #if (FEATURE_CLUSTER == 1)
    if (!pi_is_fc())
    {
        __pi_perf_cl_reset();
    }
    else
    #endif  /* FEATURE_CLUSTER */
    {
        __pi_perf_fc_reset();
    }
}

static inline void pi_perf_fc_reset()
{
    __pi_perf_fc_reset();
}

static inline void pi_perf_cl_reset()
{
    /* Reset all performance counters to 0. */
    #if (FEATURE_CLUSTER == 1)
    __pi_perf_cl_reset();
    #endif  /* FEATURE_CLUSTER */
}

static inline void pi_perf_start()
{
    /* Initialize timer if needed and start counters. */
    #if (FEATURE_CLUSTER == 1)
    if (!pi_is_fc())
    {
        __pi_perf_cl_start();
    }
    else
    #endif  /* FEATURE_CLUSTER */
    {
        __pi_perf_fc_start();
    }
}

static inline void pi_perf_fc_start()
{
    __pi_perf_fc_start();
}

static inline void pi_perf_cl_start()
{
    /* Initialize timer if needed and start counters. */
    #if (FEATURE_CLUSTER == 1)
    __pi_perf_cl_start();
    #endif  /* FEATURE_CLUSTER */
}

static inline void pi_perf_stop()
{
    /* Stop counters and timers, and save values. */
    #if (FEATURE_CLUSTER == 1)
    if (!pi_is_fc())
    {
        __pi_perf_cl_stop();
    }
    else
    #endif  /* FEATURE_CLUSTER */
    {
        __pi_perf_fc_stop();
    }
}

static inline unsigned int pi_perf_read(int id)
{
    #if (FEATURE_CLUSTER == 1)
    if (!pi_is_fc())
    {
        return __pi_perf_cl_read(id);
    }
    else
    #endif  /* FEATURE_CLUSTER */
    {
        return __pi_perf_fc_read(id);
    }
}

static inline unsigned int pi_perf_fc_read(int id)
{
    return __pi_perf_fc_read(id);
}

static inline unsigned int pi_perf_cl_read(int id)
{
    #if (FEATURE_CLUSTER == 1)
    return __pi_perf_cl_read(id);
    #endif  /* FEATURE_CLUSTER */
}

#endif  /* __PI_PERF_H__ */
