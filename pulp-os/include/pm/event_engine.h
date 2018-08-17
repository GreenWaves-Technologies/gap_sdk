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


#ifndef __PM_EVENT_ENGINE_H__
#define __PM_EVENT_ENGINE_H__

#include "pm/event_engine_data.h"



static inline void ee_cluster_conf_init(ee_cluster_conf_t *conf);

static inline void ee_cluster_start(int cluster_id, ee_cluster_conf_t *conf, ee_event_t event);

void ee_cluster_stop(int cluster_id, ee_event_t event);





/// @cond IMPLEM

static inline void ee_cluster_conf_init(ee_cluster_conf_t *conf)
{
  conf->stack_size = STACK_SIZE;
}

void __ee_cluster_start(int cluster_id, int stack_size, ee_event_t event);

static inline void ee_cluster_start(int cluster_id, ee_cluster_conf_t *conf, ee_event_t event)
{
  __ee_cluster_start(cluster_id, conf->stack_size, event);
}

/// @endcond



#endif
