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

#ifndef __POS_IMPLEM_LINK_H__
#define __POS_IMPLEM_LINK_H__

extern unsigned char __irq_vector_base;

static inline uint32_t pos_irq_vector_base()
{
    return (uint32_t)&__irq_vector_base;
}




extern char _l1_preload_start_inL2[];
extern char _l1_preload_start[];
extern char _l1_preload_size[];


#if defined(ARCHI_HAS_L2)

#if defined(ARCHI_HAS_L2_MULTI)

extern unsigned char __l2_priv0_end;
extern unsigned char __l2_priv1_end;
extern unsigned char __l2_shared_end;

static inline void *pos_l2_priv0_base() {
  if ((int)&__l2_priv0_end >= ARCHI_L2_PRIV1_ADDR)
    return NULL;
  else
    return (void *)&__l2_priv0_end;
}

static inline int pos_l2_priv0_size() {
  if ((int)&__l2_priv0_end >= ARCHI_L2_PRIV1_ADDR)
    return 0;
  else
    return ARCHI_L2_PRIV1_ADDR - (int)&__l2_priv0_end;
}

static inline void *pos_l2_priv1_base() {
  if ((int)&__l2_priv1_end >= ARCHI_L2_SHARED_ADDR)
    return NULL;
  else
    return (void *)&__l2_priv1_end;
}

static inline int pos_l2_priv1_size() {
  if ((int)&__l2_priv1_end >= ARCHI_L2_SHARED_ADDR)
    return 0;
  else
    return ARCHI_L2_SHARED_ADDR - (int)&__l2_priv1_end;
}

static inline void *pos_l2_shared_base() {
  return (void *)&__l2_shared_end;
}

#ifdef CONFIG_XIP
static inline int pos_l2_shared_size() {
  return ARCHI_L2_SHARED_ADDR + ARCHI_L2_SHARED_SIZE - (int)&__l2_shared_end - CONFIG_XIP_SIZE;
}
#else
static inline int pos_l2_shared_size() {
  return ARCHI_L2_SHARED_ADDR + ARCHI_L2_SHARED_SIZE - (int)&__l2_shared_end;
}
#endif

#else

#if PULP_CHIP_FAMILY == CHIP_GAP

extern unsigned char __l2_end;
extern unsigned char __fc_tcdm_end;

static inline void *pos_l2_base() { return (void *)&__l2_end; }
static inline int pos_l2_size() { return ARCHI_L2_ADDR + ARCHI_L2_SIZE - (int)&__l2_end; }
static inline void *pos_fc_tcdm_base() { return (void *)&__fc_tcdm_end; }
static inline int pos_fc_tcdm_size() { return ARCHI_FC_TCDM_ADDR + ARCHI_FC_TCDM_SIZE - (int)&__fc_tcdm_end; }

#else

extern unsigned char __l2_heap_start;
extern unsigned char __l2_heap_size;

static inline void *pos_l2_base() { return (void *)&__l2_heap_start; }
static inline int pos_l2_size() { return (int)&__l2_heap_size; }

#endif

#endif

#endif



#if defined(ARCHI_HAS_L1)

extern unsigned char __l1_heap_start;
extern unsigned char __l1_heap_size;

static inline void *pos_l1_base(int cid) { return ((char *)&__l1_heap_start) + cid * ARCHI_CLUSTER_SIZE; }
static inline int pos_l1_size(int cid) { return (int)&__l1_heap_size; }

#endif


extern unsigned char _bss_start;
extern unsigned char _bss_end;

static inline void *pos_bss_start() { return ((char *)&_bss_start); }
static inline int pos_bss_end() { return (int)&_bss_end; }


#endif
