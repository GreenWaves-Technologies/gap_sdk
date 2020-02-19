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

#ifndef __PMSIS_MEM_SLAB_H__
#define __PMSIS_MEM_SLAB_H__

#include "pmsis/pmsis_types.h"

/**
* @ingroup groupRTOS
*/

/**
 * @defgroup Mem_slab Memory slab allocator
 *
 */

/**
 * @addtogroup Mem_slab
 * @{
 */

/**@{*/

/**
 * @brief Initialize a memory slab.
 *
 * Initializes a memory slab, prior to its first use.
 *
 * The memory slab's buffer contains @a slab_num_blocks memory blocks
 * that are @a slab_block_size bytes long. The buffer must be aligned to an
 * N-byte boundary, where N is a power of 2 larger than 2 (i.e. 4, 8, 16, ...).
 * To ensure that each memory block is similarly aligned to this boundary,
 * @a slab_block_size must also be a multiple of N.
 *
 * @param slab Address of the memory slab.
 * @param buffer Pointer to buffer used for the memory blocks.
 * @param block_size Size of each memory block (in bytes).
 * @param num_blocks Number of memory blocks.
 */
extern void pi_mem_slab_init(pi_mem_slab_t *slab, void *buffer,
                             size_t block_size, uint32_t num_blocks);

/**
 * @brief Allocate memory from a memory slab.
 *
 * This routine allocates a memory block from a memory slab.
 *
 * @param slab Address of the memory slab.
 * @param mem Pointer to block address area.
 * @param timeout For future use, not supported for now.
 *
 * @retval 0 Memory allocated. The block address area pointed at by @a mem
 *         is set to the starting address of the memory block.
 * @retval -1 No block was available.
 */
extern int pi_mem_slab_alloc(pi_mem_slab_t *slab, void **mem,
                            int32_t timeout);

/**
 * @brief Free memory allocated from a memory slab.
 *
 * This routine releases a previously allocated memory block back to its
 * associated memory slab.
 *
 * @param slab Address of the memory slab.
 * @param mem Pointer to block address area (as set by k_mem_slab_alloc()).
 */
extern void pi_mem_slab_free(pi_mem_slab_t *slab, void **mem);


//!@}

/**
 * @}
 */

#endif  /* __PMSIS_MEM_SLAB_H__ */
