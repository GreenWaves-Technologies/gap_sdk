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

#ifndef _PMSIS_FC_EVENT_H_
#define _PMSIS_FC_EVENT_H_

#include "pmsis.h"

/*!
 * @addtogroup FC_EventHandler
 * @{
 */

/*******************************************************************************
 * Variables, macros, structures,... definitions
 ******************************************************************************/

/*******************************************************************************
 * APIs
 ******************************************************************************/

#define NB_SOC_EVENTS    ( SOC_EU_NB_FC_EVENTS )

typedef void (*pi_fc_event_handler_t)(void* arg);

void pi_fc_event_handler_init(uint32_t fc_event_irq);

/*!
 * @brief FC event handler.
 *
 * This function pops an event and executes the handler corresponding to the event.
 */
void fc_soc_event_handler(void);

void pi_fc_event_handler_set(uint32_t event_id, pi_fc_event_handler_t event_handler);

void pi_fc_event_handler_clear(uint32_t event_id);

void clear_fc_event_handler(uint32_t event_id);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

/* @} */

#endif /*_PMSIS_FC_EVENT_H_*/
