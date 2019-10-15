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

#ifndef __RT_IMPLEM_HYPERRAM_H__
#define __RT_IMPLEM_HYPERRAM_H__

struct pi_cl_hyper_req_s {
  struct pi_device *device;
  void *addr;
  uint32_t hyper_addr;
  uint32_t size;
  int32_t stride;
  uint32_t length;
  rt_event_t event;
  struct pi_cl_hyper_req_s *next;
  int done;
  unsigned char cid;
  unsigned char is_write;
  unsigned char is_2d;
};

struct pi_cl_hyperram_alloc_req_s {
  struct pi_device *device;
  uint32_t result;
  uint32_t  size;
  rt_event_t event;
  char done;
  char cid;
};

struct pi_cl_hyperram_free_req_s {
  struct pi_device *device;
  uint32_t result;
  uint32_t size;
  uint32_t chunk;
  rt_event_t event;
  char done;
  char cid;
};

#if defined(ARCHI_HAS_CLUSTER)


extern void __cl_hyper_cluster_copy(struct pi_device *device,
uint32_t hyper_addr, void *addr, uint32_t size, pi_cl_hyper_req_t *req, int ext2loc);



extern void __cl_hyper_cluster_copy_2d(struct pi_device *device,
  uint32_t hyper_addr, void *addr, uint32_t size, uint32_t stride, uint32_t length, pi_cl_hyper_req_t *req, int ext2loc);



static inline void pi_cl_hyper_read(struct pi_device *device,
  uint32_t hyper_addr, void *addr, uint32_t size, pi_cl_hyper_req_t *req)
{
  __cl_hyper_cluster_copy(device, hyper_addr, addr, size, req, 1);
}



static inline __attribute__((always_inline)) void pi_cl_hyper_read_wait(pi_cl_hyper_req_t *req)
{
  while((*(volatile int *)&req->done) == 0)
  {
    eu_evt_maskWaitAndClr(1<<RT_CLUSTER_CALL_EVT);
  }
}



static inline void pi_cl_hyper_write(struct pi_device *device,
  uint32_t hyper_addr, void *addr, uint32_t size, pi_cl_hyper_req_t *req)
{
  __cl_hyper_cluster_copy(device, hyper_addr, addr, size, req, 0);
}



static inline void pi_cl_hyper_read_2d(struct pi_device *device,
  uint32_t hyper_addr, void *addr, uint32_t size, uint32_t stride, uint32_t length, pi_cl_hyper_req_t *req)
{
  __cl_hyper_cluster_copy_2d(device, hyper_addr, addr, size, stride, length, req, 1);
}



static inline void pi_cl_hyper_write_2d(struct pi_device *device,
  uint32_t hyper_addr, void *addr, uint32_t size, uint32_t stride, uint32_t length, pi_cl_hyper_req_t *req)
{
  __cl_hyper_cluster_copy_2d(device, hyper_addr, addr, size, stride, length, req, 0);
}



static inline __attribute__((always_inline)) void pi_cl_hyper_write_wait(pi_cl_hyper_req_t *req)
{
  while((*(volatile int *)&req->done) == 0)
  {
    eu_evt_maskWaitAndClr(1<<RT_CLUSTER_CALL_EVT);
  }
}



static inline uint32_t pi_cl_hyperram_alloc_wait(pi_cl_hyperram_alloc_req_t *req)
{
  while((*(volatile char *)&req->done) == 0)
  {
    eu_evt_maskWaitAndClr(1<<RT_CLUSTER_CALL_EVT);
  }
  return req->result;
}



static inline void pi_cl_hyperram_free_wait(pi_cl_hyperram_free_req_t *req)
{
  while((*(volatile char *)&req->done) == 0)
  {
    eu_evt_maskWaitAndClr(1<<RT_CLUSTER_CALL_EVT);
  }
}

static inline void pi_cl_hyper_copy(struct pi_device *device,
  uint32_t hyper_addr, void *addr, uint32_t size, int ext2loc, pi_cl_hyper_req_t *req)
{
  __cl_hyper_cluster_copy(device, hyper_addr, addr, size, req, ext2loc);
}

static inline void pi_cl_hyper_copy_2d(struct pi_device *device,
  uint32_t hyper_addr, void *addr, uint32_t size, uint32_t stride, uint32_t length, int ext2loc, pi_cl_hyper_req_t *req)
{
  __cl_hyper_cluster_copy_2d(device, hyper_addr, addr, size, stride, length, req, ext2loc);
}

// Performs a direct aligned copy:
//  - hyper addr is multiple of 2
//  - l2 addr is multiple of 4
//  - size is multiple of 4
void __attribute__((noinline)) __pi_hyper_copy_aligned(int channel,
  uint32_t addr, uint32_t hyper_addr, uint32_t size, pi_task_t *event);

#endif

#endif