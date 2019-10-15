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

#ifndef __GAP_FC_MALLOC_H__
#define __GAP_FC_MALLOC_H__

#include "gap_util.h"
#include "gap_malloc.h"


/*!
 * @addtogroup fc_malloc
 * @{
 */

/*******************************************************************************
 * Variables, macros, structures,... definitions
 ******************************************************************************/

extern char __heapfcram_start; /*!< FC Heap start address. */
extern char __heapfcram_size;  /*!< FC Heap size. */

/*! @brief FC Free function alias. */
#define FC_Free     FC_MallocFree

/*******************************************************************************
 * APIs
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*!
 * @brief Initializes the fc malloc.
 *
 * @note This is will bind the FC heap start and stop address to the inner stucture malloc_t __fc_malloc,
 *        It will record the allocation information of FC heap
 */
void FC_MallocInit();

/*!
 * @brief Allocate memory from fc malloc.
 *
 * @note Allocate the memory according to the size and return the address pointer.
 *
 * @param size Memory allocation size.
 *
 * @return memory address pointer.
 *
 */
void *FC_Malloc(int32_t size);

/*!
 * @brief Allocate the fc heap memory with aligned size.
 *
 * @param size  Memory allocation size.
 * @param align Memory allocation align size.
 *
 * @return memory address pointer.
 *
 */
void *FC_MallocAlign(int32_t size, int32_t align);

/*!
 * @brief Free the fc malloc.
 *
 * @note Free the memory pointed by the _chunk
 *
 * @param _chunk Address of memory need to free.
 *
 */
void FC_MallocFree(void *_chunk);


#if defined(__cplusplus)
}
#endif /* __cplusplus */

/* @} */

#endif /*__GAP_FC_MALLOC_H__*/
