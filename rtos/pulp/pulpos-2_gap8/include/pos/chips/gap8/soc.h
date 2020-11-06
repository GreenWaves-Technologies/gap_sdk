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

#ifndef __POS_CHIPS_GAP_SOC_H__
#define __POS_CHIPS_GAP_SOC_H__


#define POS_FLL_CL 1
#define POS_FLL_PERIPH 0
#define POS_FLL_FC 0

extern PI_FC_TINY int pos_freq_domains[ARCHI_NB_FLL];


void pos_soc_init();

static inline int pos_freq_get_fll(int domain)
{
    switch (domain)
    {
        case PI_FREQ_DOMAIN_FC:
        case PI_FREQ_DOMAIN_PERIPH:
            return POS_FLL_FC;

        case PI_FREQ_DOMAIN_CL:
        default:
            return POS_FLL_CL;
    }
}


#endif
