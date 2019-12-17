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

#include "rt/rt_api.h"

#define __RT_FLL_REGULATION_TOL   0 // tolerance in percent of middle value
#define __RT_FLL_CLK_GOOD_TOL     5 // tolerance in percent of middle value

typedef struct __rt_fll_settings_s {   
  unsigned int __rt_fll_freq[2];
  char __rt_fll_active_set;
  char __rt_fll_is_on;
  unsigned int __rt_fll_ref;
} __rt_fll_settings_t;

__rt_fll_settings_t __rt_fll_settings[ARCHI_NB_FLL];

static unsigned int __rt_fll_get_mult_from_freq(unsigned int freq, unsigned int ref, unsigned int *mult)
{
  unsigned int M = freq/ref;

  *mult = M;
  return ref*M;
}

static unsigned int __rt_fll_get_freq_from_mult(unsigned int mult, unsigned int ref)
{
  return ref*mult;
}

// set the frequency of the given set
unsigned int __rt_fll_set_freq_set(hal_fll_e fll, hal_fll_reg_set_e set, unsigned int frequency)
{
  if(fll >= ARCHI_NB_FLL) return 0;

  unsigned int real_freq, mult;
  unsigned int tolerance;

  real_freq = __rt_fll_get_mult_from_freq(frequency, __rt_fll_settings[fll].__rt_fll_ref, &mult);
  rt_trace(RT_TRACE_FREQ, "Setting FLL frequency (fll: %d, freq: %d, mult: %d, real_freq: %d)\n", fll, frequency, mult, real_freq);

  __rt_fll_settings[fll].__rt_fll_freq[set] = real_freq;

  tolerance = (mult * __RT_FLL_REGULATION_TOL)/100;
  hal_fll_set_regulation_set(fll,set,mult,tolerance);

  tolerance = (mult * __RT_FLL_CLK_GOOD_TOL)/100;
  hal_fll_set_clk_good_set(fll,set,mult,tolerance);

  __rt_fll_settings[fll].__rt_fll_freq[set] = real_freq;

  return real_freq;
}

// get the frequency of the given set
unsigned int __rt_fll_set_freq_get(hal_fll_e fll, hal_fll_reg_set_e set)
{
  return __rt_fll_settings[fll].__rt_fll_freq[set];
}

// set the active set
unsigned int __rt_fll_set_active_set(hal_fll_e fll, hal_fll_reg_set_e set)
{
  hal_fll_set_sel_set(fll, set);
  __rt_fll_settings[fll].__rt_fll_active_set = set;
  
  return __rt_fll_settings[fll].__rt_fll_freq[(unsigned int)__rt_fll_settings[fll].__rt_fll_active_set];
}

// get the active set
hal_fll_reg_set_e __rt_fll_set_active_get(hal_fll_e fll)
{
  return (hal_fll_reg_set_e) __rt_fll_settings[fll].__rt_fll_active_set;
}

// get active frequency
unsigned int __rt_fll_active_freq_get(hal_fll_e fll)
{
  if(fll<ARCHI_NB_FLL) return __rt_fll_settings[fll].__rt_fll_freq[(unsigned int)__rt_fll_settings[fll].__rt_fll_active_set];
  else return 0;
}

// init sets of fll - does not turn the FLL on!!
void __rt_fll_set_init(hal_fll_e fll)
{
  rt_trace(RT_TRACE_INIT, "Initializing FLL sets (fll: %d)\n", fll);

  // init FLL configs
  __rt_fll_set_freq_set(fll,0,__rt_fll_settings[fll].__rt_fll_freq[0]);
  __rt_fll_set_freq_set(fll,1,__rt_fll_settings[fll].__rt_fll_freq[1]);
  __rt_fll_set_active_set(fll, __rt_fll_settings[fll].__rt_fll_active_set);  // do not do that for SOC FLL!!
}

unsigned int __rt_fll_enable(hal_fll_e fll)
{
  rt_trace(RT_TRACE_INIT, "Enable FLL (fll: %d)\n", fll);

  hal_fll_enable_set(fll,1);

  __rt_fll_settings[fll].__rt_fll_is_on = 1;

  rt_trace(RT_TRACE_INIT, "FLL is locked (fll: %d, freq: %d)\n", fll, freq);

  return __rt_fll_settings[fll].__rt_fll_freq[0];
}


void __rt_fll_disable(hal_fll_e fll)
{
  if (fll == HAL_FLL_SOC) return;   // currently SOC Fll is not stopped
  else 
  {
    hal_fll_enable_set(fll, 0); // disable FLL
    __rt_fll_settings[fll].__rt_fll_is_on = 0;    
  }
}

void __rt_flls_constructor(void)
{
  for (int i=0; i<ARCHI_NB_FLL; i++)
  {
    __rt_fll_settings[i].__rt_fll_freq[0] = 10000000;
    __rt_fll_settings[i].__rt_fll_freq[1] = 10000000;
    __rt_fll_settings[i].__rt_fll_active_set = 0;
    __rt_fll_settings[i].__rt_fll_is_on = 0;
    __rt_fll_settings[i].__rt_fll_ref = ARCHI_FLL_REF_CLOCK_SLOW;
  }
}
