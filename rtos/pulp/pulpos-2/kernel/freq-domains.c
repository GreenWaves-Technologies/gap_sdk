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
 * Authors: Eric Flamand, GreenWaves Technologies (eric.flamand@greenwaves-technologies.com)
 *          Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */

#include "pmsis.h"




PI_FC_TINY int pos_freq_domains[ARCHI_NB_FLL];



int pos_freq_set_and_get(pi_freq_domain_e domain, unsigned int freq, unsigned int *out_freq)
{
    int irq = hal_irq_disable();

    FREQ_TRACE(POS_LOG_INFO, "Setting domain frequency (domain: %d, freq: %d)\n", domain, freq);

    pos_cbsys_exec(POS_CBSYS_PERIPH_SETFREQ_BEFORE);

    int fll_freq = pos_fll_set_freq(pos_freq_get_fll(domain), freq);
    if (out_freq)
      *out_freq = fll_freq;

    pos_freq_domains[domain] = freq;

    pos_cbsys_exec(POS_CBSYS_PERIPH_SETFREQ_AFTER);

    hal_irq_restore(irq);

    return 0;
}


