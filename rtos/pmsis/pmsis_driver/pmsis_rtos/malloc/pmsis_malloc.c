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
#include "pmsis/rtos/malloc/pmsis_malloc.h"
#include "pmsis/rtos/malloc/pmsis_fc_tcdm_malloc.h"
#include "pmsis/rtos/malloc/pmsis_l2_malloc.h"
#include "core_utils.h"
/*******************************************************************************
 * Variables, macros, structures,... definition
 ******************************************************************************/

/*******************************************************************************
 * Variables, macros, structures,... definition
 ******************************************************************************/
// purely internal so declare in c file
#define HASH_TABLE_KEYS 5 /*!< Hash table's number of entries. */

/*!
 * @brief Memory bloc structure.
 *
 * This structure is used to keep history of malloc. A bloc is allocated each time
 * malloc function is called.
 */
typedef struct mem_bloc
{
    uint32_t *ptr;          /*!< Pointer to the allocated memory bloc. */
    uint32_t size;          /*!< Size of the allocated memory bloc. */
    struct mem_bloc *next; /*!< Pointer to next bloc. */
    struct mem_bloc *prev; /*!< Pointer to previous bloc. */
} mem_bloc_t;

static struct mem_bloc *mem_table[HASH_TABLE_KEYS]; /* Hash table for allocated memory blocs history. */

/*******************************************************************************
 * Function definition
 ******************************************************************************/

static inline int32_t hash_code(uint32_t key)
{
#ifdef __GAP8__
    uint32_t address = (key & 0xFFFF0000);
#else
    uint32_t address = (key & 0xFFF80000);
#endif
    switch (address)
    {
#ifdef __GAP8__
    case 0x00000000 :
    case 0x1B000000 :
        return 0;
        break;
    case 0x1C000000 :
    case 0x1C010000 :
        return 1;
        break;
    case 0x1C020000 :
    case 0x1C030000 :
        return 2;
        break;
    case 0x1C040000 :
    case 0x1C050000 :
        return 3;
        break;
    case 0x1C060000 :
    case 0x1C070000 :
        return 4;
        break;
#else
    case 0x1C000000 :
        return 0;
        break;
    case 0x1C080000 :
        return 1;
        break;
    case 0x1C100000 :
        return 0;
        break;
    case 0x1C180000 :
        return 1;
        break;
#endif
    default :
        break;
    }
    return -1;
}

void pmsis_malloc_init(void *fc_heap_start, uint32_t fc_heap_size,
        void *l2_heap_start, uint32_t l2_heap_size)
{
    for (uint32_t i=0; i<5; i++)
    {
        mem_table[i] = NULL;
    }
#ifdef __GAP8__
    if (__native_is_fc())
    {
        pmsis_fc_tcdm_malloc_init(fc_heap_start, fc_heap_size);
    }
#endif
    pmsis_l2_malloc_init(l2_heap_start, l2_heap_size);
}

void *pmsis_malloc(size_t size)
{
    /* Real memory bloc to return. */
    void *ptr = NULL;
#ifdef __GAP8__
    if (__native_is_fc())
    {
        ptr = pi_fc_tcdm_malloc(size);
    }
    if (ptr == NULL)
#endif
    {
        ptr = pi_l2_malloc(size);
    }
    /* Memory bloc to keep history of mallocs */
    void *ptr_bloc = NULL;
#ifdef __GAP8__
    if (__native_is_fc())
    {
        ptr_bloc = pi_fc_tcdm_malloc(sizeof(struct mem_bloc));
    }
    if (ptr_bloc == NULL)
#endif
    {
        ptr_bloc = pi_l2_malloc(sizeof(struct mem_bloc));
    }
    if ((ptr == NULL) || (ptr_bloc == NULL))
    {
        return NULL;
    }

    uint32_t key = hash_code((uint32_t) ptr);
    struct mem_bloc *new_bloc = (struct mem_bloc *) ptr_bloc;
    new_bloc->ptr = (uint32_t *) ptr;
    new_bloc->size = size;
    new_bloc->next = NULL;
    new_bloc->prev = NULL;
    if (mem_table[key] == NULL)
    {
        mem_table[key] = new_bloc;
    }
    else
    {
        struct mem_bloc *tmp = mem_table[key];
        while (tmp->next != NULL)
        {
            tmp = tmp->next;
        }
        new_bloc->prev = tmp;
        tmp->next = new_bloc;
    }
    return ptr;
}

void *pmsis_malloc_align(size_t size, uint32_t align)
{
    return NULL;
}

void pmsis_malloc_free(void *_chunk)
{
    uint32_t key = hash_code((uint32_t) _chunk);
    uint32_t size = -1;
    if (mem_table[key] == NULL)
    {
        return;
    }
    else
    {
        struct mem_bloc *tmp = mem_table[key];
        while (tmp != NULL)
        {
            if (tmp->ptr == (uint32_t *) _chunk)
            {
                size = tmp->size;
                /* Node is first of list. */
                if (tmp == mem_table[key])
                {
                    mem_table[key] = tmp->next;
                    mem_table[key]->prev = NULL;
                }
                /* Node is elsewhere in list. */
                else
                {
                    tmp->prev->next = tmp->next;
                    tmp->next->prev = tmp->prev;
                }
                /* Free MemBloc_t struct. */
                uint8_t bloc_area = ((uint32_t) tmp >> 24);
                switch (bloc_area)
                {
#ifdef __GAP8__
                case 0x00 :
                case 0x1B :
                    pi_fc_tcdm_free((void *) tmp, sizeof(struct mem_bloc));
                    break;
#endif
                case 0x1C :
                    pi_l2_free((void *) tmp, sizeof(struct mem_bloc));
                    break;
                default :
                    break;
                }
                break;
            }
            tmp = tmp->next;
        }
    }

    /* Free allocated memory bloc. */
    uint8_t area = ((uint32_t) _chunk >> 24);
    switch (area)
    {
#ifdef __GAP8__
    case 0x00 :
    case 0x1B :
        pi_fc_tcdm_free(_chunk, size);
        break;
#endif
    case 0x1C :
        pi_l2_free(_chunk, size);
        break;
    default :
        break;
    }
}

void pmsis_malloc_display(void)
{
    pmsis_l2_malloc_dump();
    for (uint32_t i=0; i<5; i++)
    {
        struct mem_bloc *tmp = mem_table[i];
        printf("Table %d", i);
        while (tmp != NULL)
        {
            printf("\tAddress %x\t Size %d\n", tmp->ptr, tmp->size);
            tmp = tmp->next;
        }
        printf("\n");
    }
}
