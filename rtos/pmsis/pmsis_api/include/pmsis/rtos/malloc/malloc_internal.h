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

#ifndef __PMSIS_RTOS_MALLOC_MALLOC_INTERNAL_H__
#define __PMSIS_RTOS_MALLOC_MALLOC_INTERNAL_H__

#include <stdint.h>

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

/**
 * \brief Type of memory.
 */
#define INTERNAL_MALLOC (0)
#define EXTERNAL_MALLOC (1)

/**
 * \brief Memory block structure.
 */
typedef struct malloc_block_s
{
    int32_t                size; /*!< Size of the memory block. */
    struct malloc_block_s *next; /*!< Pointer to the next memory block. */
    uint32_t               addr; /*!< Address of the allocated chunk. */
} malloc_chunk_t;

/**
 * \brief Memory allocator structure.
 */
typedef struct malloc_s
{
    malloc_chunk_t *first_free; /*!< List of memory blocks. */
    uint8_t         type;       /*!< External or internal memory. */
} malloc_t;

/*******************************************************************************
 * API
 ******************************************************************************/

/**
 * \brief Get memory allocator information.
 */
void __malloc_info(malloc_t *a, int32_t *_size, void **first_chunk, int32_t *_nb_chunks);

/**
 * \brief Print information of a memory allocator.
 *
 * This function diplays information such as free blocks, size of the block,...
 * of a memory allocator.
 *
 * \param a              Pointer to a memory allocator.
 */
void __malloc_dump(malloc_t *a);

/**
 * \brief Initialize a memory allocator.
 *
 * This function initializes a memory allocator(FC or L1).
 *
 * \param a              Pointer to a memory allocator.
 * \param _chunk         Start address of a memory region.
 * \param size           Size of the memory region to be used by the allocator.
 */
void __malloc_init(malloc_t *a, void *_chunk, int32_t size);

/**
 * \brief Allocate memory from an allocator.
 *
 * This function allocates a memory chunk.
 *
 * \param a              Pointer to a memory allocator.
 * \param size           Size of the memory to be allocated.
 *
 * \return Start address of an allocated memory chunk or NULL if there is not
 *         enough memory to allocate.
 */
void *__malloc(malloc_t *a, int32_t size);

/**
 * \brief Free an allocated memory chunk.
 *
 * This function frees an allocated memory chunk.
 * The freed memory chunk is chained back to the list of free space to allocate
 * again.
 *
 * \param a              Pointer to a memory allocator.
 * \param _chunk         Start address of an allocated memory chunk.
 * \param size           Size of the allocated memory chunk.
 */
void __malloc_free(malloc_t *a, void *_chunk, int32_t size);

/**
 * \brief Allocate memory from an allocator with aligned address.
 *
 * This function allocates an adress aligned memory chunk.
 *
 * \param a              Pointer to a memory allocator.
 * \param size           Size of the memory to be allocated.
 * \param align          Memory alignement size.
 *
 * \return Start address of an allocated memory chunk or NULL if there is not
 *         enough memory to allocate.
 */
void *__malloc_align(malloc_t *a, int32_t size, int32_t align);

/**
 * \brief Initialize an external memory allocator.
 *
 * This function initializes an external memory allocator(HyperRam).
 *
 * \param a              Pointer to a memory allocator.
 * \param _chunk         Start address of a memory region.
 * \param size           Size of the memory region to be used by the allocator.
 */
uint32_t __malloc_extern_init(malloc_t *a, void *_chunk, int32_t size);

/**
 * \brief Free the memory bloc used by an external memory allocator.
 *
 * \param a              Pointer to a memory allocator.
 */
void __malloc_extern_deinit(malloc_t *a);

/**
 * \brief Allocate memory from an external allocator.
 *
 * This function allocates a memory chunk.
 *
 * \param a              Pointer to a memory allocator.
 * \param size           Size of the memory to be allocated.
 *
 * \return Start address of an allocated memory chunk or NULL if there is not
 *         enough memory to allocate.
 */
void *__malloc_extern(malloc_t *a, int32_t size);

/**
 * \brief Free an allocated memory chunk from external memory.
 *
 * This function frees an allocated memory chunk. The freed memory chunk is
 * chained back to the list of free space to allocate again.
 *
 * \param a              Pointer to a memory allocator.
 * \param _chunk         Start address of an allocated memory chunk.
 * \param size           Size of the allocated memory chunk.
 */
uint32_t __malloc_extern_free(malloc_t *a, void *_chunk, int32_t size);

#endif /* __PMSIS_RTOS_MALLOC_MALLOC_INTERNAL_H__ */
