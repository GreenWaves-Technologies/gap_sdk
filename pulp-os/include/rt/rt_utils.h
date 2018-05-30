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

#ifndef __RT_RT_UTILS_H__
#define __RT_RT_UTILS_H__

#include <string.h>
#include "rt_data.h"
#include "archi/pulp.h"


/**        
 * @ingroup groupKernel        
 */

/**        
 * @defgroup Utils Utility functions
 * 
 */



/**        
 * @addtogroup Utils
 * @{        
 */

/**        
 * @defgroup UtilsRuntime Runtime information
 *
 */

/**@{*/

/** \brief Returns the default stack size on the cluster for the master core.
 *
 * The master core is the one receiving a call for the cluster. This stack size is only used if no stack size is specified when generating
 * a call to cluster side.
 *
 * \return           The stack size in bytes.
 */
static inline int rt_cl_master_stack_size_get();



/** \brief Returns the default stack size on cluster side for the slave cores.
 *
 * The slave cores are those cores used by the master core to start a multi core task. This stack size is only used if no stack size is
 * specified when starting a call to cluster.
 *
 * \return           The stack size in bytes.
 */
static inline int rt_cl_slave_stack_size_get();


//!@}

/**        
 * @}  
 */





/**        
 * @addtogroup Utils
 * @{        
 */

/**        
 * @defgroup UtilsArchi Architecture information
 *
 */

/**@{*/

/** \brief Tell if the calling core is the fabric controller.
 *
 * \return           1 if the calling core is the fabric controller, 0 otherwise.
 */
static inline int rt_is_fc();

/** \brief Give the cluster identifier of the calling core.
 *
 * \return           The cluster identifier.
 */
static inline int rt_cluster_id();

/** \brief Give the core identifier of the calling core.
 *
 * \return           The core identifier.
 */
static inline int rt_core_id();

/** \brief Returns the number of cores on the first cluster.
 *
 * \return           The number of cores.
 */
static inline int rt_nb_pe();


//!@}

/**        
 * @}  
 */



/**        
 * @addtogroup Utils
 * @{        
 */

/**        
 * @defgroup UtilsMem  Static data memory placement
 *
 * By default, a global variable is put in the fabric L2 memory, as it is the biggest one.
 * The following macros can be used to change this behavior and specify in which memory it must be placed.
 *
 * This has to be used on the declaration of the variable. A few other behavior can also be specified:
 *   - A variable can sometimes be put in the short section, resulting in better optimized accesses as the address generation can then be done with only 1 instruction instead of 2 or 3.
 *   - A variable is by default using a global address which allows the variable to be accessed from anywhere. For some memories, it is possible to ask for a local address so that the variable can be access as a local one on each cluster.
 * 
 */

/**@{*/


/** Put the data inside cluster 0 memory, using a gobal address. */
#define RT_CL_DATA __attribute__((section(".data_l1")))

/** Put the data inside each cluster memory, using a local address. */
#define RT_CL_ALIAS_DATA __attribute__((section(".data_alias_l1")))

/** Put the data inside each cluster memory, using a short local address. */
#define RT_CL_SHORT_DATA __attribute__((section(".data_tiny_l1")))



/** \brief Gives the start address of the runtime cluster code
 *
 * The runtime code used by the cluster is put into a specific section so that
 * it can be better controlled. This function can be used to get the start
 * address of this section.
 */
static inline void *rt_section_cluster_text_start();



/** \brief Gives the size of the runtime cluster code
 *
 * The runtime code used by the cluster is put into a specific section so that
 * it can be better controlled. This function can be used to get the size
 * of this section.
 */
static inline int rt_section_cluster_text_size();

//!@}

/**        
 * @}      
 */




/// @cond IMPLEM

#include "hal/pulp.h"

extern unsigned char __cluster_text_start;
extern unsigned char __cluster_text_size;
static inline void *rt_section_cluster_text_start() { return (void *)&__cluster_text_start; }
static inline int rt_section_cluster_text_size() { return (int)&__cluster_text_size; }

extern unsigned char __rt_cl_master_stack_size;
extern unsigned char __rt_cl_slave_stack_size;
extern unsigned char __rt_stack_size;

#if defined(ARCHI_HAS_CLUSTER)

static inline unsigned int __rt_tas_addr(unsigned int addr) {
  return addr | (1<<ARCHI_L1_TAS_BIT);
}

static inline int rt_tas_lock_8(unsigned int addr) {
  __asm__ __volatile__ ("" : : : "memory");
  int result = *(volatile unsigned char *)__rt_tas_addr(addr);
  __asm__ __volatile__ ("" : : : "memory");
  return result;
}

static inline void rt_tas_unlock_8(unsigned int addr, unsigned char value) {
  __asm__ __volatile__ ("" : : : "memory");
  *(volatile unsigned char *)addr = value;
  __asm__ __volatile__ ("" : : : "memory");
}

static inline int rt_tas_lock_16(unsigned int addr) {
  __asm__ __volatile__ ("" : : : "memory");
  int result = *(volatile unsigned short *)__rt_tas_addr(addr);
  __asm__ __volatile__ ("" : : : "memory");
  return result;
}

static inline void rt_tas_unlock_16(unsigned int addr, unsigned short value) {
  __asm__ __volatile__ ("" : : : "memory");
  *(volatile unsigned short *)addr = value;
  __asm__ __volatile__ ("" : : : "memory");
}

static inline int rt_tas_lock_32(unsigned int addr) {
  __asm__ __volatile__ ("" : : : "memory");
  int result = *(volatile unsigned int *)__rt_tas_addr(addr);
  __asm__ __volatile__ ("" : : : "memory");
  return result;
}

static inline void rt_tas_unlock_32(unsigned int addr, unsigned int value) {
  __asm__ __volatile__ ("" : : : "memory");
  *(volatile unsigned int *)addr = value;
  __asm__ __volatile__ ("" : : : "memory");
}

#endif

static inline int rt_cl_master_stack_size_get()
{
  return (int)(long)&__rt_cl_master_stack_size;
}

static inline int rt_stack_size_get()
{
  return (int)(long)&__rt_stack_size;
}

static inline int rt_cl_slave_stack_size_get()
{
  return (int)(long)&__rt_cl_slave_stack_size;
}


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

extern unsigned char __l2_priv0_heap_start;
extern unsigned char __l2_priv0_heap_size;
extern unsigned char __l2_priv1_heap_start;
extern unsigned char __l2_priv1_heap_size;
extern unsigned char __l2_shared_heap_start;
extern unsigned char __l2_shared_heap_size;

#else

extern unsigned char __l2_heap_start;
extern unsigned char __l2_heap_size;

#endif

#endif
extern unsigned char __l1_heap_start;;
extern unsigned char __l1_heap_size;
extern unsigned char __fc_tcdm_heap_start;
extern unsigned char __fc_tcdm_heap_size;
extern unsigned char __irq_vector_base;
extern unsigned char __rt_nb_cluster;
extern unsigned char __rt_nb_pe;
extern unsigned char __rt_platform;
extern unsigned char __rt_iodev;
extern unsigned char __rt_iodev_uart_baudrate;
extern unsigned char __rt_iodev_uart_channel;


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
  return (int)&__rt_nb_cluster;
}

static inline int rt_nb_active_pe()
{
  return (int)&__rt_nb_pe;
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



static inline int rt_platform()
{
  return (int)&__rt_platform;
}

#define RT_IODEV_DEFAULT 0
#define RT_IODEV_UART 1

static inline int rt_iodev()
{
  return (int)&__rt_iodev;
}

static inline int rt_iodev_uart_baudrate()
{
  return (int)&__rt_iodev_uart_baudrate;
}

static inline int rt_iodev_uart_channel()
{
  return (int)&__rt_iodev_uart_channel;
}


static inline void *rt_irq_vector_base() { return (void *)&__irq_vector_base; }



#if defined(__RT_ALLOC_L2_MULTI)

#if defined(ARCHI_HAS_L2)

static inline void *rt_l2_priv0_base() { return (void *)&__l2_priv0_heap_start; }

static inline int rt_l2_priv0_size() { return (int)&__l2_priv0_heap_size; }

static inline void *rt_l2_priv1_base() { return (void *)&__l2_priv1_heap_start; }

static inline int rt_l2_priv1_size() { return (int)&__l2_priv1_heap_size; }

static inline void *rt_l2_shared_base() { return (void *)&__l2_shared_heap_start; }

static inline int rt_l2_shared_size() { return (int)&__l2_shared_heap_size; }

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

static inline void *rt_l2_base() { return (void *)&__l2_heap_start; }

static inline int rt_l2_size() { return (int)&__l2_heap_size; }

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
  return (void *)&__fc_tcdm_heap_start;
#else
  return NULL;
#endif
}

static inline int rt_fc_tcdm_size()
{
#if defined(ARCHI_HAS_FC_TCDM)
  return (int)&__fc_tcdm_heap_size;
#else
  return 0;
#endif
}

#ifdef __riscv__
static inline void rt_wait_for_interrupt()
{
#if !defined(ARCHI_HAS_FC) || defined(ARCHI_HAS_FC_EU)
#if EU_VERSION == 3
  // TODO On FC side, the event unit is used as an interrupt controller and thus
  // we should never replay the clock-gated load. Just use a normal load
  // to go to sleep. However this does not work with a normal load, we should
  // investigate why.
  eu_evt_wait_noreplay();
  //eu_evt_wait();
#else
  eu_evt_wait();
#endif
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
#if defined(APB_SOC_VERSION) && APB_SOC_VERSION == 1
  return ARCHI_L2_ADDR;
#else
  if (rt_is_fc()) {
#if defined(ARCHI_CORE_HAS_SECURITY)
    return __builtin_pulp_spr_read(SR_MTVEC);
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

  return 0;
}

static inline void __rt_set_fc_vector_base(unsigned int base)
{
#if defined(APB_SOC_VERSION) && APB_SOC_VERSION == 1
#else
  if (rt_is_fc()) {
#if defined(ARCHI_CORE_HAS_SECURITY)
    __builtin_pulp_spr_write(SR_MTVEC, base);
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

/// @endcond

#endif
