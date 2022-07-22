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

    testlib_i2s_trafficgen_t traffic_gen_0;
    testlib_i2s_trafficgen_t traffic_gen_1;

    testlib_i2s_trafficgen_config_t config;
    testlib_i2s_trafficgen_conf_init(&config);
    config.itf = 0;
    testlib_i2s_trafficgen_init(&traffic_gen_0, &config);
    testlib_i2s_trafficgen_start(&traffic_gen_0);

    testlib_i2s_trafficgen_conf_init(&config);
    config.itf = 1;
    testlib_i2s_trafficgen_init(&traffic_gen_1, &config);
    testlib_i2s_trafficgen_start(&traffic_gen_1);

    pi_time_wait_us(10000);

    testlib_i2s_trafficgen_stop(&traffic_gen_0);
    testlib_i2s_trafficgen_deinit(&traffic_gen_0);
    testlib_i2s_trafficgen_stop(&traffic_gen_1);
    testlib_i2s_trafficgen_deinit(&traffic_gen_1);

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