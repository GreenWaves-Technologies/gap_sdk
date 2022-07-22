/* 
 * Copyright (C) 2017 ETH Zurich, University of Bologna and GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */

#include "pmsis.h"
#include "stdio.h"


#define NB_ITER 2048
//#define MIXED 1

extern void jmp_block_256_0(int nb_iter);

extern void jmp_block_512_0(int nb_iter);

extern void jmp_block_1024_0(int nb_iter);

extern void jmp_block_2048_0(int nb_iter);
extern void jmp_block_2048_1(int nb_iter);
extern void jmp_block_2048_2(int nb_iter);
extern void jmp_block_2048_3(int nb_iter);

extern void jmp_block_4096_0(int nb_iter);

extern void jmp_block_8192_0(int nb_iter);


static void (*jmp_block_2048[])(int) = {
    jmp_block_2048_0, jmp_block_2048_1, jmp_block_2048_2, jmp_block_2048_3
};


static void pe_entry(void *arg)
{
    int block_size = (int)arg;

    void (*jmp_block)(int);
    int nb_iter = NB_ITER * 256 / block_size;

#ifdef MIXED

    switch (block_size)
    {
        case 2048: jmp_block = jmp_block_2048[pi_core_id() % 4]; break;
        default: break;
    }

#else

    switch (block_size)
    {
        case 256: jmp_block = jmp_block_256_0; break;
        case 512: jmp_block = jmp_block_512_0; break;
        case 1024: jmp_block = jmp_block_1024_0; break;
        case 2048: jmp_block = jmp_block_2048_0; break;
        case 4096: jmp_block = jmp_block_4096_0; break;
        case 8192: jmp_block = jmp_block_8192_0; break;
        default: break;
    }

#endif

    pi_perf_conf(1<<PI_PERF_ACTIVE_CYCLES);
    pi_perf_reset();
    pi_perf_start();

    jmp_block(nb_iter);

    pi_perf_stop();

    int cycles = pi_perf_read(PI_PERF_ACTIVE_CYCLES);
    printf("    Core %d: Cycles: %d Cycles/instruction: %f\n", pi_core_id(), cycles, ((float)cycles) / nb_iter / block_size * 4);
}


static void check_block(int nb_cores, int block_size)
{
    printf("Starting (block_size: %d, nb_cores: %d)\n", block_size, nb_cores);
    if (nb_cores == pi_cl_cluster_nb_cores())
    {
        pi_cl_team_fork_cc(nb_cores, pe_entry, (void *)block_size);
    }
    else
    {
        pi_cl_team_fork(nb_cores, pe_entry, (void *)block_size);
    }
    printf("\n");

}


static void cluster_entry(void *arg)
{
#ifdef MIXED
    check_block(9, 2048);
#else
    check_block(1, 256);
    check_block(1, 512);
    check_block(1, 1024);
    check_block(1, 2048);
    check_block(1, 4096);
    check_block(1, 8192);

    check_block(9, 256);
    check_block(9, 512);
    check_block(9, 1024);
    check_block(9, 2048);
    check_block(9, 4096);
    check_block(9, 8192);
#endif
}

static int test_task_sync()
{
    struct pi_device cluster_dev;
    struct pi_cluster_conf conf;
    struct pi_cluster_task task;

    pi_cluster_conf_init(&conf);
    conf.id = 0;

    pi_open_from_conf(&cluster_dev, &conf);
      
    pi_cluster_open(&cluster_dev);

    pi_cluster_task(&task, &cluster_entry, NULL);
    pi_cluster_send_task_to_cl(&cluster_dev, &task);
    pi_cluster_close(&cluster_dev);

    return 0;
}

int test_entry()
{
    int errors = 0;

    printf("Starting test\n");

    errors += test_task_sync();

    if (errors)
      printf("Test failure\n");
    else
      printf("Test success\n");

    return errors;
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
