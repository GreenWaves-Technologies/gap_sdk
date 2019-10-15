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

#ifndef __RT_IMPLEM_UTILS_H__
#define __RT_IMPLEM_UTILS_H__

#include <string.h>
#include "rt/rt_data.h"
#include "archi/pulp.h"

/// @cond IMPLEM

#include "hal/pulp.h"

extern unsigned int __rt_iodev;
extern unsigned int __rt_iodev_uart_baudrate;
extern unsigned int __rt_iodev_uart_channel;

extern unsigned char __cluster_text_start;
extern unsigned char __cluster_text_size;
static inline void *rt_section_cluster_text_start() { return (void *)&__cluster_text_start; }
static inline int rt_section_cluster_text_size() { return (int)&__cluster_text_size; }

#if defined(ARCHI_HAS_CLUSTER)

static inline unsigned int __rt_tas_addr(unsigned int addr) {
  return addr | (1<<ARCHI_L1_TAS_BIT);
}

static inline signed char rt_tas_lock_8(unsigned int addr) {
  __asm__ __volatile__ ("" : : : "memory");
  signed char result = *(volatile signed char *)__rt_tas_addr(addr);
  __asm__ __volatile__ ("" : : : "memory");
  return result;
}

static inline void rt_tas_unlock_8(unsigned int addr, signed char value) {
  __asm__ __volatile__ ("" : : : "memory");
  *(volatile signed char *)addr = value;
  __asm__ __volatile__ ("" : : : "memory");
}

static inline signed short rt_tas_lock_16(unsigned int addr) {
  __asm__ __volatile__ ("" : : : "memory");
  signed short result = *(volatile signed short *)__rt_tas_addr(addr);
  __asm__ __volatile__ ("" : : : "memory");
  return result;
}

static inline void rt_tas_unlock_16(unsigned int addr, signed short value) {
  __asm__ __volatile__ ("" : : : "memory");
  *(volatile signed short *)addr = value;
  __asm__ __volatile__ ("" : : : "memory");
}

static inline int rt_tas_lock_32(unsigned int addr) {
  __asm__ __volatile__ ("" : : : "memory");
  signed int result = *(volatile signed int *)__rt_tas_addr(addr);
  __asm__ __volatile__ ("" : : : "memory");
  return result;
}

static inline void rt_tas_unlock_32(unsigned int addr, signed int value) {
  __asm__ __volatile__ ("" : : : "memory");
  *(volatile signed int *)addr = value;
  __asm__ __volatile__ ("" : : : "memory");
}

#endif


#if defined(ARCHI_HAS_L2)

#if defined(__RT_ALLOC_L2_MULTI)

static inline void *rt_l2_priv0_base();

static inline int rt_l2_priv0_size();

static inline void *rt_l2_priv1_base();

static inline int rt_l2_priv1_size();

static inline void *rt_l2_shared_base();

static inline int rt_l2_shared_size();

#else

static inline void *rt_l2_base();

static inline int rt_l2_size();

#endif


#ifdef ARCHI_L2_SCM_ADDR

static inline void *rt_l2_scm_base();

static inline int rt_l2_scm_size();

#endif


#endif

static inline void *rt_l1_base(int cid);

static inline int rt_l1_size(int cide);

static inline void *rt_fc_tcdm_base();

static inline int rt_fc_tcdm_size();

#if defined(__ariane__)

static inline int rt_cluster_id()
{
  return 0;
}

static inline int rt_core_id()
{
  return 0;
}

static inline int rt_has_fc()
{
  return 0;
}

static inline int rt_is_fc()
{
  return 0;
}

static inline int rt_platform()
{
  return 0;
}

#else

#if defined(ARCHI_HAS_L2)

#if defined(__RT_ALLOC_L2_MULTI)

#ifdef ARCHI_L2_SCM_ADDR

extern unsigned char __l2_scm_end;

#endif

extern unsigned char __l2_priv0_end;
extern unsigned char __l2_priv1_end;
extern unsigned char __l2_shared_end;

#else

#if PULP_CHIP_FAMILY == CHIP_GAP

extern unsigned char __l2_end;
extern unsigned char __fc_tcdm_end;

#else

extern unsigned char __l2_heap_start;
extern unsigned char __l2_heap_size;
extern unsigned char __fc_tcdm_heap_start;
extern unsigned char __fc_tcdm_heap_size;

#endif

#endif

#endif
extern unsigned char __l1_heap_start;;
extern unsigned char __l1_heap_size;
extern unsigned char __irq_vector_base;


static inline int rt_has_fc()
{
  return hal_has_fc();
}

static inline int rt_is_fc()
{
  return hal_is_fc();
}

static inline int rt_cluster_id()
{
  return hal_cluster_id();
}

static inline int rt_core_id()
{
  return hal_core_id();
}

#if defined(ARCHI_HAS_CLUSTER)

static inline int rt_nb_cluster()
{
#ifndef ARCHI_NB_CLUSTER
  return 1;
#else
  return ARCHI_NB_CLUSTER;
#endif
}

static inline int rt_nb_active_pe()
{
#ifdef ARCHI_HAS_CC
  return ARCHI_CLUSTER_NB_PE + 1;
#else
  return ARCHI_CLUSTER_NB_PE;
#endif
}

#if defined(PLP_NO_BUILTIN) || defined(__cplusplus)


static inline int rt_nb_pe()
{
  return ARCHI_NB_PE;
}

#else

static inline int rt_nb_pe()
{
  return __builtin_pulp_CoreCount();
}

#endif

#else

static inline int rt_nb_cluster()
{
  return 0;
}

static inline int rt_nb_active_pe()
{
  return 0;
}

static inline int rt_nb_pe()
{
  return 0;
}

#endif


#ifndef __PLATFORM__

extern unsigned int __rt_platform;

static inline int rt_platform()
{
  return __rt_platform;
}

#else

static inline int rt_platform()
{
  return __PLATFORM__;
}

#endif

#define RT_IODEV_DEFAULT 0
#define RT_IODEV_UART 1

static inline int rt_iodev()
{
#ifdef CONFIG_IODEV
  return CONFIG_IODEV;
#else
  return __rt_iodev;
#endif
}

static inline int rt_iodev_uart_baudrate()
{
#ifdef CONFIG_UART_BAUDRATE
  return CONFIG_UART_BAUDRATE;
#else
  return __rt_iodev_uart_baudrate;
#endif
}

static inline int rt_iodev_uart_channel()
{
#ifdef CONFIG_UART_CHANNEL
  return CONFIG_UART_CHANNEL;
#else
  return __rt_iodev_uart_channel;
#endif
}


static inline void *rt_irq_vector_base() { return (void *)&__irq_vector_base; }



#if defined(__RT_ALLOC_L2_MULTI)

#if defined(ARCHI_HAS_L2)


#if PULP_CHIP == CHIP_QUENTIN

static inline void *rt_l2_priv0_base() {
  if ((int)&__l2_priv0_end >= ARCHI_L2_SCM_ADDR)
    return NULL;
  else
    return (void *)&__l2_priv0_end;
}

static inline int rt_l2_priv0_size() {
  if ((int)&__l2_priv0_end >= ARCHI_L2_SCM_ADDR)
    return 0;
  else
    return ARCHI_L2_SCM_ADDR - (int)&__l2_priv0_end;
}

static inline void *rt_l2_scm_base() {
  if ((int)&__l2_scm_end >= ARCHI_L2_PRIV1_ADDR)
    return NULL;
  else
    return (void *)&__l2_scm_end;
}

static inline int rt_l2_scm_size() {
  if ((int)&__l2_scm_end >= ARCHI_L2_PRIV1_ADDR)
    return 0;
  else
    return ARCHI_L2_PRIV1_ADDR - (int)&__l2_scm_end;
}

static inline void *rt_l2_priv1_base() {
  if ((int)&__l2_priv1_end >= ARCHI_L2_SHARED_ADDR)
    return NULL;
  else
    return (void *)&__l2_priv1_end;
}

static inline int rt_l2_priv1_size() {
  if ((int)&__l2_priv1_end >= ARCHI_L2_SHARED_ADDR)
    return 0;
  else
    return ARCHI_L2_SHARED_ADDR - (int)&__l2_priv1_end;
}

static inline void *rt_l2_shared_base() {
  return (void *)&__l2_shared_end;
}

static inline int rt_l2_shared_size() {
  return ARCHI_L2_SHARED_ADDR + ARCHI_L2_SHARED_SIZE - (int)&__l2_shared_end;
}

#else

static inline void *rt_l2_priv0_base() {
  if ((int)&__l2_priv0_end >= ARCHI_L2_PRIV1_ADDR)
    return NULL;
  else
    return (void *)&__l2_priv0_end;
}

static inline int rt_l2_priv0_size() {
  if ((int)&__l2_priv0_end >= ARCHI_L2_PRIV1_ADDR)
    return 0;
  else
    return ARCHI_L2_PRIV1_ADDR - (int)&__l2_priv0_end;
}

static inline void *rt_l2_priv1_base() {
  if ((int)&__l2_priv1_end >= ARCHI_L2_SHARED_ADDR)
    return NULL;
  else
    return (void *)&__l2_priv1_end;
}

static inline int rt_l2_priv1_size() {
  if ((int)&__l2_priv1_end >= ARCHI_L2_SHARED_ADDR)
    return 0;
  else
    return ARCHI_L2_SHARED_ADDR - (int)&__l2_priv1_end;
}

static inline void *rt_l2_shared_base() {
  return (void *)&__l2_shared_end;
}

static inline int rt_l2_shared_size() {
  return ARCHI_L2_SHARED_ADDR + ARCHI_L2_SHARED_SIZE - (int)&__l2_shared_end;
}

#endif

static inline void *rt_l2_base() { return rt_l2_shared_base(); }

static inline int rt_l2_size() { return rt_l2_shared_size(); }

#else

static inline void *rt_l2_priv0_base() { return NULL; }

static inline int rt_l2_priv0_size() { return 0; }

static inline void *rt_l2_priv1_base() { return NULL; }

static inline int rt_l2_priv1_size() { return 0; }

static inline void *rt_l2_shared_base() { return NULL; }

static inline int rt_l2_shared_size() { return 0; }

static inline void *rt_l2_base() { return NULL; }

static inline int rt_l2_size() { return 0; }

#endif

#else

#if defined(ARCHI_HAS_L2)

#if PULP_CHIP_FAMILY == CHIP_GAP

static inline void *rt_l2_base() { return (void *)&__l2_end; }

static inline int rt_l2_size() { return ARCHI_L2_ADDR + ARCHI_L2_SIZE - (int)&__l2_end; }

#else

static inline void *rt_l2_base() { return (void *)&__l2_heap_start; }

static inline int rt_l2_size() { return (int)&__l2_heap_size; }

#endif

#else

static inline void *rt_l2_base() { return NULL; }

static inline int rt_l2_size() { return 0; }

#endif

#endif

static inline void *rt_l1_base(int cid)
{
#if defined(ARCHI_HAS_L1)
  return ((char *)&__l1_heap_start) + cid * ARCHI_CLUSTER_SIZE;
#else
  return NULL;
#endif
}

static inline int rt_l1_size(int cid)
{
#if defined(ARCHI_HAS_L1)
  return (int)&__l1_heap_size;
#else
  return 0;
#endif
}

static inline void *rt_fc_tcdm_base()
{
#if defined(ARCHI_HAS_FC_TCDM)
  return (void *)&__fc_tcdm_end;
#else
  return NULL;
#endif
}

static inline int rt_fc_tcdm_size()
{
#if defined(ARCHI_HAS_FC_TCDM)
  return ARCHI_FC_TCDM_ADDR + ARCHI_FC_TCDM_SIZE - (int)&__fc_tcdm_end;
#else
  return 0;
#endif
}

#include <stdio.h>

#ifdef __riscv__
static inline void rt_wait_for_interrupt()
{
#if !defined(ARCHI_HAS_FC) || defined(ARCHI_HAS_FC_EU)
  eu_evt_wait();
#else
  hal_itc_wait_for_interrupt();
#endif
}
#else
void rt_wait_for_interrupt();
#endif

static inline void rt_compiler_barrier() {
  __asm__ __volatile__ ("" : : : "memory");
}

static inline void *rt_cluster_tiny_addr(int cid, void *data)
{
  // TODO due to a compiler bug, we have to cast the tiny data to avoid the propagation of the tiny attribute
  return (void *)(ARCHI_CLUSTER_GLOBAL_ADDR(cid) + ((int)data & 0xFFF));
}

#endif

#include <stdio.h>

static inline unsigned int __rt_get_fc_vector_base()
{
#if defined(ARCHI_CORE_RISCV_ITC)
  return hal_spr_read(0x305) & ~1;
#else
#if defined(APB_SOC_VERSION) && APB_SOC_VERSION == 1
  return ARCHI_L2_ADDR;
#else
  if (rt_is_fc()) {
#if defined(ARCHI_CORE_HAS_SECURITY) && !defined(ARCHI_CORE_HAS_1_10)
    return __builtin_pulp_spr_read(SR_MTVEC);
#elif defined(ARCHI_CORE_HAS_1_10)
#ifdef RV_ISA_RV32
    return 0;
#else
    return __builtin_pulp_spr_read(SR_MTVEC) & ~1;
#endif
#elif defined(APB_SOC_VERSION) && APB_SOC_VERSION >= 2
    return apb_soc_bootaddr_get();
#endif
  }
  else
  {
#if defined(ARCHI_HAS_CLUSTER)
#if defined(ARCHI_CLUSTER_CTRL_ADDR)
    return plp_ctrl_bootaddr_get();
#endif
#endif
  }
#endif
#endif

  return 0;
}

static inline void __rt_set_fc_vector_base(unsigned int base)
{
#if defined(ARCHI_CORE_RISCV_ITC)
  hal_spr_write(0x305, base);
#else
#if defined(APB_SOC_VERSION) && APB_SOC_VERSION == 1
#else
  if (rt_is_fc()) {
#if defined(ARCHI_CORE_HAS_SECURITY)
    __builtin_pulp_spr_write(SR_MTVEC, base);
#elif defined(ARCHI_CORE_HAS_1_10)
#ifdef RV_ISA_RV32
#else
    __builtin_pulp_spr_write(SR_MTVEC, base | 1);
#endif
#elif defined(APB_SOC_VERSION) && APB_SOC_VERSION >= 2
    apb_soc_bootaddr_set(base);
#endif
  }
  else
  {
#if defined(ARCHI_HAS_CLUSTER)
#if defined(ARCHI_CLUSTER_CTRL_ADDR)
    plp_ctrl_bootaddr_set(base);
#endif
#endif
  }
#endif
#endif
}

typedef enum {
  RT_CBSYS_START,
  RT_CBSYS_STOP,
  RT_CBSYS_POWEROFF,
  RT_CBSYS_POWERON,
  RT_CBSYS_PERIPH_SETFREQ_BEFORE,
  RT_CBSYS_PERIPH_SETFREQ_AFTER,
  RT_CBSYS_NB
} __rt_cbsys_e;

int __rt_cbsys_add(__rt_cbsys_e cbsys, int (*cb)(void *), void *cb_arg); 

void __rt_cbsys_del(__rt_cbsys_e cbsys_id, int (*cb)(void *), void *cb_arg);

int __rt_cbsys_exec(__rt_cbsys_e cbsys_id);

static inline void __rt_mutex_lock(rt_mutex_t *mutex)
{
  // TODO implement lock to make several API calls work with multi-threading
}

static inline void __rt_mutex_unlock(rt_mutex_t *mutex)
{
  
}

RT_FC_BOOT_CODE void __rt_utils_init();

void __rt_fc_lock(rt_fc_lock_t *lock);

void __rt_fc_unlock(rt_fc_lock_t *lock);

void __rt_fc_cluster_lock(rt_fc_lock_t *lock, rt_fc_lock_req_t *req);

void __rt_fc_cluster_unlock(rt_fc_lock_t *lock, rt_fc_lock_req_t *req);

static inline void __rt_fc_lock_init(rt_fc_lock_t *lock)
{
#if defined(ARCHI_HAS_FC)
  lock->waiting = NULL;
  lock->locked = 0;
  lock->fc_wait = NULL;
#else
  lock->lock = 0;
#endif
}

#if defined(ARCHI_HAS_CLUSTER)

static inline void __rt_fc_cluster_lock_wait(rt_fc_lock_req_t *req)
{
  while((*(volatile char *)&req->done) == 0)
  {
    eu_evt_maskWaitAndClr(1<<RT_CLUSTER_CALL_EVT);
  }
}

#endif

static inline void __rt_wait_for_event(unsigned int mask) {
#if defined(ITC_VERSION)
  hal_itc_wait_for_event_noirq(mask);
#else
  eu_evt_maskWaitAndClr(mask);
#endif
}

/// @endcond


#endif