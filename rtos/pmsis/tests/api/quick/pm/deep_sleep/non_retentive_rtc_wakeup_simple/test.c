#include <stdio.h>
#include <pmsis.h>
#include "testbench.h"


#if defined(__PLATFORM_RTL__)
#define RTC_DIV   0x80
#define RTC_COUNT 0x1
#else
#define RTC_DIV   0x8000
#define RTC_COUNT 1*32768
#endif


void testbench_prepare_pads()
{
    pi_testbench_prepare_pads(NULL);

}

void testbench_exit(int status)
{
    pi_testbench_set_status(pi_testbench_get(), status);
    //while(1);
}


int test_entry()
{
    int status = -1;

    // Setup the Pads now to avoid triggering some random communication with the testbench
    testbench_prepare_pads();

    // And then release the outputs that we forced in case we come back from deep sleep
    pi_pad_sleep_cfg_force(0);


    if (pi_pmu_boot_state_get() == PI_PMU_DOMAIN_STATE_OFF)
    {
        printf("STA\n");

        struct pi_rtc_conf conf;
        uint32_t countdown;
        struct pi_device rtc;
        pi_rtc_conf_init(&conf);

        conf.clk_div = RTC_DIV;
        countdown = RTC_COUNT;

        pi_open_from_conf(&rtc, &conf);

        if (pi_rtc_open(&rtc))
            return -1;

        pi_rtc_timer_set(&rtc, countdown);
        pi_rtc_ioctl(&rtc, PI_RTC_TIMER_START, (void *)1);

        // Force outputs during deep sleep to avoid random communication
        pi_pad_sleep_cfg_force(1);
        //printf("Setup pmu\n");
        pi_pmu_wakeup_control(PI_PMU_WAKEUP_RTC, 0);

        pi_pmu_domain_state_change(PI_PMU_DOMAIN_CHIP, PI_PMU_DOMAIN_STATE_DEEP_SLEEP, 0);
    }
    else
    {
        printf("TOK\n");
        status = 0;
        testbench_exit(status);
        return status;
    }

    printf("TKO\n");
    testbench_exit(status);
    return status;
}

static void test_kickoff(void *arg)
{
    int ret = test_entry();
    pmsis_exit(ret);
}

int main()
{
    return pmsis_kickoff((void *) test_kickoff);
}
