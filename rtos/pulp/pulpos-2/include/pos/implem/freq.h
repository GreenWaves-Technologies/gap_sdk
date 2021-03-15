/*
 * Copyright (C) 2019 GreenWaves Technologies
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* 
 * Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */

#ifndef __POS_IMPLEM_FREQ_H__
#define __POS_IMPLEM_FREQ_H__

#include "pmsis.h"

#define PI_FLL_PERIPH POS_FLL_PERIPH
#define PI_FLL_FC POS_FLL_FC
#define PI_FLL_CL POS_FLL_CL
#define PI_FLL_SFU POS_FLL_SFU

static inline unsigned int pos_fll_offset_get(int fll)
{
    return ARCHI_FLL_ADDR + fll*8;
}

unsigned int pos_fll_set_freq(int fll, unsigned int frequency);

unsigned int pos_fll_init(int fll, int frequency);

void pos_fll_constructor();

static inline uint32_t pi_freq_get(pi_freq_domain_e domain)
{
    return pos_freq_domains[pos_freq_get_fll(domain)];
}

static inline int32_t pi_freq_set(pi_freq_domain_e domain, uint32_t freq)
{
    pos_freq_domains[pos_freq_get_fll(domain)] = freq;
    return pos_fll_set_freq(pos_freq_get_fll(domain), freq);
}

static inline void pos_freq_set_value(pi_freq_domain_e domain, uint32_t freq)
{
    pos_freq_domains[pos_freq_get_fll(domain)] = freq;
}


static inline uint32_t pi_fll_mult_get(int fll)
{
    uint32_t base = pos_fll_offset_get(fll);
    uint32_t cr2 = fll_f0cr2_get(base);
    return FLL_F0CR2_MFI_GET(cr2);
}


static inline void pi_fll_mult_set(int fll, int mult)
{
    uint32_t base = pos_fll_offset_get(fll);
    fll_f0cr2_set(base, FLL_F0CR2_MFI(mult));
}


static inline uint32_t pi_fll_div_get(int fll)
{
    uint32_t ccr1 = fll_ccr1_get(ARCHI_FLL_ADDR);
    return GAP_BEXTRACTU(ccr1, 8, 8*fll);
}


static inline void pi_fll_div_set(int fll, int div)
{
    uint32_t ccr1 = fll_ccr1_get(ARCHI_FLL_ADDR);
    ccr1 = __BITINSERT_R(ccr1, div, 8, 8*fll);
    fll_ccr1_set(ARCHI_FLL_ADDR, ccr1);
}


#endif