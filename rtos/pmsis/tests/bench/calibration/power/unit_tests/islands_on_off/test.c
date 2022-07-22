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
#define TIME_FACTOR 100
#else
// Each time will last 5 seconds
#define TIME_FACTOR 5000
#endif

static inline void power_start()
{
    *(volatile int *)0x10000000 = 0xabbaabba;
}

static inline void power_stop()
{
    *(volatile int *)0x10000000 = 0xdeadcaca;
}



int test_entry()
{
    printf("Starting test\n");

#if defined(TEST0) || defined(TEST_ALL)
    // First measure power when soc is on doing nothing
    {
        printf("Measuring soc idle\n");
        power_start();
        pi_time_wait_us(1000 * TIME_FACTOR);
        power_stop();
    }
#endif

#if defined(TEST1) || defined(TEST_ALL)
    // Then in soc idle
    {
        printf("Measuring soc clock-gating\n");
        power_start();
        pi_soc_idle_us(1000 * TIME_FACTOR);
        power_stop();
    }
#endif

#if defined(TEST2) || defined(TEST_ALL)
    // Then measure soc doing nothing plus cluster doing nothing
    {
        printf("Measuring cluster clock-gating\n");
        struct pi_device cluster_dev;
        struct pi_cluster_conf conf;

        pi_cluster_conf_init(&conf);
        conf.id = 0;

        pi_open_from_conf(&cluster_dev, &conf);

        pi_cluster_open(&cluster_dev);

        power_start();
        pi_time_wait_us(100000);
        power_stop();

        pi_cluster_close(&cluster_dev);
    }
#endif

#if defined(TEST3) || defined(TEST_ALL)
    {
        printf("Measuring cluster idle\n");
        struct pi_device cluster_dev;
        struct pi_cluster_conf conf;

        pi_cluster_conf_init(&conf);
        conf.id = 0;

        pi_open_from_conf(&cluster_dev, &conf);

        pi_cluster_open(&cluster_dev);

        // Start cluster timer to keep cluster in idle
        timer_conf_set(timer_base_cl(0, 0, 1), TIMER_CFG_LO_ENABLE(1));

        power_start();
        pi_time_wait_us(100000);
        power_stop();

        timer_conf_set(timer_base_cl(0, 0, 1), TIMER_CFG_LO_ENABLE(0));

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
