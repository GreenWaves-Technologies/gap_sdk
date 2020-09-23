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

#define HASH_TABLE_KEYS 5 /*!< Hash table's number of entries. */

/**
 * \brief Memory bloc structure.
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

static struct mem_bloc *mem_table[HASH_TABLE_KEYS]; /*!< Hash table for allocated memory blocs history. */

/*******************************************************************************
 * Function definition
 ******************************************************************************/

static inline int32_t hash_code(uint32_t key)
{
    #if defined(__GAP8__)
    uint32_t address = (key & 0xFFFF0000);
    #elif defined(__GAP9__)
    uint32_t address = (key & 0xFFFF8000);
    #endif  /* __GAP8__ */

    switch (address)
    {
    #if defined(__GAP8__)
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
    #elif defined(__GAP9__)
    case 0x1C000000 :           /* L2_PRIV0. */
        return 0;
        break;
    case 0x1C008000 :           /* L2_PRIV1. */
        return 1;
        break;
    case 0x1C010000 :           /* L2_SHARED. */
        return 2;
        break;
    #endif  /* __GAP8__ */
    default :
        break;
    }
    return -1;
}

void pi_malloc_init(void *fc_heap_start, uint32_t fc_heap_size,
                    void *l2_heap_start, uint32_t l2_heap_size)
{
    for (uint32_t i=0; i<5; i++)
    {
        mem_table[i] = NULL;
    }

    pi_fc_l1_malloc_init(fc_heap_start, fc_heap_size);
    pi_l2_malloc_init(l2_heap_start, l2_heap_size);
}

void *pi_malloc(size_t size)
{
    /* Real memory bloc to return. */
    void *ptr = NULL;

    ptr = pi_fc_l1_malloc(size);
    if (ptr == NULL)
    {
        ptr = pi_l2_malloc(size);
    }
    /* Memory bloc to keep history of mallocs */
    void *ptr_bloc = NULL;
    ptr_bloc = pi_fc_l1_malloc(sizeof(struct mem_bloc));
    if (ptr_bloc == NULL)
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

void *pi_malloc_align(size_t size, uint32_t align)
{
    return NULL;
}

void pi_free(void *_chunk)
{
    uint32_t key_chunk = hash_code((uint32_t) _chunk);
    uint32_t size = -1;
    if (mem_table[key_chunk] == NULL)
    {
        return;
    }
    else
    {
        struct mem_bloc *tmp = mem_table[key_chunk];
        while (tmp != NULL)
        {
            if (tmp->ptr == (uint32_t *) _chunk)
            {
                size = tmp->size;
                /* Node is first of list. */
                if (tmp == mem_table[key_chunk])
                {
                    mem_table[key_chunk] = tmp->next;
                    mem_table[key_chunk]->prev = NULL;
                }
                /* Node is elsewhere in list. */
                else
                {
                    if (tmp->prev != NULL)
                    {
                        tmp->prev->next = tmp->next;
                    }
                    if (tmp->next != NULL)
                    {
                        tmp->next->prev = tmp->prev;
                    }
                }
                /* Free MemBloc_t struct. */
                #if defined(__GAP8__)
                uint8_t bloc_area = ((uint32_t) tmp >> 24);
                switch (bloc_area)
                {
                case 0x00 :
                case 0x1B :
                    pi_fc_l1_free((void *) tmp, sizeof(struct mem_bloc));
                    break;
                case 0x1C :
                    pi_l2_free((void *) tmp, sizeof(struct mem_bloc));
                    break;
                default :
                    break;
                }
                #elif defined(__GAP9__)
                uint32_t key_mem_bloc = hash_code((uint32_t) tmp);
                if (key_mem_bloc < 2)
                {
                    pi_fc_l1_free((void *) tmp, sizeof(struct mem_bloc));
                }
                else
                {
                    pi_l2_free((void *) tmp, sizeof(struct mem_bloc));
                }
                #endif  /* __GAP8__ */
                break;
            }
            tmp = tmp->next;
        }
    }

    /* Free allocated memory bloc. */
    #if defined(__GAP8__)
    uint8_t area = ((uint32_t) _chunk >> 24);
    switch (area)
    {
    case 0x00 :
    case 0x1B :
        pi_fc_l1_free(_chunk, size);
        break;
    case 0x1C :
        pi_l2_free(_chunk, size);
        break;
    default :
        break;
    }
    #elif defined(__GAP9__)
    if (key_chunk < 2)
    {
        pi_fc_l1_free(_chunk, size);
    }
    else
    {
        pi_l2_free(_chunk, size);
    }
    #endif  /* __GAP8__ */
}

void pi_malloc_display(void)
{
    for (uint32_t i=0; i<5; i++)
    {
        struct mem_bloc *tmp = mem_table[i];
        printf("Table %ld", i);
        while (tmp != NULL)
        {
            printf("\tAddress %lx\t Size %ld\n", (uint32_t) tmp->ptr, tmp->size);
            tmp = tmp->next;
        }
        printf("\n");
    }
}
