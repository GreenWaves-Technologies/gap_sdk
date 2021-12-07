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


#ifndef __KERNEL_PULPRT_WRAPPER_H__
#define __KERNEL_PULPRT_WRAPPER_H__

static inline int __rt_cluster_lock(rt_fc_cluster_data_t *cluster)
{
  return rt_irq_disable();
}

static inline void __rt_cluster_unlock(rt_fc_cluster_data_t *cluster, int lock)
{
  rt_irq_restore(lock);
}

#endif
