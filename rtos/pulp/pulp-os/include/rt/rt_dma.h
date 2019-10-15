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

#ifndef __RT_RT_DMA_H__
#define __RT_RT_DMA_H__


/**        
 * @ingroup groupCluster        
 */


/**        
 * @defgroup DMA DMA management
 *
 * The cluster has its own local memory for fast access from the cluster cores while the other memories are relatively slow if accessed by the cluster.
 * To keep all the cores available for computation each cluster contains a DMA unit whose role is to asynchronously transfer data between a remote memory
 * and the cluster memory.
 *
 * The following API can be used to control the DMA in various ways:
 *   - Simple completion. This is only usable by a cluster. A set of transfers can be queued together. An identifier is allocated for the first transfer and reused for the following transfers. This identifier can then be used to block the calling core until all transfers are completed.
 *   - Event-based completion. This can be used on either the fabric controller or a cluster to enqueue a transfer and get notified on completion via an event on the fabric controller.
 */

/**        
 * @addtogroup DMA
 * @{        
 */

/**@{*/

/** \enum rt_dma_dir_e
 * \brief DMA transfer direction.
 *
 * Describes the direction for a DMA transfer.
 */
typedef enum {
  RT_DMA_DIR_LOC2EXT  = 0,    /*!< Transfer from cluster memory to external memory. */
  RT_DMA_DIR_EXT2LOC  = 1,    /*!< Transfer from external memory to cluster memory. */
} rt_dma_dir_e;



/** \brief DMA copy structure.
 *
 * This structure is used by the runtime to manage a DMA copy.
 * It must be instantiated once for each copy and must be kept alive until the copy is finished.
 * It can be instantiated as a normal variable, for example as a global variable, a local one on the stack,
 * or through the memory allocator.
 */
typedef struct rt_dma_copy_s rt_dma_copy_t;



/** \brief 1D DMA memory transfer. 
 *
 * This enqueues a 1D DMA memory transfer (i.e. classic memory copy) with simple completion based on transfer identifier.
 *
 * This can only be called on a cluster.
 * 
 * \param   ext     Address in the external memory where to access the data. There is no restriction on memory alignment.
 * \param   loc     Address in the cluster memory where to access the data. There is no restriction on memory alignment.
 * \param   size    Number of bytes to be transfered. The only restriction is that this size must fit in 16 bits, i.e. must be less than 65536.
 * \param   dir     Direction of the transfer. If RT_DMA_DIR_EXT2LOC, the transfer is loading data from external memory and storing to cluster memory. If RT_DMA_DIR_LOC2EXT, it is the opposite.
 * \param   merge   If 1, this transfer will be merged with the previous one, i.e. they will share the same transfer identifier. Otherwise a new identifier will be allocated.
 * \param   copy    The structure for the copy. This can be used with rt_dma_wait to wait for the completion of this transfer.
 */
static inline void rt_dma_memcpy(unsigned int ext, unsigned int loc, unsigned short size, rt_dma_dir_e dir, int merge, rt_dma_copy_t *copy);



/** \brief 2D DMA memory transfer. 
 *
 * This enqueues a 2D DMA memory transfer (rectangle area) with simple completion based on transfer identifier.
 *
 * This can only be called on a cluster.
 * 
 * \param   ext     Address in the external memory where to access the data. There is no restriction on memory alignment.
 * \param   loc     Address in the cluster memory where to access the data. There is no restriction on memory alignment.
 * \param   size    Number of bytes to be transfered. The only restriction is that this size must fit in 16 bits, i.e. must be less than 65536.
 * \param   stride  2D stride, which is the number of bytes which are added to the beginning of the current line to switch to the next one. Must fit in 16 bits, i.e. must be less than 65536.
 * \param   length  2D length, which is the number of transfered bytes after which the DMA will switch to the next line. Must fit in 16 bits, i.e. must be less than 65536.
 * \param   dir     Direction of the transfer. If it is RT_DMA_DIR_EXT2LOC, the transfer is loading data from external memory and storing to cluster memory. If RT_DMA_DIR_LOC2EXT, it is the opposite.
 * \param   merge   If 1, this transfer will be merged with the previous one, i.e. they will share the same transfer identifier. Otherwise a new identifier will be allocated.
 * \param   copy    The structure for the copy. This can be used with rt_dma_wait to wait for the completion of this transfer.
 */
static inline void rt_dma_memcpy_2d(unsigned int ext, unsigned int loc, unsigned short size, unsigned short stride, unsigned short length, rt_dma_dir_e dir, int merge, rt_dma_copy_t *copy);


/** \brief Simple DMA transfer completion flush. 
 *
 * This blocks the core until the DMA does not have any pending transfers. 
 *
 * This can only be called on a cluster.
 */
static inline void rt_dma_flush();



/** \brief Simple DMA transfer completion wait.
 *
 * This blocks the core until the specified transfer is finished. The transfer must be described trough the identifier returned by the copy function.
 *
 * This can only be called on a cluster.
 *
 * \param   copy  The copy structure.
 */
static inline void rt_dma_wait(rt_dma_copy_t *copy);



//!@}

/**        
 * @} end of DMA group        
 */


/// @cond IMPLEM

#if defined(MCHAN_VERSION)

#if MCHAN_VERSION >= 6

static inline void rt_dma_memcpy(unsigned int ext, unsigned int loc, unsigned short size, rt_dma_dir_e dir, int merge, rt_dma_copy_t *copy)
{
#ifdef __RT_USE_PROFILE
  int trace = __rt_pe_trace[rt_core_id()];
  gv_vcd_dump_trace(trace, 4);
#endif

  int id = -1;
#if MCHAN_VERSION >= 7
  eu_mutex_lock_from_id(0);
#endif
  if (!merge) id = plp_dma_counter_alloc();
  unsigned int cmd = plp_dma_getCmd(dir, size, PLP_DMA_1D, PLP_DMA_TRIG_EVT, PLP_DMA_NO_TRIG_IRQ, PLP_DMA_SHARED);
  // Prevent the compiler from pushing the transfer before all previous
  // stores are done
  __asm__ __volatile__ ("" : : : "memory");
  plp_dma_cmd_push(cmd, loc, ext);
#if MCHAN_VERSION >= 7
  eu_mutex_unlock_from_id(0);
#endif
  if (!merge) copy->id = id;

#ifdef __RT_USE_PROFILE
  gv_vcd_dump_trace(trace, 1);
#endif
}


static inline void rt_dma_memcpy_2d(unsigned int ext, unsigned int loc, unsigned short size, unsigned short stride, unsigned short length, rt_dma_dir_e dir, int merge, rt_dma_copy_t *copy)
{
#ifdef __RT_USE_PROFILE
  int trace = __rt_pe_trace[rt_core_id()];
  gv_vcd_dump_trace(trace, 4);
#endif

  int id = -1;
#if MCHAN_VERSION >= 7
  eu_mutex_lock_from_id(0);
#endif
  if (!merge) id = plp_dma_counter_alloc();
  unsigned int cmd = plp_dma_getCmd(dir, size, PLP_DMA_2D, PLP_DMA_TRIG_EVT, PLP_DMA_NO_TRIG_IRQ, PLP_DMA_SHARED);
  // Prevent the compiler from pushing the transfer before all previous
  // stores are done
  __asm__ __volatile__ ("" : : : "memory");
  plp_dma_cmd_push_2d(cmd, loc, ext, stride, length);
  if (!merge) copy->id = id;

#if MCHAN_VERSION >= 7
  eu_mutex_unlock_from_id(0);
#endif
  
#ifdef __RT_USE_PROFILE
  gv_vcd_dump_trace(trace, 1);
#endif
}

static inline void rt_dma_flush()
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

static inline void rt_dma_wait(rt_dma_copy_t *copy)
{
#ifdef __RT_USE_PROFILE
  int trace = __rt_pe_trace[rt_core_id()];
  gv_vcd_dump_trace(trace, 5);
#endif
  plp_dma_wait(copy->id);
#ifdef __RT_USE_PROFILE
  gv_vcd_dump_trace(trace, 1);
#endif
}

#else

static inline void rt_dma_memcpy(unsigned int ext, unsigned int loc, unsigned short size, rt_dma_dir_e dir, int merge, rt_dma_copy_t *copy)
{
}


static inline void rt_dma_memcpy_2d(unsigned int ext, unsigned int loc, unsigned short size, unsigned short stride, unsigned short length, rt_dma_dir_e dir, int merge, rt_dma_copy_t *copy)
{
}

static inline void rt_dma_flush()
{

}

static inline void rt_dma_wait(rt_dma_copy_t *copy)
{
}

#endif

#else

static inline void rt_dma_memcpy(unsigned int ext, unsigned int loc, unsigned short size, rt_dma_dir_e dir, int merge, rt_dma_copy_t *copy)
{
}


static inline void rt_dma_memcpy_2d(unsigned int ext, unsigned int loc, unsigned short size, unsigned short stride, unsigned short length, rt_dma_dir_e dir, int merge, rt_dma_copy_t *copy)
{
}

static inline void rt_dma_flush()
{

}

static inline void rt_dma_wait(rt_dma_copy_t *copy)
{
}

#endif


/** \brief 1D DMA memory transfer with event completion.
 *
 * This function is very similar to rt_dma_memcpy, except that once the transfer is finished, the runtime will enqueue an event on fabric controller side.
 *
 * This can be called either from fabric controller or cluster side.
 * 
 * \param   ext     Address in the external memory where to access the data. There is no restriction on memory alignment.
 * \param   loc     Address in the cluster memory where to access the data. There is no restriction on memory alignment.
 * \param   size    Number of bytes to be transfered. The only restriction is that this size must fit in 16 bits, i.e. must be less than 65536.
 * \param   dir     Direction of the transfer. If it is RT_DMA_DIR_EXT2LOC, the transfer is loading data from external memory and storing to cluster memory. If RT_DMA_DIR_LOC2EXT, it is the opposite.
 * \param   copy    A pointer to the copy node. This structure is used by the runtime to maintain the state of the transfer and must be allocated by the caller. It must be kept allocated until the end of transfer is notified.
 * \param   event   An event to specify how to be notified when the transfer is finished. This will always trigger an event on fabric controller side.
 */
void rt_dma_memcpy_event(unsigned int ext, unsigned int loc, unsigned short size, rt_dma_dir_e dir, rt_dma_copy_t *copy, rt_event_t *event);



/** \brief 2D DMA memory transfer with event completion.
 *
 * This function is very similar to rt_dma_memcpy_2d, except that once the transfer is finished, the runtime will enqueue an event on fabric controller side.
 *
 * This can be called either from fabric controller or cluster side.
 * 
 * \param   ext     Address in the external memory where to access the data. There is no restriction on memory alignment.
 * \param   loc     Address in the cluster memory where to access the data. There is no restriction on memory alignment.
 * \param   size    Number of bytes to be transfered. The only restriction is that this size must fit in 16 bits, i.e. must be less than 65536.
 * \param   stride  2D stride, which is the number of bytes which are added to the beginning of the current line to switch to the next one. Must fit in 16 bits, i.e. must be less than 65536.
 * \param   length  2D length, which is the number of transfered bytes after which the DMA will switch to the next line. Must fit in 16 bits, i.e. must be less than 65536.
 * \param   dir     Direction of the transfer. If it is RT_DMA_DIR_EXT2LOC, the transfer is loading data from external memory and storing to cluster memory. If RT_DMA_DIR_LOC2EXT, it is the opposite.
 * \param   copy    A pointer to the copy node. This structure is used by the runtime to maintain the state of the transfer and must be allocated by the caller. It must be kept allocated until the end of transfer is notified.
 * \param   event   An event to specify how to be notified when the transfer is finished. This will always trigger an event on fabric controller side.
 */
void rt_dma_memcpy_2d_event(unsigned int ext, unsigned int loc, unsigned short size, unsigned short stride, unsigned short length, rt_dma_dir_e dir, rt_dma_copy_t *copy, rt_event_t *event);

/// @endcond

#endif
