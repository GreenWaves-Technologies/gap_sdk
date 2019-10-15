/*
 * Copyright (c) 2017, GreenWaves Technologies, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of GreenWaves Technologies, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __GAP_MALLOC_H__
#define __GAP_MALLOC_H__

#include "gap_util.h"

/*!
 * @addtogroup malloc
 * @{
 */

/*******************************************************************************
 * Variables, macros, structures,... definitions
 ******************************************************************************/
/*
 * Allocate at least 4 bytes to avoid misaligned accesses when parsing free blocks
 * and actually 8 to fit free chunk header size and make sure a e free block to always have
 * at least the size of the header.
 * This also requires the initial chunk to be correctly aligned.
 */
#define MIN_CHUNK_SIZE 8

#define ALIGN_UP(addr,size)   (((addr) + (size) - 1) & ~((size) - 1))
#define ALIGN_DOWN(addr,size) ((addr) & ~((size) - 1))

/*! @brief Type od memory.  */
#define INTERNAL_MALLOC (0)
#define EXTERNAL_MALLOC (1)

/*! @brief Memory block structure. */
typedef struct malloc_block_s
{
    int32_t                size; /*!< Size of the memory block. */
    struct malloc_block_s *next; /*!< Pointer to the next memory block. */
    uint32_t               addr; /*!< Address of the allocated chunk. */
} malloc_chunk_t;

/*! @brief Memory allocator structure. */
typedef struct malloc_s
{
    malloc_chunk_t *first_free; /*!< List of memory blocks. */
    uint8_t         type;       /*!< External or internal memory. */
} malloc_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*!
 * @brief Get memory allocator information.
 */
void __malloc_info(malloc_t *a, int32_t *_size, void **first_chunk, int32_t *_nb_chunks);

/*!
 * @brief Print information of a memory allocator.
 *
 * This function diplays information such as free blocks, size of the block,... of a memory allocator.
 *
 * @param a Pointer to a memory allocator.
 */
void __malloc_dump(malloc_t *a);

/*!
 * @brief Initialize a memory allocator.
 *
 * This function initializes a memory allocator(FC or L1).
 *
 * @param a      Pointer to a memory allocator.
 * @param _chunk Start address of a memory region.
 * @param size   Size of the memory region to be used by the allocator.
 */
void __malloc_init(malloc_t *a, void *_chunk, int32_t size);

/*!
 * @brief Allocate memory from an allocator.
 *
 * This function allocates a memory chunk.
 *
 * @param a      Pointer to a memory allocator.
 * @param size   Size of the memory to be allocated.
 *
 * @return Start address of an allocated memory chunk or NULL if there is not enough memory to allocate.
 */
void *__malloc(malloc_t *a, int32_t size);

/*!
 * @brief Free an allocated memory chunk.
 *
 * This function frees an allocated memory chunk. The freed memory chunk is chained back to the list of free space to allocate again.
 *
 * @param a      Pointer to a memory allocator.
 * @param _chunk Start address of an allocated memory chunk.
 * @param size   Size of the allocated memory chunk.
 */
void __malloc_free(malloc_t *a, void *_chunk, int32_t size);

/*!
 * @brief Allocate memory from an allocator with aligned address.
 *
 * This function allocates an adress aligned memory chunk.
 *
 * @param a      Pointer to a memory allocator.
 * @param size   Size of the memory to be allocated.
 * @param align  Memory alignement size.
 *
 * @return Start address of an allocated memory chunk or NULL if there is not enough memory to allocate.
 */
void *__malloc_align(malloc_t *a, int32_t size, int32_t align);

/*!
 * @brief Initialize an external memory allocator.
 *
 * This function initializes an external memory allocator(HyperRam).
 *
 * @param a      Pointer to a memory allocator.
 * @param _chunk Start address of a memory region.
 * @param size   Size of the memory region to be used by the allocator.
 */
uint32_t __malloc_extern_init(malloc_t *a, void *_chunk, int32_t size);

/*!
 * @brief Free used memory bloc by the external memory allocator.
 *
 * @param a      Pointer to a memory allocator.
 */
void __malloc_extern_deinit(malloc_t *a);

/*!
 * @brief Allocate memory from an external allocator.
 *
 * This function allocates a memory chunk.
 *
 * @param a      Pointer to a memory allocator.
 * @param size   Size of the memory to be allocated.
 *
 * @return Start address of an allocated memory chunk or NULL if there is not enough memory to allocate.
 */
void *__malloc_extern(malloc_t *a, int32_t size);

/*!
 * @brief Free an allocated memory chunk from external memory.
 *
 * This function frees an allocated memory chunk. The freed memory chunk is
 * chained back to the list of free space to allocate again.
 *
 * @param a      Pointer to a memory allocator.
 * @param _chunk Start address of an allocated memory chunk.
 * @param size   Size of the allocated memory chunk.
 */
uint32_t __malloc_extern_free(malloc_t *a, void *_chunk, int32_t size);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

/* @} */

#endif /* __GAP_MALLOC_H__ */
