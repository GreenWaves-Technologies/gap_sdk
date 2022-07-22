/* 
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#include "pmsis.h"
#include <string.h>
#include "testlib.h"

static int test_entry()
{
    printf("Entering test\n");

    testlib_i2c_trafficgen_t traffic_gen;

    testlib_i2c_trafficgen_config_t config;
    testlib_i2c_trafficgen_conf_init(&config);
    config.itf = 0;
    config.baudrate = 400000;
    testlib_i2c_trafficgen_init(&traffic_gen, &config);
    testlib_i2c_trafficgen_start(&traffic_gen);

    pi_time_wait_us(5000);

    testlib_i2c_trafficgen_stop(&traffic_gen);
    testlib_i2c_trafficgen_deinit(&traffic_gen);

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