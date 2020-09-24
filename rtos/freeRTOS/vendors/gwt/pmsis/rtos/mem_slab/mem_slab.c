/*
 * Copyright (c) 2016 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "pmsis.h"
#include "pmsis/mem_slab/mem_slab.h"


static void create_free_list(struct pi_mem_slab *slab)
{
    uint32_t j;
    char *p;

    slab->free_list = NULL;
    p = slab->buffer + slab->block_size*(slab->num_blocks - 1);

    for (j = 0U; j < slab->num_blocks; j++) {
        *(char **)p = slab->free_list;
        slab->free_list = p;
        p -= slab->block_size;
    }
}


void pi_mem_slab_init(pi_mem_slab_t *slab, void *buffer,
            size_t block_size, uint32_t num_blocks)
{
    slab->num_blocks = num_blocks;
    slab->block_size = block_size;
    slab->buffer = buffer;
    slab->num_used = 0U;
    create_free_list(slab);
}


int pi_mem_slab_alloc(pi_mem_slab_t *slab, void **mem, int32_t timeout)
{
    int result;

    int irq = disable_irq();

    if (slab->free_list != NULL)
    {
        /* take a free block */
        *mem = slab->free_list;
        slab->free_list = *(char **)(slab->free_list);
        slab->num_used++;
        result = 0;
    }
    else
    {
        /* don't wait for a free block to become available */
        *mem = NULL;
        result = -1;
    }

    restore_irq(irq);

    return result;
}


void pi_mem_slab_free(pi_mem_slab_t *slab, void **mem)
{
    int irq = disable_irq();

    **(char ***)mem = slab->free_list;
    slab->free_list = *(char **)mem;
    slab->num_used--;

    restore_irq(irq);
}
