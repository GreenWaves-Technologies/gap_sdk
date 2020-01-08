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

#ifndef __CL_DMA_H__
#define __CL_DMA_H__

#include "pmsis/cluster/cl_pmsis_types.h"

/**
 * @addtogroup clusterDriver
 * @{
 */

/**
 * @defgroup ClusterDMA Cluster DMA
 *
 * This set of functions provides support for controlling the cluster DMA.
 * The cluster has its own local memory for fast access from the cluster cores
 * while the other memories are relatively slow if accessed by the cluster.
 * To keep all the cores available for computation each cluster contains a DMA
 * unit whose role is to asynchronously transfer data between a remote memory
 * and the cluster memory.
 *
 * The DMA is using HW counters to track the termination of transfers.
 * Each transfer is by default allocating one counter, which is freed when
 * the wait function is called and returns, which is limiting the maximum
 * number of transfers which can be done at the same time.
 * You can check the chip-specific section to know the number of HW counters.
 */

/**
 * @addtogroup ClusterDMA
 * @{
 */

/**@{*/

/** \enum pi_cl_dma_dir_e
 * \brief DMA transfer direction.
 *
 * Describes the direction for a DMA transfer.
 */
typedef enum {
  PI_CL_DMA_DIR_LOC2EXT = 0, /*!< Transfer from cluster memory to external
    memory. */
  PI_CL_DMA_DIR_EXT2LOC = 1  /*!< Transfer from external memory to cluster
    memory. */
} pi_cl_dma_dir_e;

/** \brief Structure for DMA commands.
 *
 * This structure is used by the runtime to manage a DMA command.
 * It must be instantiated once for each copy and must be kept alive
 * until the copy is finished.
 * It can be instantiated as a normal variable, for example as a global
 * variable, a local one on the stack, or through the memory allocator.
 */
typedef struct pi_cl_dma_cmd_s pi_cl_dma_cmd_t;

/** \brief Structure for 1D DMA copy structure.
 *
 * This structure is used by the runtime to manage a 1D DMA copy.
 * It must be instantiated once for each copy and must be kept alive
 * until the copy is finished.
 * It can be instantiated as a normal variable, for example as a global
 * variable, a local one on the stack, or through the memory allocator.
 */
typedef struct pi_cl_dma_copy_s pi_cl_dma_copy_t;

/** \brief Structure for 2D DMA copy structure.
 *
 * This structure is used by the runtime to manage a 2D DMA copy.
 * It must be instantiated once for each copy and must be kept alive
 * until the copy is finished.
 * It can be instantiated as a normal variable, for example as a global
 * variable, a local one on the stack, or through the memory allocator.
 */
typedef pi_cl_dma_copy_t pi_cl_dma_copy_2d_t;

/** \brief 1D DMA memory transfer. 
 *
 * This enqueues a 1D DMA memory transfer (i.e. classic memory copy) with
 * simple completion based on transfer identifier.
 *
 * \param   ext     Address in the external memory where to access the data.
 * \param   loc     Address in the cluster memory where to access the data.
 * \param   size    Number of bytes to be transferred.
 * \param   dir     Direction of the transfer. If it is PI_CL_DMA_DIR_EXT2LOC,
 *   the transfer is loading data from external memory and storing to cluster
 *   memory. If it is PI_CL_DMA_DIR_LOC2EXT, it is the opposite.
 * \param   cmd    A pointer to the structure for the copy. This can be used
 *   with pi_cl_dma_wait to wait for the completion of this transfer.
 */
static inline void pi_cl_dma_cmd(uint32_t ext, uint32_t loc, uint32_t size,
  pi_cl_dma_dir_e dir, pi_cl_dma_cmd_t *cmd);

/** \brief 2D DMA memory transfer. 
 *
 * This enqueues a 2D DMA memory transfer (rectangle) with
 * simple completion based on transfer identifier.
 *
 * \param   ext     Address in the external memory where to access the data.
 * \param   loc     Address in the cluster memory where to access the data.
 * \param   size    Number of bytes to be transferred.
 * \param   stride  2D stride, which is the number of bytes which are added to
 *   the beginning of the current line to switch to the next one.
 * \param   length  2D length, which is the number of transferred bytes after
 * which the DMA will switch to the next line.
 * \param   dir     Direction of the transfer. If it is PI_CL_DMA_DIR_EXT2LOC,
 *   the transfer is loading data from external memory and storing to cluster
 *   memory. If it is PI_CL_DMA_DIR_LOC2EXT, it is the opposite.
 * \param   cmd    A pointer to the structure for the copy. This can be used
 *   with pi_cl_dma_wait to wait for the completion of this transfer.
 */
static inline void pi_cl_dma_cmd_2d(uint32_t ext, uint32_t loc, uint32_t size,
  uint32_t stride, uint32_t length, pi_cl_dma_dir_e dir, pi_cl_dma_cmd_t *cmd);

/** \brief Simple DMA transfer completion wait.
 *
 * This blocks the core until the specified transfer is finished. The transfer
 * must be described trough the identifier given to the copy function.
 *
 * \param   cmd  The copy structure (1d or 2d).
 */
static inline void pi_cl_dma_cmd_wait(pi_cl_dma_cmd_t *cmd);

/** \brief Simple DMA transfer completion flush.
 *
 * This blocks the core until the DMA does not have any pending transfer.
 */
static inline void pi_cl_dma_flush();

/** \brief 1D DMA memory transfer.
 *
 * This enqueues a 1D DMA memory transfer (i.e. classic memory copy) with simple
 * completion based on transfer identifier.
 *
 * \param copy A pointer to the structure describing the transfer. The same
 * structure can be used with pi_cl_dma_wait to wait for the completion of
 * this transfer.
 */
static inline void pi_cl_dma_memcpy(pi_cl_dma_copy_t *copy);

/** \brief 2D DMA memory transfer.
 *
 * This enqueues a 2D DMA memory transfer (rectangle area) with simple
 * completion based on transfer identifier.
 *
 * \param copy A pointer to the structure describing the transfer. The same
 * structure can be used with pi_cl_dma_wait to wait for the completion of
 * this transfer.
 */
static inline void pi_cl_dma_memcpy_2d(pi_cl_dma_copy_2d_t *copy);

/** \brief Simple DMA transfer completion wait.
 *
 * This blocks the core until the specified transfer is finished. The transfer
 * must be described trough the identifier given to the copy function.
 *
 * \param   copy  The copy structure (1d or 2d).
 */
static inline void pi_cl_dma_wait(void *copy);


//!@}

/**
 * @}
 */

/**
 * @}
 */


/// @cond IMPLEM

#define CL_DMA_COMMON                           \
    uint32_t ext;                               \
    uint32_t loc;                               \
    uint32_t id;                                \
    uint16_t size;                              \
    pi_cl_dma_dir_e dir;                        \
    uint8_t merge;

struct pi_cl_dma_copy_s
{
    CL_DMA_COMMON
    // 2d transfers args
    uint32_t stride;
    uint32_t length;
};

/// @endcond

#endif  /* __CL_DMA_H__ */
