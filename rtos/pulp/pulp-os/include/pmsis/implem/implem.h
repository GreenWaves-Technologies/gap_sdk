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

#ifndef __PMSIS_IMPLEM_IMPLEM_H__
#define __PMSIS_IMPLEM_IMPLEM_H__

#ifdef UDMA_VERSION
#include "pmsis/implem/udma.h"
#endif
#include "pmsis/implem/perf.h"
#include "pmsis/implem/cpi.h"
#include "pmsis/implem/uart.h"
#ifdef MCHAN_VERSION
#include "pmsis/implem/dma.h"
#endif
#include "rt/implem/implem.h"

#if PULP_CHIP_FAMILY != CHIP_VIVOSOC3 && PULP_CHIP_FAMILY != CHIP_VIVOSOC3_1

static inline int32_t pi_freq_set(pi_freq_domain_e domain, uint32_t freq)
{
  return rt_freq_set_and_get(domain, freq, NULL);
}

static inline uint32_t pi_freq_get(pi_freq_domain_e domain)
{
  return __rt_freq_domains[domain];
}

#endif

#endif
