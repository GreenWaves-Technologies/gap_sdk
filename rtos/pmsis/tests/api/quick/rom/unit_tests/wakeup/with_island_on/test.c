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

#include <bsp/ram/hyperram.h>
#ifdef USE_SPIRAM
#include <bsp/ram/spiram.h>
#endif

#include "pmsis/cluster/cluster_team/cl_team.h"

#define STACK_SIZE 2048

static int nb_fork;
static int step;

static void pe_entry(void *arg)
{
    int *var = (int *)arg;
    pi_cl_team_critical_enter();
    *var |= 1 << (pi_core_id());
    pi_cl_team_critical_exit();
}

static int check_fork(int nb_cores)
{
    unsigned int var = 0;
    nb_fork++;
    pi_cl_team_fork(nb_cores, pe_entry, (void *)&var);
    return (1 << pi_cl_team_nb_cores()) - 1 != var;
}

static void cluster_entry(void *arg)
{
    int *errors = (int *)arg;

    *errors += check_fork(0);
}


static int check_cluster(int close)
{
    struct pi_device cluster_dev;
    struct pi_cluster_conf conf;
    struct pi_cluster_task task;
    int errors = 0;

    nb_fork = 0;

    pi_cluster_conf_init(&conf);
    conf.id = 0;

    pi_open_from_conf(&cluster_dev, &conf);

    pi_cluster_open(&cluster_dev);

    pi_cluster_task(&task, &cluster_entry, (void *)&errors);
    pi_cluster_send_task_to_cl(&cluster_dev, &task);

    if (close)
    {
        pi_cluster_close(&cluster_dev);
    }

    if (nb_fork == 0)
        errors++;

    return errors;
}


static int open_ram(struct pi_device *ram)
{
#ifdef USE_HYPERRAM
    struct pi_hyperram_conf conf;
    pi_hyperram_conf_init(&conf);
#else
    struct pi_aps25xxxn_conf conf;
    pi_aps25xxxn_conf_init(&conf);
#endif

    pi_open_from_conf(ram, &conf);

    if (pi_ram_open(ram))
        return -1;

  return 0;
}



int test_entry()
{
    struct pi_device ram;

    // Setup the Pads now to avoid triggering some random communication with the testbench
    //testbench_prepare_pads();

    // And then release the outputs that we forced in case we come back from deep sleep
    pi_pad_sleep_cfg_force(0);

    if (open_ram(&ram))
        return -1;

    if (pi_pmu_boot_state_get() == PI_PMU_DOMAIN_STATE_OFF)
    {
        printf("STA\n");

        pi_ram_write(&ram, 0, (void *)&step, sizeof(step));

        struct pi_rtc_conf conf;
        uint32_t countdown;
        struct pi_device rtc;
        pi_rtc_conf_init(&conf);

        conf.clk_div = 0x20;
        countdown = 1;

        pi_open_from_conf(&rtc, &conf);

        if (pi_rtc_open(&rtc))
            return -1;

        pi_rtc_timer_set(&rtc, countdown);
        pi_rtc_ioctl(&rtc, PI_RTC_TIMER_START, (void *)1);

        // Force outputs during deep sleep to avoid random communication
        pi_pad_sleep_cfg_force(1);

        pi_pmu_wakeup_control(PI_PMU_WAKEUP_RTC, 0);
        pi_pmu_set_state(PI_PMU_DOMAIN_CHIP, PI_PMU_DOMAIN_STATE_DEEP_SLEEP, 0);
    }
    else
    {
        pi_ram_read(&ram, 0, (void *)&step, sizeof(step));

        step++;

        pi_ram_write(&ram, 0, (void *)&step, sizeof(step));

        if (step == 1)
        {
            if (check_cluster(0))
                goto error;

            pi_pmu_wakeup_fll_settings(1);
            pi_pmu_wakeup_sequence(PI_PMU_WAKEUP_SEQUENCE_SOC_CL_MRAM_ON);
            pi_pmu_wakeup_control(PI_PMU_WAKEUP_RTC, 0);

            pi_pmu_domain_state_change(PI_PMU_DOMAIN_CHIP, PI_PMU_DOMAIN_STATE_DEEP_SLEEP, 0);
        }
        else if (step == 2)
        {
            if (check_cluster(1))
                goto error;
        }

    }

    printf("Test success\n");
    return 0;

error:
    printf("Test failure\n");
    return -1;
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
