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

#ifndef __PMSIS_IMPLEM_DMA_H__
#define __PMSIS_IMPLEM_DMA_H__

#include "pmsis/cluster/dma/cl_dma.h"
#include "pmsis.h"
#include "hal/pulp.h"

struct pi_cl_dma_cmd_s
{
  int id;
  uint32_t cmd;
  uint32_t size;
  uint32_t stride;
  uint32_t length;
  uint32_t loc_addr;
  uint32_t ext_addr;
  struct pi_cl_dma_cmd_s *next;
};

extern RT_L1_TINY_DATA pi_cl_dma_cmd_t *__rt_dma_first_pending;
extern RT_L1_TINY_DATA pi_cl_dma_cmd_t *__rt_dma_last_pending;

static inline void __cl_dma_memcpy(unsigned int ext, unsigned int loc, unsigned short size, pi_cl_dma_dir_e dir, int merge, pi_cl_dma_cmd_t *copy)
{
#ifdef __RT_USE_PROFILE
  int trace = __rt_pe_trace[rt_core_id()];
  gv_vcd_dump_trace(trace, 4);
#endif

  int id = -1;
  if (!merge) id = plp_dma_counter_alloc();
  unsigned int cmd = plp_dma_getCmd(dir, size, PLP_DMA_1D, PLP_DMA_TRIG_EVT, PLP_DMA_NO_TRIG_IRQ, PLP_DMA_SHARED);
  // Prevent the compiler from pushing the transfer before all previous
  // stores are done
  __asm__ __volatile__ ("" : : : "memory");
  plp_dma_cmd_push(cmd, loc, ext);
  if (!merge) copy->id = id;

  copy->length = 0;

#ifdef __RT_USE_PROFILE
  gv_vcd_dump_trace(trace, 1);
#endif
}


static inline void __cl_dma_memcpy_2d(unsigned int ext, unsigned int loc, unsigned int size, unsigned int stride, unsigned short length, pi_cl_dma_dir_e dir, int merge, pi_cl_dma_cmd_t *copy)
{
#ifdef __RT_USE_PROFILE
  int trace = __rt_pe_trace[rt_core_id()];
  gv_vcd_dump_trace(trace, 4);
#endif

  int id = -1;
  if (!merge) id = plp_dma_counter_alloc();

#if MCHAN_VERSION <= 6
  if (stride < (1<<15))
#endif
  {
    unsigned int cmd = plp_dma_getCmd(dir, size, PLP_DMA_2D, PLP_DMA_TRIG_EVT, PLP_DMA_NO_TRIG_IRQ, PLP_DMA_SHARED);
    // Prevent the compiler from pushing the transfer before all previous
    // stores are done
    __asm__ __volatile__ ("" : : : "memory");
    plp_dma_cmd_push_2d(cmd, loc, ext, stride, length);
    if (!merge) copy->id = id;
    copy->length = 0;
  }
#if MCHAN_VERSION <= 6
  else
  {
    uint32_t iter_length = size < length ? size : length;
    unsigned int cmd = plp_dma_getCmd(dir, iter_length, PLP_DMA_1D, PLP_DMA_NO_TRIG_EVT, PLP_DMA_TRIG_IRQ, PLP_DMA_SHARED);
    // Prevent the compiler from pushing the transfer before all previous
    // stores are done
    copy->loc_addr = loc + iter_length;
    copy->ext_addr = ext + stride;
    copy->size = size - iter_length;
    copy->stride = stride;
    copy->length = length;
    copy->cmd = plp_dma_getCmd(dir, 0, PLP_DMA_1D, PLP_DMA_NO_TRIG_EVT, PLP_DMA_TRIG_IRQ, PLP_DMA_SHARED);
    copy->id = id;
    copy->next = NULL;

    int irq = rt_irq_disable();
    rt_compiler_barrier();

    if (__rt_dma_first_pending)
      __rt_dma_last_pending->next = copy;
    else
    {
      __asm__ __volatile__ ("" : : : "memory");
      plp_dma_cmd_push(cmd, loc, ext);
      __rt_dma_first_pending =copy;
    }

    __rt_dma_last_pending = copy;
    rt_compiler_barrier();
    rt_irq_restore(irq);
  }
#endif

#ifdef __RT_USE_PROFILE
  gv_vcd_dump_trace(trace, 1);
#endif
}

static inline void __cl_dma_flush()
{
#ifdef __RT_USE_PROFILE
  int trace = __rt_pe_trace[rt_core_id()];
  gv_vcd_dump_trace(trace, 5);
#endif
  plp_dma_barrier();
#ifdef __RT_USE_PROFILE
  gv_vcd_dump_trace(trace, 1);
#endif
}

static inline void __cl_dma_wait(pi_cl_dma_cmd_t *copy)
{
#ifdef __RT_USE_PROFILE
  int trace = __rt_pe_trace[rt_core_id()];
  gv_vcd_dump_trace(trace, 5);
#endif
  if (copy->length == 0)
  {
    plp_dma_wait(copy->id);
  }
  else
  {
    while(*(volatile uint32_t *)&copy->size > 0)
      eu_evt_maskWaitAndClr(1<<RT_DMA_EVENT);

    plp_dma_counter_free(copy->id);
  }
#ifdef __RT_USE_PROFILE
  gv_vcd_dump_trace(trace, 1);
#endif
}

static inline void pi_cl_dma_memcpy(pi_cl_dma_copy_t *copy)
{
  __cl_dma_memcpy(copy->ext, copy->loc, copy->size, copy->dir, copy->merge, (pi_cl_dma_cmd_t *)copy);
}


static inline void pi_cl_dma_memcpy_2d(pi_cl_dma_copy_2d_t *copy)
{
  __cl_dma_memcpy_2d(copy->ext, copy->loc, copy->size, copy->stride, copy->length, copy->dir, copy->merge, (pi_cl_dma_cmd_t *)copy);
}


static inline void pi_cl_dma_flush()
{
  __cl_dma_flush();
}


static inline void pi_cl_dma_wait(void *copy)
{
  __cl_dma_wait(copy);
}

static inline void pi_cl_dma_cmd(uint32_t ext, uint32_t loc, uint32_t size, pi_cl_dma_dir_e dir, pi_cl_dma_cmd_t *cmd)
{
  __cl_dma_memcpy(ext, loc, size, dir, 0, (pi_cl_dma_cmd_t *)cmd);
}

static inline void pi_cl_dma_cmd_2d(uint32_t ext, uint32_t loc, uint32_t size, uint32_t stride, uint32_t length, pi_cl_dma_dir_e dir, pi_cl_dma_cmd_t *cmd)
{
  __cl_dma_memcpy_2d(ext, loc, size, stride, length, dir, 0, (pi_cl_dma_cmd_t *)cmd);
}

static inline void pi_cl_dma_cmd_wait(pi_cl_dma_cmd_t *cmd)
{
  __cl_dma_wait((pi_cl_dma_cmd_t *)cmd);
}

#endif