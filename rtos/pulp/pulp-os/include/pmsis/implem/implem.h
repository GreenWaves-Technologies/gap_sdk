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
#include "pmsis/implem/pwm.h"
#include "pmsis/implem/perf.h"
#include "pmsis/implem/cpi.h"
#include "pmsis/implem/uart.h"
#ifdef MCHAN_VERSION
#include "pmsis/implem/dma.h"
#endif
#include "rt/implem/implem.h"

#if PULP_CHIP_FAMILY != CHIP_VIVOSOC3 && PULP_CHIP_FAMILY != CHIP_VIVOSOC3_1 && PULP_CHIP_FAMILY != CHIP_VIVOSOC4

static inline int __pi_freq_get_domain(int domain)
{
    return domain == PI_FREQ_DOMAIN_FC ? RT_FREQ_DOMAIN_FC : domain == PI_FREQ_DOMAIN_CL ? RT_FREQ_DOMAIN_CL : RT_FREQ_DOMAIN_PERIPH;
}

static inline int32_t pi_freq_set(pi_freq_domain_e domain, uint32_t freq)
{
    return rt_freq_set_and_get(__pi_freq_get_domain(domain), freq, NULL);
}

static inline uint32_t pi_freq_get(pi_freq_domain_e domain)
{
    return __rt_freq_domains[__pi_freq_get_domain(domain)];
}

#endif

#ifdef ARCHI_HAS_CLUSTER

static inline void cl_wait_task(unsigned char *done)
{
    while ((*(volatile char *)done) == 0)
    {
        eu_evt_maskWaitAndClr(1<<RT_CLUSTER_CALL_EVT);
    }
}


static inline void pi_task_push(pi_task_t *task)
{
  rt_event_enqueue(task);
}


#endif

static inline void pi_task_wait_on(struct pi_task *task)
{
  while(!task->done)
    rt_event_yield(NULL);
}


struct pi_task *pi_task_callback(struct pi_task *task, void (*callback)(void*), void *arg)
{
  task->id = PI_TASK_CALLBACK_ID;
  task->arg[0] = (uint32_t)callback;
  task->arg[1] = (uint32_t)arg;
  task->implem.keep = 1;
  __rt_task_init(task);
  return task;
}

static inline pi_callback_t *pi_callback(pi_callback_t *callback,
                                          void (*entry)(void*), void *arg)
{
  callback->entry = entry;
  callback->arg = arg;
  return callback;
}


#endif
