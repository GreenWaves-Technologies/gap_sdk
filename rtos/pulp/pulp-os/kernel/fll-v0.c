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
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */

#include "rt/rt_api.h"

#define __RT_FLL_DEFAULT_FREQ 50000000

unsigned int __rt_fll_set_freq(int fll, unsigned int frequency)
{
  rt_trace(RT_TRACE_FREQ, "Setting FLL frequency (fll: %d, freq: %d)\n", fll, frequency);
  
  return fll_set_soc_frequency(frequency);
}

unsigned int __rt_fll_init(int fll)
{
  rt_trace(RT_TRACE_INIT, "Initializing FLL (fll: %d, freq: %d)\n", fll, __RT_FLL_DEFAULT_FREQ);
 
  fll_set_soc_frequency(__RT_FLL_DEFAULT_FREQ);

  return __RT_FLL_DEFAULT_FREQ;
}


void __rt_fll_deinit(int fll)
{
}
