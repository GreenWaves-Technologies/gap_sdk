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

#ifndef __RT_IMPLEM_CLUSTER_H__
#define __RT_IMPLEM_CLUSTER_H__

#if defined(ARCHI_HAS_CLUSTER)

#include "pmsis/cluster/dma/cl_dma.h"
#include "pmsis/cluster/cl_malloc.h"

static inline void *rt_alloc_cluster_wait(pi_cl_alloc_req_t *req)
{
  while((*(volatile char *)&req->done) == 0)
  {
    eu_evt_maskWaitAndClr(1<<RT_CLUSTER_CALL_EVT);
  }
  return req->result;
}

static inline void rt_free_cluster_wait(pi_cl_free_req_t *req)
{
  while((*(volatile char *)&req->done) == 0)
  {
    eu_evt_maskWaitAndClr(1<<RT_CLUSTER_CALL_EVT);
  }
}

static inline void *pi_cl_l2_malloc_wait(pi_cl_alloc_req_t *req)
{
  return rt_alloc_cluster_wait((pi_cl_alloc_req_t *)req);
}

static inline void pi_cl_l2_free_wait(pi_cl_free_req_t *req)
{
  rt_free_cluster_wait((pi_cl_free_req_t *)req);
}

#endif

#endif