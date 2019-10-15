/*
 * Copyright (C) 2018 ETH Zurich, University of Bologna and GreenWaves Technologies
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

#ifdef ARCHI_FPGA_FREQUENCY
extern int __rt_fpga_fc_frequency;
extern int __rt_fpga_periph_frequency;
#endif

int __rt_freq_domains[RT_FREQ_NB_DOMAIN];

int rt_freq_set_and_get(rt_freq_domain_e domain, unsigned int freq, unsigned int *out_freq)
{
  int irq = rt_irq_disable();

  rt_trace(RT_TRACE_FREQ, "Setting domain frequency (domain: %d, freq: %d)\n", domain, freq);

  int fll_freq = __rt_fll_set_freq(domain, freq);
  if (out_freq)
    *out_freq = fll_freq;

  __rt_freq_domains[domain] = freq;

  rt_irq_restore(irq);

  return 0;
}

void __rt_freq_init()
{
  __rt_flls_constructor();

#ifdef ARCHI_FPGA_FREQUENCY
  if (rt_platform() != ARCHI_PLATFORM_FPGA)
#endif
  {
    __rt_freq_domains[RT_FREQ_DOMAIN_FC] = __rt_fll_init(__RT_FLL_FC);

    __rt_freq_domains[RT_FREQ_DOMAIN_PERIPH] = __rt_fll_init(__RT_FLL_PERIPH);

  #if __RT_FREQ_DOMAIN_CL < RT_FREQ_NB_DOMAIN
    __rt_freq_domains[RT_FREQ_DOMAIN_CL] = __rt_fll_init(__RT_FLL_CL);
  #endif
  }
#ifdef ARCHI_FPGA_FREQUENCY
  else
  {
    __rt_freq_domains[RT_FREQ_DOMAIN_FC] = __rt_fpga_fc_frequency;

    __rt_freq_domains[RT_FREQ_DOMAIN_PERIPH] = __rt_fpga_periph_frequency;

  #if __RT_FREQ_DOMAIN_CL < RT_FREQ_NB_DOMAIN
    __rt_freq_domains[RT_FREQ_DOMAIN_CL] = ARCHI_FPGA_FREQUENCY;
  #endif
  }
#endif

#if PULP_CHIP == CHIP_VEGA || PULP_CHIP == CHIP_GAP9
  // By default on Vega, we connect each FLL on each domain
  apb_soc_clk_sel_set(
    ARCHI_APB_SOC_CTRL_ADDR,
    APB_SOC_CLK_SEL_CLK_SOC(1) | 
    APB_SOC_CLK_SEL_CLK_CLUSTER(2)
  );
#endif
}
