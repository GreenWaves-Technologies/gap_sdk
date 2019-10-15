/*
 * Copyright (c) 2018, GreenWaves Technologies, Inc.
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

#ifndef _GAP_PERFORMANCE_H_
#define _GAP_PERFORMANCE_H_

#include <assert.h>
#include <string.h>
#include "cmsis.h"

/*!
 * @addtogroup Performance
 * @{
 */

/*******************************************************************************
 * Variables, macros, structures,... definitions
 ******************************************************************************/

#define PERFORMANCE_USING_TIMER_SHIFT 17U                                      /*!< PCER: Timer COUNT Position. */
#define PERFORMANCE_USING_TIMER_MASK  (0x1UL << PERFORMANCE_USING_TIMER_SHIFT) /*!< PCER: Timer COUNT Mask. */

/*!
 * @brief Performance counter structure.
 *
 * This structure holds information on the events used by the performance counter
 * and their value.
 */
typedef struct performance_s
{
    uint32_t events_mask;                /*!< Events' mask. */
    uint32_t count[PCER_EVENTS_NUM + 1]; /*!< Performance counter's value for each events of the above mask. */
} performance_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*!
 * @brief Start the performance counter.
 *
 * This function initializes the performance counter, configures it
 * according to the events specified through the mask parameter then enables the counter.
 *
 * @param base  Performance base pointer.
 * @param mask  Mask of the events.
 */
void PERFORMANCE_Start(performance_t *base, uint32_t mask);

/*!
 * @brief Stop the performance counter.
 *
 * This function stops the performance counter, saves each events' counter value
 * then disables the counter.
 *
 * @param base  Performance base pointer.
 */
void PERFORMANCE_Stop(performance_t *base);

/*!
 * @brief Get the performance counter's value for a specific event.
 *
 * This function returns the performance counter's value for the specified event.
 *
 * @param base  Performance base pointer.
 * @param event Event of the counter to return.
 *
 * @return Performance counter's value.
 */
uint32_t PERFORMANCE_Get(performance_t *base, uint32_t event);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

/* @} */

#endif /*_GAP_PERFORMANCE_H_*/
