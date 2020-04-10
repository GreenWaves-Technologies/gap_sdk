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

#ifndef __PI_PERF_INTERNAL_H__
#define __PI_PERF_INTERNAL_H__

#include "pmsis/drivers/perf.h"
#include "pmsis/chips/gap8/perf.h"
#include "pmsis/implem/drivers/timer/timer.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Driver data
 *****************************************************************************/

typedef struct
{
    uint32_t perf_mask;                     /*!< Mask of Performance counters being used. */
    uint32_t perf_counter[PCER_EVENTS_NUM]; /*!< Performance counters value. */
} perf_t;

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

/**
 * \brief Configure FC perf counter.
 *
 * This function configures Performance Counter with given events mask.
 *
 * \param events         Events mask.
 */
void __pi_perf_fc_conf(uint32_t events);

/**
 * \brief Reset FC perf counters.
 *
 * This function resets all Performance Counter.
 */
void __pi_perf_fc_reset();

/**
 * \brief Start FC perf counters.
 *
 * This function starts Performance Counter.
 */
void __pi_perf_fc_start();

/**
 * \brief Stop FC perf counters.
 *
 * This function stops Performance Counter.
 */
void __pi_perf_fc_stop();

/**
 * \brief Read FC perf counters value.
 *
 * This function returns Performance Counter for a given event.
 *
 * \param id             ID of the event.
 *
 * \return Value         Counter's value for the given event.
 */
uint32_t __pi_perf_fc_read(int id);

#if defined(FEATURE_CLUSTER)
/**
 * \brief Configure Cluster perf counter.
 *
 * This function configures Performance Counter with given events mask.
 *
 * \param events         Events mask.
 */
void __pi_perf_cl_conf(uint32_t events);

/**
 * \brief Reset Cluster perf counters.
 *
 * This function resets all Performance Counter.
 */
void __pi_perf_cl_reset();

/**
 * \brief Start Cluster perf counters.
 *
 * This function starts Performance Counter.
 */
void __pi_perf_cl_start();

/**
 * \brief Stop Cluster perf counters.
 *
 * This function stops Performance Counter.
 */
void __pi_perf_cl_stop();

/**
 * \brief Read Cluster perf counters value.
 *
 * This function returns Performance Counter for a given event.
 *
 * \param id             ID of the event.
 *
 * \return Value         Counter's value for the given event.
 */
uint32_t __pi_perf_cl_read(int id);

#endif  /* FEATURE_CLUSTER */

/*******************************************************************************
 * Internal functions
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

#endif  /* __PI_PERF_INTERNAL_H__ */
