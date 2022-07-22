/* 
 * Copyright (C) 2021 ETH Zurich, University of Bologna and GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 * Authors: Antoine Faravelon (antoine.faravelon@greenwaves-technologies.com)
 */

#include "pmsis.h"

#ifdef CLUSTER_TEST
#define BASE_ADDR 0x10000000
#define SIZE 0x00020000
#elif defined(PRIV_TEST)
        #warning "using L2 private"
        #define BASE_ADDR 0x1c000000
        #define SIZE 0x00010000
#else
        #define BASE_ADDR 0x1c010000
        #define SIZE 0x180000
#endif


volatile uint32_t *tested_mem = BASE_ADDR;    
uint32_t test_val[] = 
    {
        0x00000000,
        0xffffffff,
        0x00000000,
        0x01010101,
        0x02020202,
        0x04040404,
        0x08080808,
        0x10101010,
        0x20202020,
        0x40404040,
        0x80808080,
        0x00000000,
        0x0f0f0f0f,
        0x0e0e0e0e,
        0x08080808,
        0x00000000,
        0xf0f0f0f0,
        0xe0e0e0e0,
        0xc0c0c0c0,
        0x80808080,
        0x00000000,
    };
#define TEST_NB 21


static int test_entry()
{
    printf("test entry\n");
    int error = 0;

#ifdef CLUSTER_TEST
    struct pi_device cluster_dev;
    struct pi_cluster_conf cl_conf;

    pi_cluster_conf_init(&cl_conf);
    pi_open_from_conf(&cluster_dev, &cl_conf);
    if (pi_cluster_open(&cluster_dev))
    {
        return -1;
    }

#endif
    for (int test = 0; test < TEST_NB; test++)
    {
        for (int addr = 0; addr < SIZE/4; addr++)
        {
            tested_mem[addr] = test_val[test];
            if(tested_mem[addr] != test_val[test])
            {
                error++;
                printf("Memory is broken at %i + %i",BASE_ADDR, addr);
            }
        }
    }
    printf("there was %i errors",error);
    return error;
}

static void test_kickoff(void *arg)
{
    int ret = test_entry();
    pmsis_exit(ret);
}

int main()
{
    return pmsis_kickoff((void *)test_kickoff);
}
