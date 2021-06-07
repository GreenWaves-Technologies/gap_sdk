/*
 * Copyright (c) 2019, GreenWaves Technologies, Inc.
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

#include <stdio.h>
#include "pmsis/rtos/malloc/pi_malloc.h"
#include "pmsis/rtos/malloc/fc_l1_malloc.h"
#include "pmsis/rtos/malloc/l2_malloc.h"
#include "pmsis/targets/target.h"

/*******************************************************************************
 * Variables, macros, structures,... definition
 ******************************************************************************/

/*******************************************************************************
 * Function definition
 ******************************************************************************/

void pi_malloc_init(void *fc_heap_start, uint32_t fc_heap_size,
                    void *l2_heap_start, uint32_t l2_heap_size)
{
    pi_fc_l1_malloc_init(fc_heap_start, fc_heap_size);
    pi_l2_malloc_init(l2_heap_start, l2_heap_size);
}

void *pi_malloc(size_t size)
{
    void *ptr = NULL;

    uint32_t allocate_size = size + sizeof(uint32_t);
    ptr = pi_fc_l1_malloc(allocate_size);
    if (ptr == NULL)
    {
        ptr = pi_l2_malloc(allocate_size);
    }
    *(uint32_t *) ptr = allocate_size;

    /* Real memory bloc to return, address shifted. */
    ptr += sizeof(uint32_t);

    return ptr;
}

void *pi_malloc_align(size_t size, uint32_t align)
{
    return NULL;
}

void pi_free(void *_chunk)
{
    uint32_t *real_chunk = (uint32_t*) ((uint32_t) _chunk - sizeof(uint32_t));
    uint32_t size = *real_chunk;
    /* Free allocated memory bloc. */
    #if defined(__GAP8__)
     uint8_t area = ((uint32_t) real_chunk >> 24);
    switch (area)
    {
    case 0x00 :
    case 0x1B :
        pi_fc_l1_free(real_chunk, size);
        break;
    case 0x1C :
        pi_l2_free(real_chunk, size);
        break;
    default :
        break;
    }
    #elif defined(__GAP9__) || defined (__VEGA__)
    if ((uint32_t) real_chunk < (uint32_t) L2_SHARED_ADDR)
    {
        pi_fc_l1_free(real_chunk, size);
    }
    else
    {
        pi_l2_free(real_chunk, size);
    }
    #endif  /* __GAP8__ */
}

void pi_malloc_dump(void)
{
    pi_fc_l1_malloc_dump();
    pi_l2_malloc_dump();
}
