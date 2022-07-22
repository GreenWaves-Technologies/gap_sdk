#include <stdio.h>
#include <pmsis.h>
#include "testbench.h"


#define WAKEUP_DELAY 1000000000

#if TEST_DURATION >= 100
#ifdef LONG_PERIOD
#define NB_EVENTS 150
#else
#define NB_EVENTS 600
#endif

#elif TEST_DURATION >= 75
#ifdef LONG_PERIOD
#define NB_EVENTS 25
#else
#define NB_EVENTS 100
#endif

#elif TEST_DURATION >= 50
#ifdef LONG_PERIOD
#define NB_EVENTS 5
#else
#define NB_EVENTS 20
#endif

#elif TEST_DURATION >= 25
#ifdef LONG_PERIOD
#define NB_EVENTS 2
#else
#define NB_EVENTS 5
#endif

#else
#define NB_EVENTS 1
#endif




static int my_var = NB_EVENTS;
static int step_cycles;
static pi_device_t bench;
static uint64_t start_time;



uint64_t testbench_get_time()
{
    return pi_testbench_get_time_ps(pi_testbench_get());
}

void testbench_exit(int status)
{
    pi_testbench_set_status(pi_testbench_get(), status);
    while(1);
}


int test_entry()
{
    // Setup the Pads now to avoid triggering some random communication with the testbench
    pi_testbench_reopen();

    // And then release the outputs that we forced in case we come back from deep sleep
    pi_pad_sleep_cfg_force(0);

    if (pi_pmu_boot_state_get() == PI_PMU_DOMAIN_STATE_OFF)
    {
        printf("STA\n");

        start_time = testbench_get_time();

        // We'll try to shift the wakeup at each iteration so that we cover a full RTC period
        // For that compute the number of cycles of 1 RTC period and divide by the number of iterations
        int64_t freq = pi_freq_get(PI_FREQ_DOMAIN_FC);

        int period_cycles = ((uint64_t)WAKEUP_DELAY) * freq / 1000000000000;

        printf("PRIOED CYCLES %d\n", period_cycles);

        #if NB_EVENTS != 1
        // Increase by 1.5 to cover a bit more than 1 period
        step_cycles = period_cycles / (NB_EVENTS - 1) * 3 / 2;
        #endif

        #ifndef CONFIG_BOOT_MODE_SPISLAVE
        pi_testbench_req_spim_verif_setup_t setup_config = { .enabled=1, .itf=4, .cs=0, .is_master=1, .mem_size_log2=16 };
        if (pi_testbench_spim_verif_setup(pi_testbench_get(), &setup_config))
            return -1;
        #endif

        pi_testbench_req_spim_verif_spi_wakeup_t config = { .delay = WAKEUP_DELAY, .frequency=50000000, .mode = PI_TESTBENCH_REQ_SPIM_VERIF_SPI_WAKEUP_MODE_CMD, .spi_reload=0};
        pi_testbench_spim_verif_spi_wakeup(pi_testbench_get(), 4, 0, &config);

        // Force outputs during deep sleep to avoid random communication
        pi_pad_sleep_cfg_force(1);

        pi_pmu_wakeup_control(PI_PMU_WAKEUP_SPISLAVE, 0);

        pi_pmu_domain_state_change(PI_PMU_DOMAIN_CHIP, PI_PMU_DOMAIN_STATE_DEEP_SLEEP_RETENTIVE, 0);
    }
    else
    {
        pi_pmu_wakeup_control(0, 0);

        //printf("Wakeup from deep sleep my_var=%d\n", my_var);
        my_var--;

        if (my_var == 0)
        {
            uint64_t total_time = testbench_get_time() - start_time;
            uint64_t min_time = ((uint64_t)WAKEUP_DELAY) * NB_EVENTS;

            #ifdef VERBOSE
            printf("Finished with time %lld min time %lld\n", total_time, min_time);
            #endif
            if (total_time >= min_time)
            {
                printf("TOK\n");
                testbench_exit(0);
                return 0;
            }
            else
            {
                printf("TKO\n");
                testbench_exit(-1);
                return -1;
            }
        }

        #ifndef CONFIG_BOOT_MODE_SPISLAVE
        pi_testbench_req_spim_verif_setup_t setup_config = { .enabled=1, .itf=4, .cs=0, .is_master=1, .mem_size_log2=16 };
        if (pi_testbench_spim_verif_setup(pi_testbench_get(), &setup_config))
            return -1;
        #endif

        pi_testbench_req_spim_verif_spi_wakeup_t config = { .delay = WAKEUP_DELAY, .frequency=50000000, .mode = PI_TESTBENCH_REQ_SPIM_VERIF_SPI_WAKEUP_MODE_CMD, .spi_reload=0};
        pi_testbench_spim_verif_spi_wakeup(pi_testbench_get(), 4, 0, &config);

        pi_pad_sleep_cfg_force(1);

        pi_pmu_wakeup_control(PI_PMU_WAKEUP_SPISLAVE, 0);

        #if !defined(__FREERTOS__)
        #ifndef LONG_PERIOD
        timer_cmp_set(timer_base_fc(1, 0),
            step_cycles*(NB_EVENTS-my_var)
        );

        timer_conf_set(timer_base_fc(1, 0),
            TIMER_CFG_LO_ENABLE(1) |
            TIMER_CFG_LO_RESET(1) |
            TIMER_CFG_LO_IRQEN(1) |
            TIMER_CFG_LO_ONE_S(1)
        );

        pos_wait_for_event(1 << ARCHI_FC_EVT_TIMER1_LO);
        #endif
        #endif  /* __FREERTOS__ */


        pi_pmu_domain_state_change(PI_PMU_DOMAIN_CHIP, PI_PMU_DOMAIN_STATE_DEEP_SLEEP_RETENTIVE, 0);
    }

    printf("TKO\n");
    testbench_exit(-1);
    return -1;
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
