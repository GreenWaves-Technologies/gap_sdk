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

/* 
 * Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */

#ifndef __POS_IMPLEM_DMA_H__
#define __POS_IMPLEM_DMA_H__


struct pi_cl_dma_cmd_s
{
  int id;
  struct pi_cl_dma_cmd_s *next;
};


static inline void __cl_dma_flush()
{
  plp_dma_barrier();
}


static inline void __cl_dma_wait_safe(pi_cl_dma_cmd_t *copy)
{
  int counter = copy->id;

  while(DMA_READ(MCHAN_STATUS_OFFSET) & (1 << counter)) {
    eu_evt_maskWaitAndClr(1<<ARCHI_CL_EVT_DMA0);
  }

  plp_dma_counter_free(counter);
}


static inline void __cl_dma_wait(pi_cl_dma_cmd_t *copy)
{
  int counter = copy->id;

  eu_mutex_lock_from_id(0);

  while(DMA_READ(MCHAN_STATUS_OFFSET) & (1 << counter)) {
    eu_mutex_unlock_from_id(0);
    eu_evt_maskWaitAndClr(1<<ARCHI_CL_EVT_DMA0);
    eu_mutex_lock_from_id(0);
  }

  plp_dma_counter_free(counter);

  eu_mutex_unlock_from_id(0);
}


static inline void __cl_dma_memcpy(unsigned int ext, unsigned int loc, unsigned short size, pi_cl_dma_dir_e dir, int merge, pi_cl_dma_cmd_t *copy)
{
  eu_mutex_lock_from_id(0);
  
  int id = -1;
  if (!merge) id = plp_dma_counter_alloc();
  unsigned int cmd = plp_dma_getCmd(dir, size, PLP_DMA_1D, PLP_DMA_TRIG_EVT, PLP_DMA_NO_TRIG_IRQ, PLP_DMA_SHARED);
  // Prevent the compiler from pushing the transfer before all previous
  // stores are done
  __asm__ __volatile__ ("" : : : "memory");
  plp_dma_cmd_push(cmd, loc, ext);
  if (!merge) copy->id = id;

  eu_mutex_unlock_from_id(0);
}


static inline void __cl_dma_memcpy_safe(unsigned int ext, unsigned int loc, unsigned short size, pi_cl_dma_dir_e dir, int merge, pi_cl_dma_cmd_t *copy)
{
  int id = -1;
  if (!merge) id = plp_dma_counter_alloc();
  unsigned int cmd = plp_dma_getCmd(dir, size, PLP_DMA_1D, PLP_DMA_TRIG_EVT, PLP_DMA_NO_TRIG_IRQ, PLP_DMA_SHARED);
  // Prevent the compiler from pushing the transfer before all previous
  // stores are done
  __asm__ __volatile__ ("" : : : "memory");
  plp_dma_cmd_push(cmd, loc, ext);
  if (!merge) copy->id = id;
}


#if 0
static inline void __cl_dma_memcpy_irq(unsigned int ext, unsigned int loc, unsigned short size, pi_cl_dma_dir_e dir, int merge, pi_cl_dma_cmd_t *copy)
{
  eu_mutex_lock_from_id(0);
  
  int id = -1;
  if (!merge) id = plp_dma_counter_alloc();
  unsigned int cmd = plp_dma_getCmd(dir, size, PLP_DMA_1D, PLP_DMA_NO_TRIG_EVT, PLP_DMA_TRIG_IRQ, PLP_DMA_SHARED);
  // Prevent the compiler from pushing the transfer before all previous
  // stores are done
  __asm__ __volatile__ ("" : : : "memory");
  plp_dma_cmd_push(cmd, loc, ext);
  if (!merge) copy->id = id;

  if (pos_cluster_dma_first)
  {
    pos_cluster_dma_last->next = copy;
  }
  else
  {
    pos_cluster_dma_first = copy;    
  }

  copy->next = NULL;
  pos_cluster_dma_last = NULL;  

  eu_mutex_unlock_from_id(0);
}
#endif



static inline void __cl_dma_memcpy_2d(unsigned int ext, unsigned int loc, unsigned int size, unsigned int stride, unsigned short length, pi_cl_dma_dir_e dir, int merge, pi_cl_dma_cmd_t *copy)
{
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
  }

  eu_mutex_unlock_from_id(0);
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


static inline void pi_cl_dma_cmd_safe(uint32_t ext, uint32_t loc, uint32_t size, pi_cl_dma_dir_e dir, pi_cl_dma_cmd_t *cmd)
{
  __cl_dma_memcpy_safe(ext, loc, size, dir, 0, (pi_cl_dma_cmd_t *)cmd);
}


static inline void pi_cl_dma_cmd_2d(uint32_t ext, uint32_t loc, uint32_t size, uint32_t stride, uint32_t length, pi_cl_dma_dir_e dir, pi_cl_dma_cmd_t *cmd)
{
  __cl_dma_memcpy_2d(ext, loc, size, stride, length, dir, 0, (pi_cl_dma_cmd_t *)cmd);
}


static inline void pi_cl_dma_cmd_wait(pi_cl_dma_cmd_t *cmd)
{
  __cl_dma_wait((pi_cl_dma_cmd_t *)cmd);
}

static inline void pi_cl_dma_cmd_wait_safe(pi_cl_dma_cmd_t *cmd)
{
  __cl_dma_wait_safe((pi_cl_dma_cmd_t *)cmd);
}


#endif