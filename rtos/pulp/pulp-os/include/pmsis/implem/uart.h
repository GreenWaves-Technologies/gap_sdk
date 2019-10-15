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

#ifndef __PMSIS_IMPLEM_UART_H__
#define __PMSIS_IMPLEM_UART_H__

#include "pmsis/drivers/uart.h"
#include "pmsis/data/data.h"

struct pi_cl_uart_req_s {
  pi_device_t *device;
  void *buffer;
  size_t size;
  rt_event_t event;
  int done;
  char cid;
};

#ifdef ARCHI_HAS_CLUSTER

static inline __attribute__((always_inline)) void pi_cl_uart_write_wait(pi_cl_uart_req_t *req)
{
  while((*(volatile int *)&req->done) == 0)
  {
    eu_evt_maskWaitAndClr(1<<RT_CLUSTER_CALL_EVT);
  }
}

static inline __attribute__((always_inline)) void pi_cl_uart_read_wait(pi_cl_uart_req_t *req)
{
  while((*(volatile int *)&req->done) == 0)
  {
    eu_evt_maskWaitAndClr(1<<RT_CLUSTER_CALL_EVT);
  }
}

#endif

#endif