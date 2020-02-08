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

#ifndef __PMSIS_RTOS_MALLOC_MALLOC_H__
#define __PMSIS_RTOS_MALLOC_MALLOC_H__

/**
 * \brief Initialize the memory allocators.
 *
 * This function initializes the FC and L2 memory allocators to allocate
 * from the FC or L2 memory heap.
 *
 * \param fc_heap_start  FC memory heap base address.
 * \param fc_heap_size   FC memory heap size.
 * \param l2_heap_start  L2 memory heap base address.
 * \param l2_heap_size   L2 memory heap size.
 *
 * \note If this function is called first, those subfunctions do not need to be
 *       called.
 */
void pi_malloc_init(void *fc_heap_start, uint32_t fc_heap_size,
                    void *l2_heap_start, uint32_t l2_heap_size);

/**
 * \brief Allocate memory from FC or L2 memory allocator.
 *
 * This function allocates a memory chunk in FC if there is enough memory to
 * allocate otherwise in L2.
 *
 * \param size           Size of the memory to be allocated.
 *
 * \return Pointer to an allocated memory chunk or NULL if there is not enough
 *         memory to allocate.
 */
void *pi_malloc(size_t size);

/**
 * \brief Allocate memory from FC or L2 memory allocator with aligned address.
 *
 * This function allocates an adress aligned memory chunk in FC if there is
 * enough memory to allocate required chunk of memory otherwise in L2.
 *
 * \param size           Size of the memory to be allocated.
 * \param align          Memory alignement size.
 *
 * \return Pointer to an allocated memory chunk or NULL if there is not enough
 *         memory to allocate.
 */
void *pi_malloc_align(size_t size, uint32_t align);

/**
 * \brief Free an allocated memory chunk.
 *
 * This function frees an allocated memory chunk.
 *
 * \param _chunk Start address of an allocated memory chunk.
 */
void pi_free(void *_chunk);

/**
 * \brief Display allocated blocs.
 *
 * This function displays allocated blocs in either FC or L2 memory.
 */
void pi_malloc_display(void);

#define pmsis_malloc_init    pi_malloc_init
#define pmsis_malloc         pi_malloc
#define pmsis_malloc_align   pi_malloc_align
#define pmsis_malloc_free    pi_free
#define pmsis_malloc_display pi_malloc_display

#endif  /* __PMSIS_RTOS_MALLOC_MALLOC_H__ */
