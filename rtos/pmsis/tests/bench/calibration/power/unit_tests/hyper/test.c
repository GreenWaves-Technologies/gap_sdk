/* 
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#include "pmsis.h"
#include "testbench.h"
#include <string.h>
#include "testlib.h"

static int test_entry()
{
    printf("Entering test\n");

    testlib_hyperram_trafficgen_t traffic_gen;

    testlib_hyperram_trafficgen_config_t config;
    testlib_hyperram_trafficgen_conf_init(&config);
    testlib_hyperram_trafficgen_init(&traffic_gen, &config);
    testlib_hyperram_trafficgen_start(&traffic_gen);


    pi_time_wait_us(1000);


    testlib_hyperram_trafficgen_stop(&traffic_gen);
    testlib_hyperram_trafficgen_deinit(&traffic_gen);

    return 0;
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