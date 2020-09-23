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


#ifndef __PI_FREQ_H__
#define __PI_FREQ_H__

#include "pmsis.h"
#include "pmsis/implem/drivers/drivers.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Driver data
 ******************************************************************************/

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

/*******************************************************************************
 * API implementation
 ******************************************************************************/

static inline uint32_t pi_freq_get(pi_freq_domain_e domain)
{
    switch(domain)
    {
    case PI_FREQ_DOMAIN_FC :
        return pi_fll_get_frequency(FLL_SOC, 0);
    case PI_FREQ_DOMAIN_CL :
        return pi_fll_get_frequency(FLL_CLUSTER, 0);
    case PI_FREQ_DOMAIN_PERIPH :
        return pi_fll_get_frequency(FLL_SOC, 0);
    default :
        return 0;
    }
}

static inline int32_t pi_freq_set(pi_freq_domain_e domain, uint32_t freq)
{
    int32_t retval = -1;
    switch(domain)
    {
    case PI_FREQ_DOMAIN_FC :
        retval = pi_fll_set_frequency(FLL_SOC, freq, 1);
        break;
    case PI_FREQ_DOMAIN_CL :
        retval = pi_fll_set_frequency(FLL_CLUSTER, freq, 1);
        break;
    case PI_FREQ_DOMAIN_PERIPH :
        retval = pi_fll_set_frequency(FLL_SOC, freq, 1);
        break;
    default :
        retval = -1;
    }

    return ((retval == -1) ? -1 : 0);
}

#endif  /* __PI_FREQ_H__ */
