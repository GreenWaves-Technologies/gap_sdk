/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna
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
 * Copyright (C) 2018 GreenWaves Technologies
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
 *          Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */

#include "rt/rt_api.h"

int __rt_freq_domains[RT_FREQ_NB_DOMAIN];
int __rt_freq_next_domains[RT_FREQ_NB_DOMAIN];

void __rt_freq_init()
{
  __rt_freq_domains[RT_FREQ_DOMAIN_CL] = __rt_fll_init(0);
}

int rt_freq_set_and_get(rt_freq_domain_e domain, unsigned int freq, unsigned int *out_freq)
{
  int irq = rt_irq_disable();
  int err = 0;

  __rt_freq_next_domains[domain] = freq;

  if (__rt_cbsys_exec(RT_CBSYS_PERIPH_SETFREQ_BEFORE))
  {
    // In case we get an error, this means a driver is not happy with the new
    // frequency. just go on with the same frequency as before.
    freq = __rt_freq_domains[RT_FREQ_DOMAIN_CL];
    err = -1;
  }

  __rt_fll_set_freq(0, freq);

  __rt_freq_domains[RT_FREQ_DOMAIN_CL] = freq;

  __rt_cbsys_exec(RT_CBSYS_PERIPH_SETFREQ_AFTER);

  rt_irq_restore(irq);

  return err;
}
