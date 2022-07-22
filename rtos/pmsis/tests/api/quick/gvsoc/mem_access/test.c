/* 
 * Copyright (C) 2021 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#include "pmsis.h"
#include "stdio.h"


#define BUFFER_SIZE (64*1024)


static uint32_t current_write_value = 0;
static uint32_t current_read_value = 0;


int test_entry()
{
    uint32_t *buffer = pi_l2_malloc(BUFFER_SIZE);
    if (buffer == NULL) return -1;

    while(*(volatile int *)0x1c000000 != 0x11223344);

    while(1)
    {
        for (int i=0; i<BUFFER_SIZE/4; i++)
        {
            buffer[i] = current_write_value += 11;
        }

        *(volatile uint32_t *)0x1c000000 = (uint32_t)buffer;

        while(*(volatile int *)0x1c000000 != 0);

        for (int i=0; i<BUFFER_SIZE/4; i++)
        {
            if (buffer[i] != current_read_value)
            {
                printf("Error, expected %x, got %x\n", current_read_value, buffer[i]);
                exit(1);
            }
            current_read_value += 43;
        }
    }

    return 0;
}

void test_kickoff(void *arg)
{
    int ret = test_entry();
    pmsis_exit(ret);
}

int main()
{
    return pmsis_kickoff((void *)test_kickoff);
}
