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

#ifdef __PLATFORM_GVSOC__
#define TIME_FACTOR 10
#else
// Each time will last 5 seconds
#define TIME_FACTOR 5000
#endif

#define BUFF_SIZE (1024*64)

static PI_L2 char ext_buff[BUFF_SIZE];
static PI_L1 char loc_buff[BUFF_SIZE];

static inline void cl_power_start()
{
    *(volatile int *)0x10000000 = 0xabbaabba;
}

static inline void cl_power_stop()
{
    *(volatile int *)0x10000000 = 0xdeadcaca;
}


static void dma_loc2ext_cluster_entry(void *arg)
{
    pi_cl_dma_cmd_t copy;

    cl_power_start();

    for (int i=0; i<10*TIME_FACTOR; i++)
    {
        pi_cl_dma_cmd((int)ext_buff, (int)loc_buff, BUFF_SIZE, PI_CL_DMA_DIR_LOC2EXT, &copy);
        pi_cl_dma_cmd_wait(&copy);
    }

    cl_power_stop();
}

static void dma_ext2loc_cluster_entry(void *arg)
{
    pi_cl_dma_cmd_t copy;

    cl_power_start();

    for (int i=0; i<10*TIME_FACTOR; i++)
    {
        pi_cl_dma_cmd((int)ext_buff, (int)loc_buff, BUFF_SIZE, PI_CL_DMA_DIR_EXT2LOC, &copy);
        pi_cl_dma_cmd_wait(&copy);
    }

    cl_power_stop();
}


int test_entry()
{
    printf("Starting test\n");

#if defined(TEST0) || defined(TEST_ALL)
    {
        printf("Measuring DMA ext2loc_transfers\n");
        struct pi_device cluster_dev;
        struct pi_cluster_conf conf;
        struct pi_cluster_task task;

        pi_cluster_conf_init(&conf);
        conf.id = 0;

        pi_open_from_conf(&cluster_dev, &conf);

        pi_cluster_open(&cluster_dev);

        pi_cluster_task(&task, &dma_ext2loc_cluster_entry, NULL);
        pi_cluster_send_task(&cluster_dev, &task);

        pi_cluster_close(&cluster_dev);
    }
#endif

#if defined(TEST1) || defined(TEST_ALL)
    {
        printf("Measuring DMA loc2ext_transfers\n");
        struct pi_device cluster_dev;
        struct pi_cluster_conf conf;
        struct pi_cluster_task task;

        pi_cluster_conf_init(&conf);
        conf.id = 0;

        pi_open_from_conf(&cluster_dev, &conf);

        pi_cluster_open(&cluster_dev);

        pi_cluster_task(&task, &dma_loc2ext_cluster_entry, NULL);
        pi_cluster_send_task(&cluster_dev, &task);

        pi_cluster_close(&cluster_dev);
    }
#endif

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
