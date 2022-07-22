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

static int my_var;
static PI_CL_L1 int my_var_l1;

static inline void power_start()
{
    *(volatile int *)0x1C000000 = 0xabbaabba;
}

static inline void power_stop()
{
    *(volatile int *)0x1C000000 = 0xdeadcaca;
}

static inline void cl_power_start()
{
    *(volatile int *)0x10000000 = 0xabbaabba;
}

static inline void cl_power_stop()
{
    *(volatile int *)0x10000000 = 0xdeadcaca;
}


static void l1_read_cluster_entry(void *arg)
{
    cl_power_start();

    volatile int *addr = (volatile int *)&my_var_l1;

    for (int i=0; i<10000*TIME_FACTOR; i++)
    {
        *addr;
        *addr;
        *addr;
        *addr;
    }

    cl_power_stop();
}


static void l1_write_cluster_entry(void *arg)
{
    cl_power_start();

    volatile int *addr = (volatile int *)&my_var_l1;

    for (int i=0; i<10000*TIME_FACTOR; i++)
    {
        *addr = 0;
        *addr = 0;
        *addr = 0;
        *addr = 0;
    }

    cl_power_stop();
}


int test_entry()
{
    printf("Starting test\n");

#if defined(TEST0) || defined(TEST_ALL)
    {
        printf("Measuring l2 fc read access\n");

        volatile int *addr = (volatile int *)&my_var;

        power_start();

        for (int i=0; i<10000*TIME_FACTOR; i++)
        {
            *addr;
            *addr;
            *addr;
            *addr;
        }

        power_stop();
    }
#endif

#if defined(TEST1) || defined(TEST_ALL)
    {
        printf("Measuring l2 fc read access\n");

        volatile int *addr = (volatile int *)&my_var;

        power_start();

        for (int i=0; i<10000*TIME_FACTOR; i++)
        {
            *addr = 1;
            *addr = 1;
            *addr = 1;
            *addr = 1;
        }

        power_stop();
    }
#endif

#if defined(TEST2) || defined(TEST_ALL)
    {
        printf("Measuring cluster l1 read access\n");
        struct pi_device cluster_dev;
        struct pi_cluster_conf conf;
        struct pi_cluster_task task;

        pi_cluster_conf_init(&conf);
        conf.id = 0;

        pi_open_from_conf(&cluster_dev, &conf);

        pi_cluster_open(&cluster_dev);

        pi_cluster_task(&task, &l1_read_cluster_entry, NULL);
        pi_cluster_send_task(&cluster_dev, &task);

        pi_cluster_close(&cluster_dev);
    }
#endif

#if defined(TEST3) || defined(TEST_ALL)
    {
        printf("Measuring cluster l1 write access\n");
        struct pi_device cluster_dev;
        struct pi_cluster_conf conf;
        struct pi_cluster_task task;

        pi_cluster_conf_init(&conf);
        conf.id = 0;

        pi_open_from_conf(&cluster_dev, &conf);

        pi_cluster_open(&cluster_dev);

        pi_cluster_task(&task, &l1_write_cluster_entry, NULL);
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
