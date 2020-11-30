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

#ifndef __PMSIS_IMPLEM_PERF_H__
#define __PMSIS_IMPLEM_PERF_H__

#include "archi/pulp.h"
#include "hal/pulp.h"

/// @cond IMPLEM

#if defined(TIMER_VERSION) && TIMER_VERSION >= 2

static inline void pi_perf_conf(unsigned events)
{
  if ((events >> PI_PERF_CYCLES) & 1)
  {
  }

  cpu_perf_conf_events(events);
}

static inline void pi_perf_cl_reset()
{
#ifdef ARCHI_HAS_CLUSTER
  timer_reset(timer_base_cl(0, 0, 0));
  cpu_perf_setall(0);
#endif
}

static inline void pi_perf_fc_reset()
{
#ifdef ARCHI_HAS_FC
  timer_reset(timer_base_fc(0, 0));
  cpu_perf_setall(0);
#endif
}

static inline void pi_perf_reset()
{
  if (hal_is_fc())
    pi_perf_fc_reset();
  else
    pi_perf_cl_reset();
}

static inline void pi_perf_cl_start()
{
#ifdef ARCHI_HAS_CLUSTER
  timer_start(timer_base_cl(0, 0, 0));
  cpu_perf_conf(PCMR_ACTIVE | PCMR_SATURATE);
#endif
}

static inline void pi_perf_fc_start()
{
#ifdef ARCHI_HAS_FC
  timer_start(timer_base_fc(0, 0));
  cpu_perf_conf(PCMR_ACTIVE | PCMR_SATURATE);
#endif
}

static inline void pi_perf_start()
{
  if (hal_is_fc())
    pi_perf_fc_start();
  else
    pi_perf_cl_start();
}

static inline void pi_perf_cl_stop()
{
#ifdef ARCHI_HAS_CLUSTER
  timer_conf_set(timer_base_cl(0, 0, 0), TIMER_CFG_LO_ENABLE(0));
  cpu_perf_conf(0);
#endif
}

static inline void pi_perf_fc_stop()
{
#ifdef ARCHI_HAS_FC
  timer_conf_set(timer_base_fc(0, 0), TIMER_CFG_LO_ENABLE(0));
  cpu_perf_conf(0);
#endif
}

static inline void pi_perf_stop()
{
  if (hal_is_fc())
    pi_perf_fc_stop();
  else
    pi_perf_cl_stop();
}

static inline unsigned int pi_perf_cl_read(int event)
{
#ifdef ARCHI_HAS_CLUSTER
  if (event == PI_PERF_CYCLES)
  {
    return timer_count_get(timer_base_cl(0, 0, 0));
  }
  else
  {
    return cpu_perf_get(event);
  }
#else
  return 0;
#endif
}

static inline unsigned int pi_perf_fc_read(int event)
{
#ifdef ARCHI_HAS_FC
  if (event == PI_PERF_CYCLES)
  {
    return timer_count_get(timer_base_fc(0, 0));
  }
  else
  {
    return cpu_perf_get(event);
  }
#else
  return 0;
#endif
}

static inline unsigned int pi_perf_read(int event)
{
  if (hal_is_fc())
    return pi_perf_fc_read(event);
  else
    return pi_perf_cl_read(event);
}

#endif

/// @endcond

#endif
