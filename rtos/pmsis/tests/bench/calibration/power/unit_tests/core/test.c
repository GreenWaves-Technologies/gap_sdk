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

void add_loop(int nb_iter, int a, int b);
void nop_loop(int nb_iter);

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


static void nop_pe_entry(void *arg)
{
    if (pi_core_id())
    {
        cl_power_start();
    }

    nop_loop(10000*TIME_FACTOR);

    if (pi_core_id())
    {
        cl_power_stop();
    }
}


static void add_pe_entry(void *arg)
{
    if (pi_core_id())
    {
        cl_power_start();
    }

    add_loop(10000*TIME_FACTOR, 0x55555555, 0xCCCCCCCC);

    if (pi_core_id())
    {
        cl_power_stop();
    }
}

static void nop_cluster_entry(void *arg)
{
    pi_cl_team_fork((int)arg, nop_pe_entry, NULL);
}

static void add_cluster_entry(void *arg)
{
    pi_cl_team_fork((int)arg, add_pe_entry, NULL);
}


int test_entry()
{
    printf("Starting test\n");

#if defined(TEST0) || defined(TEST_ALL)
    {
        printf("Measuring fc nop\n");

        volatile int *addr = (volatile int *)&my_var;

        power_start();

        nop_loop(10000*TIME_FACTOR);

        power_stop();
    }
#endif

#if defined(TEST1) || defined(TEST_ALL)
    {
        printf("Measuring fc add\n");

        volatile int *addr = (volatile int *)&my_var;

        power_start();

        int a=0, b=0x55555555, c=0xCCCCCCCC;
        add_loop(10000*TIME_FACTOR, 0x55555555, 0xCCCCCCCC);

        power_stop();
    }
#endif

#if defined(TEST2) || defined(TEST_ALL)
    {
        printf("Measuring cluster nop 1 core\n");
        struct pi_device cluster_dev;
        struct pi_cluster_conf conf;
        struct pi_cluster_task task;

        pi_cluster_conf_init(&conf);
        conf.id = 0;

        pi_open_from_conf(&cluster_dev, &conf);

        pi_cluster_open(&cluster_dev);

        pi_cluster_task(&task, &nop_cluster_entry, (void *) 1);
        pi_cluster_send_task(&cluster_dev, &task);

        pi_cluster_close(&cluster_dev);
    }
#endif

#if defined(TEST3) || defined(TEST_ALL)
    {
        printf("Measuring cluster nop 8 core\n");
        struct pi_device cluster_dev;
        struct pi_cluster_conf conf;
        struct pi_cluster_task task;

        pi_cluster_conf_init(&conf);
        conf.id = 0;

        pi_open_from_conf(&cluster_dev, &conf);

        pi_cluster_open(&cluster_dev);

        pi_cluster_task(&task, &nop_cluster_entry, (void *) 8);
        pi_cluster_send_task(&cluster_dev, &task);

        pi_cluster_close(&cluster_dev);
    }
#endif

#if defined(TEST4) || defined(TEST_ALL)
    {
        printf("Measuring cluster add 1 core\n");
        struct pi_device cluster_dev;
        struct pi_cluster_conf conf;
        struct pi_cluster_task task;

        pi_cluster_conf_init(&conf);
        conf.id = 0;

        pi_open_from_conf(&cluster_dev, &conf);

        pi_cluster_open(&cluster_dev);

        pi_cluster_task(&task, &add_cluster_entry, (void *) 1);
        pi_cluster_send_task(&cluster_dev, &task);

        pi_cluster_close(&cluster_dev);
    }
#endif

#if defined(TEST5) || defined(TEST_ALL)
    {
        printf("Measuring cluster add 8 core\n");
        struct pi_device cluster_dev;
        struct pi_cluster_conf conf;
        struct pi_cluster_task task;

        pi_cluster_conf_init(&conf);
        conf.id = 0;

        pi_open_from_conf(&cluster_dev, &conf);

        pi_cluster_open(&cluster_dev);

        pi_cluster_task(&task, &add_cluster_entry, (void *) 8);
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
