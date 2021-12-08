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

void rt_perf_init(rt_perf_t *perf)
{
  for (int i=0; i<RT_PERF_NB_EVENTS; i++)
  {
    perf->values[i] = 0;
  }
}

void rt_perf_conf(rt_perf_t *perf, unsigned events)
{
	perf->events = events;

	if ((events >> RT_PERF_CYCLES) & 1)
	{
	}

  cpu_perf_conf_events(events);
}

void rt_perf_save(rt_perf_t *perf)
{
	unsigned int mask = perf->events;
	while (mask)
	{
    int event = __FL1(mask);
    mask &= ~(1<<event);

#ifdef ARCHI_HAS_FC
    if (hal_is_fc())
      perf->values[event] += rt_perf_fc_read(event);
    else
#endif
      perf->values[event] += rt_perf_cl_read(event);
  }
}


unsigned int rt_perf_get_average(rt_perf_t *perf, int id, int nb_cores)
{
  unsigned int result = 0;
  for (int i=0; i<nb_cores; i++)
  {
    result += perf[i].values[id];
  }
  return result / nb_cores;
}
