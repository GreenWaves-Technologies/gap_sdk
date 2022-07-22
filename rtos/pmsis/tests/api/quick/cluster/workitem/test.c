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

#define NB_WORKITEMS 12
#define NB_WORKGROUPS 8

static pi_task_t end_task[NB_WORKGROUPS];
static PI_CL_L1 pi_cl_workitem_t workitems[NB_WORKITEMS];
static PI_CL_L1 int nb_tasks;
static PI_CL_L1 int nb_pes;
static int nb_errors;

static void pe_entry(pi_cl_workitem_t *task, int id)
{
    for (volatile int i=0; i<100; i++)
    {

    }
    pi_cl_team_critical_enter();
    nb_pes--;
    pi_cl_team_critical_exit();
}

static void end_of_pe_task(pi_cl_workitem_t *task)
{
    nb_tasks--;

    if (nb_tasks == 0)
    {
        if (nb_pes)
        {
            nb_errors++;
        }

        pi_cl_workitem_disable();
    }

}

static void check_workgroup()
{
    int var = 0;

    pi_cl_workitem_enable();

    nb_pes = 0;
    nb_tasks = NB_WORKITEMS;
    for (int i=0; i<NB_WORKITEMS; i++)
    {
        nb_pes += (i % 8) + 1;
    }

    for (int i=0; i<NB_WORKITEMS; i++)
    {
        pi_cl_workitem_t *workitem = &workitems[i];

        pi_cl_workitem_init(workitem, pe_entry, end_of_pe_task);
        pi_cl_workitem_set_arg(workitem, 0, (uint32_t)&var);
        pi_cl_workitem_push(workitem, (i % 8) + 1);
    }
}

static void cluster_entry(void *arg)
{
    check_workgroup();
}

static int test_task_wave()
{
    struct pi_device cluster_dev;
    struct pi_cluster_conf conf;
    struct pi_cluster_task cluster_task[NB_WORKGROUPS];

    pi_cluster_conf_init(&conf);
    conf.id = 0;

    pi_open_from_conf(&cluster_dev, &conf);

    pi_cluster_open(&cluster_dev);

    for (int i=0; i<NB_WORKGROUPS; i++)
    {
        pi_task_block(&end_task[i]);
        pi_cluster_task(&cluster_task[i], cluster_entry, NULL);
        pi_cluster_send_task_async(&cluster_dev, &cluster_task[i], &end_task[i]);
    }
 
    for (int i=0; i<NB_WORKGROUPS; i++)
    {
        pi_task_wait_on(&end_task[i]);
    }
    
    if (nb_tasks)
    {
        nb_errors += 1;
    }

    pi_cluster_close(&cluster_dev);

    return nb_errors;
}

int test_entry()
{
    int errors = 0;

    printf("Starting test\n");

    errors += test_task_wave();

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
