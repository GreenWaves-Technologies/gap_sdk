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

// This file is used to keep compatibilty with old software and should not be used

#ifndef __RT_RT_COMPAT_H__
#define __RT_RT_COMPAT_H__

/// @cond IMPLEM

#define PLP_L1_DATA RT_L1_GLOBAL_DATA
#define PLP_FC_DATA RT_FC_TINY_DATA
#define PLP_L2_DATA RT_L2_DATA
#define PULP_L2_DATA PLP_L2_DATA

#define int_enable hal_irq_enable
#define int_disable hal_irq_disable
#define is_fc rt_is_fc
#define plp_isFc rt_is_fc
#define get_core_id rt_core_id
#define get_core_num rt_nb_pe
#define plp_clusterId rt_cluster_id
#define get_cluster_id rt_cluster_id

static inline void synch_barrier() {
#if defined(ARCHI_HAS_CLUSTER)
#if defined(EU_VERSION) && EU_VERSION >= 3
  if (!rt_is_fc()) rt_team_barrier();
#endif
#endif
}

static inline int int_add(unsigned long irq, void (* handler)(void *), void *arg)
{
  rt_irq_set_handler(irq, (void (*)())handler);
  return 0;
}

static inline void wait_soc_event(int event)
{
  __rt_periph_wait_event(event, 0);
}

static inline void clear_soc_event(int event)
{
  __rt_periph_clear_event(event);
}

/// @endcond


#endif
