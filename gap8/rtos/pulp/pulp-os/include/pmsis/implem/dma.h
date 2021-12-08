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

#if MCHAN_VERSION >= 7

static inline void __cl_dma_memcpy(unsigned int ext, unsigned int loc, unsigned short size, pi_cl_dma_dir_e dir, int merge, pi_cl_dma_cmd_t *copy)
{
#ifdef __RT_USE_PROFILE
  int trace = __rt_pe_trace[rt_core_id()];
  gv_vcd_dump_trace(trace, 4);
#endif

  eu_mutex_lock_from_id(0);

  int id = -1;
  if (!merge) id = plp_dma_counter_alloc();
  unsigned int cmd = plp_dma_getCmd(dir, size, PLP_DMA_1D, PLP_DMA_TRIG_EVT, PLP_DMA_NO_TRIG_IRQ, PLP_DMA_SHARED);
  // Prevent the compiler from pushing the transfer before all previous
  // stores are done
  __asm__ __volatile__ ("" : : : "memory");
  plp_dma_cmd_push(cmd, loc, ext);
  if (!merge) copy->id = id;

  copy->length = 0;

  eu_mutex_unlock_from_id(0);

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

  eu_mutex_lock_from_id(0);

  int id = -1;
  if (!merge) id = plp_dma_counter_alloc();

  {
    unsigned int cmd = plp_dma_getCmd(dir, size, PLP_DMA_2D, PLP_DMA_TRIG_EVT, PLP_DMA_NO_TRIG_IRQ, PLP_DMA_SHARED);
    // Prevent the compiler from pushing the transfer before all previous
    // stores are done
    __asm__ __volatile__ ("" : : : "memory");
    plp_dma_cmd_push_2d(cmd, loc, ext, stride, length);
    if (!merge) copy->id = id;
    copy->length = 0;
  }

  eu_mutex_unlock_from_id(0);

#ifdef __RT_USE_PROFILE
  gv_vcd_dump_trace(trace, 1);
#endif
}

#else

static inline void __cl_dma_copy(uint32_t ext, uint32_t loc, uint32_t len,
                                 uint32_t stride, uint32_t length,
                                 pi_cl_dma_dir_e dir, uint8_t merge, uint32_t dma_cmd,
                                 pi_cl_dma_cmd_t *cmd)
{
    int32_t tid = -1;
    uint32_t iter_length = (len < length) ? len : length;

    cmd->loc_addr = loc;
    cmd->ext_addr = ext;
    cmd->size = len;
    cmd->stride = stride;
    cmd->length = length;
    cmd->id = tid;
    cmd->next = NULL;

    if (__rt_dma_first_pending)
    {
        __rt_dma_last_pending->next = cmd;
    }
    else
    {
        cmd->loc_addr += iter_length;
        cmd->ext_addr += stride;
        cmd->size -= iter_length;
        __asm__ __volatile__ ("" : : : "memory");
        cmd->id = plp_dma_counter_alloc();
        plp_dma_cmd_push(dma_cmd, loc, ext);
        if (cmd->length != 0)
        {
            __rt_dma_first_pending = cmd;
        }
    }
    __rt_dma_last_pending = cmd;
}

static inline void __cl_dma_memcpy(unsigned int ext, unsigned int loc, unsigned short size, pi_cl_dma_dir_e dir, int merge, pi_cl_dma_cmd_t *copy)
{
    #ifdef __RT_USE_PROFILE
    int trace = __rt_pe_trace[rt_core_id()];
    gv_vcd_dump_trace(trace, 4);
    #endif

    int irq = rt_irq_disable();

    uint32_t max_len = (1 << 15);
    if (size < max_len)
    {
        int id = -1;
        if (!merge) id = plp_dma_counter_alloc();
        unsigned int cmd = plp_dma_getCmd(dir, size, PLP_DMA_1D, PLP_DMA_TRIG_EVT, PLP_DMA_NO_TRIG_IRQ, PLP_DMA_SHARED);
        // Prevent the compiler from pushing the transfer before all previous
        // stores are done
        __asm__ __volatile__ ("" : : : "memory");
        plp_dma_cmd_push(cmd, loc, ext);
        if (!merge) copy->id = id;

        copy->length = 0;
    }
    else
    {
        uint32_t stride = 0;
        uint32_t length = 0;
        uint32_t iter_length = 0;
        uint32_t dma_cmd = 0;

        iter_length = max_len;
        stride = max_len;
        length = max_len;
        dma_cmd = plp_dma_getCmd(dir, iter_length, PLP_DMA_1D,
                                 PLP_DMA_NO_TRIG_EVT, PLP_DMA_TRIG_IRQ, PLP_DMA_SHARED);
        copy->cmd = plp_dma_getCmd(dir, 0, PLP_DMA_1D,
                                   PLP_DMA_NO_TRIG_EVT, PLP_DMA_TRIG_IRQ, PLP_DMA_SHARED);

        __cl_dma_copy(ext, loc, size, stride, length, dir, merge, dma_cmd, copy);
    }

    rt_irq_restore(irq);

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

    int irq = rt_irq_disable();

    int id = -1;

    if (stride < (1<<15))
    {
        if (!merge) id = plp_dma_counter_alloc();
        unsigned int cmd = plp_dma_getCmd(dir, size, PLP_DMA_2D, PLP_DMA_TRIG_EVT, PLP_DMA_NO_TRIG_IRQ, PLP_DMA_SHARED);
        // Prevent the compiler from pushing the transfer before all previous
        // stores are done
        __asm__ __volatile__ ("" : : : "memory");
        plp_dma_cmd_push_2d(cmd, loc, ext, stride, length);
        if (!merge) copy->id = id;
        copy->length = 0;
    }
    else
    {
        /* Emul 2D transfer, using multiple 1D transfers. */
        uint32_t iter_length = (size < length) ? size : length;
        uint32_t dma_cmd = plp_dma_getCmd(dir, iter_length, PLP_DMA_1D,
                                          PLP_DMA_NO_TRIG_EVT, PLP_DMA_TRIG_IRQ, PLP_DMA_SHARED);
        copy->cmd = plp_dma_getCmd(dir, 0, PLP_DMA_1D,
                                   PLP_DMA_NO_TRIG_EVT, PLP_DMA_TRIG_IRQ, PLP_DMA_SHARED);
        __cl_dma_copy(ext, loc, size, stride, length, dir, merge, dma_cmd, copy);
    }

    rt_irq_restore(irq);

    #ifdef __RT_USE_PROFILE
    gv_vcd_dump_trace(trace, 1);
    #endif
}

#endif

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

#if MCHAN_VERSION >= 7

static inline void __cl_dma_wait(pi_cl_dma_cmd_t *copy)
{
#ifdef __RT_USE_PROFILE
  int trace = __rt_pe_trace[rt_core_id()];
  gv_vcd_dump_trace(trace, 5);
#endif

  int counter = copy->id;

  eu_mutex_lock_from_id(0);

  while(DMA_READ(MCHAN_STATUS_OFFSET) & (1 << counter)) {
    eu_mutex_unlock_from_id(0);
    eu_evt_maskWaitAndClr(1<<ARCHI_CL_EVT_DMA0);
    eu_mutex_lock_from_id(0);
  }

  plp_dma_counter_free(counter);

  eu_mutex_unlock_from_id(0);
  
#ifdef __RT_USE_PROFILE
  gv_vcd_dump_trace(trace, 1);
#endif
}

#else

static inline void __cl_dma_wait(pi_cl_dma_cmd_t *copy)
{
#ifdef __RT_USE_PROFILE
  int trace = __rt_pe_trace[rt_core_id()];
  gv_vcd_dump_trace(trace, 5);
#endif
  if (copy->length == 0)
  {
    int irq = rt_irq_disable();

    while(DMA_READ(PLP_DMA_STATUS_OFFSET) & (1 << copy->id)) {
      rt_irq_restore(irq);
      eu_evt_maskWaitAndClr(1<<ARCHI_CL_EVT_DMA0);
      irq = rt_irq_disable();
    }

    plp_dma_counter_free(copy->id);

    rt_irq_restore(irq);
  }
  else
  {
    while(*(volatile uint32_t *)&copy->ext_addr != 0)
      eu_evt_maskWaitAndClr(1<<RT_DMA_EVENT);
  }
  
#ifdef __RT_USE_PROFILE
  gv_vcd_dump_trace(trace, 1);
#endif
}

#endif

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
