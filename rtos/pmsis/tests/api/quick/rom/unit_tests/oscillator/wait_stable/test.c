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

static int step;

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
        int osc_down = 0;
        int fixed_wait = 0;
#ifdef FIXED_WAIT
        fixed_wait = 1;
#endif
#ifdef SHUTDOWN_OSCILLATOR
        osc_down = 1;
#endif
        printf("STA (fixed_wait: %d, osc_down: %d)\n", fixed_wait, osc_down);

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

        #if defined(SHUTDOWN_OSCILLATOR)
        printf("Osc down\n");
        apb_soc_ctrl_reg_osc_ctrl_set(ARCHI_APB_SOC_CTRL_ADDR,
                                      APB_SOC_CTRL_REG_OSC_CTRL_SLOW_OSC_EN(1));
        #endif

        pi_pmu_domain_state_change(PI_PMU_DOMAIN_CHIP, PI_PMU_DOMAIN_STATE_DEEP_SLEEP, 0);
    }
    else
    {
        pi_ram_read(&ram, 0, (void *)&step, sizeof(step));

        step++;

        pi_ram_write(&ram, 0, (void *)&step, sizeof(step));

        if (step == 1)
        {
            pi_pmu_wakeup_control(PI_PMU_WAKEUP_RTC, 0);

            pi_pmu_domain_state_change(PI_PMU_DOMAIN_CHIP, PI_PMU_DOMAIN_STATE_DEEP_SLEEP, 0);
        }
        else if (step == 2)
        {
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
