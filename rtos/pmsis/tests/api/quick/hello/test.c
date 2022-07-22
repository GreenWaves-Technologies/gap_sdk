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

#if defined(CONFIG_HELLOWORLD_CLUSTER)
void pe_entry(void *arg)
{
    printf("Hello from (%d, %d)\n", pi_cluster_id(), pi_core_id());
}

void cluster_entry(void *arg)
{
    pi_cl_team_fork(0, pe_entry, 0);
}
#endif

static int test_entry()
{
#if defined(CONFIG_HELLOWORLD_CLUSTER)
    struct pi_device cluster_dev;
    struct pi_cluster_conf cl_conf;
    struct pi_cluster_task cl_task;

    pi_cluster_conf_init(&cl_conf);
    pi_open_from_conf(&cluster_dev, &cl_conf);
    if (pi_cluster_open(&cluster_dev))
    {
        return -1;
    }
    pi_cluster_send_task_to_cl(&cluster_dev, pi_cluster_task(&cl_task, cluster_entry, NULL));
    pi_cluster_close(&cluster_dev);
#endif

    printf("Hello\n");

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